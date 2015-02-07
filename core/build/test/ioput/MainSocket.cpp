// MainSocket.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/07/21
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

#include "MyOscListener.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void ex_simple ();
void ex_stream ();
void ex_video ();
void ex_file ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssicamera.dll");
	Factory::RegisterDLL ("ssigraphic.dll");

	ex_simple ();	  
	ex_stream ();
	ex_video ();
	ex_file ();
	
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

	Socket::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	{
		Socket *sender = Socket::CreateAndConnect (Socket::UDP, Socket::SEND, 1111, "localhost");
		Socket *receiver = Socket::CreateAndConnect (Socket::UDP, Socket::RECEIVE, 1111);

		ssi_char_t msg[] = "hello world!";
		sender->send (msg, ssi_cast (ssi_size_t, strlen (msg) + 1));
		ssi_char_t string[512];

		if (receiver->recv (string, 512) > 0) {
			ssi_print ("received message from %s:\n%s\n", receiver->getRecvAddress (), string);
		} else {
			ssi_print ("couldn't receive message\n");
		}

		int data[5] = {1,2,3,4,5};

		sender->send (data, 5 * sizeof (int));
		int data_recv[512];
		int result = receiver->recv (data_recv, 512 * sizeof (int));
		if (result > 0) {
			ssi_print ("received data from %s:\n", receiver->getRecvAddress (), string);
			for (size_t i = 0; i < result / sizeof (int); i++) {
				ssi_print ("%d ", data_recv[i]);
			}
			ssi_print ("\n");
		} else {
			ssi_print ("couldn't receive data\n");
		}

		delete sender;
		delete receiver;

	}

	{

		Socket *sender_socket = Socket::Create (Socket::UDP, Socket::SEND, 1111, "localhost");
		SocketOsc sender (*sender_socket, 2000);
		Socket *receiver_socket = Socket::Create (Socket::UDP, Socket::RECEIVE, 1111);
		SocketOsc receiver (*receiver_socket, 2000);

		sender.connect ();
		receiver.connect ();

		sender.send_message ("ssi", 0, 0, "hello world!");

		MyOscListener listener;

		if ((receiver.recv (&listener, 2000)) <= 0) {
			ssi_print ("couldn't receive message\n");
		}

		float data[9] = {1.0f,2.0f,3.0f,
			4.0f,5.0f,6.0f,
			7.0f,8.0f,9.0f};

		sender.send_stream ("ssi", 5000, 20.0f, 3, 3, sizeof (float), SSI_FLOAT, data);

		if ((receiver.recv (&listener,2000)) <= 0) {
			ssi_print ("couldn't receive message\n");
		}

		ssi_char_t *event_name[] = {"event_1",
			"event_2",
			"event_3"};
		ssi_real_t event_value[] = {1.0f,
			2.0f,
			3.0f};

		sender.send_event ("ssi", 5000, 20000, SSI_ESTATE_COMPLETED, 3, event_name, event_value);

		if ((receiver.recv (&listener,2000)) <= 0) {
			ssi_print ("couldn't receive message\n");
		}

		sender.disconnect ();
		receiver.disconnect ();

		delete sender_socket;
		delete receiver_socket;

	} 

	{
	
		ssi_video_params_t params;
		params.heightInPixels = 80;
		params.widthInPixels = 60;
		params.depthInBitsPerChannel = 8;
		params.numOfChannels = 1;

		ssi_size_t n_image = ssi_video_size (params);
		ssi_byte_t *image = new ssi_byte_t[n_image];
		memset (image, 0, n_image); 
		ssi_byte_t *check = new ssi_byte_t[n_image];
		memset (check, 1, n_image); 

		int port = 9999;
		Socket *socksend = Socket::CreateAndConnect (Socket::UDP, Socket::SEND, port);
		Socket *sockrecv =  Socket::CreateAndConnect (Socket::UDP, Socket::RECEIVE, port);

		SocketImage imgsend (*socksend);
		imgsend.setLogLevel (SSI_LOG_LEVEL_DEBUG);
		imgsend.sendImage (params, image, n_image);

		SocketImage imgrecv (*sockrecv);
		imgrecv.setLogLevel (SSI_LOG_LEVEL_DEBUG);
		imgrecv.recvImage (params, check, n_image, 5000);

		delete socksend;
		delete sockrecv;

	}

	ssi_print ("\n\n\tPress enter to continue\n\n");
	getchar ();
}

