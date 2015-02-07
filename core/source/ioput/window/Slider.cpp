// Slider.cpp
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

#include "ioput/window/Slider.h"
#include "base/Factory.h"

#include <shlwapi.h> 
#include <commctrl.h>

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

Slider::Slider (const ssi_char_t *name, 
	ssi_size_t pos_x, 
	ssi_size_t pos_y, 
	ssi_size_t width, 
	ssi_size_t height,
	ssi_real_t value,
	ssi_real_t min,
	ssi_real_t max,
	ssi_size_t steps)
: _pos_x (pos_x), 
	_pos_y (pos_y),
	_width (width),
	_height (height),
	_steps (steps),
	_value (value),
	_min (min),
	_max (max),
	_callback (0),
	_hWnd (0) {
	
	_name = ssi_strcpy (name);
	Thread::setName (_name);
}

Slider::~Slider () {

	delete[] _name;
}

void Slider::close () {
	
	::SendMessage(_hWnd, WM_CLOSE, NULL, NULL);
	::SendMessage(_hWnd, WM_QUIT, NULL, NULL);
	::SendMessage(_hWnd, WM_DESTROY, NULL, NULL);
	::DestroyWindow (_hWnd);

	_hWnd = 0;
}

void Slider::set (ssi_real_t value) {

	ssi_real_t new_value = (value - _min) / (_max - _min); 

	if (new_value < 0 || new_value > 1.0f) {
		ssi_wrn ("ignore value %.2f out of range [0..1]", value);
		return;
	}

	::SendMessage (_hWnd, TBM_SETPOS, true, ssi_cast (ssi_size_t, value * _steps + 0.5f));
}

ssi_real_t Slider::get () {

	ssi_size_t pos = static_cast<ssi_size_t>(::SendMessage(_hWnd, TBM_GETPOS, 0, 0));
	ssi_real_t value =  _min + (_max - _min) * pos / (1.0f * _steps);

	return value;
}

void Slider::enter () {

	HWND hParent = GetConsoleWindow();
	HMODULE hInstance = 0;	
	hInstance = ::GetModuleHandle (NULL);
	_hWnd = ::CreateWindow (TRACKBAR_CLASS, 
		_name,
		WS_VISIBLE | TBS_AUTOTICKS,
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
	::SendMessage(_hWnd, TBM_SETRANGEMAX, true, _steps);
	::SendMessage(_hWnd, TBM_SETTICFREQ, _steps/10, 0);
	set (_value);
}

void Slider::run () {

	MSG msg;
	while (::GetMessage(&msg, NULL, NULL, NULL)) {
		
		if (msg.message == WM_LBUTTONUP) {

			_value =  get();
			ssi_msg (SSI_LOG_LEVEL_DEBUG, "value=%.2f", _value);

			ssi_char_t name[SSI_MAX_CHAR];
			ssi_sprint (name, "%s (%f)", _name, _value);
			::SendMessage(_hWnd, WM_SETTEXT, 0, (LPARAM) name);

			if (_callback) {
				_callback->update (_value);
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void Slider::flush () {
}

}
