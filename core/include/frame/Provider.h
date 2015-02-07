// Provider.h
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
//Changes:
//added Interface ITransformable


#ifndef SSI_FRAME_PROVIDER_H
#define SSI_FRAME_PROVIDER_H

#include "frame/FrameLibCons.h"
#include "base/IProvider.h"
#include "base/IFilter.h"
#include "base/ITransformable.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class TheFramework;

class Provider : public Thread, public IProvider, public ITransformable {

	friend class TheFramework;

	public:

		const void *getMetaData (ssi_size_t &size) { size = _meta_size; return _meta; };	
		void setMetaData (ssi_size_t size, const void *meta);

		int getBufferId () {
			SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);
			return _buffer_id;
		};

		ssi_time_t getSampleRate () {
			SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);
			return _sample_rate;
		}

		ssi_size_t getSampleDimension () {
			SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);
			return _sample_dimension;
		}

		ssi_size_t getSampleBytes () {
			SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);
			return _sample_bytes;
		}

		ssi_type_t getSampleType () {
			SSI_ASSERT (_buffer_id != THEFRAMEWORK_ERROR);
			return _sample_type;
		}

		static void SetLogLevel (int level) {
			ssi_log_level = level;
		}
		
	protected:

		Provider (IFilter *filter = 0,
			ssi_time_t buffer_capacity_in_seconds = THEFRAMEWORK_DEFAULT_BUFFER_CAP,
			ssi_time_t check_interval_in_seconds = 1.0,
			ssi_time_t sync_interval_in_seconds = 5.0);
		~Provider ();

		IOptions *getOptions () { return 0; };
		const ssi_char_t *getName () { return "ssi_provider_frame"; };
		const ssi_char_t *getInfo () { return "provides stream to the framework"; };

		static char *ssi_log_name;
		static int ssi_log_level;	

		void init (IChannel *channel);

		bool provide (ssi_byte_t *data, 
			ssi_size_t sample_number);

		void enter ();
		void run ();
		void flush ();

		ssi_size_t _meta_size;
		ssi_byte_t *_meta;

		int _buffer_id;
		ssi_time_t _sample_rate;
		ssi_size_t _sample_dimension;
		ssi_size_t _sample_bytes;
		ssi_type_t _sample_type;
		ssi_time_t _buffer_capacity;

		double _time;
		unsigned int _run_sleep; // sleep time in milliseconds between two calls to run ()

		bool _do_watch;
		bool _watch; // stores if new data has been provided since last check
		Mutex _watch_mutex; // allows thread safe access to check flag
		unsigned int _watch_iter; // total number of run iteration until next check
		unsigned int _watch_iter_counter; // number of run iterations left until next check
		bool _is_providing_zeros;

		bool _do_sync;
		unsigned int _sync_iter;	// total number of run iteration until next sync
		unsigned int _sync_iter_counter; // number of run iterations left until next sync

		IFilter *_filter;
		ssi_stream_t _stream;
		ssi_stream_t _filter_stream;

		TheFramework *_frame;
};

}

#endif
