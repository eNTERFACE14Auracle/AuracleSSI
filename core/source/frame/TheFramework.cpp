// TheFramework.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/23 
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

#include "frame/TheFramework.h"
#include "frame/FrameMonitor.h"
#include "frame/TimeServer.h"
#include "thread/Mutex.h"

#include <fstream>
#include <iomanip>

#ifdef FRAMEWORK_LOG
static std::ofstream logfile;
static ssi::Mutex logmutex;
#endif
#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *TheFramework::ssi_log_name = "framework_";
ssi_char_t *TheFramework::info_file_name = "framework.info";

// constructor
TheFramework::TheFramework (const ssi_char_t *file)
: _monitor (0),
	 _file (0),
	 _last_run_time (0),
	 _start_run_time (0),
	 _info (0),
	 _is_running (false),
	 _is_auto_run (true),
	 _tserver (0),
	 _sync_socket (0),
	 ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

	memset (&_system_time, 0, sizeof (_system_time));
	memset (&_local_time, 0, sizeof (_system_time));

#ifdef FRAMEWORK_LOG 
{
	Lock lock (logmutex);
	logfile.open (THEFRAMEWORK_LOGFILENAME);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Initialize framework.." << std::endl;
}
#endif

	// mark all buffers as unused
    for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++)
        bufferInUse[i] = false;

	// set all threads to 0
	for (int i = 0; i < THEFRAMEWORK_THREAD_NUM; i++)
		runnable[i] = 0;

	// set all component to 0
	for (int i = 0; i < THEFRAMEWORK_COMPONENT_NUM; i++)
		component[i] = 0;
	component_counter = 0;
	
	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Framework has been initialized !" << std::endl;
}
#endif
}

// deconstructor
TheFramework::~TheFramework () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void TheFramework::SetLogLevel (int level) {
	Consumer::SetLogLevel (level);
	ConsumerBase::SetLogLevel (level);
	Transformer::SetLogLevel (level);
	Provider::SetLogLevel (level);
}

void TheFramework::Start () {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Starting framework.." << std::endl;
}
#endif

	SetLogLevel (_options.loglevel);

	ssi_msg (SSI_LOG_LEVEL_BASIC, "start threads");

	// start pre jobs
	std::vector<job_s>::iterator iter;
	for (iter = _jobs.begin (); iter != _jobs.end (); iter++) {
		if (iter->type == EXECUTE::PRE) {
			if (!ssi_execute (iter->exe.str (), iter->args.str (), iter->wait)) {
				ssi_wrn ("failed executing '%s' with arguments '%s'", iter->exe.str (), iter->args.str ());
			}
		}
	}

	// reset all buffers
	for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++) {
		if (bufferInUse[i]) {
			ssi_pcast (TimeBuffer, buffer[i])->reset (0);
		}
	}

	// start monitor
	if (_options.monitor) {
		_monitor = new FrameMonitor (_options.mupd, _options.mpos[0], _options.mpos[1], _options.mpos[2], _options.mpos[3]);
		_monitor->start ();
	}

	// move console
	if (_options.console) {
		HWND hWnd = GetConsoleWindow();
		::MoveWindow(hWnd, _options.cpos[0], _options.cpos[1], _options.cpos[2], _options.cpos[3], true);
	}

	// start all threads
	for (int i = 0; i < THEFRAMEWORK_THREAD_NUM; i++) {
		if (runnable[i]) {
			runnable[i]->start ();
		}
	}

	// countdown
	if (_options.countdown > 0) {
		ssi_print ("\n  seconds to start:   ");
		for (ssi_size_t i = _options.countdown; i > 0; i--) {
			ssi_print ("\b\b%02u", i);			
			Sleep (1000);			
		}
		ssi_print ("\b\bok\n\n");
	}

	// send sync signal
	SYSTEMTIME server_system_time;
	ssi_size_t sync_delay = 0;
	if (_options.sync) {		
		if (_options.slisten) {
			_sync_socket = Socket::CreateAndConnect (_options.stype, Socket::RECEIVE, _options.sport, "");
			ssi_print ("\n\n\twait for message to start pipeline\n\n\n");
			int result = _sync_socket->recv (&server_system_time, sizeof (server_system_time));
			if (result <= 0) {
				ssi_wrn ("could not receive start message");
			}			
		} else {
			_sync_socket = Socket::CreateAndConnect (_options.stype, Socket::SEND, _options.sport, "");	
		}
	}
	
	// signal that framework is running
	_is_running = true;

	// set frame start time
	_start_run_time = ::timeGetTime ();		
	::GetSystemTime (&_system_time);
	::SystemTimeToTzSpecificLocalTime (NULL, &_system_time, &_local_time);

	if (_options.sync) {				
		if (_options.slisten) {

			FILETIME start_system_filetime, server_system_filetime;
			SystemTimeToFileTime (&_system_time, &start_system_filetime);
			SystemTimeToFileTime (&server_system_time, &server_system_filetime);			
			ULARGE_INTEGER ul1;
			ul1.LowPart = start_system_filetime.dwLowDateTime;
			ul1.HighPart = start_system_filetime.dwHighDateTime;
			ULARGE_INTEGER ul2;
			ul2.LowPart = server_system_filetime.dwLowDateTime;
			ul2.HighPart = server_system_filetime.dwHighDateTime;
			ul2.QuadPart -= ul1.QuadPart;
			ULARGE_INTEGER uliRetValue;
			uliRetValue.QuadPart = 0;
			uliRetValue = ul2;
			uliRetValue.QuadPart /= 10;
			uliRetValue.QuadPart /= 1000; // To Milliseconds
			sync_delay = ssi_cast (ssi_size_t, uliRetValue.QuadPart);

			ssi_msg (SSI_LOG_LEVEL_BASIC, "received start message '%02d/%02d/%02d %02d:%02d:%02d:%d' with a delay of %ums", (int) server_system_time.wYear, (int) server_system_time.wMonth, (int) server_system_time.wDay, (int) server_system_time.wHour, (int) server_system_time.wMinute, (int) server_system_time.wSecond, (int) server_system_time.wMilliseconds, sync_delay);
		} else {
			int result = _sync_socket->send (&_system_time, sizeof (_system_time));
			if (result <= 0) {
				ssi_wrn ("could not send start message");
			}							
			ssi_msg (SSI_LOG_LEVEL_BASIC, "sent start message '%02d/%02d/%02d %02d:%02d:%02d:%d' (port=%d protocol=%s)", (int) _system_time.wYear, (int) _system_time.wMonth, (int) _system_time.wDay, (int) _system_time.wHour, (int) _system_time.wMinute, (int) _system_time.wSecond, (int) _system_time.wMilliseconds, _options.sport, _options.stype == Socket::UDP ? "udp" : "tcp");
		}
	}

	ssi_msg (SSI_LOG_LEVEL_BASIC, "start pipeline");	

	if (_options.info) {
		_info = File::CreateAndOpen (File::ASCII, File::WRITE, info_file_name);
		if (_info) {
			FILE *fp = _info->getFile ();
			ssi_fprint (fp, "start system %02d/%02d/%02d %02d:%02d:%02d:%d\n",  (int) _system_time.wYear, (int) _system_time.wMonth, (int) _system_time.wDay, (int) _system_time.wHour, (int) _system_time.wMinute, (int) _system_time.wSecond, (int) _system_time.wMilliseconds);
			ssi_fprint (fp, "start local %02d/%02d/%02d %02d:%02d:%02d:%d\n",  (int) _local_time.wYear, (int) _local_time.wMonth, (int) _local_time.wDay, (int) _local_time.wHour, (int) _local_time.wMinute, (int) _local_time.wSecond, (int) _local_time.wMilliseconds);					
			ssi_fprint (fp, "start %s\n", _options.sync ? (_options.slisten ? "as client" : "as server") : "is off");			
			ssi_fprint (fp, "start offset in ms %u\n", sync_delay);			
			_info->flush ();
		}
	}

	// start time server
	if (_options.tserver) {
		_tserver = new ssi::TimeServer (_options.tport);
		_tserver->start ();
	}
}

