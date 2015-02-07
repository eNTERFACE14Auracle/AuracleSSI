// MainFile.cpp
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

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void ex_filepath ();
void ex_simple ();	
void ex_memory ();
void ex_framework ();	
void ex_simulator ();	
void ex_stream ();	
void ex_convert ();
void ex_elan ();

void test (File &file, int *data_out, ssi_size_t size);

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimouse.dll");

	ssi_random_seed ();

	ex_filepath ();	  
	ex_simple ();	  	  
	ex_memory ();
	ex_framework ();  
	ex_simulator ();  
	ex_stream ();
	ex_elan ();
	
	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_filepath () {

	{
		ssi_char_t *filepath = "C:\\mydir\\subdir\\myfile.ext";
		FilePath fp (filepath);
		fp.print ();
	}
	{
		ssi_char_t *filepath = "C:\\mydir\\subdir\\";
		FilePath fp (filepath);
		fp.print ();
	}
	{
		ssi_char_t *filepath = "myfile.ext";
		FilePath fp (filepath);
		fp.print ();
	}
	{
		ssi_char_t *filepath = "myfile";
		FilePath fp (filepath);
		fp.print ();
	}
	{
		ssi_char_t *filepath = "C:\\.myfile";
		FilePath fp (filepath);
		fp.print ();
	}

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_simple () {
		
	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	int data[] = {1,2,3,4,5,6,7,8,9,10};

	{
		ssi_char_t filename[] = "out.txt";
		File *file = File::CreateAndOpen (File::ASCII, File::WRITEPLUS, filename);		
		file->setType (SSI_INT);
		file->setFormat (", ", "010");
		test (*file, data, 10);
		ssi_pcast (FileAscii, file)->show ();
		delete file;
	}

	{
		ssi_char_t filename[] = "out.bin";
		File *file = File::CreateAndOpen (File::BINARY, File::WRITEPLUS, filename);
		file->setType (SSI_INT);
		file->setFormat (", ", "010");
		test (*file, data, 10);
		delete file;
	}

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_memory () {

	FileMem *mem = FileMem::Create (FileMem::ASCII);

	int arr[10];
	for (int i = 0; i < 10; i++) {
		arr[i] = i;
	}

	ssi_size_t n_string = 50;
	ssi_char_t *string = new ssi_char_t[n_string];
	mem->set (n_string, string, true);
	mem->setType (SSI_INT);
	
	mem->writeLine ("");
	mem->writeLine ("array=[");
	mem->write (arr, 2, 10);
	mem->writeLine ("]");
	printf ("%s (%u characters)\n", mem->getMemory (), mem->getPosition ());

	mem->set (strlen (string) + 1, string, false);

	mem->readLine (n_string, string);
	printf ("%s", string);
	mem->read (arr, 2, 10);
	for (int i = 0; i < 10; i++) {
		printf ("%d ", arr[i]);
	}
	mem->readLine (n_string, string);
	printf ("%s\n", string);
	
	delete mem;
	delete[] string;

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_framework () {
	
	ITheFramework *frame = Factory::GetFramework ();

	Mouse *mouse = ssi_pcast (Mouse, Factory::Create (Mouse::GetCreateName (), "mouse"));
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);	
	frame->AddSensor (mouse);

	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	bool continuous = true;

	FileWriter *writer;
	
	writer = ssi_create (FileWriter, 0, true);
	writer->getOptions ()->setPath ("cursor_asc"); 
	writer->getOptions ()->type = File::ASCII;
	writer->getOptions ()->stream = continuous;
	frame->AddConsumer (cursor_p, writer, "0.5s");

	writer = ssi_create (FileWriter, 0, true);	
	writer->getOptions ()->setPath ("cursor_bin");
	writer->getOptions ()->type = File::BINARY;
	writer->getOptions ()->stream = continuous;
	frame->AddConsumer (cursor_p, writer, "0.5s");

	writer = ssi_create (FileWriter, 0, true);	
	writer->getOptions ()->type = File::ASCII;
	writer->getOptions ()->stream = continuous;
	frame->AddConsumer (cursor_p, writer, "0.5s");

	frame->Start ();
	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();
	frame->Stop ();
	frame->Clear ();
}

void ex_simulator () {

	ITheFramework *frame = Factory::GetFramework ();

	File::SetLogLevel (SSI_LOG_LEVEL_DEBUG);
	
	FileReader *reader = ssi_pcast (FileReader, Factory::Create (FileReader::GetCreateName ()));
	ssi_strcpy (reader->getOptions ()->path, "cursor_bin");
	reader->getOptions ()->block = 0.05;	
	reader->getOptions ()->loop = true;
	reader->getOptions ()->offset = 0.1;
	ITransformable *cursor_p = frame->AddProvider (reader, SSI_FILEREADER_PROVIDER_NAME);
	frame->AddSensor (reader);

	FileWriter *writer = ssi_pcast (FileWriter, Factory::Create (FileWriter::GetCreateName ()));
	writer->getOptions ()->type = File::ASCII;
	frame->AddConsumer (cursor_p, writer, "0.05s");

	frame->Start ();
	//ssi_print ("press enter to continue\n");
	//getchar ();
	reader->wait ();
	frame->Stop ();
	frame->Clear ();

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_stream () {

	ssi_stream_t data;
	FileTools::ReadStreamFile ("cursor_bin", data);
	FileTools::WriteStreamFile (File::ASCII, "cursor_bin_check", data);

	ssi_stream_print (data, ssiout);

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void test (File &file, int *data_out, ssi_size_t size) {

	int *data_in = new int[size];

	long int pos = file.tell ();
	if (file.getType () == File::ASCII) {
		if (!file.write (data_out, 2, size)) {
			ssi_err ("write() failed");
		}
	} else {
		if (!file.write (data_out, sizeof (int), size)) {
			ssi_err ("write() failed");
		}
	}
	if (!file.writeLine ("a b c d e f g h i j k l m n o p q r s t u v w x y z")) {
		ssi_err ("writeLine() failed");
	}

	file.seek (pos);
	if (file.getType () == File::ASCII) {
		if (!file.read (data_in, 2, size)) {
			ssi_err ("read() failed");
		}
	} else {
		if (!file.read (data_in, sizeof (int), size)) {
			ssi_err ("read() failed");
		}
	}
	char buffer[100];
	if (!file.readLine (100, buffer)) {
		ssi_err ("readLine() failed");
	}

	for (ssi_size_t i = 0; i < size; i++) {
		ssi_print ("%d ", data_in[i]);
	}
	ssi_print ("\n");
	ssi_print ("%s\n", buffer);
 
	delete[] data_in;

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();
}

void ex_elan () {

	const ssi_char_t *filepath = "test.eaf";

	ElanDocument *elanDoc = ElanDocument::Read (filepath);
	ElanDocument::iterator tier;
	for (tier = elanDoc->begin (); tier != elanDoc->end (); tier++) {		
		ssi_print ("TIER: %s\n", tier->name ());
		ElanTier::iterator anno;		
		for (anno = tier->begin (); anno != tier->end (); anno++) {
			ssi_print ("\t%d-%d:%s\n", anno->from, anno->to, anno->value.str ());
		}
	}

	elanDoc->print ();
	elanDoc->write ("check.eaf");

	ElanTier &tierByName = (*elanDoc)["Transcription"];
	ssi_print ("TIER %s has %u entries\n", tierByName.name (), tierByName.size ());

	ElanTier packedTier ("PackedTranscription");
	tierByName.pack (packedTier);
	packedTier.print ();
	packedTier.unify ("Speech");
	packedTier.print ();

	ElanTier myTier ("MyTier");
	ElanAnnotation a;
	ssi_size_t last_to = 1000;	
	for (int i = 0; i < 10; i++) {
		a.from = last_to + ssi_random (1000u);
		a.to = a.from + ssi_random (1000u);
		last_to = a.to;
		a.value = "label";
		myTier.push_back (a);
	}
	myTier.print ();
	myTier.sort ();
	myTier.print ();

	ElanTier myTierSplit ("MyTierSplit");
	myTier.split (myTierSplit, 1000, 500, 500);
	myTierSplit.print ();
	
	ElanDocument myDoc;
	myDoc.media ().push_back (elanDoc->media ()[0]);
	myDoc.media ().push_back (elanDoc->media ()[1]);
	myDoc.push_back (myTierSplit);
	myDoc.write ("my.eaf");	

	ssi_print ("\n\n\tpress enter to continue\n");
	getchar ();

	delete elanDoc;
}