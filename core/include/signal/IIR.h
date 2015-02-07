// IIR.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/01/04
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

/**

Provides second-order infinite impulse response (IIR) filtering.

*/

#pragma once

#ifndef SSI_SIGNAL_IIR_H
#define SSI_SIGNAL_IIR_H

#include "base/IFilter.h"
#include "signal/MatrixOps.h"
#include "signal/FFT.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class IIR : public IFilter {

public:

	static const ssi_char_t *GetCreateName () { return "ssi_filter_IIR"; };
	static IObject *Create (const ssi_char_t *file) { return new IIR (file); };
	~IIR ();

	IOptions *getOptions () { return 0; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Infinite impulse response (IIR) filter."; };

	void transform_enter (ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);
	void transform (ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);
	void transform_flush (ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);

	ssi_size_t getSampleDimensionOut (ssi_size_t sample_dimension_in) {
		return sample_dimension_in;
	}
	ssi_size_t getSampleBytesOut (ssi_size_t sample_bytes_in) {
		return sizeof (ssi_real_t);
	}
	ssi_type_t getSampleTypeOut (ssi_type_t sample_type_in) {
		if (sample_type_in != SSI_REAL) {
			ssi_err ("type %s not supported", SSI_TYPE_NAMES[sample_type_in]);
		}
		return SSI_REAL;
	}

	virtual void setCoefs (const Matrix<ssi_real_t> *coefs);
	virtual const Matrix<ssi_real_t> *getCoefs () { return _coefs; };

protected:

	IIR (const ssi_char_t *file = 0);

	int _sections;
	Matrix<ssi_real_t> *_coefs;
	Matrix<ssi_real_t> *_history;

public:

	static Matrix<std::complex<ssi_real_t>> *IIR::Response (const Matrix<ssi_real_t> *coefs, int nfft) {

		/* Matlab code:

		% convert second-order section coefficients to transfer function form
		b = 1;
		a = 1;
		for i = 1:size (_coefs, 1),
		   b1 = _coefs(i,1:3);
		   a1 = _coefs(i,4:6);
		   b = conv(b,b1);
		   a = conv(a,a1);
		end
		b = b.*gain;

		% calculate frequency response
		bz = [b zeros(1,nfft - length (b))];
		az = [a zeros(1, nfft - length(a))]; 
		H = fft(bz) ./ (fft(az) + eps);
		H = H(1:nfftr);

		*/
		
		// convert second-order section coefficients to transfer function form
		int sections = coefs->rows;
		Matrix<ssi_real_t> *b = new Matrix<ssi_real_t> (1,1);
		*b->data = 1;
		Matrix<ssi_real_t> *a = new Matrix<ssi_real_t> (1,1);
		*a->data = 1;
		Matrix<ssi_real_t> *btmp, *atmp;
		Matrix<ssi_real_t> *bcoefs = MatrixOps<ssi_real_t>::Ones (3, 1);
		Matrix<ssi_real_t> *acoefs = MatrixOps<ssi_real_t>::Ones (3, 1);
		ssi_real_t *coefsptr = coefs->data;
		for (int i = 0; i < sections; i++) {
			*(acoefs->data + 1) = *coefsptr++;
			*(acoefs->data + 2) = *coefsptr++;
			*(bcoefs->data + 0) = *coefsptr++;
			*(bcoefs->data + 1) = *coefsptr++;
			*(bcoefs->data + 2) = *coefsptr++;
			btmp = MatrixOps<ssi_real_t>::Clone (b);
			atmp = MatrixOps<ssi_real_t>::Clone (a);
			delete a;
			delete b;
			b = MatrixOps<ssi_real_t>::Conv (btmp, bcoefs, MATRIX_DIMENSION_COL);
			a = MatrixOps<ssi_real_t>::Conv (atmp, acoefs, MATRIX_DIMENSION_COL);
			delete btmp;
			delete atmp;
		}
		delete bcoefs;
		delete acoefs;

		// calculate frequency response
		FFT fft (nfft, 1);
		Matrix<std::complex<ssi_real_t>> *affts = new Matrix<std::complex<ssi_real_t>> (1, fft.rfft);
		fft.transform (a, affts);
		Matrix<std::complex<ssi_real_t>> *bffts = new Matrix<std::complex<ssi_real_t>> (1, fft.rfft);
		fft.transform (b, bffts);
		delete a;
		delete b;
		
		// divide bffts by affts
		MatrixOps<std::complex<ssi_real_t>>::Plus (affts, SPL_REAL_EPSILON);
		MatrixOps<std::complex<ssi_real_t>>::Div (bffts, affts);
		delete affts;

		return bffts;
	}

};

}

#endif
