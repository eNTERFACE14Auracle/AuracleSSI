// Trainer.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/06/12
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

#include "model/Trainer.h"
#include "model/ModelTools.h"
#include "base/Factory.h"
#include "model/ISMissingData.h"
#include "model/ISSelectDim.h"
#include "model/ISTransform.h"
#include "model/Selection.h"
#include "signal/SignalTools.h"
#include "ioput/option/OptionList.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *Trainer::ssi_log_name = "trainer___";
int Trainer::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;

Trainer::VERSION Trainer::DEFAULT_VERSION = Trainer::V4;

Trainer::Trainer ()
	: _n_models (0),
	_models (0),	
	_fusion (0),	
	_n_classes (0),
	_class_names (0), 
	_n_users (0),
	_user_names (0),
	_n_streams (0),
	_stream_index (0),
	_stream_refs (0),
	_n_stream_select (0),
	_stream_select (0),
	_has_selection (false),
	_transformer (0),
	_transformer_frame (0),
	_transformer_delta (0),
	_has_transformer (false),
	_is_trained (false),
	_preproc_mode (false) {
}

Trainer::Trainer (IModel *model,
	ssi_size_t stream_index) 
	: _n_models (1),
	_models (0),	
	_fusion (0),	
	_n_classes (0),
	_class_names (0), 
	_n_users (0),
	_user_names (0),
	_n_streams (0),
	_stream_refs (0),
	_stream_index (stream_index),
	_n_stream_select (0),
	_stream_select (0),
	_has_selection (false),
	_transformer (0),
	_transformer_frame (0),
	_transformer_delta (0),
	_has_transformer (false),
	_is_trained (false),
	_preproc_mode (false) {

	_models = new IModel*[1];
	_models[0] = model;
};

Trainer::Trainer (ssi_size_t n_models,
	IModel **models,
	IFusion *fusion)
	: _n_models (n_models),
	_models (0),
	_fusion (fusion),
	_n_classes (0),
	_class_names (0), 
	_n_users (0),
	_user_names (0),
	_n_streams (0),
	_stream_index (0),
	_stream_refs (0),
	_n_stream_select (0),
	_stream_select (0),
	_has_selection (false),
	_transformer (0),
	_transformer_frame (0),
	_transformer_delta (0),
	_has_transformer (false),
	_is_trained (false),
	_preproc_mode (false) {

	_models = new IModel*[_n_models];
	for (ssi_size_t n_model = 0; n_model < n_models; n_model++) {
		_models[n_model] = models[n_model];
	}

}

Trainer::~Trainer () {

	release ();	
	delete[] _models;
	delete[] _stream_refs;
}

void Trainer::init_class_names (ISamples &samples) {

	free_class_names ();

	_n_classes = samples.getClassSize ();
	_class_names = new ssi_char_t *[_n_classes];
	for (ssi_size_t i = 0; i < _n_classes; i++) {
		_class_names[i] = ssi_strcpy (samples.getClassName (i));
	}
}

void Trainer::free_class_names () {

	if (_class_names) {
		for (ssi_size_t i = 0; i < _n_classes; i++) {
			delete[] _class_names[i];
		}
		delete[] _class_names;
		_class_names = 0;
		_n_classes = 0;
	}	
}

void Trainer::init_user_names (ISamples &samples) {

	free_user_names ();

	_n_users = samples.getUserSize ();
	_user_names = new ssi_char_t *[_n_users];
	for (ssi_size_t i = 0; i < _n_users; i++) {
		_user_names[i] = ssi_strcpy (samples.getUserName (i));
	}
}

void Trainer::free_user_names () {

	if (_user_names) {
		for (ssi_size_t i = 0; i < _n_users; i++) {
			delete[] _user_names[i];
		}
		delete[] _user_names;
		_user_names = 0;
		_n_users = 0;
	}	
}

bool Trainer::setSelection (ssi_size_t n_streams, 
	ssi_size_t *n_stream_select, 
	ssi_size_t **stream_select) {

	if (_is_trained) {
		ssi_wrn ("already trained");
		return false;
	}

	if (_has_selection) {
		ssi_wrn ("already has a selection");
		return false;
	}

	if (_has_transformer && n_streams != _n_streams) {
		ssi_wrn ("#streams in selection (%u) must not differ from #transformer (%u)", n_streams, _n_streams);
	} else {
		_n_streams = n_streams;
	}

	_n_stream_select = new ssi_size_t[_n_streams];
	_stream_select = new ssi_size_t *[_n_streams];
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		_n_stream_select[i] = n_stream_select[i];
		_stream_select[i] = 0;
		if (_n_stream_select[i] > 0) {
			_stream_select[i] = new ssi_size_t[_n_stream_select[i]];
			for (ssi_size_t j = 0; j < _n_stream_select[i]; j++) {
				_stream_select[i][j] = stream_select[i][j];
			}
		}
	}

	_has_selection = true;

	return true;
}

