// Gate.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2013/06/06
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

Provides simple signal gate.

*/

#pragma once

#ifndef SSI_SIGNAL_GATE_H
#define SSI_SIGNAL_GATE_H

#include "base/IFilter.h"
#include "signal/SignalCons.h"
#include "ioput/option/OptionList.h"
#include "thread/Lock.h"

namespace ssi {

class Gate : public IFilter {

public:

	class Options : public OptionList {

	public:

		Options ()
			: thres (0), replace (0), upper (false) {

			setThresId ("thres");

			addOption ("thres", &thres, 1, SSI_REAL, "threshold (everything below is set to 'replace')");		
			addOption ("thresid", thresid, SSI_MAX_CHAR, SSI_CHAR, "name of threshold tuple value");	
			addOption ("replace", &replace, 1, SSI_REAL, "replace value");	
			addOption ("upper", &upper, 1, SSI_BOOL, "applies threshold as upper limit");
		};

		void setThresId (const ssi_char_t *thresid) {
			if (thresid) {
				ssi_strcpy (this->thresid, thresid);
			}
		}

		ssi_real_t thres;
		ssi_real_t replace;
		ssi_char_t thresid[SSI_MAX_CHAR];	
		bool upper;
	};

public:

	static const ssi_char_t *GetCreateName () { return "ssi_filter_Gate"; };
	static IObject *Create (const ssi_char_t *file) { return new Gate (file); };
	~Gate ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Replaces values below threshold."; };

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
		return sample_bytes_in;
	}
	ssi_type_t getSampleTypeOut (ssi_type_t sample_type_in) {
		if (sample_type_in != SSI_REAL) {
			ssi_err ("type %s not supported", SSI_TYPE_NAMES[sample_type_in]);
		}
		return SSI_REAL;
	}

	void listen_enter ();
	bool update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms);
	void listen_flush ();

protected:

	Gate (const ssi_char_t *file = 0);
	Gate::Options _options;
	ssi_char_t *_file;

	Mutex _thres_mutex;
	ssi_real_t _thres, _replace;
	ssi_size_t _thres_id;
};

}

#endif
