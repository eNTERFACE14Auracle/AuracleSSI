// Procedure.h
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

#ifndef SSI_GRAPHIC_PROCEDURE_H
#define SSI_GRAPHIC_PROCEDURE_H

#include "graphic/GraphicLibCons.h"

// Offers a callback function to proceeed system/application messages sent to a canvas.
// Once a message is received the function automatically fowards it to the canvas
// which is connected with the window handle.
namespace ssi {

	typedef LRESULT (CALLBACK * ProcPtr)
		(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK Procedure
		(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
}

#endif // _PROCEDURE_H
