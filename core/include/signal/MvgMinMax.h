// MvgMinMax.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/12/09
// Copyright (C) 2007-10 University of Augsburg, Johannes Wagner
//
// *************************************************************************************************
//
// This file is part of Smart Sensor Integration (SSI) developed at the 
// Lab for Multimedia Concepts and Applications of the University of Augsburg
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

#pragma once

#ifndef SSI_SIGNAL_MVGMINMAX_H
#define SSI_SIGNAL_MVGMINMAX_H

#include "base/IFilter.h"
#include "signal/SignalCons.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class MvgMinMax : public IFilter {

public:

	class Implementation {

		public:

			virtual ~Implementation () {};

			virtual void transform_enter (ssi_stream_t &stream_in,
				ssi_stream_t &stream_out) = 0;
			virtual void transform (ITransformer::info info,
				ssi_stream_t &stream_in,
				ssi_stream_t &stream_out) = 0;
			virtual void transform_flush (ssi_stream_t &stream_in,
				ssi_stream_t &stream_out) = 0;
	};

	enum METHOD {
		MOVING = 0,
		SLIDING		
	};

	enum FORMAT {
		MIN = 1<<0,
		MAX = 1<<1,
		ALL = MIN | MAX
	};

	class Options : public OptionList {

	public:

		Options () 
			: win (10.0), format (ALL), method (MOVING), nblock (10) {

			addOption ("win", &win, 1, SSI_TIME, "size of moving/sliding window in seconds");	
			addOption ("method", &method, 1, SSI_INT, "method (0=moving,1=sliding)");	
			addOption ("format", &format, 1, SSI_INT, "format (1=min,2=max,3=both)");	
			addOption ("nblock", &nblock, 1, SSI_SIZE, "n blocks (for moving filter only)");
		};

		ssi_time_t win;
		FORMAT format;
		METHOD method;
		ssi_size_t nblock;
	};


public:

	static const ssi_char_t *GetCreateName () { return "ssi_filter_MvgMinMax"; };
	static IObject *Create (const ssi_char_t *file) { return new MvgMinMax (file); };
	~MvgMinMax ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Computes moving/sliding minim and/or maximum of the input stream for the chosen window."; };

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
		ssi_size_t bits = 0;
		if (_options.format & MIN)
			++bits;
		if (_options.format & MAX)
			++bits;
		return sample_dimension_in * bits;
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

protected:

	MvgMinMax (const ssi_char_t *file = 0);
	Options _options;
	ssi_char_t *_file;

	static ssi_char_t *ssi_log_name;

	Implementation *_impl;

	class Moving : public Implementation {

	public:

		Moving (Options &options);
		~Moving ();

		void transform_enter (ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);
		void transform (ITransformer::info info,
			ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);
		void transform_flush (ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);

	protected:

		ssi_time_t _windowSize;
		ssi_size_t _numberOfBlocks;
		ssi_size_t _windowSizeInSamples;
		ssi_size_t _blockLengthInSamples;
		ssi_real_t *_history;
		ssi_real_t *_minmax;
		ssi_size_t _currentSampleIndex;
		ssi_size_t _currentBlockIndex;		
		bool _store_min, _store_max;
		bool _first_call;
		FORMAT _format;
	};

	class Sliding : public Implementation {

	public:

		Sliding (Options &options);
		~Sliding ();

		void transform_enter (ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);
		void transform (ITransformer::info info,
			ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);
		 void transform_flush (ssi_stream_t &stream_in,
			ssi_stream_t &stream_out);

	protected:
		
		ssi_time_t _window_size;
		ssi_real_t _alpha;
		ssi_real_t _1_alpha;
		ssi_real_t *_min_hist;
		ssi_real_t *_max_hist;
		bool _first_call, _store_min, _store_max;
		FORMAT _format;
	};
	
};

}
#endif
