// Canvas.h
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

#ifndef SSI_GRAPHIC_CANVAS_H
#define SSI_GRAPHIC_CANVAS_H

#include "graphic/PaintHandler.h"

namespace ssi {

// A canvas offers a window to display graphical objects
// e.g. to plot a function
class Canvas {

friend LRESULT CALLBACK Procedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:

	Canvas (HINSTANCE hInstance, LPCTSTR className, const char *windowName, Colormap *colormap);
	virtual ~Canvas ();

	// to force a repaint from outside
	void update ();

	// to destroy from outside
	void destroy ();

	// to move from outside
	bool move (int x, int y, int width, int height);

	//like move, but ywith more options
	bool moveEX (int x, int y, int width, int height, bool paramsDescribeClientRect = false, MOVE_EX_TYPE moveAndOrResize = MOVE_EX_TYPE_MOVE_AND_RESIZE); 

	// handles painting
	PaintHandler *painter;

	// window handle
	HWND hwnd;

protected:

	// paint method
	void paint (HDC hdc);

	// window initialization
	bool initInstance (HINSTANCE hInstance, LPCTSTR className, const char *windowName);

	// event handling
	bool onKeydown () throw ();
	bool onPaint () throw ();
	bool onDestroy () throw ();

	
	// background color
	COLORREF bgColor;

};

}

#endif // _CANVAS_H