void TheFramework::Wait () {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Wait to stop framework.." << std::endl;
}
#endif

	if (_options.sync && _options.slisten) {
		ssi_print ("\n\n\twait for message to stop pipeline\n\n\n");

		SYSTEMTIME server_system_time;
		FILETIME server_system_filetime;

		int result = _sync_socket->recv (&server_system_time, sizeof (server_system_time));
		if (result <= 0) {
			ssi_wrn ("could not receive stop message");
		}
		SystemTimeToFileTime (&server_system_time, &server_system_filetime);
		
		ssi_msg (SSI_LOG_LEVEL_BASIC, "received stop message '%02d/%02d/%02d %02d:%02d:%02d:%d'", (int) server_system_time.wYear, (int) server_system_time.wMonth, (int) server_system_time.wDay, (int) server_system_time.wHour, (int) server_system_time.wMinute, (int) server_system_time.wSecond, (int) server_system_time.wMilliseconds);


	} else {
		ssi_print ("\n\n\tpress enter to stop pipeline\n\n");
		getchar ();
	}
}

void TheFramework::Stop () {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Stopping framework.." << std::endl;
}
#endif

	// signal that we try to stop framework
	_last_run_time = TheFramework::GetElapsedTimeMs ();
	_is_running = false;

	// send sync signal to stop clients
	if (_options.sync && ! _options.slisten) {
		::GetSystemTime (&_system_time);
		::SystemTimeToTzSpecificLocalTime (NULL, &_system_time, &_local_time);
		int result = _sync_socket->send (&_system_time, sizeof (_system_time));
		if (result <= 0) {
			ssi_wrn ("could not send stop message");
		}					
		ssi_msg (SSI_LOG_LEVEL_BASIC, "sent stop message '%02d/%02d/%02d %02d:%02d:%02d:%d' (port=%d protocol=%s)", (int) _system_time.wYear, (int) _system_time.wMonth, (int) _system_time.wDay, (int) _system_time.wHour, (int) _system_time.wMinute, (int) _system_time.wSecond, (int) _system_time.wMilliseconds, _options.sport, _options.stype == Socket::UDP ? "udp" : "tcp");
		delete _sync_socket; _sync_socket = 0;
	}

	ssi_msg (SSI_LOG_LEVEL_BASIC, "stop pipeline (runtime=%.2lfs)", _last_run_time / 1000.0);
	if (_info) {
		SYSTEMTIME stop_sytem_time, stop_local_time;
		::GetSystemTime (&stop_sytem_time);
		::SystemTimeToTzSpecificLocalTime (NULL, &stop_sytem_time, &stop_local_time);
		FILE *fp = _info->getFile ();
		ssi_fprint (fp, "stop system %02d/%02d/%02d %02d:%02d:%02d:%d\n",  (int) stop_sytem_time.wYear, (int) stop_sytem_time.wMonth, (int) stop_sytem_time.wDay, (int) stop_sytem_time.wHour, (int) stop_sytem_time.wMinute, (int) stop_sytem_time.wSecond, (int) stop_sytem_time.wMilliseconds);
		ssi_fprint (fp, "stop local %02d/%02d/%02d %02d:%02d:%02d:%d\n",  (int) stop_local_time.wYear, (int) stop_local_time.wMonth, (int) stop_local_time.wDay, (int) stop_local_time.wHour, (int) stop_local_time.wMinute, (int) stop_local_time.wSecond, (int) stop_local_time.wMilliseconds);		
		_info->flush ();
	}
	delete _info; _info = 0;

    // wake up threads
    for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++) {
        if (!bufferInUse[i]) continue;
		// wake up waiting threads
		bufferCondFull[i].wakeAll ();
		bufferCondEmpty[i].wakeAll ();
    }

	// stop all threads
	for (int i = 0; i < THEFRAMEWORK_THREAD_NUM; i++) {
		if (runnable[i]) {
			runnable[i]->stop ();
		}
	}

	// stop monitor
	if (_options.monitor) {
		_monitor->stop ();
		delete _monitor;
		_monitor = 0;
	}

	// stop time server
	if (_options.tserver) {
		//TcpSocket sock;	
		//sock.create ();		
		//if (!sock.connect ("localhost", _options.tport)) {
//			ssi_wrn ("could not connect to time server (port=%d)", _options.tport);
	//	} else {
			_tserver->close ();		
		//	sock.create ();
			//sock.connect ("localhost", 1111);	
			//sock.recv (buffer, 100);
			//sock.shutdown (SD_BOTH);
			//sock.close ();			
			_tserver->stop ();
			delete _tserver;
			_tserver = 0;
		//}
	}

	// start post jobs
	std::vector<job_s>::iterator iter;
	for (iter = _jobs.begin (); iter != _jobs.end (); iter++) {
		if (iter->type == EXECUTE::POST) {
			if (!ssi_execute (iter->exe.str (), iter->args.str (), iter->wait)) {
				ssi_wrn ("failed executing '%s' with arguments '%s'", iter->exe.str (), iter->args.str ());
			}
		}
	}


