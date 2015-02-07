// Queue.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/09/08
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

#include "thread/Queue.h"
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

Queue::Queue () {

	q.first = 0;
    q.last = SSI_THREAD_QUEUE_MAXSIZE-1;
    q.count = 0;

	for (ssi_size_t i = 0; i < SSI_THREAD_QUEUE_MAXSIZE; i++) {
		q.q[i] = 0;
	}
}

Queue::~Queue () {
}

bool Queue::enqueue (void *x) {

	if (full ()) {
		return false;
	}

	{
		Lock lock (mutex);
		     
		q.last = (q.last+1) % SSI_THREAD_QUEUE_MAXSIZE;
		q.q[q.last] = x;    
		q.count++;
	}

	return true;
}

bool Queue::dequeue (void **x) {

	if (empty ()) {
		return false;
	}

	{
		Lock lock (mutex);
	     
		*x = q.q[q.first];
		q.q[q.first] = 0;
		q.first = (q.first+1) % SSI_THREAD_QUEUE_MAXSIZE;
		q.count--;
	}

	return true;
}

bool Queue::empty () {

	int count;

	{
		Lock lock (mutex);
		count = q.count;
	}

	return count <= 0;
}

bool Queue::full () {

	int count;

	{
		Lock lock (mutex);
		count = q.count;
	}

	return count >= SSI_THREAD_QUEUE_MAXSIZE;
}

void Queue::print (void (*toString) (ssi_size_t size, ssi_char_t *str, void *x)) {

	{
		Lock lock (mutex);

		ssi_print ("[");
		if (empty ()) {
			ssi_print ("]\n");
			return;
		}

		int i;

		i = q.first; 
		ssi_char_t string[SSI_MAX_CHAR];
		while (i != q.last) {
			toString (SSI_MAX_CHAR, string, q.q[i]);
			ssi_print ("%s ", string);
			i = (i+1) % SSI_THREAD_QUEUE_MAXSIZE;
		}
		toString (SSI_MAX_CHAR, string, q.q[i]);
		ssi_print ("%s ]\n", string);
	}
}

}
