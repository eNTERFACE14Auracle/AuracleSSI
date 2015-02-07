// SignalTools.cpp
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

#include "signal/SignalTools.h"
#include "signal/FFT.h"
#include "signal/IFFT.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

void SignalTools::Transform (ssi_stream_t &from,
	ssi_stream_t &to,
	ITransformer &transformer,
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	bool call_enter,
	bool call_flush) {

	if (frame_size <= 0) {

		ssi_time_t sample_rate_in = from.sr;
		ssi_size_t sample_number_in = from.num;
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_bytes_in = from.byte;
		ssi_type_t sample_type_in = from.type;
		ssi_size_t sample_number_out = sample_number_in == 0 ? 0 : transformer.getSampleNumberOut (sample_number_in);
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_bytes_in);
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = sample_number_in == 0 ? 0 : (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, sample_number_in)) * sample_rate_in;

		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);

		if (sample_number_out > 0) {
			ssi_stream_adjust (to, sample_number_out);
			if (call_enter) {
				transformer.transform_enter (from, to, 0, 0);	
			}
			ITransformer::info tinfo;
			tinfo.delta_num = 0;
			tinfo.frame_num = from.num;
			tinfo.time = 0;
			transformer.transform (tinfo, from, to, 0, 0);
			if (call_flush) {
				transformer.transform_flush (from, to, 0, 0);
			}
		}

	} else {

		ssi_time_t sample_rate_in = from.sr;

		ssi_size_t from_num = from.num;
		ssi_size_t from_tot = from.tot;
		SSI_ASSERT (from_num > frame_size + delta_size);
		ssi_size_t max_shift = (from_num - delta_size) / frame_size;

		ssi_size_t sample_number_in = frame_size + delta_size;
		ssi_size_t sample_number_out = transformer.getSampleNumberOut (frame_size);	
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_dimension_out = transformer.getSampleDimensionOut (sample_dimension_in);
		ssi_size_t sample_bytes_in = from.byte;
		ssi_size_t sample_bytes_out = transformer.getSampleBytesOut (sample_bytes_in);
		ssi_type_t sample_type_in = from.type;
		ssi_type_t sample_type_out = transformer.getSampleTypeOut (sample_type_in);
		ssi_time_t sample_rate_out = (ssi_cast (ssi_time_t, sample_number_out) / ssi_cast (ssi_time_t, frame_size)) * sample_rate_in;

		ssi_size_t to_num = max_shift * sample_number_out;
		ssi_stream_init (to, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);
		ssi_stream_adjust (to, to_num);
		ssi_size_t to_tot = to.tot;

		ssi_byte_t *from_ptr = from.ptr;
		ssi_byte_t *to_ptr = to.ptr;
		from.num = sample_number_in;
		to.num = sample_number_out;
		ssi_size_t byte_shift_in = sample_bytes_in * sample_dimension_in * frame_size;
		from.tot = byte_shift_in;
		ssi_size_t byte_shift_out = sample_bytes_out * sample_dimension_out * sample_number_out;
		to.tot = byte_shift_out;

		if (call_enter) {
			transformer.transform_enter (from, to, 0, 0);	
		}
		ITransformer::info tinfo;
		tinfo.delta_num = delta_size;
		tinfo.frame_num = frame_size;
		tinfo.time = 0;
		for (ssi_size_t i = 0; i < max_shift; i++) {			
			transformer.transform (tinfo, from, to, 0, 0);					
			tinfo.time += frame_size / sample_rate_in;
			from.ptr += byte_shift_in;
			to.ptr += byte_shift_out;
		}
		if (call_flush) {
			transformer.transform_flush (from, to, 0, 0);
		}

		from.ptr = from_ptr;
		from.num = from_num;
		from.tot = from_tot;
		to.ptr = to_ptr;
		to.num = to_num;
		to.tot = to_tot;
	}
}

