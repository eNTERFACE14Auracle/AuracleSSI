// MyFilter.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/10/02
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

#include "MyFilter.h"

char MyFilter::ssi_log_name[] = "myfilter__";

MyFilter::MyFilter (const ssi_char_t *file) {
}

MyFilter::~MyFilter () {
}

void MyFilter::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {

	ssi_msg (SSI_LOG_LEVEL_BASIC, "enter()..ok");
}

void MyFilter::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_byte_t *ptr_in = stream_in.ptr;
	ssi_byte_t *ptr_out = stream_out.ptr;
	ssi_size_t n_bytes = stream_in.byte * stream_in.dim;

	for (ssi_size_t i = 0; i < stream_in.num; i++) {
		for (ssi_size_t j = 0; j < stream_in.dim; j++) {
			memcpy (ptr_out + (stream_in.dim - j - 1) * stream_in.byte, 
				ptr_in + j * stream_in.byte, 
				stream_in.byte);			
		}
		ptr_in += n_bytes;
		ptr_out += n_bytes;
	}
}

void MyFilter::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_msg (SSI_LOG_LEVEL_BASIC, "flush()..ok");
}

