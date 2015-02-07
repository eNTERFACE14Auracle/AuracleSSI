// Classifier.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/03/04
// Copyright (C) 2007-13 University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "model/Classifier.h"
#include "signal/MatrixOps.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t Classifier::ssi_log_name[] = "classifier";

Classifier::Classifier (const ssi_char_t *file) 
	: _file (0),
	_trainer (0),
	_probs (0),
	_n_handler (0),
	_n_classes (0),
	_merged_sample_dimension (0),
	_consumer_sr (0),
	_consumer_dim (0),
	_consumer_byte (0),
	_consumer_num (0),
	_n_metas (0),
	_metas (0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {	 

	for (ssi_size_t i = 0; i < SSI_CLASSIFIER_MAXHANDLER; i++) {
		_handler[0] = 0;
	}

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Classifier::~Classifier () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void Classifier::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	// set up handler
	if (_options.console) {
		FileHandler *handler = new FileHandler ();
		addHandler (handler);
	}
	if (_options.file[0] != '\0') {
		FileHandler *handler = new FileHandler (_options.file);
		addHandler (handler);
	}
	if (_options.socket) {		
		SocketHandler *handler = new SocketHandler (_options.host, _options.port, _options.type, _options.osc, _options.oscid, _options.full);
		addHandler (handler);
	}

	// load trainer
	if (_options.trainer[0] != '\0') {
		_trainer = new Trainer ();
		if (!Trainer::Load (*_trainer, _options.trainer)) {
			ssi_err ("could not load trainer '%s'", _options.trainer);
		}

	} else if (!_trainer) {
		ssi_err ("trainer not set");
	}

	// prepare classification
	_n_classes = _trainer->getClassSize ();
	_probs = new ssi_real_t[_n_classes];
	_merged_sample_dimension = 0;
	_consumer_sr = 0;
	_consumer_byte = sizeof (ssi_real_t);
	_consumer_dim = _trainer->getClassSize ();
	_consumer_num = 1;	
	_n_metas = _trainer->getMetaSize ();
	if (_n_metas > 0) {
		_metas = new ssi_real_t[_n_metas];
	}

	// in case of several streams
	// test if sample rates are equal
	if (_options.merge && stream_in_num > 0) {
		_merged_sample_dimension = stream_in[0].dim;
		if (stream_in_num > 0) {
			for (ssi_size_t i = 1; i < stream_in_num; i++) {
				if (stream_in[i].type != stream_in[0].type) {
					ssi_err ("sample type must not differ (%s != %s)", SSI_TYPE_NAMES[stream_in[i].type], SSI_TYPE_NAMES[stream_in[0].type]);
				}
				if (stream_in[i].byte != stream_in[0].byte) {
					ssi_err ("sample size must not differ (%u != %u)", stream_in[i].byte, stream_in[0].byte);
				}
				_merged_sample_dimension += stream_in[i].dim;
			}
		}
	}
}

void Classifier::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	ssi_real_t *dataptr = ssi_pcast (ssi_real_t, stream_in[0].ptr);

	bool result = false;
	if (_options.merge && stream_in_num > 0) {
		ssi_stream_t tmp;
		ssi_stream_init (tmp, stream_in[0].num, _merged_sample_dimension, stream_in[0].byte, stream_in[0].type, stream_in[0].sr);
		ssi_byte_t *tmp_ptr = tmp.ptr;
		for (ssi_size_t i = 0; i < stream_in_num; i++) {

			if (stream_in[i].num != stream_in[0].num) {
				ssi_err ("#samples in merged streams must not differ (%u != %u)", stream_in[i].num, stream_in[0].num);
			}

			memcpy (tmp_ptr, stream_in[i].ptr, stream_in[i].tot);
			tmp_ptr += stream_in[i].tot;
		}
		result = _trainer->forward_probs (tmp, _n_classes, _probs);
		ssi_stream_destroy (tmp);
	} else if (stream_in_num > 0) {
		ssi_stream_t **streams = new ssi_stream_t *[stream_in_num];
		for (ssi_size_t i = 0; i < stream_in_num; i++) {
			streams[i] = &stream_in[i];		
		}
		result = _trainer->forward_probs (stream_in_num, streams, _n_classes, _probs);
		delete[] streams;
	} else {
		result = _trainer->forward_probs (stream_in[0], _n_classes, _probs);
	}

	if (result && _options.pthres != 0) {
		bool exceeds_pthres = false;
		for (ssi_size_t i = 0; i < _n_classes; i++) {
			if (_probs[i] > _options.pthres) {
				exceeds_pthres = true;
			}
		}
		result = exceeds_pthres;
	}

	if (result) {

		ssi_size_t max_ind = 0;
		ssi_real_t max_val = _probs[0];
		for (ssi_size_t i = 1; i < _n_classes; i++) {
			if (_probs[i] > max_val) {
				max_val = _probs[i];
				max_ind = i;
			}
		}

		bool has_meta = false;
		if (_n_metas > 0) {
			has_meta = _trainer->getMetaData (_n_metas, _metas);
		}

		ssi_msg (SSI_LOG_LEVEL_DETAIL, "recognized class %s", _trainer->getClassName (max_ind));	

		for (ssi_size_t i = 0; i < _n_handler; i++) {
			if(_handler[i])
			{	
				if (has_meta) {
					_handler[i]->handle (consume_info.time, consume_info.dur, _n_classes, max_ind, _probs, _trainer->getClassNames (), _n_metas, _metas);	
				} else {
					_handler[i]->handle (consume_info.time, consume_info.dur, _n_classes, max_ind, _probs, _trainer->getClassNames (), 0, 0);	
				}
			}
		}
	}
}

