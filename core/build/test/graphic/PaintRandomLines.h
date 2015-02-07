// PaintRandomLines.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/06
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

#ifndef _PAINTRANDOMLINES_H
#define _PAINTRANDOMLINES_H

#include <cstdlib>

#include "graphic/PaintObject.h"
using namespace ssi;

class PaintRandomLines : public PaintObject {
	
public:

	PaintRandomLines (int n) : num (n) {};

	void setData (const void *data, ssi_size_t sample_number, ssi_time_t time = 0) {
	};

	void paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object) {

		POINT *points = new POINT[num];
	
		// Draw random lines on screen
		for (int i = 0 ; i < num - 1; i++) {

			points[i].x = rand() % rect.right;
			points[i].y = rand() % rect.bottom;
		}
		points[num - 1].x = points[0].x;
		points[num - 1].y = points[0].y;

		int style = rand() % 4;
		int width = 1 + rand() % 8;
		COLORREF col = RGB (rand () % 255, rand () % 255, rand () % 255);

		HGDIOBJ old = SelectObject (hdc, ::CreatePen (style, width, col));
		::Polyline (hdc, points, num);
		::SelectObject(hdc,old);

		delete points;
	};

	void rescale () {};

private:

	int num;
};

#endif
