// test.cpp
// author: Benjamin Hrzek <hrzek@arcor.de>
// created: 2010/06/14
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
#include "ssiasio.h"
using namespace ssi;

void input ();
void output ();

int main(int argc, char* argv[])
{

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssiasio.dll");	
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssisignal.dll");

	input ();
	//output ();

	ssi_print ("\n\n\tpress enter to quit!\n");
	getchar();

	Factory::Clear ();

	return 0;
}

void input () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
		
	AudioAsio* audio = ssi_create (AudioAsio, "asio", true);
	//ssi_size_t inds[] = {1,0,1};
	//audio->getOptions()->setChannels (3, inds);		
	audio->getOptions()->block = 512;
	audio->getOptions ()->api = 3;
	ITransformable *audio_p = frame->AddProvider (audio, SSI_AUDIOASIO_PROVIDER_NAME);
	frame->AddSensor (audio);

	SignalPainter* audioPainter = ssi_pcast(SignalPainter, Factory::Create(SignalPainter::GetCreateName()));
	audioPainter->getOptions()->setName("AudioAsio");
	audioPainter->getOptions()->size = 2.0;
	audioPainter->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, audioPainter, "0.1s");

	Selector *selector = ssi_create (Selector, 0, true);
	selector->getOptions ()->set (0);
	ITransformable *audio_sel = frame->AddTransformer (audio_p, selector, "256");

	WavWriter *wavwriter = ssi_create (WavWriter, 0, true);
	wavwriter->getOptions ()->setPath ("audio.wav");
	frame->AddConsumer (audio_sel, wavwriter, "256");

	frame->Start();
	getchar();
	frame->Stop();
	frame->Clear();
	painter->Clear ();
}

void output () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
		
	WavReader* audio = ssi_create (WavReader, 0, true);	
	audio->getOptions()->setPath ("audio.wav");
	audio->getOptions ()->blockInSamples = 512;	
	ITransformable *audio_p = frame->AddProvider (audio, SSI_WAVREADER_PROVIDER_NAME);
	frame->AddSensor (audio);

	SignalPainter* audioPainter = ssi_pcast(SignalPainter, Factory::Create(SignalPainter::GetCreateName()));
	audioPainter->getOptions()->setName("AudioAsio");
	audioPainter->getOptions()->size = 2.0;
	audioPainter->getOptions()->type = PaintSignalType::AUDIO;
	frame->AddConsumer (audio_p, audioPainter, "512");

	AudioAsioPlayer *player = ssi_create (AudioAsioPlayer, 0, true);	
	//player->getOptions ()->defdevice = true;
	frame->AddConsumer (audio_p, player, "512");

	frame->Start();
	audio->wait ();
	frame->Stop();
	frame->Clear();
	painter->Clear ();
}