bool Trainer::setSelection (ISamples &samples,
	ISelection *fselmethod,
	ISelection *pre_fselmethod,
	ssi_size_t n_pre_feature) {

	if (_is_trained) {
		ssi_wrn ("already trained");
		return false;
	}
	
	if (_has_selection) {
		ssi_wrn ("already has a selection");
		return false;
	}

	if (_has_transformer && samples.getStreamSize () != _n_streams) {
		ssi_wrn ("#streams in selection (%u) must not differ from #transformer (%u)", samples.getStreamSize (), _n_streams);
	} else {
		_n_streams = samples.getStreamSize ();
	}

	if (_n_streams != _n_models) {
		ssi_wrn ("to apply feature selection #streams (%u) must be equal to #models (%u)", _n_streams, _n_models);
		return false;
	}

	_n_stream_select = new ssi_size_t[_n_streams];
	_stream_select = new ssi_size_t*[_n_streams];

	if (pre_fselmethod && n_pre_feature > 0) {
			
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "apply feature selection '%s'->%u->'%s' using %u streams", pre_fselmethod->getName (), n_pre_feature, fselmethod->getName (), _n_streams);		

		ISSelectDim pre_samples (&samples);
		
		for (ssi_size_t nstream = 0; nstream < _n_streams; nstream++) {

			// pre

			Selection pre_selection;
		
			if (pre_fselmethod->isWrapper ()) {
				pre_fselmethod->setModel (*_models[nstream]);
			}

			if (samples.hasMissingData ()) {
				ISMissingData samples_m (&samples);
				samples_m.setStream (nstream);							
				pre_fselmethod->train (samples_m, nstream);		
			} else {
				pre_fselmethod->train (samples, nstream);		
			}

			pre_selection.set (pre_fselmethod->getSize (), pre_fselmethod->getScores (), pre_fselmethod->sortByScore ());
			pre_selection.selNFirst (n_pre_feature);

			pre_samples.setSelection (nstream, pre_selection.getSize (), pre_selection.getSelected (), false); 

			ssi_msg (SSI_LOG_LEVEL_DEBUG, "selected %u feature applying '%s' on stream %u", n_pre_feature, pre_fselmethod->getName (), nstream);
		
			// final

			Selection final_selection (&pre_selection);

			if (fselmethod->isWrapper ()) {
				fselmethod->setModel (*_models[nstream]);
			}

			if (pre_samples.hasMissingData ()) {
				ISMissingData samples_m (&pre_samples);
				samples_m.setStream (nstream);							
				fselmethod->train (samples_m, nstream);		
			} else {
				fselmethod->train (pre_samples, nstream);		
			}

			final_selection.set (fselmethod->getSize (), fselmethod->getScores (), fselmethod->sortByScore ());
			final_selection.selNBest ();

			_n_stream_select[nstream] = final_selection.getSize ();
			_stream_select[nstream] = new ssi_size_t[_n_stream_select[nstream]];
			memcpy (_stream_select[nstream], final_selection.getSelected (), _n_stream_select[nstream] * sizeof (ssi_size_t));

			ssi_msg (SSI_LOG_LEVEL_DEBUG, "selected %u feature applying '%s' on stream %u", final_selection.getSize (), fselmethod->getName (), nstream);
		}

	} else {

		ssi_msg (SSI_LOG_LEVEL_DETAIL, "apply feature selection '%s' using %u streams", fselmethod->getName (), _n_streams);		
		
		for (ssi_size_t nstream = 0; nstream < _n_streams; nstream++) {
		
			// final

			Selection final_selection;

			if (fselmethod->isWrapper ()) {
				fselmethod->setModel (*_models[nstream]);
			}

			if (samples.hasMissingData ()) {
				ISMissingData samples_m (&samples);
				samples_m.setStream (nstream);							
				fselmethod->train (samples_m, nstream);		
			} else {
				fselmethod->train (samples, nstream);		
			}

			final_selection.set (fselmethod->getSize (), fselmethod->getScores (), fselmethod->sortByScore ());
			final_selection.selNBest ();

			_n_stream_select[nstream] = final_selection.getSize ();
			_stream_select[nstream] = new ssi_size_t[_n_stream_select[nstream]];
			memcpy (_stream_select[nstream], final_selection.getSelected (), _n_stream_select[nstream] * sizeof (ssi_size_t));

			ssi_msg (SSI_LOG_LEVEL_DEBUG, "selected %u feature applying '%s' on stream %u", final_selection.getSize (), fselmethod->getName (), nstream);
		}
	}

	_has_selection = true;

	return true;
}


bool Trainer::setTransformer (ssi_size_t n_streams,
	ITransformer **transformer,
	ssi_size_t *transformer_frame,
	ssi_size_t *transformer_delta) {
	
	if (_is_trained) {
		ssi_wrn ("already trained");
		return false;
	}
	
	if (_has_transformer) {
		ssi_wrn ("already has transformer");
		return false;
	}

	if (_has_selection && n_streams != _n_streams) {
		ssi_wrn ("#streams in selection (%u) must not differ from #transformer (%u)", _n_streams, n_streams);
	} else {
		_n_streams = n_streams;
	}

	_transformer = new ITransformer *[_n_streams];
	_transformer_frame = new ssi_size_t[n_streams];
	_transformer_delta = new ssi_size_t[n_streams];
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		_transformer[i] = transformer[i];
		_transformer_frame[i] = transformer_frame ? transformer_frame[i] : 0;
		_transformer_delta[i] = transformer_delta ? transformer_delta[i] : 0;
	}	

	_has_transformer = true;

	return true;
}

bool Trainer::preproc (ISamples &samples, SampleList &samples_pre) {

	if (samples.getSize () == 0) {
		ssi_wrn ("empty sample list");
		return true;
	}

	if (_has_transformer) {

		ssi_size_t n_streams = samples.getStreamSize ();
		if (n_streams != _n_streams) {
			ssi_wrn ("#streams in samples (%u) must not differ from #transformer (%u)", n_streams, _n_streams);
			return false;
		}		

		ISTransform istransform (&samples);
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			if (_transformer[i]) {
				istransform.setTransformer (i, *_transformer[i], _transformer_frame[i], _transformer_delta[i]);
			}
		}
		istransform.callEnter ();
		ModelTools::CopySampleList (istransform, samples_pre);
		istransform.callFlush ();

	} else {
		ModelTools::CopySampleList (samples, samples_pre);
	}

	return true;
}

bool Trainer::setPreprocMode (bool toggle,
	ssi_size_t n_streams,
	ssi_stream_t stream_refs[]) {

	if (toggle) {

		if (_n_streams > 0 && _n_streams != n_streams) {
			ssi_wrn ("#streams '%u' does not match '%u'", n_streams, _n_streams);
			return false;
		}
		
		if (_stream_refs) {
			for (ssi_size_t i = 0; i < _n_streams; i++) {
				if (stream_refs[i].byte != _stream_refs[i].byte) {
					ssi_wrn ("#bytes '%u' in stream #%u does not match '%u'", stream_refs[i].byte, i, _stream_refs[i].byte);
					return false;
				}
				if (stream_refs[i].dim != _stream_refs[i].dim) {
					ssi_wrn ("#dim '%u' in stream #%u does not match '%u'", stream_refs[i].dim, i, _stream_refs[i].dim);
					return false;
				}				
			}
		} else {
			_n_streams = n_streams;
			_stream_refs = new ssi_stream_t[_n_streams];
			for (ssi_size_t n_stream = 0; n_stream < _n_streams; n_stream++) {
				_stream_refs[n_stream] = stream_refs[n_stream];
				_stream_refs[n_stream].ptr = 0;
				ssi_stream_reset (_stream_refs[n_stream]);
			}
		}

		_preproc_mode = true;
	} else {
		_preproc_mode = false;
	}

	return true;
}

bool Trainer::train (ISamples &samples) {	

	if (_n_models == 0) {
		ssi_wrn ("no models");
		return false;
	}

	if (samples.getSize () == 0) {
		ssi_wrn ("empty sample list");
		return false;
	}

	if (_is_trained) {
		ssi_wrn ("already trained");
		return false;
	}	

	ISamples *samples_ptr = &samples;
	SampleList *samples_transf = 0;
	ISSelectDim *samples_select = 0;	

	if (!_preproc_mode) {

		_n_streams = samples.getStreamSize ();
		_stream_refs = new ssi_stream_t[_n_streams];
		for (ssi_size_t n_stream = 0; n_stream < _n_streams; n_stream++) {
			_stream_refs[n_stream] = samples.getStream (n_stream);
			_stream_refs[n_stream].ptr = 0;
			ssi_stream_reset (_stream_refs[n_stream]);
		}

		if (_has_transformer) {
			samples_transf = new SampleList ();
			preproc (samples, *samples_transf);
			samples_ptr = samples_transf;
		}

	}
	
	if (_has_selection) {

		ssi_size_t n_streams = samples_ptr->getStreamSize ();	
		if (_n_streams != n_streams) {
			ssi_wrn ("#streams in samples (%u) must not differ from #streams in selection (%u)", n_streams, _n_streams);
			return false;
		}

		samples_select = new ISSelectDim (samples_ptr);
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			if (_n_stream_select[i] > 0) {
				samples_select->setSelection (i, _n_stream_select[i], _stream_select[i], false);
			}
		}

		samples_ptr = samples_select;

	}

	bool result = train_h (*samples_ptr);
		
	delete samples_transf;
	delete samples_select;

	return result;
}

