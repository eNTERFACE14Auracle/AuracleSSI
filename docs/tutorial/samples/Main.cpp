// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/04/24
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

void ex_filter ();
void ex_feature ();
void ex_audio ();
void ex_audio_tr ();
void ex_video ();
void ex_gesture ();

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssiaudio.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimodel.dll");
	Factory::RegisterDLL ("ssiimage.dll");
	Factory::RegisterDLL ("ssicamera.dll");

	ex_filter ();
	ex_feature ();
	ex_audio ();
	ex_audio_tr ();
	ex_video ();
	ex_gesture ();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

	return 0;
}

void ex_filter () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	Mouse *mouse = ssi_create (Mouse, 0, true);
	mouse->getOptions ()->sr = 50.0;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse); 

	Selector *selector = ssi_create (Selector, 0, true);
	ssi_size_t inds[1] = {1};
	selector->getOptions ()->set (1, inds);
	ITransformable *selector_t = frame->AddTransformer (cursor_p, selector, "0.05s");

	Noise *noise = ssi_create (Noise, 0, true);
	noise->getOptions ()->ampl = 0.5f;
	ITransformable *noise_t = frame->AddTransformer (selector_t, noise, "0.05s");

	Butfilt *lowfilt = ssi_create (Butfilt, 0, true);
	lowfilt->getOptions ()->type = Butfilt::LOW;
	lowfilt->getOptions ()->low = 1;
	lowfilt->getOptions ()->order = 3;
	lowfilt->getOptions ()->norm = false;
	lowfilt->getOptions ()->zero = true;
	ITransformable *lowfilt_t = frame->AddTransformer (noise_t, lowfilt, "0.05s");

	Butfilt *highfilt = ssi_create (Butfilt, 0, true);
	highfilt->getOptions ()->type = Butfilt::HIGH;
	highfilt->getOptions ()->high = 1;
	highfilt->getOptions ()->norm = false;
	highfilt->getOptions ()->order = 3;
	ITransformable *highfilt_t = frame->AddTransformer (noise_t, highfilt, "0.05s");

	SignalPainter *sigpaint = 0;

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("raw");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (selector_t, sigpaint, "0.1s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("noisy");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (noise_t, sigpaint, "0.1s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("lowfilt");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (lowfilt_t, sigpaint, "0.1s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("highfilt");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (highfilt_t, sigpaint, "0.1s");

	frame->Start ();
	painter->Arrange (1, 4, 0, 0, 400, 800);
	painter->MoveConsole (400, 0, 400, 800);
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void ex_feature () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
			
	Mouse *mouse = ssi_create (Mouse, 0, true);
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse); 
	
	Selector *selector = ssi_create (Selector, 0, true);
	selector->getOptions ()->set (0);
	ITransformable *selector_t = frame->AddTransformer (cursor_p, selector, "0.02s");

	Derivative *drvt = ssi_create (Derivative, 0, true);
	drvt->getOptions ()->set (Derivative::D0TH | Derivative::D1ST | Derivative::D2ND);
	ITransformable *drvt_t = frame->AddTransformer (selector_t, drvt, "0.02s");

	Functionals *functs = ssi_create (Functionals, 0, true);
	functs->getOptions ()->addName ("mean");
	functs->getOptions ()->addName ("std");
	functs->getOptions ()->addName ("min");
	functs->getOptions ()->addName ("max");
	ITransformable *functs_t = frame->AddTransformer (drvt_t, functs, "0.02s", "0.08s");

	FileWriter *filewrite = ssi_create (FileWriter, 0, true);
	filewrite->getOptions ()->type = File::ASCII;
	filewrite->getOptions ()->setFormat (" ", ".2");
	frame->AddConsumer (functs_t, filewrite, "0.02s");	

	SignalPainter *sigpaint = 0;

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("cursor");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (selector_t, sigpaint, "0.1s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("derivative");
	sigpaint->getOptions ()->size = 10.0;
	frame->AddConsumer (drvt_t, sigpaint, "0.1s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("functionals");
	sigpaint->getOptions ()->size = 10.0;	
	frame->AddConsumer (functs_t, sigpaint, "0.1s");

	frame->Start ();
	painter->Arrange (1, 3, 0, 0, 400, 800);
	painter->MoveConsole (400, 0, 400, 800);
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void ex_audio () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	Audio *audio = ssi_create (Audio, "audio", true);
	audio->getOptions ()->scale = true;
	audio->getOptions ()->block = 0.01;
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	Spectrogram *spect = ssi_create (Spectrogram, 0, true);
	Matrix<ssi_real_t> *filterbank = FilterTools::Filterbank (512, audio_p->getSampleRate (), 50, 100, 5100, WINDOW_TYPE_RECTANGLE);
	spect->setFilterbank (filterbank);	
	delete filterbank;
	ITransformable *spect_t = frame->AddTransformer (audio_p, spect, "0.01s", "0.015s");

	MFCC *mfcc = ssi_create (MFCC, 0, true);
	mfcc->getOptions ()->n_first = 0;
	mfcc->getOptions ()->n_last = 13;
	ITransformable *mfcc_t = frame->AddTransformer (audio_p, mfcc, "0.01s", "0.015s");

	SignalPainter *sigpaint = 0;

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("audio");
	sigpaint->getOptions ()->type = PaintSignalType::AUDIO;
	sigpaint->getOptions ()->size = 10.0;		
	frame->AddConsumer (audio_p, sigpaint, "0.02s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("spectrogram");
	sigpaint->getOptions ()->size = 10.0;
	sigpaint->getOptions ()->type = PaintSignalType::IMAGE;
	frame->AddConsumer (spect_t, sigpaint, "0.02s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("mfccs");
	sigpaint->getOptions ()->size = 10.0;
	sigpaint->getOptions ()->type = PaintSignalType::IMAGE;
	frame->AddConsumer (mfcc_t, sigpaint, "0.02s");

	frame->Start ();
	painter->Arrange (1, 3, 0, 0, 400, 800);
	painter->MoveConsole (400, 0, 400, 800);
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();

}

void ex_audio_tr () {

	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();
	
	Audio *audio = ssi_create (Audio, "audio", true);
	audio->getOptions ()->block = 0.01;
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	AudioActivity *activity = ssi_create (AudioActivity, 0, true);
	activity->getOptions ()->method = AudioActivity::LOUDNESS;
	activity->getOptions ()->threshold = 0.15f;
	ITransformable *activity_t = frame->AddTransformer (audio_p, activity, "0.02s", "0.18s");

	ZeroEventSender *ezero = ssi_create (ZeroEventSender, 0, true);
	ezero->getOptions ()->hangin = 5;
	ezero->getOptions ()->hangin = 20;
	frame->AddConsumer (activity_t, ezero, "0.2s");
	board->RegisterSender (*ezero);

	SignalPainter *sigpaint = 0;	

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("audio (tr)");
	sigpaint->getOptions ()->size = 0;	
	sigpaint->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddEventConsumer (audio_p, sigpaint, board, ezero->getEventAddress ());

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("audio");
	sigpaint->getOptions ()->size = 10.0;		
	sigpaint->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, sigpaint, "0.02s");

	sigpaint = ssi_create (SignalPainter, 0, true);
	sigpaint->getOptions ()->setName ("activity");
	sigpaint->getOptions ()->size = 10.0;			
	frame->AddConsumer (activity_t, sigpaint, "0.02s");

	frame->Start ();
	board->Start ();
	painter->Arrange (1, 3, 0, 0, 400, 800);
	painter->MoveConsole (400, 0, 400, 800);
	getchar ();
	frame->Stop ();
	board->Stop ();
	frame->Clear ();
	board->Clear ();
	painter->Clear ();

}

void ex_video () {

	IThePainter *painter = Factory::GetPainter ();
	ITheFramework *frame = Factory::GetFramework ();

	// sensor
	
	Camera *camera = ssi_pcast (Camera, Factory::Create (Camera::GetCreateName (), "camera"));
	ITransformable *camera_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME, 0, 2.0);
	camera->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	frame->AddSensor (camera);

	Mouse *mouse = ssi_create (Mouse, 0, true);
	mouse->getOptions ()->scale = true;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse);

	// transform
	
	CVColor *cv_color = ssi_create (CVColor, 0, true);
	cv_color->getOptions ()->code = CVColor::RGB2GRAY;
	ITransformable *cv_color_t = frame->AddTransformer (camera_p, cv_color, "1");

	CVChange *cv_change = ssi_create (CVChange, 0, true);
	ITransformable *cv_chain_t = frame->AddTransformer (cv_color_t, cv_change, "1");

	ITransformable *channels[1] = {cursor_p};
	CVCrop *cv_crop = ssi_create (CVCrop, 0, true);
	cv_crop->getOptions ()->width = 160;
	cv_crop->getOptions ()->height = 120;
	cv_crop->getOptions ()->region[2] = 0.5f;
	cv_crop->getOptions ()->region[3] = 0.5f;
	cv_crop->getOptions ()->scaled = true;	
	cv_crop->getOptions ()->origin = CVCrop::ORIGIN::CENTER;
	ITransformable *cv_crop_t = frame->AddTransformer (camera_p, 1, channels, cv_crop, "1");

	CVResize *cv_resize = ssi_create (CVResize, 0, true);
	cv_resize->getOptions ()->width = 150;
	cv_resize->getOptions ()->height = 150;
	ITransformable *cv_resize_t = frame->AddTransformer (camera_p, cv_resize, "1");

	// save

	CVSave *cv_save = ssi_factory_create (CVSave, 0, true);
	cv_save->getOptions ()->set ("image", CVSave::BMP);
	cv_save->getOptions ()->number = false;
	frame->AddConsumer (camera_p, cv_save, "1");

	// plot

	VideoPainter *vidplot = 0;

	vidplot = ssi_create (VideoPainter, 0, true);
	vidplot->getOptions ()->setName ("video");
	frame->AddConsumer (cv_color_t, vidplot, "1");

	vidplot = ssi_create (VideoPainter, 0, true);
	vidplot->getOptions ()->setName ("change");
	frame->AddConsumer (cv_chain_t, vidplot, "1");

	vidplot = ssi_create (VideoPainter, 0, true);
	vidplot->getOptions ()->setName ("crop");
	frame->AddConsumer (cv_crop_t, vidplot, "1");

	vidplot = ssi_create (VideoPainter, 0, true);
	vidplot->getOptions ()->setName ("resize");
	vidplot->getOptions ()->scale = false;	
	frame->AddConsumer (cv_resize_t, vidplot, "1");

	// run

	frame->Start ();
	painter->Arrange (2, 2, 0, 0, camera->getFormat ().widthInPixels, camera->getFormat ().heightInPixels);
	painter->MoveConsole (camera->getFormat ().widthInPixels, 0, 400, 800);
	ssi_print ("Press enter to stop!\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();

	painter->Clear ();
}

void ex_gesture () {

	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();
	painter->MoveConsole (400, 0, 400, 800);

	SampleList sample_list;
	unsigned int iterations, class_num;
	char class_name[1024];

	// mouse	
	Mouse *mouse = ssi_create (Mouse, 0, true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	frame->AddSensor (mouse);

	// trigger
	ZeroEventSender *ezero = ssi_create (ZeroEventSender, 0, true);
	ezero->getOptions ()->mindur = 0.2;
	frame->AddConsumer (button_p, ezero, "0.25s");
	board->RegisterSender (*ezero);

	EventConsumer *trigger = ssi_create (EventConsumer, 0, true);
	board->RegisterListener (*trigger, ezero->getEventAddress ());

	// plot
	SignalPainter *plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse");
	plot->getOptions ()->type = PaintSignalType::PATH;
	plot->getOptions ()->setMove (0, 0, 400, 400);	
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
	
	ssi_print ("\n\n  draw gestures while pressing left mouse button\n\n\n");

	frame->Start ();
	board->Start ();
	collector->wait ();
	board->Stop ();
	frame->Stop ();
	trigger->clear ();

	trigger->AddConsumer (cursor_p, plot);

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

	frame->Start ();
	board->Start ();
	classifier->wait ();
	board->Stop ();
	frame->Stop ();		
	frame->Clear ();
	painter->Clear ();
}
