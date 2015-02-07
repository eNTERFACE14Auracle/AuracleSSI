// MainAvi.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/27
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

void ex_write (); 
void ex_read ();	
void ex_extract ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");	
	Factory::RegisterDLL ("ssiaudio.dll");	
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssigraphic.dll");

	ex_write (); 	  	 
	ex_read ();		
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_write () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	// audio_p	
	Audio *audio = ssi_create (Audio, "audio", true);
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	// cam_p	
	Camera *camera = ssi_create (Camera, "camera", true);
	ITransformable *cam_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME, 0, 2.0);
	frame->AddSensor (camera);	

	// avi writer		
	AviWriter *avi_writer = ssi_create  (AviWriter, "aviwriter", true);
	avi_writer->getOptions ()->setPath ("out.avi");
	//avi_writer->getOptions ()->setFourcc ("DIVX");
	ITransformable *ts[2] = {cam_p, audio_p};
	frame->AddConsumer (2, ts, avi_writer, "1");

	// plot
	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 2.0;		
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, audio_plot, "0.1s");
	
	VideoPainter *cam_plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	cam_plot->getOptions ()->setName ("camera");
	frame->AddConsumer (cam_p, cam_plot, "1");

	// run framework
	frame->Start ();
	painter->Arrange (1,2,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	ssi_print ("press enter to continue\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();

}

void ex_read () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	// avi reader		
	AviReader *avi = ssi_create (AviReader, 0, true);
	avi->getOptions ()->setPath ("out.avi");
	avi->getOptions ()->block = 0.05;
	avi->getOptions ()->offset = 2.0;
	ITransformable *audio_p = frame->AddProvider (avi, SSI_AVIREADER_AUDIO_PROVIDER_NAME);
	ITransformable *video_p = frame->AddProvider (avi, SSI_AVIREADER_VIDEO_PROVIDER_NAME, 0, 0.5);							
	frame->AddSensor (avi);

	// playback
	AudioPlayer *player = ssi_create (AudioPlayer, "player", true);
	frame->AddConsumer (audio_p, player, "0.05s");		

	// plot
	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 2.0;		
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, audio_plot, "0.05s");

	VideoPainter *video_plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	video_plot->getOptions ()->setName ("video");	
	frame->AddConsumer (video_p, video_plot, "1");

	frame->Start ();
	painter->Arrange (1,2,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	avi->wait ();	
	frame->Stop ();
	frame->Clear ();	
	painter->Clear ();

}