bool Trainer::train_h (ISamples &samples) {	

	if (_fusion) {
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "train '%s' using %u streams", _fusion->getName (), samples.getStreamSize ());
		if (ssi_log_level >= SSI_LOG_LEVEL_DETAIL) {
			for (ssi_size_t nstream = 0; nstream < samples.getStreamSize (); nstream++) {				 
				ssi_print ("             stream#%02u %ux%u %s\n", nstream, samples.getSize (), samples.getStream (nstream).dim, SSI_TYPE_NAMES[samples.getStream (nstream).type]);
			}	
			for (ssi_size_t nmodel = 0; nmodel < _n_models; nmodel++) {				 
				ssi_print ("             model#%02u '%s'\n", nmodel, _models[nmodel]->getName ());
			}			
		}
		_fusion->train (_n_models, _models, samples);		
	} else {
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "train '%s' using stream#%02u %ux%u %s", _models[0]->getName (), _stream_index, samples.getSize (), samples.getStream (_stream_index).dim, SSI_TYPE_NAMES[samples.getStream (_stream_index).type]);
		if (samples.hasMissingData ()) {
			ISMissingData samples_md (&samples);
			samples_md.setStream (_stream_index);
			_models[0]->train (samples_md, _stream_index);		
		} else {
			_models[0]->train (samples, _stream_index);		
		}
	}
	
	init_class_names (samples);	
	init_user_names (samples);
	_is_trained = true;	

	return true;
}

// test sample
bool Trainer::forward (ssi_stream_t &stream, ssi_size_t &class_index) {	

	ssi_stream_t *s = &stream;
	return forward (1, &s, class_index);
}

bool Trainer::forward (ssi_size_t num,
	ssi_stream_t **streams,
	ssi_size_t &class_index) {

	ssi_real_t *probs = new ssi_real_t[_n_classes];
	if (!forward_probs (num, streams, _n_classes, probs)) {
		delete[] probs;
		return false;
	}

	ssi_size_t max_ind = 0;
	ssi_real_t max_val = probs[0];
	for (ssi_size_t i = 1; i < _n_classes; i++) {
		if (probs[i] > max_val) {
			max_val = probs[i];
			max_ind = i;
		}
	}
	
	delete[] probs;
	class_index = max_ind;

	return true;
}

bool Trainer::forward_probs (ssi_stream_t &stream,
	ssi_size_t class_num,
	ssi_real_t *class_probs) {

	ssi_stream_t *s = &stream;
	return forward_probs (1, &s, class_num, class_probs);
}

bool Trainer::forward_probs (ssi_size_t n_streams,
	ssi_stream_t **streams,
	ssi_size_t n_probs,
	ssi_real_t *probs) {

	if (!_is_trained) {
		ssi_wrn ("not trained");
		return false;
	}

	if (n_streams != _n_streams) {
		ssi_wrn ("#streams not compatible (%u != %u)", n_streams, _n_streams);
		return false;
	}

	if (!_fusion && streams[_stream_index]->num == 0) {
		return false;
	}

	if (!_preproc_mode) {
		for (ssi_size_t n_stream = 0; n_stream < n_streams; n_stream++) {		
			if (streams[n_stream] && !ssi_stream_compare (*streams[n_stream], _stream_refs[n_stream])) {
				ssi_wrn ("stream #%u not compatible", n_stream);
				ssi_print ("received stream:\n");
				ssi_stream_info (*streams[n_stream], ssiout);
				ssi_print ("expected stream:\n");
				ssi_stream_info (_stream_refs[n_stream], ssiout);
				return false;			
			}
		}
	}	

	if (!_has_transformer && !_has_selection) {		
		if (_fusion) {
			return _fusion->forward (_n_models, _models, n_streams, streams, n_probs, probs);		
		} else {
			return _models[0]->forward (*streams[_stream_index], n_probs, probs);		
		}
	}

	if (_preproc_mode && !_has_selection) {		
		if (_fusion) {
			return _fusion->forward (_n_models, _models, n_streams, streams, n_probs, probs);		
		} else {
			return _models[0]->forward (*streams[_stream_index], n_probs, probs);		
		}
	}

	ssi_stream_t **streams_ptr = new ssi_stream_t *[n_streams];
	ssi_stream_t **streams_s = new ssi_stream_t *[n_streams];
	ssi_stream_t **streams_t = new ssi_stream_t *[n_streams];
	for (ssi_size_t i = 0; i < n_streams; i++) {
		streams_ptr[i] = streams[i];
		streams_s[i] = 0;
		streams_t[i] = 0;
	}

	if (!_preproc_mode && _has_transformer) {
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			if (streams_ptr[i] && _transformer[i]) {
				streams_t[i] = new ssi_stream_t;
				SignalTools::Transform (*streams_ptr[i], *streams_t[i], *_transformer[i], _transformer_frame[i], _transformer_delta[i]);
				streams_ptr[i] = streams_t[i];
			}	
		}
	}
	
	if (_has_selection) {
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			if (streams_ptr[i] && _stream_select[i]) {
				streams_s[i] = new ssi_stream_t;
				ssi_stream_select (*streams_ptr[i], *streams_s[i], _n_stream_select[i], _stream_select[i]);
				streams_ptr[i] = streams_s[i];
			}	
		}
	} 

	bool result;
	if (_fusion) {
		result = _fusion->forward (_n_models, _models, n_streams, streams_ptr, n_probs, probs);		
	} else {
		result = _models[0]->forward (*streams_ptr[_stream_index], n_probs, probs);		
	}

	for (ssi_size_t i = 0; i < n_streams; i++) {
		if (streams_s[i]) {
			ssi_stream_destroy (*streams_s[i]);
			delete streams_s[i];
		}
		if (streams_t[i]) {
			ssi_stream_destroy (*streams_t[i]);
			delete streams_t[i];
		}
	}
	delete[] streams_ptr;
	delete[] streams_s;
	delete[] streams_t;
	
	return result;
}

