// Multiply.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/01/03
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

#include "signal/Multiply.h"
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
Multiply::Multiply (const ssi_char_t *file) 
	: _factors (0),
	_n_factors (0),
	_factor (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Multiply::~Multiply () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}


void Multiply::listen_enter () {
	_factor_id = Factory::AddString (_options.factorid);
}

bool Multiply::update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {

	bool retval = false;

	if (n_new_events > 0) {
		ssi_event_t *e = events.next ();
		switch (e->type) {
			case SSI_ETYPE_NTUPLE:
				ssi_event_tuple_t *t = ssi_pcast (ssi_event_tuple_t, e->ptr);
				ssi_size_t n_t = e->tot / sizeof (ssi_event_tuple_t);
				for (ssi_size_t i = 0; i < n_t; i++) {
					if (t[i].id == _factor_id) {
						ssi_real_t value = t[i].value;
						{
							Lock lock (_factor_mutex);
							_factor = value;
							for (ssi_size_t i = 0; i < _n_factors; i++) {
								_factors[i] = _factor;
							}
						}
						retval = true;
					}
				}
				break;
		}
	}

	return retval;
}

void Multiply::listen_flush () {
}


void Multiply::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {
	
	_factor = _options.factor;
	_n_factors = stream_in.dim;
	_factors = new ssi_real_t[_n_factors];

	if (_options.single) {
		for (ssi_size_t i = 0; i < _n_factors; i++) {
			_factors[i] = _factor;
		}
	} else {
		ssi_size_t found = ssi_string2array_count (_options.factors, ',');
		if (found < stream_in.dim) {
			ssi_err ("#factors (%u) < #dimensions (%u)", found, stream_in.dim);
		}
		ssi_real_t *tmp = new ssi_real_t[found];
		ssi_string2array (found, tmp, _options.factors, ',');
		for (ssi_size_t i = 0; i < stream_in.dim; i++) {
			_factors[i] = tmp[i];
		}
		delete[] tmp;
	}
	_join = _options.join;
}

void Multiply::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {	

	Lock lock (_factor_mutex);

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);

	ssi_real_t result;
	switch (_join) {
		case JOIN::OFF:		
			for (ssi_size_t i = 0; i < stream_in.num; i++) {
				for (ssi_size_t j = 0; j < stream_in.dim; j++) {
					*dstptr++ = _factors[j] * *srcptr++;
				}	
			}
			break;
		case JOIN::MULT:			
			for (ssi_size_t i = 0; i < stream_in.num; i++) {	
				result = 1.0f;
				for (ssi_size_t j = 0; j < stream_in.dim; j++) {	
					result *= _factors[j] * *srcptr++;
				}
				*dstptr++ = result;
			}			
			break;
		case JOIN::SUM:
			for (ssi_size_t i = 0; i < stream_in.num; i++) {	
				result = 0.0f;
				for (ssi_size_t j = 0; j < stream_in.dim; j++) {	
					result += _factors[j] * *srcptr++;
				}
				*dstptr++ = result;
			}	
			break;
		case JOIN::SUMSQUARE:
			result = 0.0f;
			ssi_real_t tmp = 0.0f;
			for (ssi_size_t i = 0; i < stream_in.num; i++) {	
				result = 0.0f;
				for (ssi_size_t j = 0; j < stream_in.dim; j++) {		
					tmp = _factors[j] * *srcptr++;
					result += tmp * tmp;
				}
				*dstptr++ = result;
			}
			break;
	}
}

void Multiply::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {

	delete[] _factors; _factors = 0;
}

}
