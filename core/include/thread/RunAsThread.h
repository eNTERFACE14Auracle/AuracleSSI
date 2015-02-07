// RunAsThread.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2014/05/13
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

#ifndef SSI_THREAD_RUNASTHREAD_H
#define SSI_THREAD_RUNASTHREAD_H

#include "thread/Thread.h"

namespace ssi {
 
class RunAsThread : public Thread {

public:

	typedef void (*fp_run_t)(void *ptr);

	RunAsThread (fp_run_t fp, 
		void *ptr,
		bool single_execution = false,
		ssi_size_t timeout_in_ms = 10000);
	virtual ~RunAsThread ()  {};
	
	static void SetLogLevel (int level) {
		ssi_log_level = level;
	}

	void run ();

protected:

	static int ssi_log_level;
	static ssi_char_t *ssi_log_name;

	fp_run_t _fp;
	void *_ptr;

};


}

#endif
