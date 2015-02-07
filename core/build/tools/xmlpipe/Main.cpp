// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/03/29
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


ssi_char_t _workdir[SSI_MAX_CHAR];

void Run (const ssi_char_t *exepath, const ssi_char_t *filepath, const ssi_char_t *configpaths, bool savepipe, bool init, bool export_dlls, bool show_close_but);
BOOL WINAPI ConsoleHandler(DWORD CEvent);

int main (int argc, char **argv) {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	char info[1024];
	ssi_sprint (info, "\n%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	//set up windows event listener to catch console close event
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
		ssi_print("WARNING: Unable to install console handler!\n");

	//**** READ COMMAND LINE ****//

	CmdArgParser cmd;
	cmd.info (info);

	ssi_char_t *filepath = 0;	
	ssi_char_t *debugpath = 0;
	ssi_char_t *configpaths = 0;
	bool init_only, debug_to_file, export_dlls, show_close_but, savepipe;
	
	cmd.addText("\nArguments:");
	cmd.addSCmdArg("filepath", &filepath, "path to pipeline");	

	cmd.addText ("\nOptions:");
	cmd.addSCmdOption ("-config", &configpaths, "", "paths to global config files (seperated by ;) []");		
	cmd.addBCmdOption ("-save", &savepipe, false, "save pipeline after applying config files [false]");		
	cmd.addBCmdOption ("-init", &init_only, false, "only initialize pipepline [false]");		
	cmd.addBCmdOption ("-dbg2file", &debug_to_file, false, "debug to file [false] (deprecated, use -debug instead)");
	cmd.addSCmdOption ("-debug", &debugpath, "", "debug to file or udp socket if set to <host>:<port> [""]");
	cmd.addBCmdOption ("-export", &export_dlls, false, "copy registered dlls to the folder of the executable [false]");
	cmd.addBCmdOption ("-closebut", &show_close_but, false, "enable close button in menu of console [false]");
	
	if (cmd.read (argc, argv)) {				

		if (debug_to_file) {
			ssi_log_file_begin ("ssi_dbg.txt");
		}
		if (debugpath[0] != '\0') {
			ssi_size_t n = ssi_split_string_count (debugpath, ':');
			if (n > 1) {
				ssi_char_t **tokens = new ssi_char_t *[n];
				ssi_split_string (n, tokens, debugpath, ':');
				ssi_size_t port = 0;
				sscanf (tokens[1], "%u", &port);
				ssimsg = new SocketMessage (Socket::UDP, port, tokens[0]);
				for (ssi_size_t i = 0; i < n; i++) {
					delete[] tokens[i];
				}
				delete[] tokens;
			} else {
				ssimsg = new FileMessage (debugpath);
			}
		}
		ssi_print ("%s", info);

		if (configpaths[0] != '\0') {
			Run (argv[0], filepath, configpaths, savepipe, init_only, export_dlls, show_close_but);
		} else {
			Run (argv[0], filepath, 0, savepipe, init_only, export_dlls, show_close_but);
		}

		if (debug_to_file){
			ssi_log_file_end ();
		}
		if (debugpath[0] != '\0') {
			delete ssimsg;
			ssimsg = 0;
		}

	}

	delete[] filepath;	
	delete[] configpaths;
	delete[] debugpath;
	
#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void RemoveCloseButton () {

	HWND hWnd = GetConsoleWindow();
	// disable the [x] button.
	if(hWnd != NULL)
	{
		HMENU hMenu = GetSystemMenu(hWnd,0);
		if(hMenu != NULL)
		{
			DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
			DrawMenuBar(hWnd);
		}
	}

}

void Run (const ssi_char_t *exepath, const ssi_char_t *filepath, const ssi_char_t *configpaths, bool savepipe, bool init_only, bool export_dlls, bool show_close_but) {	

	Factory::RegisterDLL ("ssiframe.dll", ssiout, ssimsg);
	Factory::RegisterDLL ("ssievent.dll", ssiout, ssimsg);

	// disable close button
	if (!show_close_but) {
		RemoveCloseButton ();
	}

	// working directory
	::GetCurrentDirectory (SSI_MAX_CHAR, _workdir);

	// executable directory
	FilePath exepath_fp (exepath);	
	ssi_char_t exedir[SSI_MAX_CHAR];
	if (exepath_fp.isRelative ()) {
		ssi_sprint (exedir, "%s\\%s", _workdir, exepath_fp.getDir ());
	} else {
		strcpy (exedir, exepath_fp.getDir ());
	}

	// full pipepath
	FilePath filepath_fp (filepath);	
	ssi_char_t pipepath[SSI_MAX_CHAR];
	if (filepath_fp.isRelative ()) {
		ssi_sprint (pipepath, "%s\\%s", _workdir, filepath);
	} else {
		strcpy (pipepath, filepath);
	}

	// set working directory to pipeline directory
	::SetCurrentDirectory (filepath_fp.getDir ());

	XMLPipeline *xmlpipe = ssi_create (XMLPipeline, 0, true);

	xmlpipe->SetRegisterDllFptr (Factory::RegisterDLL);
	bool result = false;
	if (configpaths) {
		ssi_size_t n = 0;
		n = ssi_split_string_count (configpaths, ';');
		ssi_char_t **ns = new ssi_char_t *[n];
		ssi_split_string (n, ns, configpaths, ';');
		result = xmlpipe->parse (pipepath, n, ns, savepipe);
	} else {
		result = xmlpipe->parse (pipepath, 0, 0, savepipe);
	}

	if (!result) {
		ssi_print ("ERROR: could not parse pipeline from '%s'\n", pipepath);
		ssi_print ("\n\n\t\tpress enter to quit\n");
		getchar ();
	} else {

		if (export_dlls) {
			Factory::ExportDlls (exedir);
		}

		if (!init_only) {			

			ITheFramework *frame = Factory::GetFramework ();
			IThePainter *painter = 0;
			if (xmlpipe->startPainter ()) {
				painter = Factory::GetPainter ();
			}
			ITheEventBoard *eboard = 0;
			if (xmlpipe->startEventBoard ()) {
				eboard = Factory::GetEventBoard ();
			}						
			frame->Start ();
			if (eboard) {
				eboard->Start ();
			}
			frame->Wait ();
			if (eboard) {
				eboard->Stop ();
			}
			frame->Stop ();			
			frame->Clear ();
			if (eboard) {
				eboard->Clear ();
			}
			if (painter) {
				painter->Clear ();
			}
			
		} else {
			ssi_print ("SUCCESS: initialization ok");
			ssi_print ("\n\n\t\tpress enter to quit\n");
			getchar ();
		}
	}

	Factory::Clear ();
	::SetCurrentDirectory (_workdir);
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    switch(CEvent)
    {
    case CTRL_CLOSE_EVENT:        	
		
		ITheFramework *frame = Factory::GetFramework ();
		IThePainter *painter = 0;
		painter = Factory::GetPainter ();
		ITheEventBoard *eboard = 0;
		eboard = Factory::GetEventBoard ();

		if (eboard) {
			eboard->Stop ();
		}
		frame->Stop ();			
		frame->Clear ();
		if (eboard) {
			eboard->Clear ();
		}
		if (painter) {
			painter->Clear ();
		}		

		Factory::Clear ();
		::SetCurrentDirectory (_workdir);

		break;
    }
    return TRUE;
}