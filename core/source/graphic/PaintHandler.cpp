// PaintHandler.cpp
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

#include "graphic/PaintHandler.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

PaintHandler::PaintHandler (Colormap *colormap_)
	: colormap (colormap_),
	width (-1),
	height (-1),
	hMemBmp (0),
	memDC (0) {
}

PaintHandler::~PaintHandler () {

	delete colormap;
	::DeleteObject(hMemBmp);
	::DeleteDC(memDC);	
}

void PaintHandler::add (PaintObject *object) {
	
	Lock lock (mutex);
	objects.push_back (object);
}

void PaintHandler::clear () {

	Lock lock (mutex);
	objects.clear ();
}

void PaintHandler::rescale () {

	Lock lock (mutex);
	std::vector<PaintObject *>::iterator iter;
	for (iter = objects.begin (); iter != objects.end (); iter++) {
		(*iter)->rescale ();
	}
}

void PaintHandler::paint (HWND hwnd, HDC hdc) {

	bool first_object = true, last_object = false;

	// get dimension of the canvas
	RECT rect;
	::GetClientRect (hwnd, &rect);

	// reallocate memory bitmap if necessary
	if (width != rect.right || height != rect.bottom) {
		width = rect.right;
		height = rect.bottom;		
		::DeleteDC(memDC);	
		memDC = ::CreateCompatibleDC (hdc);
		::DeleteObject(hMemBmp);	
		hMemBmp = ::CreateCompatibleBitmap (hdc, width, height);
	}

	// switch to memory bitmap
	HBITMAP hOldBmp = (HBITMAP) ::SelectObject (memDC, hMemBmp);

	// paint objects
	{
		Lock lock (mutex);
		std::vector<PaintObject *>::iterator iter;
 		for (iter = objects.begin (); iter != objects.end (); iter++) {
			if (iter+1 == objects.end ()) {
				last_object = true;
			}
			(*iter)->paint (memDC, rect, colormap, first_object, last_object);
			first_object = false;
		}
	}

	// copy the meomry bitmap to the hdc
	BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

	// select old bitmap back into the device context
	::SelectObject(memDC, hOldBmp);	
}

}
