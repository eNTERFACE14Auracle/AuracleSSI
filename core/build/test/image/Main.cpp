// Main
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/12/20
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

#include "ssiimage.h"

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

void main_online ();
void main_offline ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssiimage.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimouse.dll");

	main_online ();
	main_offline ();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}


void main_online () {

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
	
	CVColor *cv_color = ssi_pcast (CVColor, Factory::Create (CVColor::GetCreateName ()));
	cv_color->getOptions ()->code = CVColor::RGB2GRAY;
	ITransformable *cv_color_t = frame->AddTransformer (camera_p, cv_color, "1");

	CVChange *cv_change = ssi_pcast (CVChange, Factory::Create (CVChange::GetCreateName ()));
	ITransformable *cv_chain_t = frame->AddTransformer (cv_color_t, cv_change, "1");

	ITransformable *channels[1] = {cursor_p};
	CVCrop *cv_crop = ssi_pcast (CVCrop, Factory::Create (CVCrop::GetCreateName ()));
	cv_crop->getOptions ()->width = 160;
	cv_crop->getOptions ()->height = 120;
	cv_crop->getOptions ()->region[2] = 0.5f;
	cv_crop->getOptions ()->region[3] = 0.5f;
	cv_crop->getOptions ()->scaled = true;	
	cv_crop->getOptions ()->origin = CVCrop::ORIGIN::CENTER;
	ITransformable *cv_crop_t = frame->AddTransformer (camera_p, 1, channels, cv_crop, "1");

	CVResize *cv_resize = ssi_pcast (CVResize, Factory::Create (CVResize::GetCreateName ()));
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

	vidplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vidplot->getOptions ()->setName ("video");
	frame->AddConsumer (cv_color_t, vidplot, "1");

	vidplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vidplot->getOptions ()->setName ("change");
	frame->AddConsumer (cv_chain_t, vidplot, "1");

	vidplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vidplot->getOptions ()->setName ("crop");
	frame->AddConsumer (cv_crop_t, vidplot, "1");

	vidplot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	vidplot->getOptions ()->setName ("resize");
	vidplot->getOptions ()->scale = false;	
	frame->AddConsumer (cv_resize_t, vidplot, "1");

	// run

	frame->Start ();
	painter->Arrange (2, 2, 0, 0, camera->getFormat ().widthInPixels * 2, camera->getFormat ().heightInPixels * 2);
	painter->MoveConsole (camera->getFormat ().widthInPixels * 2, 0, 400, 800);
	ssi_print ("Press enter to stop!\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();

	painter->Clear ();

}

void record () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	
	Camera *camera = ssi_create (Camera, "camera", true);	
	ITransformable *camera_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME);
	camera->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	frame->AddSensor (camera);	

	CameraWriter *writer = ssi_create (CameraWriter, 0, true);
	writer->getOptions ()->setPath ("video.avi");
	writer->getOptions ()->setCompression ("Microsoft Video 1");
	frame->AddConsumer (camera_p, writer, "1");

	VideoPainter *camera_plot = ssi_create (VideoPainter, 0, true);
	camera_plot->getOptions ()->setName ("camera");
	camera_plot->getOptions ()->setMove (0, 0, 400, 300, false);	
	frame->AddConsumer (camera_p, camera_plot, "1");

	painter->MoveConsole (0,300,800,400);
	frame->Start ();
	ssi_print ("\nrecording video...\n\n\tpress enter to continue\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void main_offline () {

	ITheFramework *frame = Factory::GetFramework ();

	const ssi_char_t *filename = "video.avi";
	
	if (!ssi_exists (filename)) {
		record ();
	}

	CameraReader *reader = ssi_factory_create (CameraReader, 0, true);	
	reader->getOptions ()->setPath (filename);
	reader->getOptions ()->best_effort_delivery = true;
	frame->AddSensor (reader);

	CVColor *cv_color = ssi_pcast (CVColor, Factory::Create (CVColor::GetCreateName ()));
	cv_color->getOptions ()->code = CVColor::RGB2GRAY;
	CVChange *cv_change = ssi_pcast (CVChange, Factory::Create (CVChange::GetCreateName ()));
	
	CVResize *cv_resize = ssi_pcast (CVResize, Factory::Create (CVResize::GetCreateName ()));
	cv_resize->getOptions ()->width = 150;
	cv_resize->getOptions ()->height = 150;

	// IFilter *filter[] = { cv_color, cv_change }; CRASHES, PROBABLY WRITING GRAY VIDEOS NOT SUPPORTED...
	IFilter *filter[] = { cv_resize };
	Chain *chain = ssi_factory_create (Chain, 0, true);
	chain->set (1, filter, 0, 0);

	CameraWriter *writer = ssi_factory_create (CameraWriter, 0, true);	
	writer->getOptions ()->setPath ("convert.avi");
	writer->getOptions ()->setCompression ("Microsoft Video 1");
	FileProvider video_p (writer, chain);
	reader->setProvider (SSI_CAMERAREADER_PROVIDER_NAME, &video_p);

	frame->Start ();
	reader->wait ();
	frame->Stop ();
	frame->Clear ();

}

