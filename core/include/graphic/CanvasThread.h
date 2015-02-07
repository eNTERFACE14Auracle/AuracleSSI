// CanvasThread.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/13
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

#ifndef SSI_GRAPHIC_CANVASTHREAD_H
#define SSI_GRAPHIC_CANVASTHREAD_H

#include "graphic/Canvas.h"

namespace ssi {

class CanvasThread : public Thread {

public:

	CanvasThread (HINSTANCE hInstance_, LPCTSTR className_, const char* name_, Colormap *colormap);
	~CanvasThread ();

	void enter ();
	void run ();
	void flush ();

	Canvas *canvas;

private:

	MSG msg;
	HINSTANCE hInstance;
	LPCTSTR className;
	bool enterMsgQueue;
	char *name;
	Colormap *colormap;
};

}

#endif // _CANVASTHREAD_H