#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Framework has been stopped !" << std::endl;
	logfile.close ();
}
#endif
}

void TheFramework::Clear () {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Shutting down signal to framework.." << std::endl;
}
#endif

	ssi_msg (SSI_LOG_LEVEL_BASIC, "framework shutdown");

	// remove all buffer
    for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++) {
        if (!bufferInUse[i]) continue;
		// wake up waiting threads
		bufferCondFull[i].wakeAll ();
		bufferCondEmpty[i].wakeAll ();
		// remove buffer
        this->RemBuffer (i);
    }

	// removes threads
	for (int i = 0; i < THEFRAMEWORK_THREAD_NUM; i++) {
		if (runnable[i]) {
			runnable[i] = 0;
		}
	}

	// delete component
	for (int i = 0; i < THEFRAMEWORK_COMPONENT_NUM; i++) {
		if (component[i]) {
			delete component[i];
			component[i] = 0;
		}
	}
	component_counter = 0;

	// delete jobs
	_jobs.clear ();

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Good bye!" << std::endl;
}
#endif
}

// Adds a buffer to the framework
//int TheFramework::AddBuffer (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_size_t sample_bytes, ssi_time_t buffer_duration, ssi_time_t sync_duration) {
int TheFramework::AddBuffer (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_size_t sample_bytes, ssi_type_t sample_type, ssi_time_t buffer_duration) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to add buffer.." << std::endl;
}
#endif

	// find a free buffer
	int buffer_id = -1;
	for (int i = 0; i < THEFRAMEWORK_BUFFER_NUM; i++) {
		// get mutex for the buffer
		Lock lock (bufferMutex[i]);
		// check if buffer is still available
		if (!bufferInUse[i]) {
			buffer_id = i;
			break;
		}
	}
	if (buffer_id == -1) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [all buffer in use]" << std::endl;
}
#endif

		return THEFRAMEWORK_ERROR;
	}

	// get mutex for the buffer
	Lock lock (bufferMutex[buffer_id]);

    // create the new buffer
	buffer[buffer_id] = (Buffer *) new TimeBuffer (buffer_duration, sample_rate, sample_dimension, sample_bytes, sample_type);
	// store sync time
