// Main
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/12/29
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
#include "ssimicrosoftkinect.h"
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

void ex_microsoftkinect ();
void ex_smiledetector ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssimicrosoftkinect.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssiioput.dll");

    ex_microsoftkinect ();	
	//ex_smiledetector ();	

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();
	
	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_microsoftkinect () {

	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();

	MicrosoftKinect *kinect = ssi_create(MicrosoftKinect, 0, true);
	kinect->getOptions ()->trackNearestPerson = false;
	kinect->getOptions ()->screenHeight = 480;
	kinect->getOptions ()->screenWidth = 640;
	kinect->getOptions ()->screenScale = true;
	kinect->getOptions ()->sr = 15.0;
	kinect->getOptions ()->showBodyTracking = true;
	kinect->getOptions ()->showFaceTracking = true;
	kinect->getOptions ()->simpleFaceTracking = false;
	kinect->getOptions ()->rgbres = MicrosoftKinect::RESOLUTION::RES_1280x960;
	kinect->getOptions ()->depthres = MicrosoftKinect::RESOLUTION::RES_640x480;
	kinect->getOptions ()->nearTrackingMode = true;

	ITransformable *face_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_FACEPOINT_PROVIDER_NAME);
	ITransformable *face3d_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_FACEPOINT3D_PROVIDER_NAME);
	ITransformable *au_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_ACTIONUNIT_PROVIDER_NAME);
	ITransformable *su_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_SHAPEUNIT_PROVIDER_NAME);
	ITransformable *skeleton_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_SKELETON_PROVIDER_NAME);
	//ITransformable *skeleton_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_SKELETON2SCREEN_PROVIDER_NAME);
	ITransformable *head_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_HEADPOSE_PROVIDER_NAME);
	ITransformable *rgb_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_RGBIMAGE_PROVIDER_NAME,0,1.0);
	ITransformable *depth_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_DEPTHIMAGE_PROVIDER_NAME,0,1.0);
	ITransformable *depthr_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_DEPTHRAW_PROVIDER_NAME,0,1.0);
	
	frame->AddSensor (kinect);
	
	MicrosoftKinectPainter *kinectpainter = ssi_create (MicrosoftKinectPainter, 0, true);
	ITransformable *sources[2] = { skeleton_p, face_p};
	ITransformable *rgbskel = frame->AddTransformer (rgb_p, 2, sources, kinectpainter, "1", 0,1.0);
	
	SignalPainter *splot;
	
	splot = ssi_create (SignalPainter, 0, true);
	splot->getOptions ()->size = 5.0;
	splot->getOptions ()->setName ("au");
	frame->AddConsumer (au_p, splot, "1");

	splot = ssi_create (SignalPainter, 0, true);
	splot->getOptions ()->size = 5.0;
	splot->getOptions ()->setName ("skeleton");
	frame->AddConsumer (skeleton_p, splot, "1");

	splot = ssi_create (SignalPainter, 0, true);
	splot->getOptions ()->size = 5.0;
	splot->getOptions ()->setName ("head");
	frame->AddConsumer (head_p, splot, "1");
	
	VideoPainter *vplot = 0;
		
	vplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vplot->getOptions ()->setName ("rgb");
	vplot->getOptions ()->flip = false;
    vplot->getOptions ()->mirror = false;
	frame->AddConsumer (rgb_p, vplot, "1");
	
	vplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vplot->getOptions ()->setName ("depth");
	vplot->getOptions ()->flip = false;
    vplot->getOptions ()->mirror = false;
	frame->AddConsumer (depth_p, vplot, "1");

	FileWriter *writer = 0;

	writer = ssi_create (FileWriter, 0, true);
	writer->getOptions ()->setPath ("face");
	writer->getOptions ()->type = File::ASCII;
	frame->AddConsumer (face_p, writer, "0.2s");
	

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

}

void ex_smiledetector () {
	
	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *board = Factory::GetEventBoard ();
	IThePainter *painter = Factory::GetPainter ();

	MicrosoftKinect *kinect = ssi_create(MicrosoftKinect, 0, true);
	kinect->getOptions ()->trackNearestPerson = true;
	kinect->getOptions ()->screenHeight = 480;
	kinect->getOptions ()->screenWidth = 640;
	kinect->getOptions ()->screenScale = true;
	kinect->getOptions ()->sr = 25.0;
	kinect->getOptions ()->showBodyTracking = false;
	kinect->getOptions ()->showFaceTracking = true;
	kinect->getOptions ()->simpleFaceTracking = true;
	kinect->getOptions ()->deviceIndex = 0;
	kinect->getOptions ()->nearTrackingMode = true;
	ITransformable *face_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_FACEPOINT_PROVIDER_NAME);
	ITransformable *au_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_ACTIONUNIT_PROVIDER_NAME);
	ITransformable *skeleton_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_SKELETON_PROVIDER_NAME);
	ITransformable *rgb_p = frame->AddProvider (kinect, SSI_MICROSOFTKINECT_RGBIMAGE_PROVIDER_NAME);	
	frame->AddSensor (kinect);

	MicrosoftKinectSmileDetector *kinectsmiledetector= ssi_create (MicrosoftKinectSmileDetector, 0, true);
	ITransformable *kinectsmiledetector_t = frame->AddTransformer (au_p, kinectsmiledetector, "5");

	ThresEventSender *thresevent = ssi_create (ThresEventSender, 0, true);
	thresevent->getOptions ()->thres = 0.0f;
	thresevent->getOptions ()->hangin = 3;
	thresevent->getOptions ()->hangout = 3;
	thresevent->getOptions ()->mindur = 1.0;
	thresevent->getOptions ()->setEvent ("smile");
	thresevent->getOptions ()->setSender ("kinect");
	frame->AddConsumer (kinectsmiledetector_t, thresevent, "5");
	board->RegisterSender (*thresevent);

	MicrosoftKinectPainter *kinectpainter = ssi_create (MicrosoftKinectPainter, 0, true);
	ITransformable *sources[2] = { skeleton_p, face_p};
	ITransformable *rgbskel = frame->AddTransformer (rgb_p, 2, sources, kinectpainter, "1");
	
	SignalPainter *splot;
	
	splot = ssi_create (SignalPainter, 0, true);
	splot->getOptions ()->size = 5.0;
	splot->getOptions ()->setName ("smile");
	frame->AddConsumer (kinectsmiledetector_t, splot, "1");
	
	VideoPainter *vplot = 0;
		
	vplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vplot->getOptions ()->setName ("video");
	vplot->getOptions ()->flip = false;
    vplot->getOptions ()->mirror = false;
	frame->AddConsumer (rgb_p, vplot, "1");
	
	EventMonitor *emonitor = ssi_create (EventMonitor, 0, true);
	emonitor->getOptions ()->setMonitorPos (400,400,400,400);
	board->RegisterListener (*emonitor, 0, 20000);

	
	frame->Start ();
	board->Start ();
	painter->MoveConsole (0,400,400,400);
	painter->Arrange (2, 1, 0, 0, 800, 400);
	ssi_print ("press enter to continue\n");
	getchar ();
	board->Stop ();
	frame->Stop ();		
	frame->Clear ();
	board->Clear ();
	painter->Clear ();
}