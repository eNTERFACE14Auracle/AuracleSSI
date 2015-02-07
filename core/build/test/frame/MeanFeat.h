// MeanFeat.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/03/25
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

#ifndef _MEANFEAT_H
#define _MEANFEAT_H

#include "base/IFeature.h"
#include "ioput/option/OptionList.h"

template <class T>
class MeanFeat : public IFeature {

public:

	IOptions *getOptions () { return 0; };
	const ssi_char_t *getName () { return "meanfilt"; };
	const ssi_char_t *getInfo () { return "mean filter"; };
	

	ssi_size_t getSampleDimensionOut (ssi_size_t sample_dimension_in) {
		return sample_dimension_in;
	}

	ssi_size_t getSampleBytesOut (ssi_size_t sample_bytes_in) {
		return sizeof (T);
	}

	ssi_type_t getSampleTypeOut (ssi_type_t sample_type_in) {
		return sample_type_in;
	}

	void transform (ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0) {

		ssi_size_t sample_number = stream_in.num;
		ssi_size_t sample_dimension = stream_in.dim;

		T *data_in_ptr = ssi_pcast (T, stream_in.ptr);
		T *data_out_ptr = ssi_pcast (T, stream_out.ptr);

		for (ssi_size_t j = 0; j < sample_dimension; j++) {
			data_out_ptr[j] = *data_in_ptr++;
		}
		for (ssi_size_t i = 1; i < sample_number; i++) {
			for (ssi_size_t j = 0; j < sample_dimension; j++) {
				data_out_ptr[j] += *data_in_ptr++;
			}
		}
		for (ssi_size_t j = 0; j < sample_dimension; j++) {
			data_out_ptr[j] /= sample_number;
		}
	}

};

#endif
