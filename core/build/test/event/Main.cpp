// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/10/11
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

void ex_address ();
void ex_eboard ();
void ex_frame ();
void ex_slider ();
void ex_thresclass ();

ssi_char_t string[SSI_MAX_CHAR];

class MySender : public IEventSender {

	IEventListener *_listener;
	ssi_event_t _e;
public:
	bool setEventListener (IEventListener *listener) {		
		static ssi_size_t _scount = 0;
		static ssi_size_t _ecount = 0;
		_listener = listener;
		ssi_sprint (string, "s%u", _scount++);
		ssi_size_t sid = Factory::AddString (string);
		ssi_sprint (string, "e%u", _ecount++);
		ssi_size_t eid = Factory::AddString (string);
		ssi_event_init (_e, SSI_ETYPE_EMPTY, sid, eid); 
		return true;
	}	
	void send_enter () {
		fire ();
	}
	void fire () {
		static ssi_size_t start_ms = ssi_time_ms ();
		_e.time = ssi_time_ms () - start_ms;
		_listener->update (_e);
	}
	void send_flush () {
		fire ();
	}
};

class MyListener : public IEventListener {
	bool update (IEvents &es, ssi_size_t n_new_events, ssi_size_t time_ms) {
		ssi_print ("CALLBACK\n\n");
		if (n_new_events > 0) {
			Factory::GetEventBoard ()->Print (es);
			ssi_print ("\n");
		} else {
			ssi_print ("no new events\n"); // must never happen
		}
		return true;
	}
	IOptions *getOptions () { return 0; }
	const ssi_char_t *getName () { return 0; }
	const ssi_char_t *getInfo () { return 0; }
};

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main () {	

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssimodel.dll");	
	Factory::RegisterDLL ("ssiioput.dll");	

	ssi_random_seed ();

	//ex_address ();
	//ex_eboard ();
	ex_frame ();
	ex_thresclass();
	ex_slider ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void ex_address () {

	EventAddress ea;

	ea.print ();

	ea.setAddress ("e1@s1,s2");
	ea.print ();

	ea.setEvents ("e2,e3");
	ea.print ();

	ea.setSender ("s3");
	ea.print ();

	ea.setAddress ("e4,e5@s4");
	ea.print ();

	ea.clear ();
	ea.setAddress ("e1@s1,s2");
	ea.print ();

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();	
}

void ex_eboard () {

	ITheEventBoard *board = Factory::GetEventBoard ();

	const ssi_size_t n_sender = 5;
	MySender sender[n_sender];
	for (ssi_size_t i = 0; i < n_sender; i++) {		
		board->RegisterSender (sender[i]);
	}

	EventAddress ea;
	ea.setEvents ("e0,e1,e2,e3,e4");
	ea.setSender ("s0,s1,s2,s3,s4");
	ssi_print ("address=%s\n", ea.getAddress ());
	MyListener listener;
	board->RegisterListener (listener, ea.getAddress ());

	board->Start ();

	for (ssi_size_t i = 0; i < 200; i++) {
		ssi_size_t index = ssi_random (n_sender-1);
		sender[index].fire ();				
		Sleep (10);
	}

	board->Stop ();
	board->Clear ();

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();	
}

void ex_frame () {

	ITheEventBoard *board = Factory::GetEventBoard ("board");
	ITheFramework *frame = Factory::GetFramework ("frame");

	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	mouse->getOptions ()->flip = true;
	mouse->getOptions ()->scale = false;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);	
	frame->AddSensor (mouse);
	
	ZeroEventSender *ezero = ssi_create (ZeroEventSender, "ezero", true);
	ezero->getOptions ()->mindur = 0.2;	
	ezero->getOptions ()->setSender ("mouse");
	ezero->getOptions ()->setEvent ("click");
	ezero->getOptions ()->eager = true;
	frame->AddConsumer (button_p, ezero, "0.25s");
	board->RegisterSender (*ezero);
	
	FloatsEventSender *flt_sender = ssi_create (FloatsEventSender, "flt_sender", true);
	flt_sender->getOptions ()->setSenderName ("mouse");
	flt_sender->getOptions ()->setEventName ("position");
	frame->AddConsumer (cursor_p, flt_sender, "2.0s");
	board->RegisterSender (*flt_sender);

	FunctionalsEventSender *funct_sender = ssi_create (FunctionalsEventSender, "funct_sender", true);
	funct_sender->getOptions ()->setSenderName ("mouse");
	funct_sender->getOptions ()->setEventName ("features");
	ssi_strcpy (funct_sender->getOptions ()->names, "");
	frame->AddEventConsumer (cursor_p, funct_sender, board, ezero->getEventAddress ());
	board->RegisterSender (*funct_sender);
	
	Trainer trainer;
	Trainer::Load (trainer, "mouse");	
	Classifier *classifier = ssi_pcast (Classifier, Factory::Create (Classifier::GetCreateName ()));
	classifier->setTrainer (trainer);	
	classifier->getOptions ()->setSenderName ("mouse");
	classifier->getOptions ()->setEventName ("gesture");
	classifier->getOptions ()->console = false;
	frame->AddEventConsumer (cursor_p, classifier, board, ezero->getEventAddress ());	
	board->RegisterSender (*classifier);
	
	EventMonitor *monitor = ssi_create (EventMonitor, 0, true);
	monitor->getOptions ()->all = true;
	monitor->getOptions ()->chars = 500;
	monitor->getOptions ()->update_ms = 1000;
	//const ssi_char_t *address = "gesture,button,position@zsender,classifier,flt_sender,funct_sender";	
	const ssi_char_t *address = "@";	
	board->RegisterListener (*monitor, address, 10000, IEvents::EVENT_STATE_FILTER::ZERODUR);

	FileEventWriter *writer = ssi_create (FileEventWriter, 0, true);
	writer->getOptions ()->setPath ("mouse");
	board->RegisterListener (*writer);

	frame->Start ();
	board->Start ();
	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();	
	frame->Stop ();
	board->Stop ();
	frame->Clear ();			
	board->Clear ();
}

