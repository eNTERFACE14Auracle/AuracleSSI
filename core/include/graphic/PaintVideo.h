// PaintVideo.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/28
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

#ifndef SSI_GRAPHIC_PAINTVIDEO_H
#define SSI_GRAPHIC_PAINTVIDEO_H

#include "graphic/GraphicLibCons.h"
#include "graphic/PaintObject.h"
#include "thread/Lock.h"

namespace ssi {

class PaintVideo : public PaintObject {
	
public:

	PaintVideo (ssi_video_params_t _video_in_params,
		bool flipForOutput = false,
		bool scaleToOutput = false,
		bool mirrorForOutput = false,
		double maxValue = -1);
	virtual ~PaintVideo ();

	void paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object);
	void rescale ();

	void setData (const void *data, ssi_size_t size, ssi_time_t time = 0);

private:

	void paintAsVideoImage (HDC hdc, RECT rect);

	BYTE *_image_out, *_image_out_tmp;
	ssi_video_params_t _video_in_params, _video_out_params;
	ssi_size_t _video_in_stride, _video_out_stride;
	ssi_size_t _video_in_size, _video_out_size;
	unsigned int size;

	bool	_flipForOutput, _mirrorForOutput, _scaleToOutput;
	double	_maxValue;
	
	Mutex _mutex;
};

}

#endif
