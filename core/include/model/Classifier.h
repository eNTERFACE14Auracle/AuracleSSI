// Classifier.h
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

#pragma once

#ifndef SSI_MODEL_CLASSIFIER_H
#define SSI_MODEL_CLASSIFIER_H

#include "base/IConsumer.h"
#include "signal/SignalCons.h"
#include "model/Trainer.h"
#include "ioput/file/File.h"
#include "ioput/socket/SocketOsc.h"
#include "ioput/option/OptionList.h"
#include "base/IEvents.h"
#include "event/EventAddress.h"

#define SSI_CLASSIFIER_MAXHANDLER 5

namespace ssi {

class Classifier : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options () 
			: console (true), merge (false), socket (false), osc (false), full (false), port (IpEndpointName::ANY_PORT), type (Socket::UDP), pthres (0) {

			trainer[0] = '\0';
			file[0] = '\0';
			host[0] = '\0';
			oscid[0] = '\0';
			ename[0] = '\0';
			sname[0] = '\0';
			addOption ("trainer", trainer, SSI_MAX_CHAR, SSI_CHAR, "filepath of trainer");			
			addOption ("pthres", &pthres, 1, SSI_REAL, "probablity threshold");			
			addOption ("merge", &merge, 1, SSI_BOOL, "in case of multiple streams merge to single stream");	
			addOption ("console", &console, 1, SSI_BOOL, "output classification to console");
			addOption ("file", file, SSI_MAX_CHAR, SSI_CHAR, "output classification to file");
			addOption ("socket", &socket, 1, SSI_BOOL, "output classification to socket");
			addOption ("host", host, SSI_MAX_CHAR, SSI_CHAR, "socket host");
			addOption ("port", &port, 1, SSI_INT, "socket port");
			addOption ("type", &type, 1, SSI_INT, "socket protocol (0=UDP,1=TCP)");
			addOption ("osc", &osc, 1, SSI_BOOL, "use osc protocol");
			addOption ("oscid", oscid, SSI_MAX_CHAR, SSI_CHAR, "osc id name");
			addOption ("full", &full, 1, SSI_BOOL, "send full probabilities (osc only)");
			addOption ("ename", ename, SSI_MAX_CHAR, SSI_CHAR, "event name");
			addOption ("sname", sname, SSI_MAX_CHAR, SSI_CHAR, "sender name");
		};

		void setTrainer (const ssi_char_t *filepath) {
			ssi_strcpy (trainer, filepath);
		}
		void setFile (const ssi_char_t *filepath) {
			ssi_strcpy (file, filepath);
		}
		void setHost (const ssi_char_t *host_) {
			ssi_strcpy (host, host_);			
		}
		void setOscId (const ssi_char_t *oscid_) {
			ssi_strcpy (oscid, oscid_);			
		}
		void setEventName (const ssi_char_t *ename) {
			ssi_strcpy (this->ename, ename);			
		}
		void setSenderName (const ssi_char_t *sname) {
			ssi_strcpy (this->sname, sname);			
		}
		
		ssi_char_t trainer[SSI_MAX_CHAR];	
		ssi_real_t pthres;
		bool merge;
		bool console;
		ssi_char_t file[SSI_MAX_CHAR];
		bool socket;
		ssi_char_t host[SSI_MAX_CHAR];
		int port;
		Socket::TYPE type;
		bool osc;
		ssi_char_t oscid[SSI_MAX_CHAR];
		bool full;
		ssi_char_t ename[SSI_MAX_CHAR];
		ssi_char_t sname[SSI_MAX_CHAR];
	};

public:

	// _handler interface

	class Handler {
	
	public:
		virtual ~Handler () {};
		virtual void handle (ssi_time_t time,
			ssi_time_t duration,
			ssi_size_t n_classes,
			ssi_size_t class_index,
			const ssi_real_t *probs,
			ssi_char_t *const*class_names,
			ssi_size_t n_metas,
			ssi_real_t *metas) = 0;
	};

public:

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_Classifier"; };
	static IObject *Create (const ssi_char_t *file) { return new Classifier (file); };
	~Classifier ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Applies classifier to a stream and outputs result as an event."; };

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

	bool setEventListener (IEventListener *listener);
	const ssi_char_t *getEventAddress () {
		return _event_address.getAddress ();
	}

	void setTrainer (Trainer &trainer) { _trainer = &trainer; };
	void wait () {
		fflush (stdin);
		printf ("Press enter to stop!\n");
		getchar ();
	};
		
	void setLogLevel (int level) {
		ssi_log_level = level;
	}

protected:

	Classifier (const ssi_char_t *file = 0);
	Classifier::Options _options;
	ssi_char_t *_file;

	EventAddress _event_address;

	int ssi_log_level;
	static ssi_char_t ssi_log_name[];

	Trainer *_trainer;
	ssi_size_t _n_classes;
	ssi_real_t *_probs;

	ssi_size_t _n_metas;
	ssi_real_t *_metas;

	ssi_size_t _merged_sample_dimension;
	
	ssi_size_t _n_handler;
	Handler *_handler[SSI_CLASSIFIER_MAXHANDLER];
	void addHandler (Handler *handler) { 
		if (_n_handler < SSI_CLASSIFIER_MAXHANDLER) {
			_handler[_n_handler++] = handler;
		} else { 
			ssi_wrn ("could not add handler"); 
		};
	};
	void releaseHandler () {
		for (ssi_size_t i = 0; i < _n_handler; i++) {
			delete _handler[i];
			_handler[i] = 0;
		}
		_n_handler = 0;
	}

	ssi_time_t _consumer_sr;
	ssi_size_t _consumer_byte;
	ssi_size_t _consumer_dim;;
	ssi_size_t _consumer_num;

public:

	class FileHandler : public Classifier::Handler {
	
	public:

		FileHandler (const ssi_char_t *filepath = 0) ;			
		virtual ~FileHandler ();
		void handle (ssi_time_t time,
			ssi_time_t duration,
			ssi_size_t n_classes,
			ssi_size_t class_index,
			const ssi_real_t *probs,
			ssi_char_t *const*class_names,
			ssi_size_t n_metas,
			ssi_real_t *metas);

	protected:

		FILE *_file;
	};

	class SocketHandler : public Classifier::Handler {
	
	public:

		SocketHandler (const ssi_char_t *host,
			int port,
			Socket::TYPE type,
			bool use_osc,
			const ssi_char_t *oscid,
			bool full);		
		virtual ~SocketHandler ();
		void handle (ssi_time_t time,
			ssi_time_t duration,
			ssi_size_t n_classes,
			ssi_size_t class_index,
			const ssi_real_t *probs,
			ssi_char_t *const*class_names,
			ssi_size_t n_metas,
			ssi_real_t *metas);

	protected:

		bool _use_osc;		
		ssi_char_t *_oscid;
		SocketOsc *_socket_osc;
		Socket *_socket;
		bool _full;
	};

	class EventHandler : public Classifier::Handler {
	
	public:

		EventHandler (IEventListener *listener,
			const ssi_char_t *sname,
			const ssi_char_t *ename);		
		virtual ~EventHandler ();
		void handle (ssi_time_t time,
			ssi_time_t duration,
			ssi_size_t n_classes,
			ssi_size_t class_index,
			const ssi_real_t *probs,
			ssi_char_t *const*class_names,
			ssi_size_t n_metas,
			ssi_real_t *metas);

	protected:
			
			ssi_event_t _event;
			IEventListener *_listener;
	};
};

}

#endif


