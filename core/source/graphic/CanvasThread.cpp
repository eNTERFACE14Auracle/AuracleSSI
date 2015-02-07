// CanvasThread.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/13
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

#include "graphic/CanvasThread.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

CanvasThread::CanvasThread (HINSTANCE hInstance_, LPCTSTR className_, const char *name_, Colormap *colormap_)
	: hInstance (hInstance_), className (className_), colormap (colormap_) {
	name = new char[strlen (name_) + 1];
	ssi_strcpy (name, name_);

	// set thread name
	ssi_char_t *thread_name = ssi_strcat ("canvas@", name_);
	Thread::setName (thread_name);
	delete[] thread_name;
}

CanvasThread::~CanvasThread () {

	if (name)
		delete name;
}

void CanvasThread::enter () {

	//cout << "enter enter" << endl;
	// prepare the thread to enter the
	// message queue
	enterMsgQueue = true;
	// now create the canvas
	canvas = new Canvas (hInstance, className, name, colormap);
	//cout << "leave enter" << endl;
}

void CanvasThread::run () {

	// now enter the message queue
	if (enterMsgQueue) {
		// to assure that we won't run another time into
		// the message queue after the canvas has
		// received a close event we set 'enterMsgQueue'
		// to false
		enterMsgQueue = false;
		// the message queue runs until the canvas
		// receives a close message
		// NOTE: first send a close message
		// before stopping the thread since otherwise
		// run () will never finish and the
		// thread never be destroyed
		while (::GetMessage (&msg, NULL, 0, 0)) {
			::TranslateMessage (&msg);
			::DispatchMessage (&msg);
		}
	}
}

void CanvasThread::flush () {
	
	// now we can delete the canvas
	delete canvas;
}

}
