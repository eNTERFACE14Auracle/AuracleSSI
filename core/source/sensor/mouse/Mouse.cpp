// Mouse.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/03/06
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

#include "sensor/mouse/Mouse.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

static char ssi_log_name[] = "mouse_____";

Mouse::Mouse (const ssi_char_t *file) 
	: _cursor_provider (0),
	_button_provider (0),
	_timer (0),
	_cursor_buffer (0),
	_cursor_buffer_ptr (0),
	_button_buffer (0),
	_button_buffer_ptr (0),
	_file (0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Mouse::~Mouse () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

bool Mouse::setProvider (const ssi_char_t *name, IProvider *provider) {

	if (strcmp (name, SSI_MOUSE_CURSOR_PROVIDER_NAME) == 0) {
		setCursorProvider (provider);
		return true;
	} else if (strcmp (name, SSI_MOUSE_BUTTON_PROVIDER_NAME) == 0) {
		setButtonProvider (provider);
		return true;
	}

	ssi_wrn ("unkown provider name '%s'", name);

	return false;
}

void Mouse::setCursorProvider (IProvider *cursor_provider) {

	if (_cursor_provider) {
		ssi_wrn ("already set");
	}

	_cursor_provider = cursor_provider;
	if (_cursor_provider) {
		_cursor_channel.stream.sr = _options.sr;
		_cursor_provider->init (&_cursor_channel);
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "cursor provider set");
	}
}

void Mouse::setButtonProvider (IProvider *button_provider) {

	if (_button_provider) {
		ssi_wrn ("already set");
	}

	_button_provider = button_provider;
	if (_button_provider) {
		_button_channel.stream.sr = _options.sr;		
		_button_provider->init (&_button_channel);
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "button provider set");
	}
}

bool Mouse::connect () {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "try to connect sensor...");

	HWND desktop = GetDesktopWindow ();
	RECT rect;
	GetWindowRect (desktop, &rect);
	_max_x = ssi_cast (float, rect.right);
	_max_y = ssi_cast (float, rect.bottom);
	_button_on = false;
	_counter = 0; 
	_event_on = 0;

	_frame_size = ssi_cast (ssi_size_t, _options.size * _options.sr + 0.5);
	_cursor_buffer = new SSI_MOUSE_CURSOR_SAMPLE_TYPE[_frame_size * _cursor_channel.stream.dim];
	_cursor_buffer_ptr = _cursor_buffer;
	_button_buffer = new SSI_MOUSE_BUTTON_SAMPLE_TYPE[_frame_size * _button_channel.stream.dim];
	_button_buffer_ptr = _button_buffer;

	_timer = 0;
	_is_providing = false;

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "connected");

	if (_cursor_provider && ssi_log_level >= SSI_LOG_LEVEL_DETAIL) {
		ssi_print ("\
             cursor\n\
             rate\t= %.2lf\t\n\
             size\t= %.2lf\n\
             dim\t= %d\n\
             bytes\t= %d\n",
			_options.sr, 
			_frame_size / _options.sr,
			_cursor_channel.stream.dim, 
			_cursor_channel.stream.byte);
	}

	if (_button_provider &&  ssi_log_level >= SSI_LOG_LEVEL_DETAIL) {
		ssi_print ("\
             button\n\
             rate\t= %.2lf\t\n\
             size\t= %.2lf\n\
             dim\t= %d\n\
             bytes\t= %d\n",
			_options.sr, 
			_frame_size / _options.sr,
			_button_channel.stream.dim, 
			_button_channel.stream.byte);
	}

	// set thread name
	Thread::setName (getName ());

	return true;
}

void Mouse::run () {

	BOOL cursor_result = ::GetCursorPos (&_cursor_pos);
	short button_pressed = ::GetKeyState (_options.mask) >> 15;

	if (cursor_result) {

		*_cursor_buffer_ptr++ = _options.scale ? _cursor_pos.x / _max_x : _cursor_pos.x;
		*_cursor_buffer_ptr++ = _options.scale ? (_options.flip ? _max_y - _cursor_pos.y : _cursor_pos.y) / _max_y : _options.flip ? _max_y - _cursor_pos.y : _cursor_pos.y;

		if (_options.single) {
			if (button_pressed) {
				if (!_button_on) {
					_event_on = !_event_on;
					_button_on = true;
				}
			} else {
				_button_on = false;
			}
		} else {
			_event_on = button_pressed ? 1 : 0;
		}

		*_button_buffer_ptr++ = _event_on;
		_counter++;		

		if (_counter == _frame_size) {
			if (_cursor_provider) {
				_is_providing = _cursor_provider->provide (ssi_pcast (char, _cursor_buffer), _frame_size);
				SSI_DBG (SSI_LOG_LEVEL_DEBUG, "cursor data provided");
			}
			if (_button_provider) {
				_is_providing = _button_provider->provide (ssi_pcast (char, _button_buffer), _frame_size);
				SSI_DBG (SSI_LOG_LEVEL_DEBUG, "button data provided");
			}
			_cursor_buffer_ptr = _cursor_buffer;
			_button_buffer_ptr = _button_buffer;
			_counter = 0;
		}

		if (_is_providing) {
			if (!_timer) {
				_timer = new Timer (1.0 / _options.sr);					
			} 
			_timer->wait ();
		} else {
			sleep_ms (1);
		}

	} else  {
		ssi_wrn ("could not receive cursor position");
    }
}


bool Mouse::disconnect () {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "try to disconnect sensor...");

	delete[] _button_buffer; _button_buffer = 0;
	delete[] _cursor_buffer; _cursor_buffer = 0;
	delete _timer; _timer = 0;

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "sensor disconnected");

	return true;
}

}
