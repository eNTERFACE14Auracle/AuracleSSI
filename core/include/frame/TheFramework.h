// TheFramework.h
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

#pragma once

#ifndef SSI_FRAME_THEFRAMEWORK_H
#define SSI_FRAME_THEFRAMEWORK_H

#include "base/ITheFramework.h"
#include "frame/FrameLibCons.h"
#include "frame/TimeBuffer.h"
#include "base/ITransformable.h"
#include "frame/Sensor.h"
#include "frame/Consumer.h"
#include "frame/EventConsumer.h"
#include "frame/Provider.h"
#include "frame/Transformer.h"
#include "base/IRunnable.h"
#include "ioput/Socket/Socket.h"
#include "ioput/file/File.h"
#include "base/String.h"

namespace ssi {

class TimeServer;

class TheFramework : public ITheFramework {

friend class Buffer;
friend class TimeBuffer;
friend class Sensor;
friend class Provider;
friend class ConsumerBase;
friend class Consumer;
friend class Transformer;
friend class Factory;

public: 

	class Options : public OptionList {

	public:
		Options () 
			: monitor (false), mupd (100), console (false), sync (false), sport (1111), stype (Socket::UDP), slisten (false), countdown (3), tserver (false), tport (2222), info (true) {

			loglevel = SSI_LOG_LEVEL_DEFAULT;

			mpos[0] = 0;
			mpos[1] = 0;
			mpos[2] = 400;
			mpos[3] = 400;

			cpos[0] = 0;
			cpos[1] = 0;
			cpos[2] = 400;
			cpos[3] = 400;	

			addOption ("countdown", &countdown, 1, SSI_SIZE, "countdown in seconds before framework is started");		
			addOption ("console", &console, 1, SSI_BOOL, "move console window");	
			addOption ("cpos", &cpos, 4, SSI_INT, "position of console window on screen [posx,posy,width,height]");
			addOption ("monitor", &monitor, 1, SSI_BOOL, "show framework monitor");		
			addOption ("mpos", &mpos, 4, SSI_INT, "position of monitor on screen [posx,posy,width,height]");
			addOption ("mupd", &mupd, 1, SSI_SIZE, "monitor update frequency in milliseconds");		
			addOption ("sync", &sync, 1, SSI_BOOL, "turn on sync mode: if not in listen mode send sync signal containing the framework UTC start time in a SYSTEMTIME struct, otherwise wait for such a signal");
			addOption ("slisten", &slisten, 1, SSI_BOOL, "serve as client, i.e. wait for sync signal by server (only if sync option is turned on)");
			addOption ("sport", &sport, 1, SSI_INT, "sync port number (-1 for any)");		
			addOption ("stype", &stype, 1, SSI_UCHAR, "sync protocol type (0=UDP, 1=TCP)");			
			addOption ("tserver", &tserver, 1, SSI_BOOL, "start time server (waits for a TCP connection and returns current framework UTC time in a in a SYSTEMTIME struct)");
			addOption ("tport", &tport, 1, SSI_INT, "time server listening port");					
			addOption ("info", &info, 1, SSI_BOOL, "create framework info file");
			addOption ("loglevel", &loglevel, 1, SSI_INT, "log level (0=error only, 1=warnings, 2=basic, 3=detailed, 4=debug, 5=verbose");
		}

		void setMonitorPos (int x, int y, int width, int height) {
			monitor = true;
			mpos[0] = x;
			mpos[1] = y;
			mpos[2] = width;
			mpos[3] = height;
		}

		void setConsolePos (int x, int y, int width, int height) {
			console = true;
			cpos[0] = x;
			cpos[1] = y;
			cpos[2] = width;
			cpos[3] = height;
		}

		ssi_size_t countdown;

		bool console;
		int cpos[4];
		bool monitor;
		int mpos[4];		
		ssi_size_t mupd;
		bool sync;

		int sport;
		Socket::TYPE stype;
		bool slisten;
		bool info;

		bool tserver;
		int tport;	

		int loglevel;
	};

public:

