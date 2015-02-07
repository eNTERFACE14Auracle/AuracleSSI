// Slider.h
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

#pragma once

#ifndef SSI_IOPUT_SLIDER_H
#define SSI_IOPUT_SLIDER_H

#include "thread/Thread.h"
#include "base/Array1D.h"

namespace ssi {

class Slider : public Thread {

public:
	
	class ICallback {

	public:
	
		virtual void update (ssi_real_t value) = 0;
	};

public:

	Slider (const ssi_char_t *name,
		ssi_size_t pos_x, 
		ssi_size_t pos_y, 
		ssi_size_t width, 
		ssi_size_t height,
		ssi_real_t value,
		ssi_real_t min,
		ssi_real_t max,
		ssi_size_t steps = 100);
	~Slider ();

	void close ();
	void show () {
		start ();
	}
	
	void set (ssi_real_t value);
	ssi_real_t get ();

	void setCallback (ICallback *callback) {
		_callback = callback;
	};

protected:

	ssi_size_t _pos_x, _pos_y;
	ssi_size_t _width, _height;
	HWND _hWnd;
	ssi_char_t *_name;
	ssi_size_t _steps;
	ssi_real_t _value, _min, _max;

	ICallback *_callback;

	void enter ();
	void run ();
	void flush ();
};

}

#endif