//	syncDur[buffer_id] = ssi_cast (ssi_size_t, sync_duration * sample_rate);
//	syncDurCounter[buffer_id] = 0;
    // and mark it as active
    bufferInUse[buffer_id] = true;

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Added buffer " << buffer_id << " !" << std::endl;
}
#endif

    return buffer_id;
}

bool TheFramework::SetMetaData (int buffer_id, ssi_size_t size, const void *meta) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to set meta data to buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
		return false;
	}

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return false;
    }

    // now set meta data
	buffer[buffer_id]->setMetaData (size, meta);

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Set meta data to buffer " << buffer_id << " !" << std::endl;
}
#endif

    return true;
}

const void *TheFramework::GetMetaData (int buffer_id, ssi_size_t &size) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to get meta data of buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
		return 0;
	}

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return 0;
    }

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Get meta data of buffer " << buffer_id << " !" << std::endl;
}
#endif

	return buffer[buffer_id]->getMetaData (size);
}

// Removes a buffer from the framework
bool TheFramework::RemBuffer (int buffer_id) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to remove buffer " << buffer_id << ".." << std::endl;
}
#endif

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return false;
    }

    // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return false;
    }

    // now remove the buffer
	delete buffer[buffer_id];
    // and mark it as inactive
    bufferInUse[buffer_id] = false;

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Removed buffer " << buffer_id << " !" << std::endl;
}
#endif

    return true;
}

// Resets the buffer
bool TheFramework::ResetBuffer (int buffer_id, ssi_time_t offset) {

	#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to reset buffer " << buffer_id << ".." << std::endl;
}
#endif

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return false;
    }

    // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return false;
    }

    // now reset the buffer
	static_cast<TimeBuffer*>(buffer[buffer_id])->reset (offset);
	

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Reseted buffer " << buffer_id << " !" << std::endl;
}
#endif

    return true;

}

// Appends data to a buffer
int TheFramework::PushData (int buffer_id, const ssi_byte_t *data, ssi_size_t samples) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to supply data to buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if framework is in running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework in is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

	// add data to buffer
	TimeBuffer::STATUS status;
	status =  static_cast<TimeBuffer*>(buffer[buffer_id])->push (data, samples);

	// check if it is time to synchronize buffer
//	if (status == TimeBuffer::SUCCESS) {
//		syncDurCounter[buffer_id] += samples;
//		if (syncDurCounter[buffer_id] > syncDur[buffer_id]) {
//			static_cast<TimeBuffer*>(buffer[buffer_id])->sync (GetElapsedTime ());
//			syncDurCounter[buffer_id] = 0;
//		}
//	}

	if (status == TimeBuffer::SUCCESS) {
		// wake up one waiting runnable (in case one or more are waiting)
		bufferCondEmpty[buffer_id].wakeAll ();
		//bufferCondEmpty[buffer_id].wakeSingle ();
	}

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	ssi_time_t duration = samples / static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate;
	ssi_time_t current_time = static_cast<TimeBuffer*>(buffer[buffer_id])->getCurrentSampleTime ();
	if (status == TimeBuffer::SUCCESS) {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Supplied data to buffer " << buffer_id << " (" << current_time << "," << duration << ")" << " !" << std::endl;	
	} else {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to supply data to buffer " << buffer_id << " (" << current_time << "," << duration << ")" << " !" << std::endl;
	}
}
#endif

    return status;
}

// Appends zeros to a buffer
int TheFramework::PushZeros (int buffer_id) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to supply zeros to buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if framework is running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

	// add zeros to buffer
	TimeBuffer::STATUS status = TimeBuffer::SUCCESS;
	ssi_time_t frame_time = GetElapsedTime ();
	ssi_time_t buffer_time = static_cast<TimeBuffer*>(buffer[buffer_id])->getLastAccessedSampleTime ();
	if (buffer_time < frame_time) {
		ssi_size_t samples = ssi_cast (ssi_size_t, (frame_time - buffer_time) * static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate);
		status =  static_cast<TimeBuffer*>(buffer[buffer_id])->pushZeros (samples);
	}
	
	if (status == TimeBuffer::SUCCESS) {
		// wake up one waiting runnable (in case one or more are waiting)
		bufferCondEmpty[buffer_id].wakeAll ();
		//bufferCondEmpty[buffer_id].wakeSingle ();
	}

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	ssi_time_t frame_time = GetElapsedTime ();
	ssi_time_t buffer_time = static_cast<TimeBuffer*>(buffer[buffer_id])->getLastAccessedSampleTime ();
	if (buffer_time < frame_time) {
		ssi_size_t samples = ssi_cast (ssi_size_t, (frame_time - buffer_time) * static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate);
		ssi_time_t duration = samples / static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate;
		ssi_time_t current_time = static_cast<TimeBuffer*>(buffer[buffer_id])->getCurrentSampleTime ();
		if (status == TimeBuffer::SUCCESS) {
			logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Supplied data to buffer " << buffer_id << " (" << current_time << "," << duration << ")" << " !" << std::endl;	
		} else {
			logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to supply data to buffer " << buffer_id << " (" << current_time << "," << duration << ")" << " !" << std::endl;
		}
	}
}
#endif

    return status;
}

