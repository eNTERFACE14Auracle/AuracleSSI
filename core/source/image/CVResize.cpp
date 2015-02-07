// CVResize.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/07/29
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

#include "image\CVResize.h"

#include "opencv2\opencv.hpp"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *CVResize::ssi_log_name = "ocvresize_";

CVResize::CVResize (const ssi_char_t *file) 
	: _file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

CVResize::~CVResize () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void CVResize::transform (ssi_time_t frame_rate,
	IplImage *image_in, 
	IplImage *image_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	cvResize (image_in, image_out, _options.method); 
}

void CVResize::setFormat (ssi_video_params_t format) {
	ICVFilter::setFormatIn (format);
	format.heightInPixels = _options.height;
	format.widthInPixels = _options.width;
	ICVFilter::setFormatOut (format);
}

}