void Classifier::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	releaseHandler ();

	if (_options.trainer[0] != '\0') {
		delete _trainer;
		_trainer = 0;
	}

	delete[] _probs; _probs = 0;
	_n_classes = 0;
	_merged_sample_dimension = 0;
	_consumer_sr = 0;
	_consumer_dim = 0;
	_consumer_byte = 0;
	_consumer_num = 0;

	delete[] _metas; _metas = 0;
	_n_metas = 0;
}

bool Classifier::setEventListener (IEventListener *listener) {

	if (!listener) {
		return false;
	}

	EventHandler *handler = new EventHandler (listener, _options.sname, _options.ename);
	addHandler (handler);

	_event_address.setSender (_options.sname);
	_event_address.setEvents (_options.ename);

	return true;
}


// class FileHandler
Classifier::FileHandler::FileHandler (const ssi_char_t *filepath)  
	: _file (0) {
	
	if (filepath != 0 && filepath[0] != '\0') {
		_file = fopen (filepath, "w");
	}

	if (!_file) {
		_file = stdout;
	}
};

Classifier::FileHandler::~FileHandler () {

	if (_file != stdout) {
		fclose (_file);
	}
}

void Classifier::FileHandler::handle (ssi_time_t time,
	ssi_time_t duration,
	ssi_size_t n_classes,
	ssi_size_t class_index,
	const ssi_real_t *probs,
	ssi_char_t *const*class_names,
	ssi_size_t n_metas,
	ssi_real_t *metas) {

	ssi_fprint (_file, "%s {%.2f", class_names[class_index], probs[0]);
	for (ssi_size_t i = 1; i < n_classes; i++) {
		ssi_fprint (_file, ", %.2f", probs[i]);
	}
	ssi_fprint (_file, "}\n");

	if (n_metas > 0) {
		ssi_fprint (_file, "metas {%.2f", metas[0]);
		for (ssi_size_t i = 1; i < n_metas; i++) {
			ssi_fprint (_file, ", %.2f", metas[i]);
		}
		ssi_fprint (_file, "}\n");
	}

	fflush (_file);
}

// Class SocketHandler
Classifier::SocketHandler::SocketHandler (const ssi_char_t *host,
	int port,
	Socket::TYPE type,
	bool use_osc,
	const ssi_char_t *oscid,
	bool full)
	: _socket (0),
	_socket_osc (0),
	_use_osc (use_osc),
	_oscid (0),
	_full (full) {

	_socket = Socket::CreateAndConnect (type, Socket::SEND, port, host);
	if (_use_osc) {
		_socket_osc = new SocketOsc (*_socket);
		if (oscid) {
			_oscid = ssi_strcpy (oscid);
		}
	}
};

Classifier::SocketHandler::~SocketHandler () {

	delete _socket_osc;
	delete[] _oscid;
	delete _socket;		
}

void Classifier::SocketHandler::handle (ssi_time_t time,
	ssi_time_t duration,
	ssi_size_t n_classes,
	ssi_size_t class_index,
	const ssi_real_t *probs,
	ssi_char_t *const*class_names,
	ssi_size_t n_metas,
	ssi_real_t *metas) { 

	if (_use_osc) {
		if (_full) {
			_socket_osc->send_event (_oscid, ssi_cast (osc_int32, time * 1000 + 0.5), ssi_cast (osc_int32, duration * 1000 + 0.5), SSI_ESTATE_COMPLETED, ssi_cast (osc_int32, n_classes), const_cast<ssi_char_t **> (class_names), const_cast<ssi_real_t *> (probs));
		} else {
			ssi_real_t event_value = probs[class_index];
			ssi_char_t *event_name = const_cast<ssi_char_t *> (class_names[class_index]);
			_socket_osc->send_event (_oscid, ssi_cast (osc_int32, time * 1000 + 0.5), ssi_cast (osc_int32, duration * 1000 + 0.5), SSI_ESTATE_COMPLETED, 1, &event_name, &event_value);
		}
	} else {
		_socket->send (class_names[class_index], 1 + ssi_cast (int, strlen (class_names[class_index]))); 
	}
};

// Class EventHandler
Classifier::EventHandler::EventHandler (IEventListener *listener,
	const ssi_char_t *sname,
	const ssi_char_t *ename) 
	: _listener (listener) {

	ssi_size_t sid = Factory::AddString (sname);
	ssi_size_t eid = Factory::AddString (ename);

	ssi_event_init (_event, SSI_ETYPE_NTUPLE, sid, eid);
};

Classifier::EventHandler::~EventHandler () {

	ssi_event_destroy (_event);	
}

void Classifier::EventHandler::handle (ssi_time_t time,
	ssi_time_t duration,
	ssi_size_t n_classes,
	ssi_size_t class_index,
	const ssi_real_t *probs,
	ssi_char_t *const*class_names,
	ssi_size_t n_metas,
	ssi_real_t *metas) { 

	if (_event.tot == 0) {
		ssi_event_adjust (_event, n_classes * sizeof (ssi_event_tuple_t));
		ssi_event_tuple_t *ptr = ssi_pcast (ssi_event_tuple_t, _event.ptr);
		for (ssi_size_t i = 0; i < n_classes; i++) {
			ptr[i].id = Factory::AddString (class_names[i]);
			ptr[i].value = probs[i];			
		}
	} else {
		ssi_event_tuple_t *ptr = ssi_pcast (ssi_event_tuple_t, _event.ptr);
		for (ssi_size_t i = 0; i < n_classes; i++) {			
			ptr[i].value = probs[i];		
		}
	}

	_event.time = ssi_cast (ssi_size_t, time * 1000 + 0.5);
	_event.dur = ssi_cast (ssi_size_t, duration * 1000 + 0.5);
	_listener->update (_event);
};

}