// Gets data from the buffer
// waits if buffer is not ready
int TheFramework::GetData (int buffer_id, ssi_byte_t **data, ssi_size_t &samples_in, ssi_size_t &samples_out, ssi_time_t start_time, ssi_time_t duration) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to receive data from buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if the framework is is running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
	bufferMutex[buffer_id].acquire ();

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    TimeBuffer::STATUS status;

	for (;;) {

		// try to get data from the buffer
		status = static_cast<TimeBuffer*>(buffer[buffer_id])->get (data, samples_in, samples_out, start_time, duration);

		if (status != TimeBuffer::SUCCESS && 
			_is_running &&
			status != TimeBuffer::DATA_NOT_IN_BUFFER_ANYMORE &&
			status != TimeBuffer::INPUT_ARRAY_TOO_SMALL && 
			status != TimeBuffer::DURATION_TOO_LARGE && 
			status != TimeBuffer::DURATION_TOO_SMALL
			) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Put receiving component to sleep (buffer " << buffer_id << ")" << std::endl;
}
#endif

			// if operation was not successful and the framework is running
			// put the calling runnable to sleep
			bufferCondEmpty[buffer_id].wait (&bufferMutex[buffer_id]);

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Wake up receiving component (buffer " << buffer_id << ")" << std::endl;
}
#endif

		} else {
			// otherwise leave
			break;
		}

		// leave if framework is in not running
		if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Leave because framework is not running (buffer " << buffer_id << ")" << std::endl;
}
#endif
			bufferMutex[buffer_id].release ();
			return THEFRAMEWORK_ERROR;
		}
	}

	// wake up one waiting runnable (in case one or more are waiting)
	//bufferCondFull[buffer_id].wakeAll ();
	bufferCondFull[buffer_id].wakeSingle ();

	// release mutex for the buffer
	bufferMutex[buffer_id].release ();

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	if (status == TimeBuffer::SUCCESS) {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Received data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	} else {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to receive data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	}
}
#endif

    return status;
}

// Gets data from the buffer
// waits if buffer is not ready
int TheFramework::GetData (int buffer_id, ssi_stream_t &stream, ssi_time_t start_time, ssi_time_t duration) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to receive data from buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if the framework is running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
	bufferMutex[buffer_id].acquire ();

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    TimeBuffer::STATUS status;

	for (;;) {

		// try to get data from the buffer
		status = static_cast<TimeBuffer*>(buffer[buffer_id])->get (&stream.ptr, stream.num_real, stream.num, start_time, duration);
		stream.tot_real = stream.num_real;
		stream.tot = stream.num * stream.byte * stream.dim;

		if (status != TimeBuffer::SUCCESS && 
			_is_running &&
			status != TimeBuffer::DATA_NOT_IN_BUFFER_ANYMORE &&
			status != TimeBuffer::INPUT_ARRAY_TOO_SMALL && 
			status != TimeBuffer::DURATION_TOO_LARGE && 
			status != TimeBuffer::DURATION_TOO_SMALL
			) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Put receiving runnable to sleep (buffer " << buffer_id << ")" << std::endl;
}
#endif

			// if operation was not successful and the framework is running
			// put the calling runnable to sleep
			bufferCondEmpty[buffer_id].wait (&bufferMutex[buffer_id]);

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Wake up receiving runnable (buffer " << buffer_id << ")" << std::endl;
}
#endif

		} else {
			// otherwise leave
			break;
		}

		// leave if framework is not running
		if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Leave because is not running (buffer " << buffer_id << ")" << std::endl;
}
#endif
			bufferMutex[buffer_id].release ();
			return THEFRAMEWORK_ERROR;
		}
	}

	// wake up one waiting runnable (in case one or more are waiting)
	//bufferCondFull[buffer_id].wakeAll ();
	bufferCondFull[buffer_id].wakeSingle ();

	// release mutex for the buffer
	bufferMutex[buffer_id].release ();

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	if (status == TimeBuffer::SUCCESS) {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Received data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	} else {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to receive data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	}
}
#endif

    return status;
}


