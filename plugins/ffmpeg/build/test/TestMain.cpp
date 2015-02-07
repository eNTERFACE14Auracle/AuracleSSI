// TestMain.cpp
// author: Florian Lingenfelser <lingenfelser@hcm-lab.de>
// created: 2013/08/06
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
#include "ssiffmpeg.h"
using namespace ssi;

//#include <vld.h>

void ex_output_file ();
void ex_input_file ();
void ex_output_stream ();
void ex_input_stream ();

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
	Factory::RegisterDLL ("ssiffmpeg.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssiaudio.dll");
	Factory::RegisterDLL ("ssiimage.dll");

	ex_output_file ();
	ex_input_file ();
	ex_output_stream ();
	ex_input_stream ();
	

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void ex_output_file () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	// audio	
	Audio *microphone = ssi_create (Audio, "audio", true);
	microphone->getOptions ()->scale = true;
	ITransformable *audio = frame->AddProvider (microphone, SSI_AUDIO_PROVIDER_NAME);	
	frame->AddSensor (microphone);

	// camera	
	Camera *camera = ssi_create (Camera, "camera", true);
	ITransformable *video = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME, 0, 2.0);	
	frame->AddSensor (camera);	

	// flip component
	CVFlip *flip = 0;	

	// writer		
	FFMPEGWriter *avwrite = ssi_create  (FFMPEGWriter, "avwrite", true);
	avwrite->getOptions ()->setUrl ("out.mp4");
	ITransformable *ts[2] = {video, audio};
	flip = ssi_create (CVFlip, 0, true);
	flip->getOptions ()->flip = true;
	ITransformer *tts[2] = {flip, 0};
	frame->AddConsumer (2, ts, avwrite, "1", 0, tts);
	
	FFMPEGWriter *vwrite = ssi_create  (FFMPEGWriter, "vwrite", true);
	vwrite->getOptions ()->setUrl ("out.mpeg");
	flip = ssi_create (CVFlip, 0, true);
	flip->getOptions ()->flip = true;
	frame->AddConsumer (video, vwrite, "1", 0, flip);
	
	FFMPEGWriter *awrite = ssi_create  (FFMPEGWriter, "awrite", true);
	awrite->getOptions ()->setUrl ("out.mp3");
	frame->AddConsumer (audio, awrite, "0.01s");

	// plot	
	VideoPainter *cam_plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	cam_plot->getOptions ()->setName ("video");
	frame->AddConsumer (video, cam_plot, "1");

	SignalPainter *aplot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	aplot->getOptions ()->setName ("audio");
	aplot->getOptions ()->size = 2.0;		
	aplot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio, aplot, "0.01s");

	WavWriter *wavwrite = ssi_create (WavWriter, 0, true);
	wavwrite->getOptions ()->setPath ("out.wav");
	frame->AddConsumer (audio, wavwrite, "0.01s");

	// run framework
	frame->Start ();
	painter->Arrange (1,2,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	ssi_print ("\n\n\tpress enter to continue..\n\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void ex_input_file () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	ssi_time_t offset = 0.0;

	FFMPEGReader *avread = ssi_create (FFMPEGReader, "avread", true);
	//avread->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	avread->getOptions ()->setUrl ("out.mp4");
	avread->getOptions ()->stream = false;
	avread->getOptions ()->buffer = 1.0;	
	avread->getOptions ()->offset = offset;
	ITransformable *video = frame->AddProvider (avread, SSI_FFMPEGREADER_VIDEO_PROVIDER_NAME);
	ITransformable *audio = frame->AddProvider (avread, SSI_FFMPEGREADER_AUDIO_PROVIDER_NAME);
	frame->AddSensor (avread);
	
	FFMPEGReader *vread = ssi_create (FFMPEGReader, "vread", true);
	//vread->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	vread->getOptions ()->setUrl ("out.mpeg");
	vread->getOptions ()->stream = false;
	vread->getOptions ()->buffer = 1.0;
	vread->getOptions ()->width = 640;
	vread->getOptions ()->height = 480;
	vread->getOptions ()->offset = 0; // offset won't work
	ITransformable *video2 = frame->AddProvider (vread, SSI_FFMPEGREADER_VIDEO_PROVIDER_NAME);	
	frame->AddSensor (vread);
	
	FFMPEGReader *aread = ssi_create (FFMPEGReader, "aread", true);
	//aread->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	aread->getOptions ()->setUrl ("out.mp3");
	aread->getOptions ()->stream = false;
	aread->getOptions ()->buffer = 1.0;	
	aread->getOptions ()->ablock = 0.01;	
	aread->getOptions ()->offset = offset;
	ITransformable *audio2 = frame->AddProvider (aread, SSI_FFMPEGREADER_AUDIO_PROVIDER_NAME);	
	frame->AddSensor (aread);
	
	VideoPainter *vpaint = ssi_create (VideoPainter, 0, true);
	vpaint->getOptions ()->setName ("video");
	vpaint->getOptions ()->flip = false;
	frame->AddConsumer (video, vpaint, "1");

	vpaint = ssi_create (VideoPainter, 0, true);
	vpaint->getOptions ()->setName ("video 2");
	vpaint->getOptions ()->flip = false;
	frame->AddConsumer (video2, vpaint, "1");

	SignalPainter *apaint;
	
	apaint = ssi_create (SignalPainter, 0, true);
	apaint->getOptions ()->setName ("audio");
	apaint->getOptions ()->size = 3.0;		
	apaint->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio, apaint, "0.1s");

	apaint = ssi_create (SignalPainter, 0, true);
	apaint->getOptions ()->setName ("audio");
	apaint->getOptions ()->size = 3.0;		
	apaint->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio2, apaint, "0.1s");

	WavWriter *wavwrite = ssi_create (WavWriter, 0, true);
	wavwrite->getOptions ()->setPath ("check.wav");
	frame->AddConsumer (audio, wavwrite, "0.1s");

	AudioPlayer *aplay = ssi_create (AudioPlayer, "aplayer", true);
	aplay->getOptions ()->numBuffers = 64;
	aplay->getOptions ()->bufferSizeSamples = 265;
	frame->AddConsumer (audio, aplay, "0.01s");
	
	frame->Start();
	painter->Arrange (2,2,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	ssi_print ("\n\n\tpress enter to continue..\n\n");
	getchar();

	frame->Stop();
	painter->Clear();
	frame->Clear();
}

