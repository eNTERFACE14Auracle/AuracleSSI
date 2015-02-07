// Factory.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/02/28
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

#include "base/Factory.h"
#include "event/TheEventBoard.h"
#include "frame/TheFramework.h"
#include "graphic/ThePainter.h"
#include "thread/Lock.h"
#include "ioput/file/FilePath.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

/*Factory::object_vector_t Factory::_objects;
Factory::dll_handle_map_t Factory::_dll_handle_map;
Factory::object_create_map_t Factory::_object_create_map;
Factory::create_fptr_t *Factory::_create_ex = Factory::CreateEx;
ITheEventBoard *Factory::_board = 0;
ITheFramework *Factory::_frame = 0;
IThePainter *Factory::_painter = 0;
ssi_size_t Factory::_id_counter = 1;
ssi_char_t *Factory::_strings[SSI_FACTORY_STRINGS_MAX];
ssi_size_t Factory::_strings_counter = 0;
Mutex Factory::_mutex;*/
Factory *Factory::_factory = new Factory ();
ssi_char_t *Factory::ssi_log_name = "factory___";
int Factory::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;

Factory::Factory () 
	: _id_counter (1),
	_strings_counter (0),
	_board (0),
	_frame (0),
	_painter (0) {

	for (ssi_size_t i = 0; i < SSI_FACTORY_STRINGS_MAX; i++) {
		_strings[i] = 0;
	}
}
Factory::~Factory () {
}

bool Factory::register_h (const ssi_char_t *name,
	IObject::create_fptr_t create_fptr) {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "register '%s'", name);

	object_create_map_t::iterator it;
	std::pair<object_create_map_t::iterator,bool> ret;

	String string (name);
	ret = _object_create_map.insert (object_create_pair_t (string,create_fptr));
	if (!ret.second) {
		ssi_wrn ("already loaded '%s'", name);
		return false;
	}

	return true;
}

bool Factory::registerDLL (const ssi_char_t *filepath, FILE *logfile, IMessage *message) {

	ssi_char_t *filepath_x = 0;
	#ifdef DEBUG
		size_t len = strlen (filepath);
		if (filepath[len-5] != 'd') {
			FilePath fp (filepath);
			filepath_x = ssi_strcat (fp.getPath (), "d.dll");
		} else {
			filepath_x = ssi_strcpy (filepath);
		}
	#else
		filepath_x = ssi_strcpy (filepath);
	#endif

	dll_handle_map_t::iterator it = _dll_handle_map.find (String (filepath_x));
	bool succeeded = true;

	if (it == _dll_handle_map.end ()) {	

		HMODULE hDLL = 0;

		hDLL = LoadLibrary (filepath_x);

		succeeded = false;
		if(hDLL) {			
			register_fptr_from_dll_t register_fptr = (register_fptr_from_dll_t) GetProcAddress (hDLL, "Register");
			if (register_fptr) {			
				_dll_handle_map.insert (dll_handle_pair_t (String (filepath_x), hDLL));
				ssi_msg (SSI_LOG_LEVEL_DEFAULT, "import '%s'", filepath_x);
				succeeded = register_fptr (_factory, logfile, message);	
			} else {
				ssi_wrn ("Register() function not found '%s'", filepath_x);
				DWORD loadLibraryError = GetLastError();
				if(!FreeLibrary(hDLL))
				{
					DWORD freeLibraryError = GetLastError();
					ssi_wrn ("FreeLibrary() failed (error: %u)", freeLibraryError);
				}
				hDLL = 0;
			}		
		} else {
			ssi_wrn ("not found '%s'", filepath_x);
		}
	} else {
		ssi_msg (SSI_LOG_LEVEL_DEFAULT, "already loaded '%s'", filepath_x);
	}

	delete[] filepath_x;

	return succeeded;
}

IObject *Factory::create (const ssi_char_t *name, const ssi_char_t *file, bool auto_free) {

	if (strcmp (name, TheEventBoard::GetCreateName ()) == 0 && _board) {
		return _board;
	} else if (strcmp (name, TheFramework::GetCreateName ()) == 0 && _frame) {
		return _frame;
	} else if (strcmp (name, ThePainter::GetCreateName ()) == 0 && _painter) {
		return _painter;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "create instance of '%s'", name);

	String string (name);
	object_create_map_t::iterator it;

	it = _object_create_map.find (name);

	if (it == _object_create_map.end ()) {	
		ssi_wrn ("not found '%s'", name);		
		return 0;
	}

	IObject *object = (*it).second (file);
	if (auto_free) {
		_objects.push_back (object);
	}

	if (strcmp (name, TheEventBoard::GetCreateName ()) == 0) {
		_board = ssi_pcast (TheEventBoard, object);
	} else if (strcmp (name, TheFramework::GetCreateName ()) == 0) {
		_frame = ssi_pcast (TheFramework, object);
	} else if (strcmp (name, ThePainter::GetCreateName ()) == 0) {
		_painter = ssi_pcast (ThePainter, object);
	}

	return object;
}

