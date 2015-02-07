// MainWav.cpp
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

void ex_simple ();
void ex_read ();
void ex_write ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssiaudio.dll");	
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssigraphic.dll");

	ex_simple ();
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

void ex_simple () {

	// create sample
	ssi_stream_t wave;
	ssi_stream_init (wave, 0, 1, sizeof (ssi_real_t), SSI_REAL, 44100.0);
	ssi_time_t fs[] = {440.0};
	ssi_real_t as[] = {1.0f};
	SignalTools::Series (wave, 5.0);
	SignalTools::Sine (wave, fs, as);

	// write wav
	WavTools::WriteWavFile ("test.wav", wave);

	// read wav
	ssi_stream_t check;
	WavTools::ReadWavFile ("test.wav", check, false);

	ssi_stream_destroy (wave);
	ssi_stream_destroy (check);

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_write () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	// audio	
	Audio *audio = ssi_pcast (Audio, Factory::Create (Audio::GetCreateName (), "audio"));	
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	// wav writer
	WavWriter *wav = ssi_pcast (WavWriter, Factory::Create (WavWriter::GetCreateName ()));
	wav->getOptions ()->setPath ("out.wav");
	frame->AddConsumer (audio_p, wav, "0.1s");

	// ssi writer
	FileWriter *ssi_writer = ssi_pcast (FileWriter, Factory::Create (FileWriter::GetCreateName ()));
	ssi_writer->getOptions ()->setPath ("out.data");
	ssi_writer->getOptions ()->stream = true;		
	frame->AddConsumer (audio_p, ssi_writer, "0.1s");

	// plot
	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio_p");
	audio_plot->getOptions ()->size = 2.0;	
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	audio_plot->getOptions ()->setMove (0, 0, 600, 200);
	frame->AddConsumer (audio_p, audio_plot, "0.1s");

	frame->Start ();
	painter->MoveConsole (0, 200, 600, 400);
	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}

void ex_read () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();

	// avi reader	
	WavReader *wav = ssi_pcast (WavReader, Factory::Create (WavReader::GetCreateName ()));
	wav->getOptions ()->setPath ("out.wav");
	wav->getOptions ()->block = 0.02;
	wav->getOptions ()->offset = 1.0;
	ITransformable *audio_p = frame->AddProvider (wav, SSI_WAVREADER_PROVIDER_NAME);
	frame->AddSensor (wav);

	// plot
	SignalPainter *audio_plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	audio_plot->getOptions ()->setName ("audio_p");
	audio_plot->getOptions ()->size = 2.0;	
	audio_plot->getOptions ()->type = PaintSignalType::AUDIO;
	audio_plot->getOptions ()->setMove (0, 0, 600, 200);
	frame->AddConsumer (audio_p, audio_plot, "0.02s");

	AudioPlayer *player = ssi_create (AudioPlayer, "player", true);
	frame->AddConsumer (audio_p, player, "0.02s");

	frame->Start ();
	painter->MoveConsole (0, 200, 600, 400);
	wav->wait ();		
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar();	
			
}






