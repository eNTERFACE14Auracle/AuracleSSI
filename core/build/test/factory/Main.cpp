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

void ex_string ();
void ex_array ();
void ex_object ();

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::Register (MyObject::GetCreateName (), MyObject::Create);

	ex_string ();
	ex_array ();
	ex_object ();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

	return 0;
}

void ex_string () {

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

void ex_object () {
	
	Factory::AddString ("test");

	MyObject *o = ssi_pcast (MyObject, Factory::Create (MyObject::GetCreateName (), "object"));
	o->print ();
	o->getOptions ()->setString ("hello world");
	o->getOptions ()->toggle = true;
	o->print ();

	ssi_print ("\n\npress enter to continue\n");
	getchar ();
}


