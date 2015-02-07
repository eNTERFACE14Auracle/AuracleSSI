// Main
// author: Tobias Baur <baur@hcm-lab.de>
// created: 2013/2/28
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
#include "ssitheeyetribe.h"
#include "..\..\eye\include\ssieye.h"

using namespace ssi;

// load libraries
#ifdef _MSC_VER 
#ifdef _DEBUG
#pragma comment(lib, "ssid.lib")
#else
#pragma comment(lib, "ssi.lib")
#endif
#endif

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

#define SIMULATE

void ex_theeyetribetest ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssitheeyetribe.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssieye.dll");


	ex_theeyetribetest ();	
	

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();
	
	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}
//



void ex_theeyetribetest () {

	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();
	char* CalibrationWindowName = "Scene"; //* Should be identical for CalibrationWindow Option and Name of Scene VideoPainter

	system("start /min EyeTribe.exe"); //Start Server is necesarry
	system("start EyeTribeUIWin.exe"); //Start UI for Calibration
	TheEyeTribe *eyetribe = ssi_create(TheEyeTribe, "eyetribe", true);
	ITransformable *eyegaze_p = frame->AddProvider (eyetribe, SSI_THEEYETRIBE_EYEGAZE_PROVIDER_NAME);

	
	
	frame->AddSensor (eyetribe);	

	FileWriter *writer = ssi_create (FileWriter, 0, true);
	writer->getOptions ()->setPath ("eyegaze");
	writer->getOptions ()->type = File::ASCII;
	frame->AddConsumer(eyegaze_p, writer, "1");


	SignalPainter *eye_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	eye_plot->getOptions ()->setName ("eye");
	eye_plot->getOptions ()->size = 2.0;
	eye_plot->getOptions ()->setArrange (1, 1, 10, 10, 800, 600);
	frame->AddConsumer (eyegaze_p, eye_plot, "0.25s"); 


	CameraScreen *screen = ssi_pcast (CameraScreen, Factory::Create (CameraScreen::GetCreateName (), "screen"));
	screen->getOptions ()->fps = 5.0;
	ITransformable *screen_p = frame->AddProvider (screen, SSI_CAMERASCREEN_PROVIDER_NAME, 0, 2.0);	
	frame->AddSensor (screen);

	EyePainter *eye_paint = ssi_pcast (EyePainter, Factory::Create (EyePainter::GetCreateName (), "eyepaint"));
	ITransformable *eye_paint_ids[] = {eyegaze_p};
	ITransformable *eye_paint_t = frame->AddTransformer (screen_p, 1, eye_paint_ids, eye_paint, "1");

	VideoPainter *eye_video = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	eye_video->getOptions ()->setName ("eye");	
	eye_video->getOptions ()->setArrange (1, 1, 10, 10, 800, 600);
	frame->AddConsumer (eye_paint_t, eye_video, "1"); 


	frame->Start ();
	board->Start ();
	painter->MoveConsole (0,400,400,400);
	painter->Arrange (3, 2, 0, 0, 800, 400);
	ssi_print ("press enter to continue\n");
	getchar ();
	board->Stop ();
	frame->Stop ();		
	frame->Clear ();
	board->Clear ();
	painter->Clear ();

	system("taskkill /IM EyeTribeUIWin.exe");
	system("taskkill /IM EyeTribe.exe");
}


