// Condition.cpp
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

#include "thread/Condition.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

// constructor
Condition::Condition ()
: waiters_count (0) {

	// Create an auto-reset event.
	events[Condition::SIGNAL] = ::CreateEvent (NULL,  // no security
		FALSE, // auto-reset event
		FALSE, // non-signaled initially
		NULL); // unnamed

  // Create a manual-reset event.
	events[Condition::BROADCAST] = ::CreateEvent (NULL,  // no security
		TRUE,  // manual-reset
		FALSE, // non-signaled initially
		NULL); // unnamed
}

// deconstructor
Condition::~Condition () {
}

// wait for condition
void Condition::wait (Mutex *mutex) {

	// Avoid race conditions.
	waiters_count_mutex.acquire ();
	waiters_count++;
	waiters_count_mutex.release ();

	// It's ok to release the <external_mutex> here since Win32
	// manual-reset events maintain state when used with
	// <SetEvent>.  This avoids the "lost wakeup" bug...
	mutex->release ();

	int result = ::WaitForMultipleObjects (2, // Wait on both events
		events, // events
		FALSE, // Wait for either event to be signaled
		INFINITE); // Wait "forever"

	waiters_count_mutex.acquire ();
	waiters_count--;
	bool last_waiter = result == WAIT_OBJECT_0 + Condition::BROADCAST && waiters_count == 0;
	waiters_count_mutex.release ();

	// Some thread called <pthread_cond_broadcast>.
	if (last_waiter)
		// We're the last waiter to be notified or to stop waiting, so
		// reset the manual event. 
		::ResetEvent (events[Condition::BROADCAST]); 

	// Reacquire the <external_mutex>.
	mutex->acquire ();
}

// wakes up a single waiting thread
void Condition::wakeSingle () {

	// Avoid race conditions.
	waiters_count_mutex.acquire ();
	bool have_waiters = waiters_count > 0;
	waiters_count_mutex.release ();

	if (have_waiters)
		::SetEvent (events[Condition::SIGNAL]);
}

// wakes up all waiting threads
void Condition::wakeAll () {

	// Avoid race conditions.
	waiters_count_mutex.acquire ();
	bool have_waiters = waiters_count > 0;
	waiters_count_mutex.release ();

	if (have_waiters)
		::SetEvent (events[Condition::BROADCAST]);
}

}