void SignalTools::Consume (ssi_stream_t &from,		
	IConsumer &consumer,		
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	bool call_enter,
	bool call_flush) {

	if (frame_size <= 0) {

		if (call_enter) {
			consumer.consume_enter (1, &from);	
		}
		IConsumer::info cinfo;
		cinfo.time = 0;
		cinfo.dur = from.num / from.sr;
		cinfo.status = IConsumer::NO_TRIGGER;
		consumer.consume (cinfo, 1, &from);
		if (call_flush) {
			consumer.consume_flush (1, &from);
		}		

	} else {

		ssi_time_t sample_rate_in = from.sr;

		ssi_size_t from_num = from.num;
		ssi_size_t from_tot = from.tot;
		SSI_ASSERT (from_num > frame_size + delta_size);
		ssi_size_t max_shift = (from_num - delta_size) / frame_size;

		ssi_size_t sample_number_in = frame_size + delta_size;		
		ssi_size_t sample_dimension_in = from.dim;
		ssi_size_t sample_bytes_in = from.byte;
		ssi_type_t sample_type_in = from.type;

		ssi_byte_t *from_ptr = from.ptr;
		from.num = sample_number_in;
		ssi_size_t byte_shift_in = sample_bytes_in * sample_dimension_in * frame_size;		
		from.tot = byte_shift_in;

		if (call_enter) {
			consumer.consume_enter (1, &from);	
		}
		IConsumer::info cinfo;		
		cinfo.time = 0;
		cinfo.dur = sample_number_in / sample_rate_in;
		cinfo.status = IConsumer::NO_TRIGGER;
		for (ssi_size_t i = 0; i < max_shift; i++) {			
			consumer.consume (cinfo, 1, &from);					
			cinfo.time += frame_size / sample_rate_in;
			from.ptr += byte_shift_in;			
		}
		if (call_flush) {
			consumer.consume_flush (1, &from);
		}

		from.ptr = from_ptr;
		from.num = from_num;
		from.tot = from_tot;
	}
}

void SignalTools::Series (ssi_stream_t &series,		
	ssi_time_t duration, 		
	ssi_real_t offset) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;

	*/

	ssi_real_t delta = ssi_cast (ssi_real_t, 1.0 / series.sr);
	ssi_size_t number = ssi_cast (ssi_size_t, duration / delta);
	SSI_ASSERT (number > 0);

	ssi_stream_adjust (series, number);
	ssi_real_t *out = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *outptr_new = out;
	ssi_real_t *outptr_old = out;
	for (ssi_size_t i = 0; i < series.dim; ++i) {
		*outptr_new++ = offset;
	}
	for (ssi_size_t i = series.dim; i < series.dim * series.num; ++i) {
		*outptr_new++ = *outptr_old++ + delta;	
	}
}

void SignalTools::Sine (ssi_stream_t &series,
	ssi_time_t *frequency, 
	ssi_real_t *amplitude) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;
	signal = A' * sin (2*pi*f*t);

	*/
	
	ssi_real_t *ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *pipif = new ssi_real_t[series.dim];

	for (ssi_size_t j = 0; j < series.dim; ++j) {
		pipif[j] = static_cast<ssi_real_t> (2.0 * PI * frequency[j]);
	}
	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*ptr = amplitude[j] * sin (pipif[j] * *ptr);
			++ptr;
		}
	}	

	delete[] pipif;
}

void SignalTools::Cosine (ssi_stream_t &series,
	ssi_time_t *frequency, 
	ssi_real_t *amplitude) {

	/* matlab code:

	t = 0:1/sr:dur-1/sr;
	signal = A' * sin (2*pi*f*t);

	*/
	
	ssi_real_t *ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *pipif = new ssi_real_t[series.dim];

	for (ssi_size_t j = 0; j < series.dim; ++j) {
		pipif[j] = static_cast<ssi_real_t> (2.0 * PI * frequency[j]);
	}
	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*ptr = amplitude[j] * cos (pipif[j] * *ptr);
			++ptr;
		}
	}	

	delete[] pipif;
}

