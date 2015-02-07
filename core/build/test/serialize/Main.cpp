// MainFile.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/02/07
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

void ex_stream_write ();
void ex_stream_read ();
void ex_sample_write ();
void ex_sample_read ();
void ex_model_write ();
void ex_model_read ();

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimodel.dll");

	ssi_random_seed ();

	ex_stream_write ();	  
	ex_stream_read ();
	ex_sample_write ();
	ex_sample_read ();
	ex_model_write ();
	ex_model_read ();
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_stream_write () {
		
	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	{
		ssi_stream_t stream;
		ssi_stream_init (stream, 20, 5, sizeof (int), SSI_INT, 1.0);
		int *ptr = ssi_pcast (int, stream.ptr);
		for (ssi_size_t i = 0; i < 100; i++) {
			*ptr++ = ssi_cast (int, i);
		}
		FileTools::WriteStreamFile (File::ASCII, "stream_i.txt", stream);
		ssi_stream_destroy (stream);
	}

	{
		ssi_stream_t stream;
		ssi_stream_init (stream, 20, 5, sizeof (int), SSI_INT, 1.0);
		int *ptr = ssi_pcast (int, stream.ptr);
		for (ssi_size_t i = 0; i < 100; i++) {
			*ptr++ = ssi_cast (int, i);
		}
		FileTools::WriteStreamFile (File::BINARY, "stream_i.data", stream);
		ssi_stream_destroy (stream);
	}

	{		
		ssi_stream_t stream;
		ssi_stream_init (stream, 20, 5, sizeof (float), SSI_FLOAT, 1.0);
		float *ptr = ssi_pcast (float, stream.ptr);
		for (ssi_size_t i = 0; i < 100; i++) {
			*ptr++ = ssi_cast (float, i);
		}
		FileTools::WriteStreamFile (File::ASCII, "stream_f.txt", stream);
		ssi_stream_destroy (stream);
	}

	{		
		ssi_stream_t stream;
		ssi_stream_init (stream, 20, 5, sizeof (float), SSI_FLOAT, 1.0);
		float *ptr = ssi_pcast (float, stream.ptr);
		for (ssi_size_t i = 0; i < 100; i++) {
			*ptr++ = ssi_cast (float, i);
		}
		FileTools::WriteStreamFile (File::BINARY, "stream_f.data", stream);
		ssi_stream_destroy (stream);		
	}
}

void ex_stream_read () {

	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	{
		ssi_stream_t stream;
		FileTools::ReadStreamFile (File::ASCII, "stream_i.txt", stream);
		FileTools::WriteStreamFile (File::ASCII, "", stream);
		ssi_stream_destroy (stream);
	}

	{
		ssi_stream_t stream;
		FileTools::ReadStreamFile (File::BINARY, "stream_i.data", stream);
		FileTools::WriteStreamFile (File::ASCII, "", stream);
		ssi_stream_destroy (stream);
	}

	{
		ssi_stream_t stream;
		FileTools::ReadStreamFile (File::ASCII, "stream_f.txt", stream);
		FileTools::WriteStreamFile (File::ASCII, "", stream);
		ssi_stream_destroy (stream);
	}

	{
		ssi_stream_t stream;
		FileTools::ReadStreamFile (File::BINARY, "stream_f.data", stream);
		FileTools::WriteStreamFile (File::ASCII, "", stream);
		ssi_stream_destroy (stream);
	}

}

void ex_sample_write () {

	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	ssi_size_t n_classes = 2;
	ssi_size_t n_samples = 2;
	ssi_size_t n_streams = 2;
	ssi_real_t distr[][3] = { 0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.5f };
	SampleList s;	

	ModelTools::CreateTestSamples (s, n_classes, n_samples, n_streams, distr, "user");
	ModelTools::SaveSampleList (s, "samples.txt", File::ASCII);
	ModelTools::SaveSampleList (s, "samples.data", File::BINARY);

}

void ex_sample_read () {

	File::SetLogLevel (SSI_LOG_LEVEL_BASIC);

	{
		SampleList s;
		ModelTools::LoadSampleList (s, "samples.txt");
		ModelTools::PrintSamples (s);
	}

	{
		SampleList s;
		ModelTools::LoadSampleList (s, "samples.data");
		ModelTools::PrintSamples (s);
	}
}

void ex_model_write () {

	ssi_size_t n_classes = 4;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.3f, 0.3f, 0.2f, 0.3f, 0.6f, 0.2f, 0.6f, 0.3f, 0.2f, 0.6f, 0.6f, 0.2f };
	SampleList strain;	
	ModelTools::CreateTestSamples (strain, n_classes, n_samples, n_streams, train_distr, "user");						

	{
		NaiveBayes *model = ssi_pcast (NaiveBayes, Factory::Create (NaiveBayes::GetCreateName ()));
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("bayes");
	}

	{
		SVM *model = ssi_pcast (SVM, Factory::Create (SVM::GetCreateName ()));
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("svm");
	}
}

void ex_model_read () {

	{
		Trainer trainer;
		Trainer::Load (trainer, "bayes");	
		ssi_print ("%s\n", trainer.getName ());
	}

	{
		Trainer trainer;
		Trainer::Load (trainer, "svm");		
		ssi_print ("%s\n", trainer.getName ());
	}
}
