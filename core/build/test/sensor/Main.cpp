// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/28
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

void ex_mouse ();
void ex_audio (); 
void ex_audioplay ();	
void ex_cam ();
void ex_camfile ();
void ex_camscreen ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssiaudio.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssimodel.dll");

	ex_mouse ();
	ex_audio (); 
	ex_audioplay ();		
	ex_cam ();
	ex_camfile ();
	ex_camscreen ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();
	
	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_mouse () {

	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (0, 400, 600, 400);
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();
	
	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	mouse->getOptions ()->flip = true;
	mouse->getOptions ()->scale = false;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	mouse->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	frame->AddSensor (mouse);
	
	FileWriter *cursor_write = ssi_create (FileWriter, 0, true);
	cursor_write->getOptions ()->setPath ("cursor.txt");
	cursor_write->getOptions ()->stream = true;
	cursor_write->getOptions ()->type = File::ASCII;
	frame->AddConsumer (cursor_p, cursor_write, "0.25s");

	ZeroEventSender *ezero = ssi_create (ZeroEventSender, "ezero", true);
	ezero->getOptions ()->mindur = 0.2;	
	frame->AddConsumer (button_p, ezero, "0.25s");
	board->RegisterSender (*ezero);

	SignalPainter *mouse_plot = ssi_create (SignalPainter, 0, true);
	mouse_plot->getOptions ()->setName ("mouse");
	mouse_plot->getOptions ()->size = 10.0;
	frame->AddConsumer (cursor_p, mouse_plot, "0.25s"); 

	SignalPainter *mouse_tr_plot = ssi_create (SignalPainter, 0, true);
	mouse_tr_plot->getOptions ()->setName ("mouse (tr)");
	mouse_tr_plot->getOptions ()->setArrange (1, 2, 0, 0, 600, 400);
	frame->AddEventConsumer (cursor_p, mouse_tr_plot, board, ezero->getEventAddress ()); 

	frame->Start ();
	board->Start ();
	frame->Wait ();
	board->Stop ();
	frame->Stop ();
	board->Clear ();
	frame->Clear ();
	painter->Clear ();
}

void ex_audio () {

	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (0, 400, 600, 400);
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();

	Audio *audio = ssi_pcast (Audio, Factory::Create (Audio::GetCreateName (), "audio"));
	audio->getOptions ()->remember = true;
	audio->getOptions ()->sr = 16000.0;
	audio->getOptions ()->channels = 1;
	audio->getOptions ()->bytes = 2;		
	audio->getOptions ()->scale = false;
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	SNRatio *snratio = ssi_pcast (SNRatio, Factory::Create (SNRatio::GetCreateName ()));
	ZeroEventSender *ezero = ssi_pcast (ZeroEventSender, Factory::Create (ZeroEventSender::GetCreateName (), "ezero"));	
	ezero->getOptions ()->mindur = 0.2;
	ezero->getOptions ()->hangin = 3;
	ezero->getOptions ()->hangout = 3;	
	frame->AddConsumer (audio_p, ezero, "0.1s", "0", snratio);
	board->RegisterSender (*ezero);
	
	FileWriter *audio_write = ssi_pcast (FileWriter, Factory::Create (FileWriter::GetCreateName ()));
	audio_write->getOptions ()->setPath ("audio");
	audio_write->getOptions ()->stream = true;
	frame->AddConsumer (audio_p, audio_write, "0.2s");

	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 10.0;		
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, audio_plot, "0.2s");

	audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio (tr)");	
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddEventConsumer (audio_p, audio_plot, board, ezero->getEventAddress ());

	frame->Start ();
	board->Start ();
	painter->Arrange (1, 2, 0, 0, 600, 400);
	frame->Wait ();
	board->Stop ();
	frame->Stop ();	
	frame->Clear ();
	board->Clear ();
	painter->Clear ();
}
	
void ex_audioplay () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	FileReader *audio = ssi_pcast (FileReader, Factory::Create (FileReader::GetCreateName ()));		
	audio->getOptions ()->block = 0.1;
	audio->getOptions ()->setPath ("audio");
	audio->getOptions ()->loop = true;
	audio->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	ITransformable *audio_p = frame->AddProvider (audio, SSI_FILEREADER_PROVIDER_NAME);
	frame->AddSensor (audio);
	
	AudioPlayer *audio_play = ssi_pcast (AudioPlayer, Factory::Create (AudioPlayer::GetCreateName ()));
	frame->AddConsumer (audio_p, audio_play, "0.1s");

	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 2.0;	
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	audio_plot->getOptions ()->setMove (0, 0, 600, 400);
	frame->AddConsumer (audio_p, audio_plot, "0.1s");

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}		

