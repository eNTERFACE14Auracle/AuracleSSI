// Controller.h
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

#ifndef SSI_GRAPHIC_CONTROLLER_H
#define SSI_GRAPHIC_CONTROLLER_H

#include "graphic/Procedure.h"

namespace ssi {

    class Controller
    {
        friend LRESULT CALLBACK Procedure (HWND hwnd, 
                        UINT message, WPARAM wParam, LPARAM lParam);
		friend class Maker;

	protected:
		Controller ()
			: _h (0)
		{}
		virtual ~Controller ()
		{}
        void SetWindowHandle (HWND hwnd) { _h = hwnd; }

	public:
		HWND Handle () const { return _h; }
		virtual bool OnCreate () throw ()
			{ return false; }
        virtual bool OnDestroy () throw ()
			{ return false; }
		virtual bool OnSize (int width, int height, int flag) throw ()
			{ return false; }
		virtual bool OnClose () throw ()
			{ return false; }
		virtual bool OnPaint () throw ()
			{ return false; }
		virtual bool OnVScroll (int code, int pos) throw ()
			{ return false; }
		virtual bool OnHScroll (int code, int pos) throw ()
			{ return false; }

    protected:
		HWND  _h;
    };
}

#endif
