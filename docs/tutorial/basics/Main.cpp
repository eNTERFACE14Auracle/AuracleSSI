// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/04/24
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
#include "MyObject.h"
using namespace ssi;

struct point_s {
	int x;
	int y;
};

void print_float (ssi_stream_t &s) {
	float *ptr = ssi_pcast (float, s.ptr);
	for (ssi_size_t nsamp = 0; nsamp < s.num; nsamp++) {
		for (ssi_size_t ndim = 0; ndim < s.dim; ndim++) {
			ssi_print ("%10.2f", *ptr++);		
		}
		ssi_print ("\n");
	}
}

void print_point (ssi_stream_t &s) {
	point_s *ptr = ssi_pcast (point_s, s.ptr);
	for (ssi_size_t nsamp = 0; nsamp < s.num; nsamp++) {
		for (ssi_size_t ndim = 0; ndim < s.dim; ndim++) {
			ssi_print ("<%2d,%2d> ", ptr->x, ptr->y);		
			ptr++;
		}
		ssi_print ("\n");
	}
}

void ex_string ();
void ex_stream ();
void ex_object ();
void ex_point ();
void ex_array ();

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::Register (MyObject::GetCreateName (), MyObject::Create);

	ex_object ();
	ex_string ();
	ex_stream ();	
	ex_point ();
	ex_array ();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

	return 0;
}

void ex_object () {
	
	MyObject *o = ssi_pcast (MyObject, Factory::Create (MyObject::GetCreateName (), "object"));
	// short: MyObject *o = ssi_create (MyObject, "object", true);
	o->print ();
	o->getOptions ()->setString ("hello world");
	o->getOptions ()->toggle = true;
	o->print ();

	printf ("\n\n");

	o->getOptions ()->print (ssiout);

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}

void ex_string () {

	String mystring;
	printf ("\"%s\"\n", mystring.str ());
	mystring = "bla";
	printf ("\"%s\"\n", mystring.str ());
	printf ("%s == bla -> %s\n", mystring, mystring == "bla" ? "true" : "false");
	printf ("%s == blau -> %s\n", mystring, mystring == "blau" ? "true" : "false");
	printf ("%s != blau -> %s\n", mystring, mystring != "blau" ? "true" : "false");
	mystring += "bla";
	printf ("\"%s\"\n", mystring.str ());
	mystring = String ("hello") + String (" ") + String ("world");
	printf ("\"%s\"\n", mystring.str ());
	
	printf ("\n\n");

	printf ("id = %u\n", -1);

	ssi_size_t id = 0;

	ssi_size_t n_strings = 10;
	ssi_char_t *strings[] = {"Fischers", "Fritze", "fischt", "frische", "Fische", "Frische", "Fische", "fischt", "Fischers", "Fritze"};

	for (ssi_size_t i = 0; i < n_strings; i++) {
		Factory::AddString (strings[i]);
	}
	for (ssi_size_t i = 0; i < n_strings; i++) {
		ssi_print ("%s[%u] ", Factory::GetString (Factory::GetStringId (strings[i])), Factory::GetStringId (strings[i]));
	}
	ssi_print ("\n");

	id = Factory::GetStringId ("invalid");
	if (id != SSI_FACTORY_STRINGS_INVALID_ID) {
		printf ("id = %u\n", id);
	} else {
		printf ("invalid id\n");
	}

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}

void ex_stream () {

	ssi_stream_t s, ss;
	ssi_random_seed ();

	// initialize s with float values

	ssi_time_t len = 1.0;
	ssi_time_t sr = 10.0;
	ssi_size_t dim = 3;
	ssi_size_t byte = sizeof (float);
	ssi_type_t type = SSI_FLOAT;

	ssi_size_t num = ssi_cast (ssi_size_t, len * sr);
	ssi_stream_init (s, num, dim, byte, type, sr);
	
	float *ptr = ssi_pcast (float, s.ptr);
	for (ssi_size_t nsamp = 0; nsamp < s.num; nsamp++) {
		for (ssi_size_t ndim = 0; ndim < s.dim; ndim++) {
			*ptr++ = ssi_cast (float, ssi_random ());
		}
	}
	
	ssi_print ("s:\n");
	print_float (s);

	// clone s

	ssi_stream_clone (s, ss);

	// append s

	ssi_stream_adjust (ss, ss.num + s.num);
	memcpy (ss.ptr + s.tot, s.ptr, s.tot);

	ssi_print ("ss:\n");
	print_float (ss);

	// using File to store and read s

	File *console = File::CreateAndOpen (File::ASCII, File::WRITE, 0);
	console->writeLine ("writing on the console...");
	console->setType (s.type);
	console->write (s.ptr, s.dim, s.dim * s.num);

	ssi_stream_destroy (ss);

	{
		console->writeLine ("writing/reading from an ascii file...");

		FileTools::WriteStreamFile (File::ASCII, "data_a", s);
		FileTools::ReadStreamFile ("data_a", ss);		
		
		console->write (ss.ptr, ss.dim, ss.dim * ss.num);		
	}

	ssi_stream_destroy (ss);

	{
		console->writeLine ("writing/reading from a binary file...");
		
		FileStreamOut file_out;
		file_out.open (s, "data_b", File::BINARY);
		file_out.write (s, true);
		file_out.write (s, true);
		file_out.write (s, true);		
		file_out.close ();
		
		FileStreamIn file_in;
		file_in.open (ss, "data_b");				
		while (file_in.read (ss)) {
			console->write (ss.ptr, ss.dim, ss.dim * ss.num);
		}		
		file_in.close ();
	}

	ssi_stream_destroy (s);
	ssi_stream_destroy (ss);
	delete console;

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}

void ex_point () {

	// stream with point values

	ssi_stream_t s;

	ssi_stream_init (s, 5, 3, sizeof (point_s), SSI_STRUCT, 1.0);
	point_s *ptr = ssi_pcast (point_s, s.ptr);
	for (ssi_size_t i = 0; i < s.num; i++) {
		for (ssi_size_t j = 0; j < s.dim; j++) {
			ptr->x = i; ptr->y = j;
			ptr++;
		}
	}

	ssi_print ("s:\n");
	print_point (s);

	ssi_stream_destroy (s);

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}

void ex_array () {

	ssi_random_seed ();

	Array1D<float> a;
	a.init (5);	
	for (float *ptr = a.ptr (); ptr < a.end (); ptr++) {	
		*ptr = ssi_cast (float, ssi_random ());
	}
	a[0] = 0;
	a.print (&Array1D<float>::ToString, ssiout);
	a.clear ();
	a.print (&Array1D<float>::ToString, ssiout, 2, 3);
	a.release ();
	a.print (&Array1D<float>::ToString);

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}
