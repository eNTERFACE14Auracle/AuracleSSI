// Canvas.cpp
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

#include "graphic/Canvas.h"

#include "graphic/Controller.h"
#include "graphic/Procedure.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Canvas::Canvas (HINSTANCE hInstance, LPCTSTR className, const char *windowName, Colormap *colormap) {
	
	// initialize painter
	painter = new PaintHandler (colormap);

	// initialize window
	if (!initInstance (hInstance, className, windowName)) {
		ssi_err ("canvas initialization failed");
	}

	// set background color
	bgColor = RGB (0,0,0);
}

Canvas::~Canvas () {

	if (painter)
		delete painter;
}

bool Canvas::initInstance (HINSTANCE hInstance, LPCTSTR className, const char* windowName) {

	HWND hParent = GetConsoleWindow();

	// With the last parameter we pass the canvas to the function.
	// When now the WM_NCCREATE message is sent (see class Procedure)
	// the calling window can use the pointer to connect itself to the canvas.
	hwnd = ::CreateWindow (className, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, hParent, NULL, hInstance, this);
	if (!hwnd)
		return false;
	// disable window close box
	::EnableMenuItem (::GetSystemMenu(hwnd, false), SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	// Show window
	::ShowWindow (hwnd, SW_SHOWNORMAL);
	::UpdateWindow (hwnd);

	return true;
}

bool Canvas::onKeydown () throw () { 

	//cout << "WM_KEYDOWN message" << endl;
	//InvalidateRect (hwnd, NULL, TRUE);

	painter->rescale ();
	
	return true;
}

bool Canvas::onPaint () throw () {

	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint (hwnd, &ps);
	paint (hdc);			
	::EndPaint (hwnd, &ps);
	return true;
}

bool Canvas::onDestroy () throw () {

	::PostQuitMessage (0);
	return true;
}

void Canvas::update () {

/*	HDC hdc = ::GetDC (hwnd);
	paint (hdc);
	::ReleaseDC (hwnd, hdc);*/

	::InvalidateRect (hwnd, 0, 0);
}

void Canvas::destroy () {

	::SendMessage (hwnd, WM_DESTROY, 0, 0);
}

bool Canvas::move (int x, int y, int width, int height) {
	
	return ::MoveWindow (hwnd, x, y, width, height, true) != 0;
}

bool Canvas::moveEX (int x, int y, int width, int height, bool paramsDescribeClientRect, MOVE_EX_TYPE moveAndOrResize)
{
	RECT wndwRC, clntRC;
	::GetWindowRect(hwnd, &wndwRC);
	::GetClientRect(hwnd, &clntRC);
	long dx = (wndwRC.right - wndwRC.left) - (clntRC.right - clntRC.left);
	long dy = (wndwRC.bottom - wndwRC.top) - (clntRC.bottom - clntRC.top);
	long newX, newY, newWidth, newHeight;
	if(moveAndOrResize == MOVE_EX_TYPE_MOVE_AND_RESIZE || moveAndOrResize == MOVE_EX_TYPE_ONLY_MOVE)
	{
		newX = x;
		newY = y;
	}
	else
	{
		newX = wndwRC.left;
		newY = wndwRC.top;
	}
	if(moveAndOrResize == MOVE_EX_TYPE_MOVE_AND_RESIZE || moveAndOrResize == MOVE_EX_TYPE_ONLY_RESIZE)
	{
		newWidth = paramsDescribeClientRect ? (width+dx) : (width);
		newHeight = paramsDescribeClientRect ? (height+dy) : (height);
	}
	else
	{
		newWidth = wndwRC.right - wndwRC.left;
		newHeight = wndwRC.bottom - wndwRC.top;
	}
	return ::MoveWindow (hwnd, newX, newY, newWidth, newHeight, true) != 0;
}

void Canvas::paint (HDC hdc) {

	// now paint objects
	painter->paint (hwnd, hdc);
}

}
