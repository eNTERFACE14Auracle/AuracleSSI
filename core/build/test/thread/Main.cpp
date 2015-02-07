// Main.cpp
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

#include "TalkingThread.h"
#include "Store.h"
#include "User.h"
#include "Supplier.h"
#include "Printer.h"
#include "PrintJob.h"
#include "TimedThread.h"
#include "Trigger.h"
#include "Listener.h"
#include "Enqueuer.h"
#include "Dequeuer.h"
#include "thread/ThreadPool.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void ex_talking ();	
void ex_printer ();	
void ex_store ();	
void ex_timer ();	
void ex_trigger ();	
void ex_queue ();	
void ex_pool ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	// init random seed
	srand ( static_cast<unsigned int> (time (0)) );
	ssi_random_seed ();

	ex_talking ();	 // threads and mutex
	ex_printer ();	 // threads and event
	ex_store ();	 // threads and condition
	ex_timer ();	 // timer
	ex_trigger ();	 // observer pattern
	ex_queue ();	 // thread and queue
	ex_pool ();      // threadpool

	ssi_print ("\n\n\tpress enter to quit\n\n");
	getchar ();
	
#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void ex_talking () {

	Counter counter;
	TalkingThread tt1 ("guten tag ", 700, &counter);
	tt1.setName ("guten tag");
	TalkingThread tt2 ("moin moin ", 1100, &counter);
	tt2.setName ("moin moin");
	TalkingThread tt3 ("gruess gott ", 1300, &counter);
	tt3.setName ("gruess gott");
	TalkingThread tt4 ("single hello ", 3000, &counter, true); 
	tt4.setName ("single hello");

	ssi_print ("thread monitor:\n");
	Thread::PrintInfo ();

	tt1.start ();
	tt2.start ();
	tt3.start ();
	tt4.start ();

	ssi_print ("thread monitor:\n");
	Thread::PrintInfo ();

	// waits until tt4 has finished
	tt4.stop ();

	ssi_print ("thread monitor:\n");
	Thread::PrintInfo ();

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	ssi_print ("thread monitor:\n");
	Thread::PrintInfo ();

	tt1.stop ();
	tt2.stop ();
	tt3.stop ();

	ssi_print ("thread monitor:\n");
	Thread::PrintInfo ();

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();
}	


void ex_printer () {

	int job_num = 10;

	Printer printer;
	PrintJob **jobs = new PrintJob*[job_num];

	for (int i = 0; i < job_num; i++) {
		jobs[i] = new PrintJob (&printer, 3+i);
		jobs[i]->start ();
	}

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	for (int i = 0; i < job_num; i++) {
		jobs[i]->stop ();
		delete jobs[i];
	}

	delete[] jobs;
}

void ex_store () {
			
	int user_num = 5;
	int supp_num = 7;

	Store store;
	User **user = new User*[user_num];
	Supplier **supp = new Supplier*[supp_num];

	for (int i = 0; i < user_num; i++) {
		user[i] = new User (&store);
		user[i]->start ();
	}
	for (int i = 0; i < supp_num; i++) {
		supp[i] = new Supplier (&store);
		supp[i]->start ();
	}

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	store.close ();

	for (int i = 0; i < user_num; i++) {
		user[i]->stop ();
		delete user[i];
	}
	for (int i = 0; i < supp_num; i++) {
		supp[i]->stop ();
		delete supp[i];
	}

	delete[] user;
	delete[] supp;
}

void ex_timer () {

	TimedThread thread (1.0);
	thread.start ();

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	thread.stop ();

}

void ex_trigger () {

	Trigger trigger;
	Listener *listener[5];
	
	for (unsigned int i = 0; i < 5; i++) {
		listener[i] = new Listener (&trigger, i*i*100);
	}

	trigger.start ();

	for (unsigned int i = 0; i < 5; i++) {
		listener[i]->start ();
	}

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	for (unsigned int i = 0; i < 5; i++) {
		listener[i]->stop ();
	}

	trigger.stop ();

	for (unsigned int i = 0; i < 5; i++) {
		delete listener[i];
	}
}

void ex_queue () {

	Queue q;
	Enqueuer *e[5];
	Dequeuer *d[5];

	for (int i = 0; i < 5; i++) {
		e[i] = new Enqueuer (&q);
		d[i] = new Dequeuer (&q);
	}

	for (int i = 0; i < 5; i++) {
		e[i]->start ();
		d[i]->start ();
	}

	getchar ();

	for (int i = 0; i < 5; i++) {
		e[i]->stop ();
		d[i]->stop ();
	}

	for (int i = 0; i < 5; i++) {
		delete e[i];
		delete d[i];
	}
}

struct pool_in_s {
	ssi_size_t id;
	ssi_size_t sleep_ms;	
};

bool pool_job (ssi_size_t n_in, void *in, ssi_size_t n_out, void *out) {
	pool_in_s *s = ssi_pcast (pool_in_s, in);
	printf ("id=%u, sleep for %u ms\n", s->id, s->sleep_ms);
	Sleep (s->sleep_ms);
	return true;
}

void ex_pool () {

	ThreadPool tp ("mypool", 5);

	ThreadPool::job_s job[20];
	pool_in_s job_in[20];
	for (ssi_size_t i = 0; i < 20; i++) {
		job_in[i].id = i;
		job_in[i].sleep_ms = 100u + ssi_random (500u);
		job[i].n_in = sizeof (job_in[i]);
		job[i].in = &job_in[i];
		job[i].n_out = 0;
		job[i].out = 0;
		job[i].job = pool_job;
		tp.add (job[i]);
	}
	tp.work ();
}