// Gets data from the buffer
// waits if buffer is not ready
int TheFramework::GetData (int buffer_id, ssi_byte_t *data, ssi_size_t samples, ssi_size_t position) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to receive data from buffer " << buffer_id << ".." << std::endl;
}
#endif

	// check if the framework is running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
	bufferMutex[buffer_id].acquire ();

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    TimeBuffer::STATUS status;

	for (;;) {

		// try to get data from the buffer
		status = static_cast<TimeBuffer*>(buffer[buffer_id])->get (data, samples, position);

		if (status != TimeBuffer::SUCCESS && 
			_is_running &&
			status != TimeBuffer::DATA_NOT_IN_BUFFER_ANYMORE &&
			status != TimeBuffer::INPUT_ARRAY_TOO_SMALL && 
			status != TimeBuffer::DURATION_TOO_LARGE && 
			status != TimeBuffer::DURATION_TOO_SMALL
			) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Put receiving runnable to sleep (buffer " << buffer_id << ")" << std::endl;
}
#endif

			// if operation was not successful and the framework is running
			// put the calling runnable to sleep
			bufferCondEmpty[buffer_id].wait (&bufferMutex[buffer_id]);

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Wake up receiving runnable (buffer " << buffer_id << ")" << std::endl;
}
#endif

		} else {
			// otherwise leave
			break;
		}

		// leave if framework is not running
		if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Leave because framework is not running (buffer " << buffer_id << ")" << std::endl;
}
#endif
			bufferMutex[buffer_id].release ();
			return THEFRAMEWORK_ERROR;
		}
	}

	// wake up one waiting runnable (in case one or more are waiting)
	//bufferCondFull[buffer_id].wakeAll ();
	bufferCondFull[buffer_id].wakeSingle ();

	// release mutex for the buffer
	bufferMutex[buffer_id].release ();

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	ssi_time_t start_time = (position / static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate) - static_cast<TimeBuffer*>(buffer[buffer_id])->getOffsetTime ();
	ssi_time_t duration = samples / static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate;
	if (status == TimeBuffer::SUCCESS) {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Received data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	} else {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to receive data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	}
}
#endif

    return status;
}

// Gets data from buffer
int TheFramework::GetDataTry (int buffer_id, ssi_byte_t **data, ssi_size_t &samples_in, ssi_size_t &samples_out, ssi_time_t start_time, ssi_time_t duration) {

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to receive data from buffer " << buffer_id << std::endl;
}
#endif

	// check if the framework is running
    if (!_is_running) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [framework is not running]" << std::endl;
}
#endif
		Sleep (THEFRAMEWORK_SLEEPTIME_IF_IDLE);
        return THEFRAMEWORK_ERROR;
    }

    // check if buffer id is valid
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [invalid buffer id " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

    // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is in use
    if (!bufferInUse[buffer_id]) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [buffer not in use: " << buffer_id << "]" << std::endl;
}
#endif
        return THEFRAMEWORK_ERROR;
    }

	// try to get data from the buffer
    TimeBuffer::STATUS status = static_cast<TimeBuffer*>(buffer[buffer_id])->get (data, samples_out, samples_in, start_time, duration);
	// wake up one waiting runnable (in case one or more are waiting)
	//bufferCondFull[buffer_id].wakeAll ();
	bufferCondFull[buffer_id].wakeSingle ();

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	if (status == TimeBuffer::SUCCESS) {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Received data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;	
	} else {
		logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Failed to receive data from buffer " << buffer_id << " (" << start_time << " to " << start_time + duration << ") !" << std::endl;
	}
}
#endif

    return status;
}

// returns buffer time
bool TheFramework::GetCurrentSampleTime (int buffer_id, ssi_time_t &time) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	 // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now return buffer time
	time = static_cast<TimeBuffer*>(buffer[buffer_id])->getCurrentSampleTime ();

	return true;
}

// returns current write position
bool TheFramework::GetCurrentWritePos (int buffer_id, ssi_size_t &position) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	 // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now return buffer time
	position = static_cast<TimeBuffer*>(buffer[buffer_id])->getCurrentWritePos ();

	return true;
}

// returns buffer time
bool TheFramework::SetCurrentSampleTime (int buffer_id, ssi_time_t time) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	 // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now return buffer time
	static_cast<TimeBuffer*>(buffer[buffer_id])->setCurrentSampleTime (time);

	return true;
}

// returns buffer time
bool TheFramework::GetLastAccessedSampleTime (int buffer_id, ssi_time_t &time) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now return buffer time
	time = static_cast<TimeBuffer*>(buffer[buffer_id])->getLastAccessedSampleTime ();

	return true;
}

// returns offset time of buffer
bool TheFramework::GetOffsetTime (int buffer_id, ssi_time_t &offset) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	 // get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	offset = static_cast<TimeBuffer*>(buffer[buffer_id])->getOffsetTime ();

	return true;
}

// returns sample rate of buffer
bool TheFramework::GetSampleRate (int buffer_id, ssi_time_t &sample_rate) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	sample_rate = static_cast<TimeBuffer*>(buffer[buffer_id])->sample_rate;

	return true;
}

// returns total sample bytes of buffer
bool TheFramework::GetTotalSampleBytes (int buffer_id, ssi_size_t &sample_bytes) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	sample_bytes = static_cast<TimeBuffer*>(buffer[buffer_id])->sample_total_bytes;

	return false;
}

// returns sample bytes of buffer
bool TheFramework::GetSampleBytes (int buffer_id, ssi_size_t &sample_bytes) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	sample_bytes = static_cast<TimeBuffer*>(buffer[buffer_id])->sample_bytes;

	return true;
}


// returns sample bytes of buffer
bool TheFramework::GetSampleType (int buffer_id, ssi_type_t &sample_type) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	sample_type = static_cast<TimeBuffer*>(buffer[buffer_id])->sample_type;

	return true;
}

