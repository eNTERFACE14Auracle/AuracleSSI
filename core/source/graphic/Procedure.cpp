// Procedure.cpp
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

#include "graphic/Procedure.h"
#include "graphic/Canvas.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

// Window procedure shared by all canvas objects
LRESULT CALLBACK Procedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	// First we have to get a handle to the canvas which is connected with the window handle.
	// @64
	Canvas * pCanvas = ssi_pcast (Canvas, ::GetWindowLongPtr (hwnd, GWLP_USERDATA));

	// Now we can foward the message to the connected canvas.
	switch (message) {

	case WM_NCCREATE:
		{
			// In case of a create message we have to connect the window with its canvas.
			CREATESTRUCT const * create = ssi_pcast (CREATESTRUCT const, lParam);
			pCanvas = ssi_cast (Canvas *, create->lpCreateParams);
			// @64
			::SetWindowLongPtr (hwnd, GWLP_USERDATA, reinterpret_cast<long> (pCanvas));
		}
		break;
	case WM_KEYDOWN:
		if (pCanvas->onKeydown ())
			return 0;
		break;
	case WM_DESTROY:
		pCanvas->onDestroy ();
		return 0;
	case WM_PAINT:
		if (pCanvas->onPaint ())
			return 0;
		break;
	case WM_ERASEBKGND:
			return 1;
		break;
	}

	// default handling
	return ::DefWindowProc (hwnd, message, wParam, lParam);
}

}
