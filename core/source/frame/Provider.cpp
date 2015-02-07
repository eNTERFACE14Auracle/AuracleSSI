// Provider.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/01/16
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

#include "frame/Provider.h"
#include "frame/TheFramework.h"
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

int Provider::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;
ssi_char_t *Provider::ssi_log_name = "provider__";

Provider::Provider (IFilter *filter,
	ssi_time_t buffer_capacity,
	ssi_time_t check_interval,
	ssi_time_t sync_interval) :
	_filter (filter),
	_run_sleep (0),
	_watch_iter (0),
	_do_watch (true),
	_is_providing_zeros (false),
	_sync_iter (0),
	_do_sync (true),
	_buffer_id (THEFRAMEWORK_ERROR),
	_buffer_capacity (buffer_capacity),
	_sample_rate (0),
	_sample_dimension (0),
	_sample_bytes (0),
	_time (0),
	_meta_size (0),
	_meta (0),
	_frame (0) {

	_frame = ssi_pcast (TheFramework, Factory::GetFramework ());

	unsigned int watch_sleep = ssi_cast (unsigned int, check_interval * 1000.0 + 0.5); 
	unsigned int sync_sleep = ssi_cast (unsigned int, sync_interval * 1000.0 + 0.5); 

	_do_sync = sync_sleep > 0;
	_do_watch = watch_sleep > 0;

	if (_do_sync && _do_watch) {
		_run_sleep = min (watch_sleep, sync_sleep);
	} else if (_do_sync) {
		_run_sleep = sync_sleep;
	} else if (_do_watch) {
		_run_sleep = watch_sleep;
	} else {
		_run_sleep = 0;
	}

	if (_run_sleep > 0) {
		_sync_iter = sync_sleep / _run_sleep - 1;
		_watch_iter = watch_sleep / _run_sleep - 1;
	}

}

Provider::~Provider () {

	if (_filter) {
		ssi_stream_destroy (_filter_stream);
	}
	delete[] _meta;
}

void Provider::setMetaData (ssi_size_t size, const void *meta) {
	
	if (size > 0 && meta != 0) {
		_meta_size = size;
		_meta = new ssi_byte_t[_meta_size];
		memcpy (_meta, meta, _meta_size);
	}
}

void Provider::init (IChannel *channel) {

	// set thread name
	ssi_char_t *thread_name = ssi_strcat ("provide@", channel->getName ());
	Thread::setName (thread_name);
	delete[] thread_name;

	ssi_stream_t stream = channel->getStream ();
	ssi_size_t sample_dimension = stream.dim;
	ssi_size_t sample_bytes = stream.byte;
	ssi_type_t sample_type = stream.type;
	ssi_time_t sample_rate = stream.sr;

	if (_filter) {
		ssi_stream_init (_stream, 0, sample_dimension, sample_bytes, sample_type, sample_rate);
		sample_dimension = _filter->getSampleDimensionOut (sample_dimension);
		sample_bytes = _filter->getSampleBytesOut (sample_bytes);
		sample_type = _filter->getSampleTypeOut (sample_type);
		ssi_stream_init (_filter_stream, 0, sample_dimension, sample_bytes, sample_type, sample_rate);		
		_filter->transform_enter (_stream, _filter_stream);
	}

	_buffer_id = _frame->AddBuffer (sample_rate,
		sample_dimension,
		sample_bytes,
		sample_type,
		_buffer_capacity);
	if (_meta) {
		_frame->SetMetaData (_buffer_id, _meta_size, _meta);
	}

	SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);

	_sample_rate = sample_rate;
	_sample_dimension = sample_dimension;
	_sample_bytes = sample_bytes;
	_sample_type = sample_type;
	_time = 0;

	ssi_msg (SSI_LOG_LEVEL_BASIC, "init '%s@%s'", getName (), channel->getName ());
	if ( ssi_log_level >= SSI_LOG_LEVEL_BASIC) {
		ssi_print ("\
             id\t\t= %d\n\
             rate\t= %.2lf\n\
             dim\t= %u\n\
             bytes\t= %u\n\
             type\t= %s\n", 
			_buffer_id,
			_sample_rate, 
			_sample_dimension, 
			_sample_bytes,
			SSI_TYPE_NAMES[_sample_type]);
	}

	// add consumer to framework
	if (_run_sleep > 0) {
		if (_frame->IsAutoRun ()) {
			_frame->AddRunnable (this);
		}
	}
}

bool Provider::provide (ssi_byte_t *data, 
	ssi_size_t sample_number) {

	if (_frame->IsInIdleMode ()) {
		sleep_ms (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
		return false;
	}

	{
		Lock lock (_watch_mutex);

		// push data into framework		
		if (_filter) {
			_stream.num = _stream.num_real = sample_number;
			_stream.tot = _stream.tot_real = sample_number * _stream.byte * _stream.dim;
			_stream.ptr = data;			
			ssi_stream_adjust (_filter_stream, _stream.num);
			ITransformer::info tinfo;
			tinfo.delta_num = 0;
			tinfo.frame_num = sample_number;
			tinfo.time = _time += sample_number / _sample_rate;
			_filter->transform (tinfo, _stream, _filter_stream);
			_frame->PushData (_buffer_id, _filter_stream.ptr, _filter_stream.num);
		} else {
			_frame->PushData (_buffer_id, data, sample_number);
		}

		// signal that new data has been provided
		_watch = true;
		if (_is_providing_zeros) {
			ssi_msg (SSI_LOG_LEVEL_WARNING, "watch check succeeded, no longer providing zeros");
			_is_providing_zeros = false;
		}

		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "provided data");
	}

	return true;
}

void Provider::enter () {

	ssi_msg (SSI_LOG_LEVEL_BASIC, "start");

	SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);

	_watch = false;
	_watch_iter_counter = _watch_iter;
	_sync_iter_counter = _sync_iter;
}

void Provider::run () {

	// sleep a bit before next check
	sleep_ms (_run_sleep);

	if (_frame->IsInIdleMode ()) {
		sleep_ms (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
		return;
	}

	// _watch routine
	if (_do_watch) {
		if (_watch_iter_counter == 0) {

			Lock lock (_watch_mutex);

			// check if new data has been provided
			// if false, provide zeros
			if (!_watch) {
				_frame->PushZeros (_buffer_id);
				if (!_is_providing_zeros) {
					ssi_wrn ("watch check failed, now providing zeros");
					_is_providing_zeros = true;
				}
			}
			// set watch flag false
			_watch = false;
			// reset counter
			_watch_iter_counter = _watch_iter;
		} else {
			_watch_iter_counter--;
		}
	}

	// sync routine
	if (_do_sync) {
		if (_sync_iter_counter == 0) {
			// synchronize buffer
			_frame->Synchronize (_buffer_id);
			// reset counter
			_sync_iter_counter = _sync_iter;

			SSI_DBG (SSI_LOG_LEVEL_DEBUG, "synchronized buffer");

		} else {
			_sync_iter_counter--;
		}
	}

	SSI_DBG (SSI_LOG_LEVEL_DEBUG, "provided data");
}

void Provider::flush () {

	ssi_msg (SSI_LOG_LEVEL_BASIC, "stop");

	if (_filter) {
		_filter->transform_flush (_stream, _filter_stream);
	}
}

}