// returns sample dimension of buffer
bool TheFramework::GetSampleDimension (int buffer_id, ssi_size_t &sample_dimension) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	sample_dimension = static_cast<TimeBuffer*>(buffer[buffer_id])->sample_dimension;

	return true;
}

// returns capacity of buffer
bool TheFramework::GetCapacity (int buffer_id, ssi_time_t &capacity) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	if (!bufferInUse[buffer_id]) {
		return false;
	}

	// now get requested value
	capacity = static_cast<TimeBuffer*>(buffer[buffer_id])->getCapacity ();

	return true;
}

// returns sample dimension of buffer
bool TheFramework::IsBufferInUse (int buffer_id) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

    // now check if buffer is not in use yet
	// in this case return false
	return bufferInUse[buffer_id];
}

// synchronize buffer buffer
bool TheFramework::Synchronize (int buffer_id) {

	// check if buffer id is valid
	// otherwise return false
    if (buffer_id < 0 || buffer_id >= THEFRAMEWORK_BUFFER_NUM) {
		return false;
    }

	// get mutex for the buffer
    Lock lock (bufferMutex[buffer_id]);

	// synchronize buffer with current framwork clock
    static_cast<TimeBuffer*>(buffer[buffer_id])->sync (GetElapsedTime ());

	return true;
}

// returns sample dimension of buffer
bool TheFramework::IsInIdleMode () {

	return !_is_running;
}

ssi_size_t TheFramework::GetStartTimeMs () {

	return !_is_running ? 0 : _start_run_time;
}

ssi_time_t TheFramework::GetStartTime () {

	return !_is_running ? 0 : _start_run_time / 1000.0;
}

void TheFramework::GetStartTimeLocal (ssi_size_t &year, ssi_size_t &month, ssi_size_t &day, ssi_size_t &hour, ssi_size_t &minute, ssi_size_t &second, ssi_size_t &msecond) {
	
	year = _local_time.wYear;
	month = _local_time.wMonth;
	day = _local_time.wDay;
	hour = _local_time.wHour;
	minute = _local_time.wMinute;
	second = _local_time.wSecond;
	msecond = _local_time.wMilliseconds;
}

void TheFramework::GetStartTimeSystem (ssi_size_t &year, ssi_size_t &month, ssi_size_t &day, ssi_size_t &hour, ssi_size_t &minute, ssi_size_t &second, ssi_size_t &msecond) {

	year = _system_time.wYear;
	month = _system_time.wMonth;
	day = _system_time.wDay;
	hour = _system_time.wHour;
	minute = _system_time.wMinute;
	second = _system_time.wSecond;
	msecond = _system_time.wMilliseconds;
}

ssi_size_t TheFramework::GetElapsedTimeMs () {

	return !_is_running ? 0 : ssi_time_ms () - _start_run_time;
}

ssi_time_t TheFramework::GetElapsedTime () {

	return !_is_running ? 0 : GetElapsedTimeMs () / 1000.0;
}

ssi_size_t TheFramework::GetRunTimeMs () {

	return _last_run_time;
}

ssi_time_t TheFramework::GetRunTime () {

	return _last_run_time > 0 ? _last_run_time / 1000.0 : 0;
}

void TheFramework::AddExeJob (const ssi_char_t *exe, const ssi_char_t *args, EXECUTE::list type, int wait) {

	if (type == EXECUTE::NOW) {
		if (!ssi_execute (exe, args, wait)) {
			ssi_wrn ("failed executing '%s' with arguments '%s'", exe, args);
		}
	} else {
		job_s job;
		job.exe = String (exe);
		job.args = String (args);
		job.wait = wait;
		job.type = type;
		_jobs.push_back (job);
	}
}

// Adds a runnable to the framework
int TheFramework::AddRunnable (IRunnable *new_thread) {

	if (_is_running) {
		return THEFRAMEWORK_ERROR;
	}

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Try to add runnable.." << std::endl;
}
#endif

	// find a free runnable
	int thread_id = -1;
	for (int i = 0; i < THEFRAMEWORK_THREAD_NUM; i++) {
		if (runnable[i] == 0) {
			runnable[i] = new_thread;
			thread_id = i;
			break;
		}
	}
	if (thread_id == -1) {
#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "ERROR [all threads in use]" << std::endl;
}
#endif
		return THEFRAMEWORK_ERROR;
	}

#ifdef FRAMEWORK_LOG
{
	Lock lock (logmutex);
	logfile << std::setw (6) << std::setprecision (2) << std::fixed << GetElapsedTime () << "\t" << "Added runnable " << thread_id << " !" << std::endl;
}
#endif

    return thread_id;
}

void TheFramework::AddEventConsumer (ITransformable *source,
	IConsumer *iconsumer, 
	ITheEventBoard *event_board,
	const ssi_char_t *address,
	ITransformer *transformer) {

	EventConsumer *consumer = new EventConsumer ();
	consumer->getOptions ()->async = true;
	component[component_counter++] = consumer;	
	
	consumer->AddConsumer (source, iconsumer, transformer);
	event_board->RegisterListener (*consumer, address);	
}

