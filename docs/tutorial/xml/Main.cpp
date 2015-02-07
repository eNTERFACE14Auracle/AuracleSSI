// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/05/13
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

void ex_xml ();

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssiaudio.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimodel.dll");

	ex_xml ();
	
	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

	return 0;
}

void ex_xml () {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
	ITheEventBoard *board = Factory::GetEventBoard ();

	XMLPipeline *xmlpipe = ssi_factory_create (XMLPipeline, 0, true);
	xmlpipe->parse ("my.pipeline");
	
	frame->Start ();
	board->Start ();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	board->Stop ();
	frame->Stop ();
	board->Clear ();
	frame->Clear ();
	painter->Clear ();
}