void ex_stream () {
	
	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();	

	// start mouse
	
	Mouse *mouse = ssi_pcast (Mouse, Factory::Create (Mouse::GetCreateName (), "mouse"));
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse);

	// start sender

	SocketWriter *socket_writer_bin = ssi_pcast (SocketWriter, Factory::Create (SocketWriter::GetCreateName ()));
	socket_writer_bin->getOptions ()->port = 1111;
	socket_writer_bin->getOptions ()->setHost ("localhost");
	socket_writer_bin->getOptions ()->type = Socket::UDP;
	socket_writer_bin->getOptions ()->format = SocketWriter::Options::FORMAT::BINARY;
	frame->AddConsumer (cursor_p, socket_writer_bin, "0.25s");
	
	SocketReader *socket_reader_bin = ssi_pcast (SocketReader, Factory::Create (SocketReader::GetCreateName ()));
	socket_reader_bin->getOptions ()->port = 1111;
	socket_reader_bin->getOptions ()->type = Socket::UDP;
	socket_reader_bin->getOptions ()->format = SocketReader::Options::FORMAT::BINARY;
	socket_reader_bin->getOptions ()->setSampleInfo (cursor_p->getSampleRate (), cursor_p->getSampleDimension (), cursor_p->getSampleBytes (), cursor_p->getSampleType ());
	ITransformable *socket_reader_bin_p = frame->AddProvider (socket_reader_bin, SSI_SOCKETREADER_PROVIDER_NAME);
	frame->AddSensor (socket_reader_bin);

	SocketWriter *socket_writer_asc = ssi_pcast (SocketWriter, Factory::Create (SocketWriter::GetCreateName ()));
	socket_writer_asc->getOptions ()->port = 2222;
	socket_writer_asc->getOptions ()->setHost ("localhost");
	socket_writer_asc->getOptions ()->type = Socket::UDP;
	socket_writer_asc->getOptions ()->format = SocketWriter::Options::FORMAT::ASCII;
	frame->AddConsumer (cursor_p, socket_writer_asc, "0.25s");
	
	SocketReader *socket_reader_asc = ssi_pcast (SocketReader, Factory::Create (SocketReader::GetCreateName ()));
	socket_reader_asc->getOptions ()->port = 2222;
	socket_reader_asc->getOptions ()->type = Socket::UDP;
	socket_reader_asc->getOptions ()->format = SocketReader::Options::FORMAT::ASCII;
	socket_reader_asc->getOptions ()->setSampleInfo (cursor_p->getSampleRate (), cursor_p->getSampleDimension (), cursor_p->getSampleBytes (), cursor_p->getSampleType ());
	ITransformable *socket_reader_asc_p = frame->AddProvider (socket_reader_asc, SSI_SOCKETREADER_PROVIDER_NAME);
	frame->AddSensor (socket_reader_asc);

	SocketWriter *socket_writer_osc = ssi_pcast (SocketWriter, Factory::Create (SocketWriter::GetCreateName ()));
	socket_writer_osc->getOptions ()->port = 3333;
	socket_writer_osc->getOptions ()->setHost ("localhost");
	socket_writer_osc->getOptions ()->type = Socket::UDP;
	socket_writer_osc->getOptions ()->format = SocketWriter::Options::FORMAT::OSC;
	socket_writer_osc->getOptions ()->setId ("mouse");
	frame->AddConsumer (cursor_p, socket_writer_osc, "0.25s");
	
	SocketReader *socket_reader_osc = ssi_pcast (SocketReader, Factory::Create (SocketReader::GetCreateName ()));
	socket_reader_osc->getOptions ()->port = 3333;
	socket_reader_osc->getOptions ()->type = Socket::UDP;
	socket_reader_osc->getOptions ()->format = SocketReader::Options::FORMAT::OSC;
	socket_reader_osc->getOptions ()->setSampleInfo (cursor_p->getSampleRate (), cursor_p->getSampleDimension (), cursor_p->getSampleBytes (), cursor_p->getSampleType ());	
	ITransformable *socket_reader_osc_p = frame->AddProvider (socket_reader_osc, SSI_SOCKETREADER_PROVIDER_NAME);
	frame->AddSensor (socket_reader_osc);

	SignalPainter *plot;

	plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	plot->getOptions ()->size = 10;	
	frame->AddConsumer (cursor_p, plot, "5");

	plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	plot->getOptions ()->size = 10;	
	frame->AddConsumer (socket_reader_bin_p, plot, "5");

	plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	plot->getOptions ()->size = 10;	
	frame->AddConsumer (socket_reader_asc_p, plot, "5");

	plot = ssi_pcast (SignalPainter, Factory::Create (SignalPainter::GetCreateName ()));
	plot->getOptions ()->size = 10;	
	frame->AddConsumer (socket_reader_osc_p, plot, "5");

	frame->Start ();
	painter->Arrange (1,4,0,0,400,600);
	painter->MoveConsole (400,0,400,600);
	ssi_print ("\n\n\tPress enter to continue\n\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();

}

