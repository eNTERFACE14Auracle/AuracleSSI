// Event.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/12
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

#include "thread/Event.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Event::Event (bool auto_block_after_release, bool start_in_blocked_mode) {
	
	// create event
	handle = ::CreateEvent (0, !auto_block_after_release, !start_in_blocked_mode, 0);
}

Event::~Event () {

	// close handle
	::CloseHandle (handle);
}

void Event::release () {

	// put into signaled state
	::SetEvent (handle); 
}

void Event::block () {

	// put into nonsignaled state
	::ResetEvent (handle);
}

void Event::wait () {

	// Wait until event is in signaled (green) state
	::WaitForSingleObject (handle, INFINITE);
}

}
