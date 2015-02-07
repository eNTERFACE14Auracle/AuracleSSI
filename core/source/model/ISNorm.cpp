// ISNorm.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/03/08
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

#include "model/ISNorm.h"
#include "model/ModelTools.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ISNorm::ISNorm (ISamples *samples)
	: _samples (*samples),	
	_as (0),
	_bs (0) {	

	ssi_sample_clone (*_samples.get (0), _sample);
	_n_streams = _samples.getStreamSize ();
	_n_features = new ssi_size_t[_n_streams];
	_n_samples = _samples.getSize ();
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		_n_features[i] = _samples.getStream (i).dim;
	}
	_as = new ssi_real_t *[_n_streams];	
	_bs = new ssi_real_t *[_n_streams];
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		_as[i] = new ssi_real_t[_n_features[i]];
		_bs[i] = new ssi_real_t[_n_features[i]];
	}

	release ();
}

ISNorm::~ISNorm () {

	release ();

	ssi_sample_destroy (_sample);
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		delete[] _as[i];
		delete[] _bs[i];
	}
	delete[] _as;
	delete[] _bs;
	delete[] _n_features;
}	

void ISNorm::release () {
		
	for (ssi_size_t i = 0; i < _n_streams; i++) {
		ssi_real_t *aptr = _as[i];
		ssi_real_t *bptr = _bs[i];
		for (ssi_size_t j = 0; j < _n_features[i]; j++) {
			*aptr++ = 0;
			*bptr++ = 0;
		}
	}
}

bool ISNorm::setNorm (Method method) {

	switch (method) {
		case ISNorm::INTERVAL:				
			return setNorm (method, 0.0f, 1.0f);
		case ISNorm::MEAN_STD:
			return setNorm (method, 0.0f, -1.0f);			
	}

	return false;
}

bool ISNorm::setNorm (Method method,
	ssi_real_t a, 
	ssi_real_t b) { 

	release ();	

	for (ssi_size_t i = 0; i < _n_streams; i++) {

		ssi_real_t **matrix;
		ssi_size_t *classes;

		ModelTools::CreateSampleMatrix (_samples, i, _n_samples, _n_features[i], &classes, &matrix);

		switch (method) {
			case ISNorm::INTERVAL: {
					ssi_size_t *minpos = new ssi_size_t[_n_features[i]];
					ssi_size_t *maxpos = new ssi_size_t[_n_features[i]];
					ssi_minmax (_n_samples, _n_features[i], matrix[0], _as[i], minpos, _bs[i], maxpos);
					ssi_real_t *aptr = _as[i];
					ssi_real_t *bptr = _bs[i];
					for (ssi_size_t j = 0; j < _n_features[i]; j++) {						
						*bptr -= *aptr++;
						if (*bptr == 0.0f) {
							ssi_wrn ("found zero interval, set to 1");
							*bptr = 1.0f;
						}
						bptr++;
					}
					delete[] minpos;
					delete[] maxpos;
				}
				break;
			case ISNorm::MEAN_STD: {
					ssi_mean (_n_samples, _n_features[i], matrix[0], _as[i]);
					ssi_var (_n_samples, _n_features[i], matrix[0], _bs[i]);					
					ssi_real_t *bptr = _bs[i];
					for (ssi_size_t j = 0; j < _n_features[i]; j++) {						
						*bptr = sqrt (*bptr);
						if (*bptr == 0.0f) {
							ssi_wrn ("found zero std, set to 1");
							*bptr = 1.0f;
						}
						bptr++;
					}
				}
				break;			
		}

		ModelTools::ReleaseSampleMatrix (_n_samples, classes, matrix);
	}

	return true;
}

ssi_sample_t *ISNorm::get (ssi_size_t index) {

	ssi_sample_t *tmp = _samples.get (index);
	if (tmp) {
		_sample.class_id = tmp->class_id;
		_sample.time = tmp->time;
		_sample.user_id = tmp->user_id;
		_sample.prob = tmp->prob;
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			memcpy (_sample.streams[i]->ptr, tmp->streams[i]->ptr, _n_features[i] * sizeof (ssi_real_t));
			norm (*_sample.streams[i], i);	
		}
		return &_sample;
	}	
	return 0;	
}

ssi_sample_t *ISNorm::next () {

	ssi_sample_t *tmp = _samples.next ();
	if (tmp) {
		_sample.class_id = tmp->class_id;
		_sample.time = tmp->time;
		_sample.user_id = tmp->user_id;
		_sample.prob = tmp->prob;
		for (ssi_size_t i = 0; i < _n_streams; i++) {
			memcpy (_sample.streams[i]->ptr, tmp->streams[i]->ptr, _n_features[i] * sizeof (ssi_real_t));
			norm (*_sample.streams[i], i);	
		}
		return &_sample;
	}	
	return 0;
}

SSI_INLINE void ISNorm::norm (ssi_stream_t &stream, ssi_size_t stream_index) {

	ssi_real_t *ptr = ssi_pcast (ssi_real_t, stream.ptr);
	ssi_real_t *aptr = _as[stream_index];
	ssi_real_t *bptr = _bs[stream_index];
	for (ssi_size_t j = 0; j < _n_features[stream_index]; j++) {
		*ptr -= *aptr++;		
		*ptr++ /= *bptr++;
	}
}

void ISNorm::norm (ssi_sample_t &sample) {

	for (ssi_size_t i = 0; i < sample.num; i++) {
		norm (*sample.streams[i], i);
	}
}

void ISNorm::norm (ISamples &samples) {

	samples.reset ();
	ssi_sample_t *sample = 0;
	while (sample = samples.next ()) {
		norm (*sample);
	}
}

}