void ex_cam () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	Audio *audio = ssi_create (Audio, "audio", true);	
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);
	
	Camera *camera = ssi_create (Camera, "camera", true);	
	ITransformable *camera_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME);
	camera->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	frame->AddSensor (camera);

	ssi_video_params_t video_params = camera->getOptions ()->params;
	
	ITransformable *ids[] = { camera_p, audio_p };
	CameraWriter *cam_write = ssi_create (CameraWriter, 0, true);
	cam_write->getOptions ()->setPath ("video.avi");
	//cam_write->getOptions ()->mirror = false;
	//cam_write->getOptions ()->flip = true;
	cam_write->getOptions ()->setCompression ("Microsoft Video 1");
	//cam_write->getOptions ()->setCompression (SSI_CAMERA_USE_NO_COMPRESSION);
	frame->AddConsumer (2, ids, cam_write, "1");

	VideoPainter *camera_plot = ssi_create (VideoPainter, 0, true);
	camera_plot->getOptions ()->mirror = false;
	camera_plot->getOptions ()->flip = true;
	camera_plot->getOptions ()->setName ("camera");
	camera_plot->getOptions ()->setMove (0, 0, 400, 300, false);	
	frame->AddConsumer (camera_p, camera_plot, "1");

	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio");
	audio_plot->getOptions ()->size = 2.0;	
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	audio_plot->getOptions ()->setMove (400, 0, 400, 300);
	frame->AddConsumer (audio_p, audio_plot, "0.1s");

	painter->MoveConsole (0,300,800,400);
	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void ex_camfile () {		

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	//ssi_pcast (TheFramework, frame)->getOptions ()->countdown = 0;
	
	CameraReader *cam = ssi_create (CameraReader, 0, true);
	cam->getOptions ()->setPath ("video.avi");
	//cam->getOptions ()->flip = true;
	ITransformable *cam_p  = frame->AddProvider (cam, SSI_CAMERAREADER_PROVIDER_NAME, 0, 2.0);
	frame->AddSensor (cam);

	VideoPainter *cam_plot = ssi_create (VideoPainter, 0, true);
	cam_plot->getOptions ()->setName ("video");		
	cam_plot->getOptions ()->setMove (0, 200);
	//cam_plot->getOptions ()->mirror = false;
	//cam_plot->getOptions ()->flip = true;
	frame->AddConsumer (cam_p, cam_plot, "1");
	
	frame->Start ();
	painter->MoveConsole (400,0,400,300);
	painter->Arrange (1,1,0,0,400,300);
	cam->wait ();
	frame->Stop ();
	frame->Clear ();

	painter->Clear ();
}		

void ex_camscreen () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	Audio *audio = ssi_pcast (Audio, Factory::Create (Audio::GetCreateName (), "audio"));
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);
	
	CameraScreen *screen = ssi_pcast (CameraScreen, Factory::Create (CameraScreen::GetCreateName (), "screen"));
	screen->getOptions ()->mouse = true;
	screen->getOptions ()->mouse_size = 10;
	screen->getOptions ()->setResize (640, 480);
	screen->getOptions ()->fps = 10.0;
	screen->getOptions ()->setRegion (0, 0, 800, 600);
	ITransformable *screen_p = frame->AddProvider (screen, SSI_CAMERASCREEN_PROVIDER_NAME, 0, 2.0);
	frame->AddSensor (screen);

	VideoPainter *cam_plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	cam_plot->getOptions ()->setName ("screen");
	ssi_video_params_t screen_format = screen->getFormat ();
	cam_plot->getOptions ()->setMove (0, 0, screen_format.widthInPixels, screen_format.heightInPixels);	
	frame->AddConsumer (screen_p, cam_plot, "1");

	ITransformable *ts[] = { screen_p, audio_p };
	CameraWriter *cam_write = ssi_pcast (CameraWriter, Factory::Create (CameraWriter::GetCreateName ()));
	cam_write->getOptions ()->setPath ("screen.avi");
	cam_write->getOptions ()->setCompression ("Microsoft Video 1");
	//cam_write->getOptions ()->setCompression (SSI_CAMERA_USE_NO_COMPRESSION);
	frame->AddConsumer (2, ts, cam_write, "1");

	frame->Start ();
	frame->Wait ();
	frame->Stop ();
	frame->Clear ();

	painter->Clear ();
}