void ex_video () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();	

	Camera *camera = ssi_create (Camera, "camera", true);	
	ITransformable *camera_p = frame->AddProvider (camera, SSI_CAMERA_PROVIDER_NAME);
	camera->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	frame->AddSensor (camera);
	
	SocketWriter *socket_writer_img = ssi_pcast (SocketWriter, Factory::Create (SocketWriter::GetCreateName ()));
	socket_writer_img->getOptions ()->port = 4444;
	socket_writer_img->getOptions ()->setHost ("localhost");
	socket_writer_img->getOptions ()->type = Socket::UDP;
	socket_writer_img->getOptions ()->format = SocketWriter::Options::FORMAT::IMAGE;
	frame->AddConsumer (camera_p, socket_writer_img, "1");
	
	SocketReader *socket_reader_img = ssi_pcast (SocketReader, Factory::Create (SocketReader::GetCreateName ()));
	socket_reader_img->getOptions ()->port = 4444;
	socket_reader_img->getOptions ()->type = Socket::UDP;
	socket_reader_img->getOptions ()->format = SocketReader::Options::FORMAT::IMAGE;
	socket_reader_img->getOptions ()->setSampleInfo (camera->getFormat ());
	ITransformable *socket_reader_img_p = frame->AddProvider (socket_reader_img, SSI_SOCKETREADER_PROVIDER_NAME);
	frame->AddSensor (socket_reader_img);
	
	VideoPainter *plot;

	plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	frame->AddConsumer (camera_p, plot, "1");
	
	plot = ssi_pcast (VideoPainter, Factory::Create (VideoPainter::GetCreateName ()));
	frame->AddConsumer (socket_reader_img_p, plot, "1");

	frame->Start ();
	painter->Arrange (1,2,0,0,320,480);
	painter->MoveConsole (320,0,320,480);
	ssi_print ("\n\n\tPress enter to continue\n\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
	
}

void recvFile (void *ptr) {
	Socket *s = Socket::CreateAndConnect (Socket::TCP, Socket::RECEIVE, 1112);
	ssi_char_t *filepath = 0;
	s->recvFile (&filepath, 2000);
	delete[] filepath;	
}

void sendFile (void *ptr) {
	Socket *s = Socket::CreateAndConnect (Socket::TCP, Socket::SEND, 1112, "localhost");
	ssi_char_t *filepath = ssi_pcast (ssi_char_t, ptr);
	s->sendFile (filepath);	
}

void ex_file () {

	RunAsThread recv (&recvFile, 0, true);
	RunAsThread send (&sendFile, "file\\hello.txt", true);

	send.start ();
	Sleep (100);
	recv.start ();
	recv.stop ();
	send.stop ();
	
}




