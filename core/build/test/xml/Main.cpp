// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/10/11
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
void ex_export ();

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main () {	

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");

	ssi_execute ("sockspy.exe", "--fork 61111 137.250.171.148:61121;137.250.171.87:61131", 0);

	ex_xml ();
	ex_export ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void ex_xml () {

	XMLPipeline *xmlpipe = ssi_create (XMLPipeline, 0, false);
	xmlpipe->SetRegisterDllFptr (Factory::RegisterDLL);
	xmlpipe->setLogLevel (SSI_LOG_LEVEL_DEBUG);
	ssi_size_t n_confs = 3;
	ssi_char_t *confs[] = {"global","global2","global3"};
	xmlpipe->parse ("my", 3, confs, true);
	
	IThePainter *painter = Factory::GetPainter ();
	ITheFramework *frame = Factory::GetFramework ();
	ITheEventBoard *eboard = Factory::GetEventBoard ();

	frame->Start ();
	if (xmlpipe->startEventBoard ()) {
		eboard->Start ();
	}
	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();
	if (xmlpipe->startEventBoard ()) {
		eboard->Stop ();
	}
	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
	eboard->Clear ();
	
	delete xmlpipe;
}

void ex_export () {	

	ITheFramework *frame = Factory::GetFramework ();
	XMLPipeline *xmlpipe = ssi_create (XMLPipeline, 0, false);
	ssi_size_t n_confs = 3;
	ssi_char_t *confs[] = {"global","global2","global3"};
	xmlpipe->parse ("my", 3, confs, false);

	const ssi_char_t *dir = "dlls";
	ssi_mkdir (dir);
	Factory::ExportDlls (dir);

	delete xmlpipe;
	frame->Clear ();
}