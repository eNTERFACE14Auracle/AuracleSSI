// Queue.h
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

#ifndef SSI_THREAD_QUEUE_H
#define SSI_THREAD_QUEUE_H

#include "thread/ThreadLibCons.h"
#include "thread/Mutex.h"

#define SSI_THREAD_QUEUE_MAXSIZE 1024

namespace ssi {

class Queue {

struct queue_t {

	void *q[SSI_THREAD_QUEUE_MAXSIZE+1];	// body of queue
    int first;								// position of first element
    int last;								// position of last element
    int count;								// number of queue elements
};

public:

	Queue ();
	virtual ~Queue ();

	bool enqueue (void *x);
	bool dequeue (void **x);
	bool empty ();
	bool full ();
	void print (void (*toString) (ssi_size_t size, ssi_char_t *str, void *x));

protected:

	queue_t q;
	Mutex mutex;
};

}

#endif

