// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/06
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

#include "PaintRandomLines.h"
#include "PaintRandomPoints.h"
#include "PaintRandomShapes.h"
#include "PaintSomeText.h"

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

void ex_slider ();
void ex_simple ();	
void ex_random ();	
void ex_matrix ();	
void ex_framework ();	

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssiaudio.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssigraphic.dll");

	ex_slider ();
	ex_simple ();	   
	ex_random ();    // creates some random shapes
	ex_matrix ();    // loads and displays matrices
	ex_framework (); // displays a continuous signal using the framework
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_slider () {

	Slider slider ("slider", 0, 0, 200, 50, 0, -20, 20, 40);
	slider.SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	slider.show ();
	
	slider.set (0.5f);
	printf ("mew slider value: %.2f\n", slider.get ());
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	slider.close ();
}

void ex_simple () {	

	int data[2][50];
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 50; j++)
			data[i][j] = rand ();

	int id;

	IThePainter *painter = Factory::GetPainter ();
	
	PaintData<int> paint_signal (2, PAINT_TYPE_SIGNAL);
	paint_signal.setData (data[0], 50);
	id = painter->AddCanvas ("Signal");
	painter->AddObject (id, &paint_signal);	
	painter->Paint (id);	
	
	PaintData<int> paint_path (2, PAINT_TYPE_PATH);
	paint_path.setData (data[0], 50);
	id = painter->AddCanvas ("Path");
	painter->AddObject (id, &paint_path);	
	painter->Paint (id);	

	PaintData<int> paint_scatter (2, PAINT_TYPE_SCATTER);
	paint_scatter.setData (data[0], 50);
	id = painter->AddCanvas ("Scatter");
	painter->AddObject (id, &paint_scatter);	
	painter->Paint (id);	
	
	PaintData<int> paint_image (2, PAINT_TYPE_IMAGE);
	paint_image.setData (data[0], 50);
	id = painter->AddCanvas ("Image");
	painter->AddObject (id, &paint_image);	
	painter->Paint (id);	
	
	painter->Arrange (2,2,0,0,400,800);	
	painter->MoveConsole (400,0,400,800);

	ssi_print ("press enter to continue\n");
	getchar ();

	painter->Clear ();
}

void ex_random () {

	IThePainter *painter = Factory::GetPainter ();

	PaintRandomPoints points (1000);
	PaintRandomLines lines (30);
	PaintRandomShapes shapes (10);
	PaintSomeText text ("Paint Some Text");
	
	int id;

	id = painter->AddCanvas ("random points");
	painter->AddObject (id, &points);
	painter->Paint (id);

	id = painter->AddCanvas ("random lines");
	painter->AddObject (id, &lines);
	painter->Paint (id);

	id = painter->AddCanvas ("random shapes");
	painter->AddObject (id, &shapes);
	painter->Paint (id);

	id = painter->AddCanvas ("some text");
	painter->AddObject (id, &text);
	painter->Paint (id);

	id = painter->AddCanvas ("all in one");
	painter->AddObject (id, &lines);
	painter->AddObject (id, &points);
	painter->AddObject (id, &shapes);
	painter->AddObject (id, &text);
	painter->Paint (id);

	painter->Arrange (2,3,0,0,400,800);
	painter->MoveConsole (400,0,400,800);

	ssi_print ("press enter to continue\n");
	getchar ();

	painter->Clear ();

}

