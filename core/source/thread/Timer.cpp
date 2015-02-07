// Timer.cpp
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

#include "thread/Timer.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Timer::Timer (ssi_size_t interval, bool high_precision)
: _highprec (high_precision){

	if (_highprec) {
		timeBeginPeriod (1);
	}

	_delta = interval / 1000.0;
	_init = ::timeGetTime ();
	_next = _delta;
}

Timer::Timer (ssi_time_t timeout, bool high_precision) 
: _highprec (high_precision){

	if (_highprec) {
		timeBeginPeriod (1);
	}

	_delta = timeout;
	_init = ::timeGetTime ();
	_next = _delta;
}

Timer::~Timer () {

	if (_highprec) {
		timeEndPeriod (1);
	}
}

void Timer::wait () {
	
	_now = ::timeGetTime () - _init;
	ssi_size_t next_ms = ssi_cast (DWORD, 1000.0*_next + 0.5);
	if (_now < next_ms) {		
		::Sleep (next_ms - _now);
	}
	_next += _delta;
}

}
