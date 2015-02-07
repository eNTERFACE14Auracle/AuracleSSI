// Monitor.h
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

#ifndef SSI_FRAME_FRAMEMONITOR_H
#define SSI_FRAME_FRAMEMONITOR_H

#include "TheFramework.h"
#include "ioput/window/Monitor.h"

namespace ssi {

class Monitor;

class FrameMonitor : public Thread {

public:

	FrameMonitor (ssi_size_t update_in_ms,
		ssi_size_t pos_x,
		ssi_size_t pos_y,
		ssi_size_t width, 
		ssi_size_t height);
	~FrameMonitor ();

	void enter ();
	void run ();
	void flush ();

private:

	ssi_char_t _buffer[SSI_MAX_CHAR];
	ssi_size_t _sleepTime;
	Monitor _monitor;
	ITheFramework *_frame;
};

}

#endif