bool Trainer::Load (Trainer &trainer,
	const ssi_char_t *filepath) {

	FilePath fp (filepath);
	ssi_char_t *filepath_with_ext = 0;
	if (strcmp (fp.getExtension (), SSI_FILE_TYPE_TRAINER) != 0) {
		filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_TRAINER);
	} else {
		filepath_with_ext = ssi_strcpy (filepath);
	}

	ssi_msg (SSI_LOG_LEVEL_BASIC, "load trainer from file '%s'", filepath_with_ext);

	TiXmlDocument doc;
	if (!doc.LoadFile (filepath_with_ext)) {
		ssi_wrn ("failed loading trainer from file '%s'", filepath_with_ext);
		return false;
	}

	TiXmlElement *body = doc.FirstChildElement();	
	if (!body || strcmp (body->Value (), "trainer") != 0) {
		ssi_wrn ("tag <trainer> missing");
		return false;	
	}

	VERSION version;
	if (body->QueryIntAttribute ("ssi-v", ssi_pcast (int, &version)) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <ssi-v> in tag <trainer> missing");
		return false;	
	}

	bool result = false;

	trainer.release ();

	switch (version) {
		case V1:
		case V2:
		case V3:
			result = Load_V1to3 (trainer, fp, body, version);
			break;
		case V4:
			result = Load_V4 (trainer, fp, body);
			break;
		default:
			ssi_wrn ("unkown version %d", version);
			return false;
	}

	trainer._is_trained = result;

	delete[] filepath_with_ext;

	return result;

}

bool Trainer::Load_V4 (Trainer &trainer,
	FilePath &fp,
	TiXmlElement *body) {

	ssi_char_t string[SSI_MAX_CHAR];

	TiXmlElement *element = body->FirstChildElement ("classes");
	if (!element || strcmp (element->Value (), "classes") != 0) {
		ssi_wrn ("tag <element> missing");
		return false;
	}

	int n_classes;
	if (element->QueryIntAttribute ("size", &n_classes) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <size> in tag <classes> missing");
		return false;	
	}	
	trainer._n_classes = n_classes; 
	trainer._class_names = new ssi_char_t *[n_classes];	  
	TiXmlElement *item = element->FirstChildElement ("item");
	for (int n_class = 0; n_class < n_classes; n_class++) {
		if (!item || strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("could not parse tag <item> in <classes>");
			return false;
		}
		const char *name = item->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <classes>");
			return false;
		}
		trainer._class_names[n_class] = ssi_strcpy (name);
		item = item->NextSiblingElement("item");
	}

	element = body->FirstChildElement ("users");
	if (!element || strcmp (element->Value (), "users") != 0) {
		ssi_wrn ("tag <users> missing");
		return false;
	}
	int n_users;
	if (element->QueryIntAttribute ("size", &n_users) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <size> in tag <users> missing");
		return false;	
	}
	trainer._n_users = n_users; 
	trainer._user_names = new ssi_char_t *[n_users];	  
	item = element->FirstChildElement ("item");
	for (int n_user = 0; n_user < n_users; n_user++) {
		if (!item || strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("could not parse tag <item> in <users>");
			return false;
		}
		const char *name = item->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <users>");
			return false;
		}
		trainer._user_names[n_user] = ssi_strcpy (name);
		item = item->NextSiblingElement("item");
	}

	element = body->FirstChildElement ("streams");
	if (!element || strcmp (element->Value (), "streams") != 0) {
		ssi_wrn ("tag <streams> missing");
		return false;
	}
	int n_streams;
	if (element->QueryIntAttribute ("size", &n_streams) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <size> in tag <streams> missing");
		return false;	
	}	
	item = element->FirstChildElement ("item");
	trainer._n_streams = n_streams; 
	trainer._stream_refs = new ssi_stream_t[n_streams];
	item = element->FirstChildElement ();
	for (int n_stream = 0; n_stream < n_streams; n_stream++) {
		if (!item || strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("could not parse tag <item> in <streams>");
			return false;
		}
		int byte = 0;
		if (!item->Attribute ("byte", &byte)) {
			ssi_wrn ("attribute <byte> missing in <item>");
			return false;
		}
		int dim = 0;
		if (!item->Attribute ("dim", &dim)) {
			ssi_wrn ("attribute <dim> missing in <item>");
			return false;
		}
		double sr = 0;
		if (!item->Attribute ("sr", &sr)) {
			ssi_wrn ("attribute <sr> missing in <item>");
			return false;
		}
		const char *type_name = item->Attribute ("type");
		if (!type_name) {
			ssi_wrn ("attribute <type> missing in <item>");
			return false;
		}
		ssi_type_t type = SSI_UNDEF;
		if (!ssi_name2type (type_name, type)) {
			ssi_wrn ("found unkown <type> in <item>");
			return false;
		}
		ssi_stream_init (trainer._stream_refs[n_stream], 0, dim, byte, type, sr);		
		item = item->NextSiblingElement("item");
	}

	if (element = body->FirstChildElement ("transform")) {
		
		trainer._has_transformer = true;
		trainer._transformer = new ITransformer *[trainer._n_streams];
		trainer._transformer_frame = new ssi_size_t[trainer._n_streams];
		trainer._transformer_delta = new ssi_size_t[trainer._n_streams];
		for (ssi_size_t i = 0; i < trainer._n_streams; i++) {
			trainer._transformer[i] = 0;
			trainer._transformer_frame[i] = 0;
			trainer._transformer_delta[i] = 0;
		}

		item = element->FirstChildElement ("item");
		if (item) {
			do {
				
				int stream_id;
				if (!item->Attribute ("stream", &stream_id)) {
					ssi_wrn ("attribute <stream> missing in <item>");
					return false;
				}
				const ssi_char_t *name = item->Attribute ("name");
				if (!name) {
					ssi_wrn ("attribute <name> missing in <item>");
					return false;
				}
				const ssi_char_t *option = item->Attribute ("option");
				if (!option) {
					ssi_wrn ("attribute <option> missing in <item>");
					return false;
				}

				IObject *transformer = 0;
				if (option[0] != '\0') {
					ssi_sprint (string, "%s%s", fp.getDir (), option);
					transformer = Factory::Create (name, string, true);
				} else {
					transformer = Factory::Create (name, 0, true);
				}

				if (!transformer) {
					ssi_wrn ("could not create instance '%s'", transformer);
					return false;
				}				
				trainer._transformer[stream_id] = ssi_pcast (ITransformer, transformer);
				
				int frame = 0, delta = 0;
				if (item->Attribute ("frame", &frame)) {
					trainer._transformer_frame[stream_id] = ssi_cast (ssi_size_t, frame);
				}				
				if (item->Attribute ("delta", &delta)) {
					trainer._transformer_frame[stream_id] = ssi_cast (ssi_size_t, delta);
				}				
				
			} while (item = item->NextSiblingElement ("item"));			
		}
	}

	if (element = body->FirstChildElement ("select")) {

		trainer._has_selection = true;
		trainer._n_stream_select = new ssi_size_t[trainer._n_streams];
		trainer._stream_select = new ssi_size_t *[trainer._n_streams];
		for (ssi_size_t i = 0; i < trainer._n_streams; i++) {
			trainer._n_stream_select[i] = 0;
			trainer._stream_select[i] = 0;
		}

		item = element->FirstChildElement ("item");
		if (item) {
			do {
				
				int stream_id;
				if (!item->Attribute ("stream", &stream_id)) {
					ssi_wrn ("attribute <stream> missing in <item>");
					return false;
				}
				if (ssi_cast (ssi_size_t, stream_id) >= trainer._n_streams) {
					ssi_wrn ("stream id (%d) exceeds #streams (%u)", stream_id, trainer._n_streams);
					return false;
				}
				int n_select;
				if (!item->Attribute ("size", &n_select)) {
					ssi_wrn ("attribute <size> missing in <item>");
					return false;
				}				
				const ssi_char_t *select = item->Attribute ("select");
				if (!select) {
					ssi_wrn ("attribute <select> missing in <item>");
					return false;
				}

				trainer._n_stream_select[stream_id] = n_select;
				trainer._stream_select[stream_id] = new ssi_size_t[n_select];
				ssi_string2array (trainer._n_stream_select[stream_id], trainer._stream_select[stream_id], select, ' ');

			} while (item = item->NextSiblingElement ("item"));			
		}
	}

	if (element = body->FirstChildElement ("model")) {

		bool create_model = false;
		if (!trainer._models) {
			trainer._n_models = 1; 
			trainer._models = new IModel *[1];
			create_model = true;
		} else if (trainer._n_models != 1) {
			ssi_wrn ("expected %u models but found only one", trainer._n_models);
			return false;
		}
		const char *name = element->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <model>");
			return false;
		}
		int stream_index;
		if (element->QueryIntAttribute ("stream", &stream_index) != TIXML_SUCCESS) {
			ssi_wrn ("attribute <stream> in tag <model> missing");
			return false;	
		}
		trainer._stream_index = stream_index;
		if (create_model) {
			trainer._models[0] = ssi_pcast (IModel, Factory::Create (name, 0));
		} else if (strcmp (trainer._models[0]->getName (), name) != 0) {
			ssi_wrn ("expected model '%s' but found '%s'", trainer._models[0]->getName (), name);
			return false;
		}		
		const char *path = element->Attribute ("path");
		if (!path) {
			ssi_wrn ("attribute <path> missing in <model>");
			return false;
		}
		ssi_sprint (string, "%s%s", fp.getDir (), path); 
		if (!trainer._models[0]->load (string)) {
			ssi_wrn ("failed loading model from '%s'", string);
			return false;
		}

	} else if (element = body->FirstChildElement ("fusion")) {

		const char *name = element->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <fusion>");
			return false;
		}
		if (!trainer._fusion) {
			trainer._fusion = ssi_pcast (IFusion, Factory::Create (name, 0));		
		} else if (strcmp (trainer._fusion->getName (), name) != 0) {
			ssi_wrn ("expected fusion '%s' but found '%s'", trainer._fusion->getName (), name);
			return false;
		}
		const char *path = element->Attribute ("path");
		if (!path) {
			ssi_wrn ("attribute <path> missing in <fusion>");
			return false;
		}
		ssi_sprint (string, "%s%s", fp.getDir (), path); 
		if (!trainer._fusion->load (string)) {
			ssi_wrn ("failed loading fusion from '%s'", string);
			return false;
		}

		element = element->FirstChildElement ("models");
		if (!element || strcmp (element->Value (), "models") != 0) {
			ssi_wrn ("tag <models> missing in <fusion>");
			return false;
		}

		int n_models;
		if (element->QueryIntAttribute ("size", &n_models) != TIXML_SUCCESS) {
			ssi_wrn ("attribute <size> in tag <models> missing");
			return false;	
		}
		bool create_models = false;
		if (!trainer._models) {
			trainer._n_models = n_models; 
			trainer._models = new IModel *[n_models];
			create_models = true;
		} else if (n_models != trainer._n_models) {
			ssi_wrn ("expected %u models but found %u", trainer._n_models, n_models);			
			return false;
		}
		item = element->FirstChildElement ("item");
		for (int n_model = 0; n_model < n_models; n_model++) {

			const char *name = item->Attribute ("name");
			if (!name) {
				ssi_wrn ("attribute <name> missing in <models>");
				return false;
			}
			if (create_models) {
				trainer._models[n_model] = ssi_pcast (IModel, Factory::Create (name, 0));		
			} else if (strcmp (trainer._models[n_model]->getName (), name) != 0) {
				ssi_wrn ("expected model '%s' but found '%s'", trainer._models[n_model]->getName (), name);
				return false;
			}
			const char *path = item->Attribute ("path");
			if (!path) {
				ssi_wrn ("attribute <path> missing in <models>");
				return false;
			}

			ssi_sprint (string, "%s%s", fp.getDir (), path); 
			if (!trainer._models[n_model]->load (string)) {
				ssi_wrn ("failed loading model #%u from '%s'", n_model, string);
				return false;
			}

			item = item->NextSiblingElement("item");
		}

	} else {
		ssi_wrn ("tag <fusion> or <model> missing");
		return false;
	}

	return true;

	return false;
}