	Options *getOptions () { return &_options; }
	static const ssi_char_t *GetCreateName () { return "ssi_object_TheFramework"; }
	const ssi_char_t *getName () { return GetCreateName(); }
	const ssi_char_t *getInfo () { return "Handles data buffering and communication between components of a pipeline."; }
	static IObject *Create (const char *file) { return new TheFramework (file); }

	void SetLogLevel (int level);

    void Start ();
    void Stop ();
    void Clear ();
	void Wait ();

	ssi_time_t GetStartTime ();
	ssi_size_t GetStartTimeMs ();
	void GetStartTimeLocal (ssi_size_t &year, ssi_size_t &month, ssi_size_t &day, ssi_size_t &hour, ssi_size_t &minute, ssi_size_t &second, ssi_size_t &msecond);
	void GetStartTimeSystem (ssi_size_t &year, ssi_size_t &month, ssi_size_t &day, ssi_size_t &hour, ssi_size_t &minute, ssi_size_t &second, ssi_size_t &msecond);
	ssi_time_t GetElapsedTime ();
	ssi_size_t GetElapsedTimeMs ();
	ssi_time_t GetRunTime ();
	ssi_size_t GetRunTimeMs ();	
	
	bool IsInIdleMode ();

	void SetAutoRun (bool flag) {
		_is_auto_run = flag;
	}
	bool IsAutoRun () {
		return _is_auto_run;
	}

	void AddConsumer (ITransformable *source,
		IConsumer *iconsumer, 
		const ssi_char_t *frame_size,
		const ssi_char_t *delta_size = 0,
		ITransformer *transformer = 0,
		ITransformable *trigger = 0);
	void AddConsumer (ssi_size_t n_sources, 
		ITransformable **sources,
		IConsumer *iconsumer, 
		const ssi_char_t *frame_size,
		const ssi_char_t *delta_size = 0,
		ITransformer **itransformer = 0,
		ITransformable *trigger = 0);

	void AddEventConsumer (ITransformable *source,
		IConsumer *iconsumer, 
		ITheEventBoard *event_board,
		const ssi_char_t *address,
		ITransformer *transformer = 0);
	void AddEventConsumer (ssi_size_t n_sources, 
		ITransformable **sources,
		IConsumer *iconsumer, 
		ITheEventBoard *event_board,
		const ssi_char_t *address,
		ITransformer **itransformer = 0);

	ITransformable *AddProvider (ISensor *sensor,
		const ssi_char_t *channel,
		IFilter *filter = 0,
		ssi_time_t buffer_capacity_in_seconds = THEFRAMEWORK_DEFAULT_BUFFER_CAP,
		ssi_time_t check_interval_in_seconds = 1.0,
		ssi_time_t sync_interval_in_seconds = 5.0);
	void AddSensor (ISensor *isensor);

	ITransformable *AddTransformer (ITransformable *source, 
		ITransformer *itransformer,
		const ssi_char_t *frame_size,
		const ssi_char_t *delta_size = 0,
		ssi_time_t buffer_capacity = THEFRAMEWORK_DEFAULT_BUFFER_CAP);
	ITransformable *AddTransformer (ITransformable *source, 
		ssi_size_t n_xtra_sources,
		ITransformable **xtra_sources,
		ITransformer *itransformer,
		const ssi_char_t *frame_size,
		const ssi_char_t *delta_size = 0,
		ssi_time_t buffer_capacity = THEFRAMEWORK_DEFAULT_BUFFER_CAP);

	int AddRunnable (IRunnable *runnable);
	void AddExeJob (const ssi_char_t *exe, const ssi_char_t *args, EXECUTE::list type, int wait);

	bool IsBufferInUse (int buffer_id);

	int GetData (int buffer_id, ssi_byte_t **data, ssi_size_t &samples_in, ssi_size_t &samples_out, ssi_time_t start_time, ssi_time_t duration);
	int GetData (int buffer_id, ssi_stream_t &stream, ssi_time_t start_time, ssi_time_t duration);
	int GetData (int buffer_id, ssi_byte_t *data, ssi_size_t samples, ssi_size_t position);
	int GetDataTry (int buffer_id, ssi_byte_t **data, ssi_size_t &samples_in, ssi_size_t &samples_out, ssi_time_t start_time, ssi_time_t duration);
	
