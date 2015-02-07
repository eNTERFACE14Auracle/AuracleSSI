// PaintSomeText.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/08
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

#ifndef _PAINTRANDOMTEXT_H
#define _PAINTRANDOMTEXT_H

#include "graphic/PaintObject.h"
using namespace ssi;

#include <string.h>

class PaintSomeText : public PaintObject {
	
public:

	PaintSomeText (const char *t) {

		text = new char[strlen(t) + 1];
		strcpy (text, t);
	};

	~PaintSomeText () {

		delete text;
	}

	void setData (const void *data, ssi_size_t sample_number, ssi_time_t time = 0) {
	};

	void paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object) {

		int x = rect.right / 2;
		int y = rect.bottom / 2;

		TextOut (hdc, x, y, text, (int) strlen (text));
	};

	void rescale () {};

private:

	char *text;
};

#endif