bool Trainer::Load_V1to3 (Trainer &trainer,
	FilePath &fp,
	TiXmlElement *body,
	VERSION version) {

	ssi_char_t string[SSI_MAX_CHAR];

	TiXmlElement *element = body->FirstChildElement ();
	if (!element || strcmp (element->Value (), "classes") != 0) {
		ssi_wrn ("tag <element> missing");
		return false;
	}

	int n_classes;
	if (element->QueryIntAttribute ("size", &n_classes) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <size> in tag <classes> missing");
		return false;	
	}
	TiXmlElement *item = element->FirstChildElement ();
	trainer._n_classes = n_classes; 
	trainer._class_names = new ssi_char_t *[n_classes];	  
	item = element->FirstChildElement ();
	for (int n_class = 0; n_class < n_classes; n_class++) {
		if (!item || strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("could not parse tag <item> in <classes>");
			return false;
		}
		const char *name = item->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <classes>");
			return false;
		}
		trainer._class_names[n_class] = ssi_strcpy (name);
		item = item->NextSiblingElement();
	}

	if (version > V1) {

		element = element->NextSiblingElement ();
		if (!element || strcmp (element->Value (), "users") != 0) {
			ssi_wrn ("tag <users> missing");
			return false;
		}

		int n_users;
		if (element->QueryIntAttribute ("size", &n_users) != TIXML_SUCCESS) {
			ssi_wrn ("attribute <size> in tag <users> missing");
			return false;	
		}
		TiXmlElement *item = element->FirstChildElement ();
		trainer._n_users = n_users; 
		trainer._user_names = new ssi_char_t *[n_users];	  
		item = element->FirstChildElement ();
		for (int n_user = 0; n_user < n_users; n_user++) {
			if (!item || strcmp (item->Value (), "item") != 0) {
				ssi_wrn ("could not parse tag <item> in <users>");
				return false;
			}
			const char *name = item->Attribute ("name");
			if (!name) {
				ssi_wrn ("attribute <name> missing in <users>");
				return false;
			}
			trainer._user_names[n_user] = ssi_strcpy (name);
			item = item->NextSiblingElement();
		}
	}

	element = element->NextSiblingElement ();
	if (!element || strcmp (element->Value (), "streams") != 0) {
		ssi_wrn ("tag <streams> missing");
		return false;
	}

	int n_streams;
	if (element->QueryIntAttribute ("size", &n_streams) != TIXML_SUCCESS) {
		ssi_wrn ("attribute <size> in tag <streams> missing");
		return false;	
	}
	if (version > Trainer::V2) {
		const ssi_char_t *select = element->Attribute ("select");
		if (!select) {
			ssi_wrn ("attribute <select> missing in <streams>");
			return false;
		}
		if (strcmp (select, "true") == 0) {
			trainer._has_selection = true;
			trainer._n_stream_select = new ssi_size_t[n_streams];
			trainer._stream_select = new ssi_size_t *[n_streams];
		}
	}
	item = element->FirstChildElement ();
	trainer._n_streams = n_streams; 
	trainer._stream_refs = new ssi_stream_t[n_streams];
	item = element->FirstChildElement ();
	for (int n_stream = 0; n_stream < n_streams; n_stream++) {
		if (!item || strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("could not parse tag <item> in <streams>");
			return false;
		}
		int byte = 0;
		if (!item->Attribute ("byte", &byte)) {
			ssi_wrn ("attribute <byte> missing in <item>");
			return false;
		}
		int dim = 0;
		if (!item->Attribute ("dim", &dim)) {
			ssi_wrn ("attribute <dim> missing in <item>");
			return false;
		}
		double sr = 0;
		if (!item->Attribute ("sr", &sr)) {
			ssi_wrn ("attribute <sr> missing in <item>");
			return false;
		}
		const char *type_name = item->Attribute ("type");
		if (!type_name) {
			ssi_wrn ("attribute <type> missing in <item>");
			return false;
		}
		ssi_type_t type = SSI_UNDEF;
		if (!ssi_name2type (type_name, type)) {
			ssi_wrn ("found unkown <type> in <item>");
			return false;
		}
		ssi_stream_init (trainer._stream_refs[n_stream], 0, dim, byte, type, sr);
		if (version > Trainer::V2 && trainer._has_selection) {
			const ssi_char_t *select = item->Attribute ("select");
			if (!select) {
				ssi_wrn ("attribute <select> missing in <item>");
				return false;
			}
			if (select[0] == '\0') {
				trainer._n_stream_select[n_stream] = 0;
				trainer._stream_select[n_stream] = 0;
			} else {
				trainer._n_stream_select[n_stream] = dim;
				trainer._stream_select[n_stream] = new ssi_size_t[dim];
				ssi_string2array (trainer._n_stream_select[n_stream], trainer._stream_select[n_stream], select, ' ');
			}
		}
		item = item->NextSiblingElement();
	}

	element = element->NextSiblingElement ();
	if (!element) {
		ssi_wrn ("tag <fusion> or <model> missing");
		return false;
	}

	if (strcmp (element->Value (), "fusion") == 0) {
	
		const char *name = element->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <fusion>");
			return false;
		}
		if (!trainer._fusion) {
			trainer._fusion = ssi_pcast (IFusion, Factory::Create (name, 0));		
		} else if (strcmp (trainer._fusion->getName (), name) != 0) {
			ssi_wrn ("expected fusion '%s' but found '%s'", trainer._fusion->getName (), name);
			return false;
		}
		const char *path = element->Attribute ("path");
		if (!path) {
			ssi_wrn ("attribute <path> missing in <fusion>");
			return false;
		}
		if (version > Trainer::V2) {
			ssi_sprint (string, "%s%s", fp.getDir (), path); 
			if (!trainer._fusion->load (string)) {
				ssi_wrn ("failed loading fusion from '%s'", string);
				return false;
			}
		} else {
			if (!trainer._fusion->load (path)) {
				ssi_wrn ("failed loading fusion from '%s'", path);
				return false;
			}
		}

		element = element->FirstChildElement ();
		if (!element || strcmp (element->Value (), "models") != 0) {
			ssi_wrn ("tag <models> missing in <fusion>");
			return false;
		}

		int n_models;
		if (element->QueryIntAttribute ("size", &n_models) != TIXML_SUCCESS) {
			ssi_wrn ("attribute <size> in tag <models> missing");
			return false;	
		}
		bool create_models = false;
		if (!trainer._models) {
			trainer._n_models = n_models; 
			trainer._models = new IModel *[n_models];
			create_models = true;
		} else if (n_models != trainer._n_models) {
			ssi_wrn ("expected %u models but found %u", trainer._n_models, n_models);			
			return false;
		}
		item = element->FirstChildElement ();
		for (int n_model = 0; n_model < n_models; n_model++) {

			const char *name = item->Attribute ("name");
			if (!name) {
				ssi_wrn ("attribute <name> missing in <models>");
				return false;
			}
			if (create_models) {
				trainer._models[n_model] = ssi_pcast (IModel, Factory::Create (name, 0));		
			} else if (strcmp (trainer._models[n_model]->getName (), name) != 0) {
				ssi_wrn ("expected model '%s' but found '%s'", trainer._models[n_model]->getName (), name);
				return false;
			}
			const char *path = item->Attribute ("path");
			if (!path) {
				ssi_wrn ("attribute <path> missing in <models>");
				return false;
			}
			if (version > Trainer::V2) {
				ssi_sprint (string, "%s%s", fp.getDir (), path); 
				if (!trainer._models[n_model]->load (string)) {
					ssi_wrn ("failed loading model #%u from '%s'", n_model, string);
					return false;
				}
			} else {
				if (!trainer._models[n_model]->load (path)) {
					ssi_wrn ("failed loading model #%u from '%s'", n_model, path);
					return false;
				}
			}

			item = item->NextSiblingElement();
		}

	} else if (strcmp (element->Value (), "model") == 0) {

		bool create_model = false;
		if (!trainer._models) {
			trainer._n_models = 1; 
			trainer._models = new IModel *[1];
			create_model = true;
		} else if (trainer._n_models != 1) {
			ssi_wrn ("expected %u models but found only one", trainer._n_models);
			return false;
		}
		const char *name = element->Attribute ("name");
		if (!name) {
			ssi_wrn ("attribute <name> missing in <model>");
			return false;
		}
		int stream_index;
		if (element->QueryIntAttribute ("stream", &stream_index) != TIXML_SUCCESS) {
			ssi_wrn ("attribute <stream> in tag <model> missing");
			return false;	
		}
		trainer._stream_index = stream_index;
		if (create_model) {
			trainer._models[0] = ssi_pcast (IModel, Factory::Create (name, 0));
		} else if (strcmp (trainer._models[0]->getName (), name) != 0) {
			ssi_wrn ("expected model '%s' but found '%s'", trainer._models[0]->getName (), name);
			return false;
		}		
		const char *path = element->Attribute ("path");
		if (!path) {
			ssi_wrn ("attribute <path> missing in <model>");
			return false;
		}
		if (version > Trainer::V2) {
			ssi_sprint (string, "%s%s", fp.getDir (), path); 
			if (!trainer._models[0]->load (string)) {
				ssi_wrn ("failed loading model from '%s'", string);
				return false;
			}
		} else {
			if (!trainer._models[0]->load (path)) {
				ssi_wrn ("failed loading model from '%s'", path);
				return false;
			}
		}

	} else {
		ssi_wrn ("tag <fusion> or <model> missing");
		return false;
	}

	return true;

};