void TheFramework::AddEventConsumer (ssi_size_t n_sources, 
	ITransformable **sources,
	IConsumer *iconsumer, 
	ITheEventBoard *event_board,
	const ssi_char_t *address,
	ITransformer **itransformer) {

	EventConsumer *consumer = new EventConsumer ();
	consumer->getOptions ()->async = true;
	component[component_counter++] = consumer;	
	
	consumer->AddConsumer (n_sources, sources, iconsumer, itransformer);
	event_board->RegisterListener (*consumer, address);	
}

void TheFramework::AddConsumer (ITransformable *source,
	IConsumer *iconsumer, 
	const ssi_char_t *frame_size,
	const ssi_char_t *delta_size,
	ITransformer *transformer,
	ITransformable *trigger) {

	ssi_size_t frame_size_in_samples = 0;
	ParseTime (frame_size, frame_size_in_samples, source->getSampleRate ());

	ssi_size_t delta_size_in_samples = 0;
	if (delta_size) {
		ParseTime (delta_size, delta_size_in_samples, source->getSampleRate ());
	}

	int trigger_id = -1;
	if (trigger) {
		trigger_id = trigger->getBufferId ();
	}
	Consumer *consumer = new Consumer (source->getBufferId (), iconsumer, frame_size_in_samples, delta_size_in_samples, transformer, trigger_id);
	component[component_counter++] = consumer;
};

void TheFramework::AddConsumer (ssi_size_t n_sources, 
	ITransformable **sources,
	IConsumer *iconsumer, 
	const ssi_char_t *frame_size,
	const ssi_char_t *delta_size,
	ITransformer **itransformer,
	ITransformable *trigger) {

	int *buffer_ids = new int[n_sources];
	for (ssi_size_t i = 0; i < n_sources; i++) {
		buffer_ids[i] = sources[i]->getBufferId ();
	}

	ssi_size_t frame_size_in_samples = 0;
	ParseTime (frame_size, frame_size_in_samples, sources[0]->getSampleRate ());

	ssi_size_t delta_size_in_samples = 0;
	if (delta_size) {
		ParseTime (delta_size, delta_size_in_samples, sources[0]->getSampleRate ());
	}

	int trigger_id = -1;
	if (trigger) {
		trigger_id = trigger->getBufferId ();
	}
	Consumer *consumer = new Consumer (n_sources, buffer_ids, iconsumer, frame_size_in_samples, delta_size_in_samples, itransformer, trigger_id);
	component[component_counter++] = consumer;
	delete[] buffer_ids;
};

ITransformable *TheFramework::AddProvider (ISensor *isensor,
	const ssi_char_t *channel,
	IFilter *ifilter,
	ssi_time_t buffer_capacity_in_seconds,
	ssi_time_t check_interval_in_seconds,
	ssi_time_t sync_interval_in_seconds) {

	Provider *provider = new Provider (ifilter, buffer_capacity_in_seconds, check_interval_in_seconds, sync_interval_in_seconds);	
	if (!isensor->setProvider (channel, provider)) {
		return 0;
	}
	component[component_counter++] = provider;

	return provider;
};

void TheFramework::AddSensor (ISensor *isensor) {

	Sensor *sensor = new Sensor (isensor);
	component[component_counter++] = sensor;
};

ITransformable *TheFramework::AddTransformer (ITransformable *source, 
	ITransformer *itransformer,
	const ssi_char_t *frame_size,
	const ssi_char_t *delta_size,
	ssi_time_t buffer_capacity) {

	ssi_size_t frame_size_in_samples = 0;
	ParseTime (frame_size, frame_size_in_samples, source->getSampleRate ());

	ssi_size_t delta_size_in_samples = 0;
	if (delta_size) {
		ParseTime (delta_size, delta_size_in_samples, source->getSampleRate ());
	}

	Transformer *transformer = new Transformer (source->getBufferId (), itransformer, frame_size_in_samples, delta_size_in_samples, buffer_capacity);
	component[component_counter++] = transformer;

	return transformer;
};

ITransformable *TheFramework::AddTransformer (ITransformable *source, 
	ssi_size_t n_xtra_sources,
	ITransformable **xtra_sources,
	ITransformer *itransformer,
	const ssi_char_t *frame_size,
	const ssi_char_t *delta_size,
	ssi_time_t buffer_capacity) {

	int *xtra_buffer_ids = new int[n_xtra_sources];
	for (ssi_size_t i = 0; i < n_xtra_sources; i++) {
		xtra_buffer_ids[i] = xtra_sources[i]->getBufferId ();
	}

	ssi_size_t frame_size_in_samples = 0;
	ParseTime (frame_size, frame_size_in_samples, source->getSampleRate ());

	ssi_size_t delta_size_in_samples = 0;
	if (delta_size) {
		ParseTime (delta_size, delta_size_in_samples, source->getSampleRate ());
	}

	Transformer *transformer = new Transformer (source->getBufferId (), n_xtra_sources, xtra_buffer_ids, itransformer, frame_size_in_samples, delta_size_in_samples, buffer_capacity);
	component[component_counter++] = transformer;
	delete[] xtra_buffer_ids;

	return transformer;
};

}