void SignalTools::Noise (ssi_stream_t &series,
	ssi_real_t *amplitude) {

	ssi_real_t *ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t r;

	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			// generate value in range [0..1]
			r = static_cast<ssi_real_t> (static_cast<double> (rand ()) / (static_cast<double> (RAND_MAX) + 1.0));
			// shift value in range [-amp..amp]
			r = amplitude[j] * (2.0f * r - 1.0f);
			// now add value
			*ptr += r;
			++ptr;
		}
	}
}

void SignalTools::Noise (ssi_stream_t &series,
	ssi_real_t noise_ampl,
	double noise_mean,
	double noise_std,
	ssi_time_t cutoff,
	ssi_time_t width) {

	/*

	y = m + s * randn (N,dim); 

	% apply fft
	NFFT = 2^nextpow2 (N);
	fy = fft (y, NFFT);

	% remove frequency in frange
	fdt = (sr/2) / (NFFT/2+1);
	fr = round (frange / fdt);
	fy(1:max(1,fr(1)),:) = 0;
	fy(min(fr(2),NFFT/2+1):NFFT-min(fr(2),NFFT/2),:) = 0;
	fy((NFFT-max(1,fr(1)):NFFT),:) = 0;

	% apply ifft
	y = ifft (fy, N);

	*/

	// determine nfft
	ssi_size_t nfft = log2 (series.num);
	nfft = pow (ssi_cast (ssi_size_t, 2),nfft);
	if (nfft < series.num) {
		nfft = log2 (series.num) + 1;
		nfft = pow (ssi_cast (ssi_size_t, 2), nfft);
	}

	// prepare noise
	ssi_real_t *noise = new ssi_real_t[nfft * series.dim];
	ssi_real_t *pnoise = noise;
	for (ssi_size_t i = 0; i < nfft; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*pnoise++ = ssi_cast (ssi_real_t, noise_mean + noise_std * ssi_random_distr (0.0,1.0));
		}
	}

	// remove frequeny range
	if (cutoff > 0) {

		FFT fft (nfft, series.dim);
		IFFT ifft (fft.rfft, series.dim);
		std::complex<ssi_real_t> *fnoise = new std::complex<ssi_real_t>[fft.rfft * series.dim];
		fft.transform (nfft, noise, fnoise);

		double fdt = (series.sr / 2.0) / (nfft / 2.0 + 1.0);
		ssi_size_t fmin = ssi_cast (ssi_size_t, cutoff / fdt + 0.5);	
		for (ssi_size_t i = 0; i < fmin; i++) {
			fnoise[i].real (0);
			fnoise[i].imag (0);
		}
		if (width > 0) {
			ssi_size_t fmax = fmin + ssi_cast (ssi_size_t, width / fdt + 0.5);
			for (ssi_size_t i = fmax - 1; i < fft.rfft; i++) {
				fnoise[i].real (0);
				fnoise[i].imag (0);
			}
		}
		ifft.transform (fnoise, noise);

		delete[] fnoise;
	}

 	// add noise to series
	ssi_real_t *pseries = ssi_pcast (ssi_real_t, series.ptr);
	pnoise = noise;
	for (ssi_size_t i = 0; i < series.num; ++i) {
		for (ssi_size_t j = 0; j < series.dim; ++j) {
			*pseries++ += noise_ampl * *pnoise++;
		}
	}

	delete[] noise;
}

void SignalTools::Sum (ssi_stream_t &series) {

	ssi_real_t *old_ptr = ssi_pcast (ssi_real_t, series.ptr);
	ssi_real_t *new_ptr = new ssi_real_t[series.num];

	ssi_real_t *srcptr = old_ptr;
	ssi_real_t *dstptr = new_ptr;
	for (ssi_size_t i = 0; i < series.num; i++) {
		*dstptr = 0;
		for (ssi_size_t j = 0; j < series.dim; j++) {
			*dstptr += *srcptr++;
		}
		dstptr++;
	}
	
	series.ptr = ssi_pcast (ssi_byte_t, new_ptr);
	series.dim = 1;
	delete[] old_ptr;	
}

}
