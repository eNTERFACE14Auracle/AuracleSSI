// Thread.cpp
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

#include "thread/Thread.h"
#include "thread/Lock.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *Thread::ssi_log_name = "thread____";
int Thread::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;
Thread *Thread::_monitor[SSI_THREAD_MONITOR_MAX];
ssi_size_t Thread::_monitor_counter = 0;
Mutex Thread::_monitor_mutex;

// constructor
Thread::Thread (bool single_execution,
	ssi_size_t timeout)
	: _single_execution (single_execution),
	_is_active (false),
	_timeout (timeout),
	_start_time (0),
	_stop_time (0) {

	_name = ssi_strcpy ("noname");

	// monitoring
	{
		Lock lock (_monitor_mutex);
		if (_monitor_counter < SSI_THREAD_MONITOR_MAX) {
			_monitor_id = _monitor_counter;
			_monitor[_monitor_counter] = this;
			++_monitor_counter;
		} else {
			_monitor_id = -1;
		}
	}
}

// deconstructor
Thread::~Thread () {

	// monitoring
	{
		Lock lock (_monitor_mutex);
		if (_monitor_id != -1) {
			_monitor[_monitor_id] = 0;
		}
	}

	delete[] _name;
}

// starts the thread
bool Thread::start () {

	ssi_msg (SSI_LOG_LEVEL_DEFAULT, "start%s'%s'", _single_execution ? " single execution " : " ", _name);

	// we block the _event
	_event.block ();
	// create thread
	uintptr_t result = ::_beginthreadex (
		0, // Security attributes
		0, // Stack size
		threadFun, 
		this, 
		0, 
		&_tid);

	if (result == -1L || result == 0) {
		switch (errno) {		
			case EAGAIN:
				ssi_wrn ("start() failed - too many threads - '%s'", _name);
				break;
			case EINVAL:
				ssi_wrn ("start() failed - invalid argument or incorrect stack size - '%s'", _name);
				break;
			case EACCES:
				ssi_wrn ("start() failed - insufficient resources - '%s'", _name);
				break;
			default:
				ssi_wrn ("start() failed - unkown error, errno=%d - '%s'", errno, _name);
				break;
		}
		return false;
	}

	_handle = ssi_pcast (HANDLE, result);

	// set thread name
#ifdef _DEBUG
	SetThreadName (_tid, "ssithread");
#endif

	// and wait until enter () has finished
	_event.wait ();

	// store start time
	_start_time = ssi_time_ms ();

	return true;
}

// stops the thread
bool Thread::stop () {

	ssi_msg (SSI_LOG_LEVEL_DEFAULT, "terminate%s'%s'", _single_execution ? " single execution " : " ", _name);

	// if not single execution signal run method to terminate
	if (!_single_execution) {
		Lock lock (_mutex);
		_is_active = false;
	}
	// give user the chance to terminate run ()
	terminate ();

	// wait until thread has terminated
	DWORD result;
	result = ::WaitForSingleObject (_handle, _timeout);
	bool success = true;
	if (result == WAIT_TIMEOUT) {
		ssi_wrn ("time-out elapsed '%s'", _name);			
		success = false;
	} else if (result == WAIT_FAILED) {
		ssi_wrn ("WaitForSingleObject() failed - error=%lu - ", ::GetLastError (), _name);			
		success = false;
	}
	// close _handle
	::CloseHandle (_handle);	

	ssi_char_t string[SSI_MAX_CHAR];
	_stop_time = ssi_time_ms ();
	ssi_time_sprint (_stop_time - _start_time, string);
	ssi_msg (SSI_LOG_LEVEL_DEFAULT, "stop%safter %s '%s'", _single_execution ? " single execution " : " ", string, _name);

	return success;
}

// thread function
unsigned int WINAPI Thread::threadFun (void * pArg) {

	// get pointer to thread
	Thread *pThread = (Thread *) pArg;

	// enter thread
	pThread->enter ();
	// signal that enter method has finished
	pThread->_event.release ();
	{
		Lock lock (pThread->_mutex);
		pThread->_is_active = true;
	}
	// check if single execution
	if (pThread->_single_execution) {
		// in this case execute run mehod once
		pThread->run ();
	} else {
		// otherwise loop run method
		while (pThread->isActive ()) {
			pThread->run ();
		}
	}

	// flush thread
	pThread->flush ();

	// in case of single execution mode set _is_active to false
	if (pThread->_single_execution) {
		Lock lock (pThread->_mutex);
		pThread->_is_active = false;
	}

	return 0;
}

bool Thread::isActive () {
	
	bool answer;
	{
		Lock lock (_mutex);
		answer = _is_active;
	}

	return answer;
}

void Thread::sleep_ms (ssi_size_t milliseconds) {

	::Sleep (milliseconds);
}

void Thread::sleep_s (ssi_time_t seconds) {

	::Sleep (ssi_cast (DWORD, 1000.0*seconds + 0.5));
}

void Thread::PrintInfo (FILE *file) {

	Lock lock (_monitor_mutex);
	ssi_char_t string[SSI_MAX_CHAR];

	ssi_fprint (file, "#   running\telapsed\t\tname\n");
	ssi_size_t count = 0;
	for (ssi_size_t i = 0; i < _monitor_counter; i++) {
		if (_monitor[i]) {
			Thread *thread = _monitor[i];
			ssi_time_sprint (thread->getElapsedTime (), string);		
			ssi_fprint (file, "%03u %s\t%s\t%s\n", count++, thread->isActive () ? "true" : "false", string, thread->getName ());
		}
	}
}

#ifdef _DEBUG
void Thread::SetThreadName( DWORD dwThreadID, LPCSTR szThreadName) {
	THREADNAME_INFO info;
	{
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
	}
	__try {
		// @64
		RaiseException (0x406D1388, 0, sizeof(info)/sizeof(DWORD), (const ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) {
	}
}
#endif

}