void ex_matrix () {

	IThePainter *painter = Factory::GetPainter ();

	File *speech_file = File::CreateAndOpen (File::BINARY, File::READ, "speech.data");
	speech_file->setType (SSI_FLOAT);
	Matrix<float> *speech = MatrixOps<float>::Read (speech_file);
	delete speech_file;

	File *mfcc_file = File::CreateAndOpen (File::BINARY, File::READ, "mfccs.data");
	mfcc_file->setType (SSI_FLOAT);
	Matrix<float> *mfcc = MatrixOps<float>::Read (mfcc_file);
	delete mfcc_file;

	File *image_file = File::CreateAndOpen (File::BINARY, File::READ, "image.data");
	image_file->setType (SSI_SHORT);
	Matrix<short> *image = MatrixOps<short>::Read (image_file);
	delete image_file;

	File *colormap_file = File::CreateAndOpen (File::BINARY, File::READ, "colormap.data");
	colormap_file->setType (SSI_UCHAR);
	Matrix<unsigned char> *colormap = MatrixOps<unsigned char>::Read (colormap_file);
	delete colormap_file;

	File *eye_file = File::CreateAndOpen (File::BINARY, File::READ, "eye.data");
	eye_file->setType (SSI_INT);
	Matrix<int> *path = MatrixOps<int>::Read (eye_file);
	delete eye_file;

	PaintData<float> signalPlot (speech->cols, PAINT_TYPE_SIGNAL);
	signalPlot.setData (speech->data, speech->rows);
	signalPlot.setLineColor (255, 0, 0);
	signalPlot.setLineWidth (2);

	PaintData<float> mfccPlot (mfcc->cols, PAINT_TYPE_IMAGE);
	mfccPlot.setData (mfcc->data, mfcc->rows);

	PaintData<short> imagePlot (image->cols, PAINT_TYPE_IMAGE);
	imagePlot.setData (image->data, image->rows);

	PaintData<int> pathPlot (path->cols, PAINT_TYPE_PATH);
	pathPlot.setData (path->data, path->rows);
	PaintData<int> scatterPlot (path->cols, PAINT_TYPE_SCATTER);
	scatterPlot.setData (path->data, path->rows);
	scatterPlot.setFillColor (255,255,255);
	
	int id;

	id = painter->AddCanvas ("signal");
	painter->AddObject (id, &signalPlot);
	painter->Paint (id);

	id = painter->AddCanvas ("mfccs");
	painter->AddObject (id, &mfccPlot);
	painter->Paint (id);

	id = painter->AddCanvas ("image", colormap->data, colormap->rows);
	painter->AddObject (id, &imagePlot);
	painter->Paint (id);

	id = painter->AddCanvas ("path");
	painter->AddObject (id, &pathPlot);
	painter->AddObject (id, &scatterPlot);
	painter->Paint (id);

	painter->Arrange (1,4,0,0,400,800);
	painter->MoveConsole (400,0,400,800);

	ssi_print ("press enter to continue\n");
	getchar ();

	painter->Clear ();

	delete speech;
	delete mfcc;
	delete image;
	delete colormap;
	delete path;

}

void ex_framework () {

	IThePainter *painter = Factory::GetPainter ();
	ITheFramework *frame = Factory::GetFramework ();
	ssi_pcast (TheFramework, frame)->getOptions ()->setConsolePos (400, 0, 800, 400);
	ITheEventBoard *board = Factory::GetEventBoard ();

	// sensor
	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;	
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);	
	frame->AddSensor (mouse);

	// trigger
	ZeroEventSender *zero_trigger = ssi_create (ZeroEventSender, "ezero", true);	 
	frame->AddConsumer (button_p, zero_trigger, "0.2s");
	board->RegisterSender (*zero_trigger);

	// audio
	Audio *audio = ssi_create (Audio, "audio", true);
	audio->getOptions ()->scale = true;	
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	// camera	
	Camera *camera = ssi_create (Camera, "camera", true);
	ITransformable *cam_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME, 0, 2.0);
	frame->AddSensor (camera);

	MFCC *mfcc = ssi_create (MFCC, "mfcc", true);
	ITransformable *mfcc_t = frame->AddTransformer (audio_p, mfcc, "0.025s", "0.025s");

	// plot
	SignalPainter *plot = 0;	

	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse");
	plot->getOptions ()->size = 2.0;
	plot->getOptions ()->type = PaintSignalType::PATH;
	frame->AddConsumer (cursor_p, plot, "0.2s"); 

	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse (tr)");
	plot->getOptions ()->type = PaintSignalType::PATH;
	frame->AddEventConsumer (cursor_p, plot, board, zero_trigger->getEventAddress ()); 

	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mouse");
	plot->getOptions ()->size = 2.0;	
	ITransformable *pp[2] = { cursor_p, button_p };
	frame->AddConsumer (2, pp, plot, "0.1s");

	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("audio");
	plot->getOptions ()->size = 2.0;		
	plot->getOptions ()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, plot, "0.1s");

	plot = ssi_create (SignalPainter, 0, true);
	plot->getOptions ()->setName ("mfccs");
	plot->getOptions ()->size = 2.0;		
	plot->getOptions ()->type = PaintSignalType::IMAGE;
	frame->AddConsumer (mfcc_t, plot, "0.1s");
	
	VideoPainter *vidplot = ssi_create (VideoPainter, 0, true);
	vidplot->getOptions ()->setName ("camera");
	vidplot->getOptions ()->setMove (10, 610);		
	frame->AddConsumer (cam_p, vidplot, "1");

	frame->Start ();
	board->Start ();
	painter->Arrange (2, 5, 0, 0, 400, 800);
	painter->MoveConsole (400,0,400,800);
	ssi_print ("press enter to continue\n");
	getchar ();
	frame->Stop ();
	board->Stop ();
	frame->Clear ();
	board->Clear ();
	painter->Clear ();
}
