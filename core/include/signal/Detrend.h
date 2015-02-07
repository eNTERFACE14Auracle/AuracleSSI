// Detrend.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/21
// Copyright (C) University of Augsburg

/**

Removes linear trend from signal (uses Lapack++)

*/

#pragma once

#ifndef SSI_SIGNAL_DETREND_H
#define SSI_SIGNAL_DETREND_H

#include "SSI_Filter.h"

namespace ssi {

class Detrend : public SSI_Filter {

public:

	Detrend ();
	virtual ~Detrend ();

	ssi_size_t getSampleDimensionOut (ssi_size_t sample_dimension_in);
	ssi_size_t getSampleBytesOut (ssi_size_t sample_bytes_in);

	void transform_enter (ssi_stream &stream_in,
		ssi_stream &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream xtra_stream_in[] = 0);
		
	void transform (ssi_size_t sample_number_delta,
		ssi_stream &stream_in,
		ssi_stream &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream xtra_stream_in[] = 0);
		
	void transform_flush (ssi_stream &stream_in,
		ssi_stream &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream xtra_stream_in[] = 0);

protected:

	double *_a;
	ssi_size_t _sample_number;

};

}

#endif