void Factory::clearObjects () {

	ssi_msg (SSI_LOG_LEVEL_DEFAULT, "clear objects");

	delete _board; _board = 0;
	delete _frame; _frame = 0;
	delete _painter; _painter = 0;

	{
		object_vector_t::iterator it;
		for (it = _objects.begin() ; it != _objects.end(); it++) {
			delete *it;
		}
		_objects.clear ();
	}	
}

void Factory::clear () {

	ssi_msg (SSI_LOG_LEVEL_DEFAULT, "clear factory");

	ClearObjects ();

	{
		_object_create_map.clear ();
	}

	{
		dll_handle_map_t::iterator it;		
		for (it = _dll_handle_map.begin() ; it != _dll_handle_map.end(); it++) {
			if(strcmp((it->first.str()),"ssimicrosoftkinectd.dll" ) != 0 && strcmp((it->first.str()),"ssimicrosoftkinect.dll" ) != 0  ) //workaround for current bug in the Microsoft Kinect SDK 1.5 (crash in the end)
			{
				if (it->second) {
					if(!FreeLibrary(it->second))
					{
						DWORD freeLibraryError = GetLastError();
						ssi_wrn("FreeLibrary() failed (error: %u)", freeLibraryError);
					}
					it->second = 0;
				}		
			}
		}
		_dll_handle_map.clear ();
	}

	{
		Lock lock (_mutex);

		for (ssi_size_t i = 0; i < _strings_counter; i++) {
			delete[] _strings[i];
			_strings[i] = 0;
		}
		_strings_counter = 0;
	}
}

ITheEventBoard *Factory::getEventBoard (const ssi_char_t *file) {

	if (!_board) {
		_board = ssi_create (TheEventBoard, file, false);
	}

	return _board;
}

IThePainter *Factory::getPainter (const ssi_char_t *file) {

	if (!_painter) {
		_painter = ssi_create (ThePainter, file, false);
	}

	return _painter;
}

ITheFramework *Factory::getFramework (const ssi_char_t *file) {
	
	if (!_frame) {
		_frame = ssi_create (TheFramework, file, false);
	}

	return _frame;	
}

bool Factory::exportDlls (const ssi_char_t *target_dir) {

	dll_handle_map_t::iterator it;		
	ssi_char_t dll_path[SSI_MAX_CHAR];
	ssi_char_t target_path[SSI_MAX_CHAR];	
	for (it = _dll_handle_map.begin() ; it != _dll_handle_map.end(); it++) {		
		if (GetModuleFileName (it->second, dll_path, SSI_MAX_CHAR) == 0) {
			ssi_wrn ("could not get module name for '%s'", it->first);
		} else {
			FilePath fp (dll_path);
			ssi_sprint (target_path, "%s\\%s", target_dir, fp.getNameFull ());
			if (CopyFile (dll_path, target_path, false) == 0) {
				ssi_wrn ("could not copy '%s' to '%s'", dll_path, target_path);
			}
		}
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "copied '%s' to '%s'", it->first, target_dir);
	}

	return true;
}


ssi_size_t Factory::addString (const ssi_char_t *name) {

	Lock lock (_mutex);

	ssi_msg (SSI_LOG_LEVEL_DEBUG, "add string '%s'", name);

	if (name == 0) {
		return SSI_FACTORY_STRINGS_INVALID_ID;
	}

	// check if string is in list
	const ssi_char_t *str = 0;
	for (ssi_size_t i = 0; i < _strings_counter; i++) {
		str = _strings[i];
		if (strcmp (name, str) == 0) {
			return i;
		}
	}

	// otherwise add string
	ssi_size_t index = 0;
	{
		index = _strings_counter;
		if (_strings_counter == SSI_FACTORY_STRINGS_MAX) {
			ssi_wrn ("#string exceeds available space '%u'", SSI_FACTORY_STRINGS_MAX);
			return SSI_FACTORY_STRINGS_INVALID_ID;
		} else {
			_strings[_strings_counter++] = ssi_strcpy (name);
		}
	}

	return index;
}

ssi_size_t Factory::getStringId (const ssi_char_t *name) {

	Lock lock (_mutex);

	for (ssi_size_t i = 0; i < _strings_counter; i++) {
		if (strcmp (name, _strings[i]) == 0) {
			return i;
		}
	}
	
	ssi_wrn ("string '%s' does not exist", name);
	return SSI_FACTORY_STRINGS_INVALID_ID;
}

const ssi_char_t *Factory::getString (ssi_size_t id) {

	Lock lock (_mutex);

	if (id == SSI_FACTORY_STRINGS_INVALID_ID) {
		return "undef";
	}

	if (id >= _strings_counter) {
		ssi_wrn ("id '%u' exceeds available #strings '%u'", id, _strings_counter);
		return 0;
	}

	return _strings[id];
}

ssi_size_t Factory::getUniqueId () {

	{
		Lock lock (_mutex);
		return _id_counter++;
	}
}

}
