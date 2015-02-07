// CursorMover.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/04/25
// Copyright (C) University of Augsburg

#include "sensor/mouse/CursorMover.h"

namespace ssi {

CursorMover::CursorMover (const ssi_char_t *file)
: _max_x (0),
_max_y (0),
_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

CursorMover::~CursorMover () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void CursorMover::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	HWND desktop = GetDesktopWindow ();
	RECT rect;
	GetWindowRect (desktop, &rect);
	_max_x = ssi_cast (int, rect.right);
	_max_y = ssi_cast (int, rect.bottom);
};

void CursorMover::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	ssi_real_t *ptr_in = ssi_pcast (ssi_real_t, stream_in[0].ptr);

	ptr_in += stream_in[0].dim * (stream_in[0].num - 1);
	float x = *(ptr_in + _options.indx);
	float y = *(ptr_in + _options.indy);

	if (x != _options.skip && y != _options.skip) {
		int X = 0;
		int Y = 0;

		if (_options.scale) {
			x = (x - _options.minx) / (_options.maxx - _options.minx);
			y = (y - _options.miny) / (_options.maxy - _options.miny);
			X = ssi_cast (int, x * _max_x);
			Y = ssi_cast (int, y * _max_y);
		} else {
			X = ssi_cast (int, x);
			Y = ssi_cast (int, y);
		}
		
		if (_options.fliph) {
			X = _max_x - X;
		}
		if (_options.flipv) {
			Y = _max_y - Y;
		}
		
		::SetCursorPos(X,Y);	
	}
};

void CursorMover::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {
};

}

