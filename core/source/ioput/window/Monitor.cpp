// Monitor.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/15
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

#include "ioput/window/Monitor.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

Monitor::Monitor (const ssi_char_t *name, 
	ssi_size_t pos_x, 
	ssi_size_t pos_y, 
	ssi_size_t width, 
	ssi_size_t height,
	ssi_size_t maxchar)
: _pos_x (pos_x), 
	_pos_y (pos_y),
	_width (width),
	_height (height),
	_hWnd (0) {

	_n_buffer = maxchar + 1;
	_buffer = new ssi_char_t[_n_buffer];	
	_buffer_count = 0;
	
	_name = ssi_strcpy (name);
	Thread::setName (_name);

	_hFont = CreateFont (14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FIXED_PITCH, TEXT("Courier New"));
}

Monitor::~Monitor () {

	::DeleteObject (_hFont);
	delete[] _buffer;
	delete[] _name;
}

void Monitor::close () {

	_leaveMsgQueue = true;
	::SendMessage(_hWnd, WM_SETTEXT, 0, (LPARAM) "");
}

void Monitor::clear () {
		
	_buffer[0] = '\0';
	_buffer_count = 1;
}

void Monitor::print (ssi_char_t *str) {

	ssi_size_t len = ssi_cast (ssi_size_t, strlen (str));
	if (_buffer_count + len >= _n_buffer - 1) {
		//ssi_wrn ("max buffer size reached '%u', crop string", _n_buffer);
		len = _n_buffer - _buffer_count - 1;
	}
	if (len > 0) {
		memcpy (_buffer + _buffer_count - 1, str, len); 
		_buffer_count += len;	
		_buffer[_buffer_count-1] = '\0';		
	}
}

void Monitor::show () {
	::SendMessage (_hWnd, WM_SETTEXT, _buffer_count, (LPARAM) _buffer);
}

void Monitor::enter () {

	HWND hParent = GetConsoleWindow();
	HMODULE hInstance = 0;	
	hInstance = ::GetModuleHandle (NULL);
	_hWnd = ::CreateWindow ("EDIT", 
		_name,
		WS_SIZEBOX | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
		_pos_x,
		_pos_y,
		_width,
		_height,
		hParent,
		NULL,
		hInstance,
		NULL);
	::EnableMenuItem (::GetSystemMenu(_hWnd, false), SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	::ShowWindow (_hWnd, SW_SHOW);
	::UpdateWindow (_hWnd);
	::SendMessage (_hWnd, WM_SETFONT, (WPARAM) _hFont, MAKELPARAM (TRUE, 0));

	_enterMsgQueue = true;
	_leaveMsgQueue = false;
}

void Monitor::run () {

	if (_enterMsgQueue) {
		_enterMsgQueue = false;
		MSG msg;
		while (!_leaveMsgQueue && ::GetMessage(&msg, NULL, NULL, NULL)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

void Monitor::flush () {

	if (_hWnd) {
		::DestroyWindow (_hWnd);
	}
	_hWnd = 0;
}

}
