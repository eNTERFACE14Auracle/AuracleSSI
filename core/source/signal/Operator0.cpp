// Operator0.cpp
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

#include "signal/Operator0.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Operator0::Operator0 (ssi_opfun0_t function)
	: _function (function) {
}

Operator0::~Operator0 () {
}

ssi_size_t Operator0::getSampleDimensionOut (ssi_size_t sample_dimension_in) {

	return sample_dimension_in;
}

ssi_size_t Operator0::getSampleBytesOut (ssi_size_t sample_bytes_in) {

	SSI_ASSERT (sample_bytes_in == sizeof (ssi_real_t));

	return sample_bytes_in;
}

ssi_type_t Operator0::getSampleTypeOut (ssi_type_t sample_type_in) {

	SSI_ASSERT (sample_type_in == SSI_REAL);

	return sample_type_in;
}

void Operator0::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = stream_in.num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);

	ssi_size_t elems = sample_number * sample_dimension;
	for (ssi_size_t i = 0; i < elems; ++i) {
		*dstptr++ = _function (*srcptr++);
	}
}

}
