// Gate.cpp
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

#include "signal/Gate.h"
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

Gate::Gate (const ssi_char_t *file)
	: _replace (0),
	_thres (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Gate::~Gate () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}


void Gate::listen_enter () {
	_thres_id = Factory::AddString (_options.thresid);
}

bool Gate::update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {

	bool retval = false;

	if (n_new_events > 0) {
		ssi_event_t *e = events.next ();
		switch (e->type) {
			case SSI_ETYPE_NTUPLE:
				ssi_event_tuple_t *t = ssi_pcast (ssi_event_tuple_t, e->ptr);
				ssi_size_t n_t = e->tot / sizeof (ssi_event_tuple_t);
				for (ssi_size_t i = 0; i < n_t; i++) {
					if (t[i].id == _thres_id) {
						ssi_real_t value = t[i].value;
						{
							Lock lock (_thres_mutex);
							_thres = value;
						}
						retval = true;
					}
				}
				break;
		}
	}

	return retval;
}

void Gate::listen_flush () {
}

void Gate::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	_thres = _options.thres;
	_replace = _options.replace;
}

void Gate::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	Lock lock (_thres_mutex);

	// do normalization
	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = info.frame_num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);
	
	if (_options.upper) {
		for (ssi_size_t i = 0; i < sample_number * sample_dimension; i++) {
			ssi_real_t value = *srcptr++;
			if (value >= _thres) {
				*dstptr++ = _replace;
			} else {
				*dstptr++ = value;
			}
		}
	} else {
		for (ssi_size_t i = 0; i < sample_number * sample_dimension; i++) {
			ssi_real_t value = *srcptr++;
			if (value <= _thres) {
				*dstptr++ = _replace;
			} else {
				*dstptr++ = value;
			}
		}
	}
}

void Gate::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {
}


}