bool Trainer::save (const ssi_char_t *filepath, VERSION version) {

	if (!_is_trained) {
		ssi_wrn ("not trained");
		return false;
	}

	ssi_char_t *filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_TRAINER);
	
	TiXmlDocument doc;

	TiXmlDeclaration head ("1.0", "", "");
	doc.InsertEndChild (head);
	
	TiXmlElement body ("trainer");		
	body.SetAttribute ("ssi-v", version);

	switch (version) {
		case V1:
		case V2:
		case V3:
			if (!save_V1to3 (filepath, body, version)) {
				ssi_wrn ("saving trainer to file '%s' failed", filepath_with_ext);	
				return false;
			}
			break;
		case V4:
			if (!save_V4 (filepath, body)) {
				ssi_wrn ("saving trainer to file '%s' failed", filepath_with_ext);	
				return false;
			}
			break;
		default:
			ssi_wrn ("unkown version %d", version);
			return false;
	}

	doc.InsertEndChild (body);
	
	if (!doc.SaveFile (filepath_with_ext)) {
		ssi_wrn ("saving trainer to file '%s' failed", filepath_with_ext);	
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_BASIC, "saved trainer to file '%s'", filepath_with_ext);
	delete[] filepath_with_ext;

	return true;
}

bool Trainer::save_V4 (const ssi_char_t *filepath, TiXmlElement &body) {
	
	ssi_char_t string[SSI_MAX_CHAR];

	TiXmlElement classes ("classes" );	
	classes.SetAttribute ("size", ssi_cast (int, _n_classes));
	for (ssi_size_t n_class = 0; n_class < _n_classes; n_class++) {
		TiXmlElement item ("item" );
		item.SetAttribute ("name", _class_names[n_class]);								
		item.SetAttribute ("id", ssi_cast (int, n_class));				
		classes.InsertEndChild (item);
	}
	body.InsertEndChild (classes);
	
	TiXmlElement users ("users" );	
	users.SetAttribute ("size", ssi_cast (int, _n_users));
	for (ssi_size_t n_user = 0; n_user < _n_users; n_user++) {
		TiXmlElement item ("item" );
		item.SetAttribute ("name", _user_names[n_user]);								
		item.SetAttribute ("id", ssi_cast (int, n_user));				
		users.InsertEndChild (item);
	}
	body.InsertEndChild (users);

	TiXmlElement streams ("streams" );	
	streams.SetAttribute ("size", ssi_cast (int, _n_streams));	
	for (ssi_size_t n_stream = 0; n_stream < _n_streams; n_stream++) {
		TiXmlElement item ("item" );
		item.SetAttribute ("byte", ssi_cast (int, _stream_refs[n_stream].byte));							
		item.SetAttribute ("dim", ssi_cast (int, _stream_refs[n_stream].dim));
		item.SetDoubleAttribute ("sr", _stream_refs[n_stream].sr);
		item.SetAttribute ("type", SSI_TYPE_NAMES[_stream_refs[n_stream].type]);		
		streams.InsertEndChild (item);
	}
	body.InsertEndChild (streams);

	if (_has_transformer) {
		TiXmlElement transform ("transform" );
		for (ssi_size_t n = 0; n < _n_streams; n++) {
			if (_transformer[n]) {
				TiXmlElement item ("item");
				item.SetAttribute ("stream", n); 
				item.SetAttribute ("name", _transformer[n]->getName ());							
				item.SetAttribute ("info", _transformer[n]->getInfo ());	
				if (_transformer[n]->getOptions ()) {
					ssi_sprint (string, "%s.%02u.%s%s", filepath, n, _transformer[n]->getName (), SSI_FILE_TYPE_OPTION);
					OptionList::SaveXML (string, *_transformer[n]->getOptions ());
					FilePath fp (filepath);
					ssi_sprint (string, "%s.%02u.%s%s", fp.getName (), n, _transformer[n]->getName (), SSI_FILE_TYPE_OPTION);
					item.SetAttribute ("option", string);					
				}
				item.SetDoubleAttribute ("frame", _transformer_frame[n]);
				item.SetDoubleAttribute ("delta", _transformer_delta[n]);
				transform.InsertEndChild (item);
			}
		}
		body.InsertEndChild (transform);
	}
	
	if (_has_selection) {
		TiXmlElement select ("select" );
		for (ssi_size_t n = 0; n < _n_streams; n++) {
			if (_stream_select[n]) {
				TiXmlElement item ("item" );				
				item.SetAttribute ("stream", ssi_cast (int, n)); 
				item.SetAttribute ("size", ssi_cast (int, _n_stream_select[n])); 
				ssi_array2string (_n_stream_select[n], _stream_select[n], SSI_MAX_CHAR, string, ' ');
				item.SetAttribute ("select", string);			
				select.InsertEndChild (item);
			}
		}
		body.InsertEndChild (select);

	}
	
	if (_fusion) {

		TiXmlElement fusion ("fusion" );
		fusion.SetAttribute ("name", _fusion->getName ());
		fusion.SetAttribute ("info", _fusion->getInfo ());

		ssi_sprint (string, "%s%s", filepath, SSI_FILE_TYPE_FUSION);				
		FilePath fp (string);			
		fusion.SetAttribute ("path", fp.getNameFull ());
		if (!_fusion->save (string)) {
			ssi_wrn ("failed saving fusion to '%s'", string);
			return false;
		}

		TiXmlElement models ("models" );
		models.SetAttribute ("size", ssi_cast (int, _n_models));	
		for (ssi_size_t n_model = 0; n_model < _n_models; n_model++) {
			TiXmlElement item ("item" );
			item.SetAttribute ("name", _models[n_model]->getName ());								
			item.SetAttribute ("info", _models[n_model]->getInfo ());
			ssi_sprint (string, "%s.%02d%s%s", filepath, n_model, _models[n_model]->getName (), SSI_FILE_TYPE_MODEL);				
			FilePath fp (string);
			item.SetAttribute ("path", fp.getNameFull ());			
			if (!_models[n_model]->save (string)) {
				ssi_wrn ("failed saving model #%u to '%s'", n_model, string);
				return false;
			}
			models.InsertEndChild (item);
		}
		fusion.InsertEndChild (models);

		body.InsertEndChild (fusion);

	} else {

		TiXmlElement model ("model" );		
		model.SetAttribute ("name", _models[0]->getName ());								
		model.SetAttribute ("info", _models[0]->getInfo ());
		model.SetAttribute ("stream", _stream_index);
		ssi_sprint (string, "%s.%s%s", filepath, _models[0]->getName (), SSI_FILE_TYPE_MODEL);				
		FilePath fp (string);
		model.SetAttribute ("path", fp.getNameFull ());		
		if (!_models[0]->save (string)) {
			ssi_wrn ("failed saving model '%s'", string);
			return false;
		}
		body.InsertEndChild (model);
	}

	return true;
}