void ex_slider () {

	ITheEventBoard *board = Factory::GetEventBoard ("board");
	ITheFramework *frame = Factory::GetFramework ("frame");
	IThePainter *painter = Factory::GetPainter();

	Selector *select = ssi_create(Selector, 0, true);
	select->getOptions()->set(1);

	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->scale = true;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME, select);
	frame->AddSensor (mouse);

	EventSlider *slider = 0;
	EventAddress address;
	
	slider = ssi_create (EventSlider, 0, true);
	slider->getOptions ()->setEvent ("min");
	slider->getOptions ()->setSender ("slider");
	slider->getOptions ()->setValue ("min");
	slider->getOptions ()->maxval = 0.5f;
	slider->getOptions ()->defval = 0.0f;
	slider->getOptions ()->setSliderPos (410, 405, 400, 75);
	board->RegisterSender (*slider);
	address.setAddress (slider->getEventAddress ());

	slider = ssi_create (EventSlider, 0, true);
	slider->getOptions ()->setEvent ("max");
	slider->getOptions ()->setSender ("slider");
	slider->getOptions ()->setValue ("max");
	slider->getOptions ()->minval = 0.5f;
	slider->getOptions ()->defval = 1.0f;
	slider->getOptions ()->setSliderPos (410, 490, 400, 75);
	board->RegisterSender (*slider);
	address.setAddress (slider->getEventAddress ());

	Limits *limits = ssi_create (Limits, 0, true);
	ITransformable* limits_t = frame->AddTransformer (cursor_p, limits, "10");
	board->RegisterListener (*limits, address.getAddress ());

	SignalPainter* paint = 0;
	
	paint = ssi_create(SignalPainter, 0, true);
	paint->getOptions ()->type = PaintSignalType::SIGNAL;
	paint->getOptions ()->size = 10;
	paint->getOptions ()->setName("Cursor");
	paint->getOptions ()->autoscale = false;
	paint->getOptions ()->fix[0] = 0;
	paint->getOptions ()->fix[1] = 1;
	frame->AddConsumer(cursor_p, paint, "0.1s");

	paint = ssi_create(SignalPainter, 0, true);
	paint->getOptions ()->type = PaintSignalType::SIGNAL;
	paint->getOptions ()->size = 10;
	paint->getOptions ()->setName("Limits");
	paint->getOptions ()->autoscale = false;
	paint->getOptions ()->fix[0] = 0;
	paint->getOptions ()->fix[1] = 1;
	frame->AddConsumer(limits_t, paint, "0.1s");
	
	frame->Start ();
	board->Start ();
	
	painter->Arrange (1,2,400,0,600,400);
	painter->MoveConsole (0,0,400,600);

	ssi_print ("press enter to continue\n");
	getchar ();	

	frame->Stop ();
	board->Stop ();
	frame->Clear ();			
	board->Clear ();
}

void ex_thresclass () {

	ITheEventBoard *board = Factory::GetEventBoard ("board");
	ITheFramework *frame = Factory::GetFramework ("frame");
	IThePainter *painter = Factory::GetPainter();

	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->scale = true;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse);
	
	Selector *select = ssi_create(Selector, 0, true);
	select->getOptions()->set(1);
	ITransformable* selector_t = frame->AddTransformer(cursor_p, select, "1");

	ThresClassEventSender *tc = ssi_create (ThresClassEventSender, 0, true);
	tc->getOptions ()->setClasses("low, med, high");
	tc->getOptions ()->setThresholds("0.1, 0.5, 0.7");
	tc->getOptions ()->minDiff = 0.01f;
	frame->AddConsumer (selector_t, tc, "1");
	board->RegisterSender (*tc);	

	SignalPainter* paint = ssi_create(SignalPainter, 0, true);
	paint->getOptions()->type = PaintSignalType::SIGNAL;
	paint->getOptions()->size = 10;
	paint->getOptions()->setName("Mouse Y-Coord");
	frame->AddConsumer(selector_t, paint, "0.1s");
	
	EventMonitor *monitor = ssi_create (EventMonitor, 0, true);
	monitor->getOptions ()->all = true;
	monitor->getOptions ()->chars = 500;
	monitor->getOptions()->setMonitorPos(600, 300, 600, 300);
	const ssi_char_t *address = "@";	
	board->RegisterListener (*monitor, address, 10000);

	frame->Start ();
	board->Start ();
	
	painter->Arrange (1,1,600,0,600,300);
	painter->MoveConsole (200,0,400,600);

	ssi_print ("press enter to continue\n");
	getchar ();	
	frame->Stop ();
	board->Stop ();
	frame->Clear ();			
	board->Clear ();
}