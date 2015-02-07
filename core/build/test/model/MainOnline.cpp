// MainOnline.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/11
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

#include "ssi.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void ex_online (); 

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssimodel.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");

	ex_online (); 	 
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_online () {

	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (0, 600, 600, 400);
	IThePainter *painter = Factory::GetPainter ();
	ITheEventBoard *board = Factory::GetEventBoard ();

	SampleList sample_list;
	unsigned int iterations, class_num;
	char class_name[1024];

	// mouse
	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	frame->AddSensor (mouse);

	// trigger
	ZeroEventSender *ezero = ssi_create (ZeroEventSender, "ezero", true);
	ezero->getOptions ()->mindur = 0.2;
	frame->AddConsumer (button_p, ezero, "0.25s");	
	board->RegisterSender (*ezero);
	
	EventConsumer *trigger = ssi_create (EventConsumer, 0, true);	
	board->RegisterListener (*trigger, ezero->getEventAddress ());

	// plot
	SignalPainter *plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse");
	plot->getOptions ()->setMove (0, 0, 600, 600);
	trigger->AddConsumer (cursor_p, plot);

	// record
	ssi_print ("number of class> ");
	scanf ("%d", &class_num);
	ssi_print ("iterations> ");
	scanf ("%d", &iterations);
	Collector *collector = ssi_create (Collector, 0, true);
	collector->getOptions ()->setUserName ("user");
	collector->getOptions ()->iter = iterations;
	collector->setSampleList (sample_list);	
	for (unsigned int i = 0; i < class_num; i++) {
		ssi_print ("name of class %d from %d> ", i+1, class_num);
		scanf ("%s", class_name);
		collector->getOptions ()->addClassName (class_name);
	}
	trigger->AddConsumer (cursor_p, collector);	

	// run
	frame->Start ();
	board->Start ();
	ssi_print ("\n\n  Perform gesture while pressing left mouse button..\n\n\n");
	collector->wait ();
	board->Stop ();
	frame->Stop ();
	painter->Clear ();
	trigger->clear ();

	// view
	sample_list.print (stdout);
	
	// create, eval and save model
	{
		ssi_print ("create trainer..\n");
		Dollar$1 *model = ssi_create (Dollar$1, 0, true);
		Trainer trainer (model);

		ssi_print ("evaluate trainer..\n");
		Evaluation eval;
		eval.evalLOO (trainer, sample_list);
		eval.print ();

		ssi_print ("train classifier..\n");
		trainer.train (sample_list);
		trainer.save ("gesture.model");
	}

	Trainer trainer;
	Trainer::Load (trainer, "gesture.model");	

	// test
	ssi_print ("start testing..\n");
	Classifier *classifier = ssi_create (Classifier, 0, true);
	classifier->setTrainer (trainer);	
	trigger->AddConsumer (cursor_p, classifier);
	trigger->AddConsumer (cursor_p, plot);

	frame->Start ();
	board->Start ();
	classifier->wait ();
	frame->Stop ();
	board->Stop ();
	frame->Clear ();
	board->Clear ();
	painter->Clear ();
}