	bool GetCurrentSampleTime (int buffer_id, ssi_time_t &time);
	bool GetCurrentWritePos (int buffer_id, ssi_size_t &position);
	bool SetCurrentSampleTime (int buffer_id, ssi_time_t time);
	bool GetLastAccessedSampleTime (int buffer_id, ssi_time_t &time);
	bool GetOffsetTime (int buffer_id, ssi_time_t &time);
	bool GetSampleRate (int buffer_id, ssi_time_t &sample_rate);
	bool GetTotalSampleBytes (int buffer_id, ssi_size_t &sample_bytes);
	bool GetSampleBytes (int buffer_id, ssi_size_t &sample_bytes);
	bool GetSampleType (int buffer_id, ssi_type_t &sample_type);
	bool GetSampleDimension (int buffer_id, ssi_size_t &sample_dimension);
	bool GetCapacity (int buffer_id, ssi_time_t &capacity);

protected:

	bool ParseTime (const ssi_char_t *timestr, ssi_size_t &samples, ssi_time_t sr) {
		
		if (!timestr || timestr[0] == '\0') {
			ssi_wrn ("invalid time string '%s'", timestr);
			return false;
		}

		size_t len = strlen (timestr);		
		int result = EOF;
		if (timestr[len-1] == 's') {
			ssi_time_t seconds = 0;
			result = sscanf (timestr, "%lf", &seconds);
			samples = ssi_cast (ssi_size_t, seconds * sr + 0.5);
		} else {
			result = sscanf (timestr, "%u", &samples);
		}

		if (result == EOF) {
			ssi_wrn ("invalid time string '%s'", timestr);
			return false;
		}

		return true;
	}
    
 	int AddBuffer (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_size_t sample_bytes, ssi_type_t sample_type, ssi_time_t buffer_capacity = THEFRAMEWORK_DEFAULT_BUFFER_CAP);
	bool SetMetaData (int buffer_id, ssi_size_t size, const void *meta);
	const void *GetMetaData (int buffer_id, ssi_size_t &size);
    bool RemBuffer (int buffer_id);
	bool ResetBuffer (int buffer_id, ssi_time_t offset);

    int PushData  (int buffer_id, const ssi_byte_t *data, ssi_size_t sample_number);
    int PushZeros  (int buffer_id);
	
	bool Synchronize (int buffer_id);

protected:

    TheFramework (const ssi_char_t *_file);
	~TheFramework ();
	ssi_char_t *_file;
	Options _options;

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;
	static ssi_char_t *info_file_name;

	Mutex _mutex;
	IRunnable *_monitor;

	Socket *_sync_socket;

	SYSTEMTIME _system_time;
	SYSTEMTIME _local_time;
	ssi_size_t _start_run_time;
	ssi_size_t _last_run_time;

	volatile bool _is_running;
	bool _is_auto_run;

    // buffer array
    Buffer *buffer[THEFRAMEWORK_BUFFER_NUM];
	// runnable array
	IRunnable *runnable[THEFRAMEWORK_THREAD_NUM];
	// component array
	IRunnable *component[THEFRAMEWORK_COMPONENT_NUM];
	ssi_size_t component_counter;
	// mutex to lock buffer
	Mutex bufferMutex[THEFRAMEWORK_BUFFER_NUM];
	// condition variables for full buffers
	Condition bufferCondFull[THEFRAMEWORK_BUFFER_NUM];
	// condition variables for empty buffers
	Condition bufferCondEmpty[THEFRAMEWORK_BUFFER_NUM];
	// is buffer in use?
    bool bufferInUse[THEFRAMEWORK_BUFFER_NUM];
	// time between two synchronized push calls
	//ssi_size_t syncDur[THEFRAMEWORK_BUFFER_NUM];
	//ssi_size_t syncDurCounter[THEFRAMEWORK_BUFFER_NUM];

	// executable jobs 
	struct job_s {
		String exe; 
		String args;
		EXECUTE::list type;
		int wait; 
	};
	std::vector<job_s> _jobs;

	File *_info;
	TimeServer *_tserver;
};

}

#endif // THEFRAMEWORK_H
