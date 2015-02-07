// Butfilt.cpp
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

#include "signal/Butfilt.h"
#include "signal/FilterTools.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *Butfilt::ssi_log_name = "butfilt___";

Butfilt::Butfilt (const ssi_char_t *file)
	: _iir (0),
	_coefs (0),
	_first_sample (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Matrix<ssi_real_t> *Butfilt::init_coefs (TYPE type, ssi_size_t order, ssi_time_t low, ssi_time_t high) {

	delete _coefs; 	_coefs = 0;

	switch (type) {
		case LOW:
			_coefs = FilterTools::LPButter (order, low);
			break;
		case HIGH:
			_coefs = FilterTools::HPButter (order, high);
			break;
		case BAND:
			_coefs = FilterTools::BPButter (order, low, high);
			break;
	}

	return _coefs;
}

Butfilt::~Butfilt () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

const Matrix<ssi_real_t> * Butfilt::getCoefs (ssi_time_t sr) { 

	double low = _options.norm ? _options.low : 2 * _options.low / sr;
	double high = _options.norm ? _options.high : 2 * _options.high / sr;
	_coefs = init_coefs (_options.type, _options.order, low, high);
	
	return _coefs; 
};

void Butfilt::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	getCoefs (stream_in.sr);

	_iir = ssi_pcast (IIR, IIR::Create (0));
	_iir->setCoefs (_coefs);
	_iir->transform_enter (stream_in, stream_out, xtra_stream_in_num, xtra_stream_in);

	_first_call = true;
}

void Butfilt::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	if (_first_call) {
		if (_options.zero) {
			_first_sample = new ssi_real_t[stream_in.dim];
			memcpy (_first_sample, stream_in.ptr, stream_in.dim * stream_in.byte);
		}
		_first_call = false;
	}

	if (_first_sample) {		
		ssi_real_t *ptr = ssi_pcast (ssi_real_t, stream_in.ptr);		
		ssi_real_t *sub = _first_sample;
		for (ssi_size_t i = 0; i < info.frame_num; i++) {
			for (ssi_size_t j = 0; j < stream_in.dim; j++) {
				*ptr++ -= *sub++;
			}
			sub = _first_sample;
		}
	}

	_iir->transform (info, stream_in, stream_out, xtra_stream_in_num, xtra_stream_in);

	if (_first_sample) {		
		ssi_real_t *ptr = ssi_pcast (ssi_real_t, stream_out.ptr);		
		ssi_real_t *sub = _first_sample;
		for (ssi_size_t i = 0; i < info.frame_num; i++) {
			for (ssi_size_t j = 0; j < stream_in.dim; j++) {
				*ptr++ += *sub++;
			}
			sub = _first_sample;
		}
	}

}

void Butfilt::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	_iir->transform_flush (stream_in, stream_out, xtra_stream_in_num, xtra_stream_in);
	delete _iir; _iir = 0;
	delete _coefs; _coefs = 0;
	delete[] _first_sample; _first_sample = 0;
}

}
