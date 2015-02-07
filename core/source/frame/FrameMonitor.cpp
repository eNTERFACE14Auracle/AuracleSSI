// FrameMonitor.cpp
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

#include "frame/FrameMonitor.h"
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

/// FrameMonitor ///

FrameMonitor::FrameMonitor (ssi_size_t update_in_ms,
	ssi_size_t pos_x, 
	ssi_size_t pos_y, 
	ssi_size_t width, 
	ssi_size_t height)
: _monitor ("framework", pos_x, pos_y, width, height),
	_sleepTime (update_in_ms) {

	Thread::setName ("ssi_frame_FrameMonitor");

	_frame = Factory::GetFramework ();
}

FrameMonitor::~FrameMonitor () {
}

void FrameMonitor::enter () {
	_monitor.start ();
}

void FrameMonitor::run () {

	if (!_frame->IsInIdleMode ()) {
		
		_monitor.clear ();
		sprintf (_buffer, "Time: %4.2f\r\n", _frame->GetElapsedTime ());
		_monitor.print (_buffer);
		sprintf (_buffer, "ID %-8.7s%-8.7s%-8.7s%-8.7s\r\n-----------------------------------\r\n", "offset", "write", "read", "diff");
		_monitor.print (_buffer);
		for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++) {
			if (_frame->IsBufferInUse (i)) {
				double offset, write_pos, read_pos;
				if (!_frame->GetOffsetTime (i, offset))
					break;
				if (!_frame->GetCurrentSampleTime (i, write_pos))
					break;
				if (!_frame->GetLastAccessedSampleTime (i, read_pos))
					break;
				sprintf (_buffer, "%02d %-7.2f %-7.2f %-7.2f %-7.2f\r\n", i, offset, write_pos, read_pos, write_pos-read_pos);
				_monitor.print (_buffer);
			}
		}
		_monitor.show ();
	}

	sleep_ms (_sleepTime);
}

void FrameMonitor::flush () {

	_monitor.close ();
	_monitor.stop ();
}


}