bool Trainer::save_V1to3 (const ssi_char_t *filepath, TiXmlElement &body, VERSION version) {	
	
	ssi_char_t string[SSI_MAX_CHAR];

	TiXmlElement classes ("classes" );
	classes.SetAttribute ("size", ssi_cast (int, _n_classes));
	for (ssi_size_t n_class = 0; n_class < _n_classes; n_class++) {
		TiXmlElement item ("item" );
		item.SetAttribute ("name", _class_names[n_class]);								
		item.SetAttribute ("id", ssi_cast (int, n_class));				
		classes.InsertEndChild (item);
	}
	body.InsertEndChild (classes);

	if (version > Trainer::V1) {
		TiXmlElement users ("users" );
		users.SetAttribute ("size", ssi_cast (int, _n_users));
		for (ssi_size_t n_user = 0; n_user < _n_users; n_user++) {
			TiXmlElement item ("item" );
			item.SetAttribute ("name", _user_names[n_user]);								
			item.SetAttribute ("id", ssi_cast (int, n_user));				
			users.InsertEndChild (item);
		}
		body.InsertEndChild (users);
	}

	TiXmlElement streams ("streams" );
	streams.SetAttribute ("size", ssi_cast (int, _n_streams));	
	if (version > Trainer::V2) {		
		streams.SetAttribute ("select", _has_selection ? "true" : "false");	
	}
	for (ssi_size_t n_stream = 0; n_stream < _n_streams; n_stream++) {
		TiXmlElement item ("item" );
		item.SetAttribute ("byte", ssi_cast (int, _stream_refs[n_stream].byte));							
		item.SetAttribute ("dim", ssi_cast (int, _stream_refs[n_stream].dim));
		item.SetDoubleAttribute ("sr", _stream_refs[n_stream].sr);
		item.SetAttribute ("type", SSI_TYPE_NAMES[_stream_refs[n_stream].type]);
		if (version > Trainer::V2 && _has_selection) {				
			ssi_array2string (_n_stream_select[n_stream], _stream_select[n_stream], SSI_MAX_CHAR, string, ' ');
			item.SetAttribute ("select", string);			
		}
		streams.InsertEndChild (item);
	}
	body.InsertEndChild (streams);
	
	if (_fusion) {
		TiXmlElement fusion ("fusion" );
		fusion.SetAttribute ("name", _fusion->getName ());
		fusion.SetAttribute ("info", _fusion->getInfo ());

		ssi_sprint (string, "%s%s", filepath, SSI_FILE_TYPE_FUSION);		
		if (version > Trainer::V2) {
			FilePath fp (string);			
			fusion.SetAttribute ("path", fp.getNameFull ());
		} else {
			fusion.SetAttribute ("path", string);
		}
		if (!_fusion->save (string)) {
			ssi_wrn ("failed saving fusion to '%s'", string);
			return false;
		}

		TiXmlElement models ("models" );
		models.SetAttribute ("size", ssi_cast (int, _n_models));	
		for (ssi_size_t n_model = 0; n_model < _n_models; n_model++) {
			TiXmlElement item ("item" );
			item.SetAttribute ("name", _models[n_model]->getName ());								
			item.SetAttribute ("info", _models[n_model]->getInfo ());
			ssi_sprint (string, "%s.%02d%s", filepath, n_model, SSI_FILE_TYPE_MODEL);	
			if (version > Trainer::V2) {
				FilePath fp (string);
				item.SetAttribute ("path", fp.getNameFull ());
			} else {
				item.SetAttribute ("path", string);
			}
			if (!_models[n_model]->save (string)) {
				ssi_wrn ("failed saving model #%u to '%s'", n_model, string);
				return false;
			}
			models.InsertEndChild (item);
		}
		fusion.InsertEndChild (models);

		body.InsertEndChild (fusion);

	} else {

		TiXmlElement model ("model" );		
		model.SetAttribute ("name", _models[0]->getName ());								
		model.SetAttribute ("info", _models[0]->getInfo ());
		model.SetAttribute ("stream", _stream_index);
		ssi_sprint (string, "%s%s", filepath, SSI_FILE_TYPE_MODEL);		
		if (version > Trainer::V2) {
			FilePath fp (string);
			model.SetAttribute ("path", fp.getNameFull ());
		} else {
			model.SetAttribute ("path", string);
		}
		if (!_models[0]->save (string)) {
			ssi_wrn ("failed saving model '%s'", string);
			return false;
		}
		body.InsertEndChild (model);
	}

	return true;

};