void ex_input_stream () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	FFMPEGReader *vread = ssi_create (FFMPEGReader, "vstreamin", true);
	//avread->SetLogLevel (SSI_LOG_LEVEL_DEBUG);
	//vread->getOptions ()->setUrl ("my-video.sdp");
	vread->getOptions ()->setUrl ("udp://127.0.0.1:1111");
	vread->getOptions ()->stream = true;
	vread->getOptions ()->buffer = 1.0;
	vread->getOptions ()->fps = 25.0;
	vread->getOptions ()->width = 320;
	vread->getOptions ()->height = 240;
	ITransformable *video = frame->AddProvider (vread, SSI_FFMPEGREADER_VIDEO_PROVIDER_NAME);	
	frame->AddSensor (vread);

	FFMPEGReader *aread = ssi_create (FFMPEGReader, "astreamin", true);
	//avread->SetLogLevel (SSI_LOG_LEVEL_DEBUG);
	//aread->getOptions ()->setUrl ("my-audio.sdp");
	aread->getOptions ()->setUrl ("udp://127.0.0.1:2222");
	aread->getOptions ()->stream = true;
	aread->getOptions ()->buffer = 1.0;
	aread->getOptions ()->asr = 16000;
	ITransformable *audio = frame->AddProvider (aread, SSI_FFMPEGREADER_AUDIO_PROVIDER_NAME);	
	frame->AddSensor (aread);
	
	// plot
	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 2.0;		
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio, audio_plot, "0.01s");
	
	VideoPainter *cam_plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	cam_plot->getOptions ()->setName ("video");
	frame->AddConsumer (video, cam_plot, "1");

	frame->Start();
	painter->Arrange (1,2,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	ssi_print ("\n\n\tpress enter to continue..\n\n");
	getchar();

	frame->Stop();
	painter->Clear();
	frame->Clear();
}
