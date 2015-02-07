// Timer.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/21
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

#ifndef SSI_THREAD_TIMER_H
#define SSI_THREAD_TIMER_H

#include "ThreadLibCons.h"

namespace ssi {

class Timer {

public:

	Timer (ssi_size_t interval_in_ms, bool high_precision = false);
	Timer (ssi_time_t interval_in_s, bool high_precision = false);
	~Timer ();

	void reset ();
	void wait ();

protected:

	bool _highprec;
	ssi_time_t _delta;
	DWORD _init;
	ssi_time_t _next;
	DWORD _now;
};

}

#endif // _Timer_H