void Trainer::release () {
	
	release_transformer ();
	release_selection ();
	if (_fusion) {
		_fusion->release ();		
	}
	for (ssi_size_t n_model = 0; n_model < _n_models; n_model++) {
		_models[n_model]->release ();
	}
	free_class_names ();
	free_user_names ();
	_n_streams = 0;
	_is_trained = false;
	_n_streams = 0;
	delete[] _stream_refs;
	_stream_refs = 0;
}

void Trainer::release_transformer () {
	
	if (_has_transformer) {
		delete[] _transformer;
		_transformer = 0;
		delete[] _transformer_frame;
		_transformer_frame = 0;
		delete[] _transformer_delta;
		_transformer_delta = 0;
	}
	_has_transformer = false;
}

void Trainer::release_selection () {

	if (_has_selection) {
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			delete[] _stream_select[i];
		}
		delete[] _stream_select;
		delete[] _n_stream_select;
	}
	_has_selection = false;
}

bool Trainer::cluster (ISamples &samples) {

	if (!_is_trained) {
		ssi_wrn ("not trained");
		return false;
	}

	if (samples.getClassSize () != _models[0]->getClassSize ()) {
		ssi_wrn ("#classes differs from #cluster");
		return false;
	}

	ssi_sample_t *sample;
	samples.reset ();
	ssi_size_t index;
	while (sample = samples.next ()) {
		forward (sample->num, sample->streams, index);
		sample->class_id = index;
	}

	return true;
}

IModel *Trainer::getModel (ssi_size_t index) {

	if (index < _n_models) {
		return _models[index];
	}
	ssi_wrn ("invalid index");
	return 0;
}

const ssi_char_t *Trainer::getName () {

	if (_fusion) {
		return _fusion->getName ();
	} else {
		return _models[0]->getName ();
	}
}

const ssi_char_t *Trainer::getInfo () {

	if (_fusion) {
		return _fusion->getInfo ();
	} else {
		return _models[0]->getInfo ();
	}
}

ssi_size_t Trainer::getMetaSize () { 

	if (_fusion) {
		return _fusion->getMetaSize ();
	} else {
		return _models[0]->getMetaSize ();
	}
};

bool Trainer::getMetaData (ssi_size_t n_metas,
	ssi_real_t *metas) { 

	if (_fusion) {
		return _fusion->getMetaData (n_metas, metas);
	} else {
		return _models[0]->getMetaData (n_metas, metas);
	}
};

}
