// IIR.cpp
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

#include "signal/IIR.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

IIR::IIR (const ssi_char_t *file)
	: _sections (0), 
	_history (0),
	_coefs (0) {
}

IIR::~IIR () {

	delete _coefs;
}

void IIR::setCoefs (const Matrix<ssi_real_t> *coefs) {

	delete _coefs; _coefs = 0;
	_sections = coefs->rows;

	// filt = [b_11, b_12, b_13, 1, a_12, a_13;
	//         b_21, b_22, b_23, 1, a_22, a_23;
	//                   ...
	//         b_n1, b_n2, b_n3, 1, a_n2, a_n3];
	//
	// so all we need to store are b_x1, b_x2, b_x3 and a_x2, a_x3
	// also, we store them in the order a_x2, a_x3, b_x1, b_x2, b_x3
	// since this is the order in which we'll access them later
	//
	_coefs = new Matrix<ssi_real_t> (_sections, 5);
	for (int i = 0; i < _sections; i++) {
		MatrixOps<ssi_real_t>::Set (_coefs, i, 0, MatrixOps<ssi_real_t>::Get (coefs, i, 4));
		MatrixOps<ssi_real_t>::Set (_coefs, i, 1, MatrixOps<ssi_real_t>::Get (coefs, i, 5));
		MatrixOps<ssi_real_t>::Set (_coefs, i, 2, MatrixOps<ssi_real_t>::Get (coefs, i, 0));
		MatrixOps<ssi_real_t>::Set (_coefs, i, 3, MatrixOps<ssi_real_t>::Get (coefs, i, 1));
		MatrixOps<ssi_real_t>::Set (_coefs, i, 4, MatrixOps<ssi_real_t>::Get (coefs, i, 2));
	}
}

void IIR::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t sample_dimension = stream_in.dim;
	_history = MatrixOps<ssi_real_t>::Zeros (_sections * sample_dimension, 2);
}

void IIR::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	/* Matlab code:

	for i = 1:length (x)
	   y(i) = x(i);
	   for j = 1:_sections
			y(i) = y(i) - hist(j,1) * sos(j,5);
			new_hist = y(i) - hist(j,2) * sos(j,6);
			y(i) = new_hist * sos(j,1) + hist(j,1) * sos(j,2);
			y(i) = y(i) + hist(j,2) * sos(j,3);
			hist(j,2) = hist(j,1);
			hist(j,1) = new_hist;
	   end
	end

	*/

	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = info.frame_num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);

	ssi_real_t *histptr = _history->data;
	ssi_real_t *histptrtmp;
	ssi_real_t *histptrtmp1;
	ssi_real_t *histptrtmp2;
	ssi_real_t new_hist;
	ssi_real_t hist1;
	ssi_real_t hist2;
	ssi_real_t *coefsptr = _coefs->data;
	ssi_real_t *coefstmpptr;
	for (ssi_size_t i = 0; i < sample_number; i++) {
		histptrtmp = histptr;
		for (ssi_size_t j = 0; j < sample_dimension; j++) {
			*dstptr = *srcptr;
			coefstmpptr = coefsptr;
			histptrtmp1 = histptrtmp;
			histptrtmp2 = histptrtmp1 + 1;
			for (int k = 0; k < _sections; k++) {
				hist1 = *histptrtmp1;
				hist2 = *histptrtmp2;
				*dstptr -= hist1 * *coefstmpptr++;
				new_hist = *dstptr - hist2 * *coefstmpptr++;
				*dstptr = new_hist * *coefstmpptr++;
				*dstptr += hist1 * *coefstmpptr++;
				*dstptr += hist2 * *coefstmpptr++;
				*histptrtmp2++ = hist1;
				*histptrtmp1++ = new_hist;
				histptrtmp2++;
				histptrtmp1++;
			}
			dstptr++;
			srcptr++;
			histptrtmp += (_sections << 1);
		}
	}
}

void IIR::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {

	delete _history; _history = 0;
}

}
