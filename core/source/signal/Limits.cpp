// Limits.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2013/06/06
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

#include "signal/Limits.h"
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

Limits::Limits (const ssi_char_t *file)
	: _min (0),
	_max (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Limits::~Limits () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}


void Limits::listen_enter () {
	_min_id = Factory::AddString (_options.minid);
	_max_id = Factory::AddString (_options.maxid);
}

bool Limits::update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {

	bool retval = false;

	if (n_new_events > 0) {
		ssi_event_t *e = events.next ();
		switch (e->type) {
			case SSI_ETYPE_NTUPLE:
				ssi_event_tuple_t *t = ssi_pcast (ssi_event_tuple_t, e->ptr);
				ssi_size_t n_t = e->tot / sizeof (ssi_event_tuple_t);
				for (ssi_size_t i = 0; i < n_t; i++) {
					if (t[i].id == _min_id) {
						ssi_real_t value = t[i].value;
						{
							Lock lock (_minmax_mutex);
							_min = value;
						}
						retval = true;
					} else if (t[i].id == _max_id) {
						ssi_real_t value = t[i].value;
						{
							Lock lock (_minmax_mutex);
							_max = value;
						}
						retval = true;
					}
				}
				break;
		}
	}

	return retval;
}

void Limits::listen_flush () {
}

void Limits::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	_min = _options.minval;
	_max = _options.maxval;
}

void Limits::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	Lock lock (_minmax_mutex);

	// do normalization
	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = info.frame_num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);

	for (ssi_size_t i = 0; i < sample_number * sample_dimension; i++) {
		ssi_real_t value = *srcptr++;
		if (value > _max) {
			*dstptr++ = _max;
		} else if (value < _min) {
			*dstptr++ = _min;
		} else {
			*dstptr++ = value;
		}
	}

}

void Limits::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {
}


}
