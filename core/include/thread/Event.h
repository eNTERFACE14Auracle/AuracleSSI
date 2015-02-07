// Event.h
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

#pragma once

#ifndef SSI_THREAD_EVENT_H
#define SSI_THREAD_EVENT_H

#include "thread/ThreadLibCons.h"

namespace ssi {

class Event {

public:

	Event (bool auto_block_after_release = true, bool start_in_blocked_mode = true);

	~Event ();

	// put into signaled state
	void release ();
	// put into nonsignaled state
	void block ();
	// wait for signal
	void wait ();
	
	HANDLE handle;

};

}

#endif // _EVENT_H