// Factory.h
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

#pragma once

#ifndef SSI_BASE_FACTORY_H
#define SSI_BASE_FACTORY_H

#include "base/IObject.h"
#include "base/IMessage.h"
#include "base/String.h"
#include "base/ITheEventBoard.h"
#include "base/ITheFramework.h"
#include "base/IThePainter.h"
#include "base/Array1D.h"

namespace ssi {

#define ssi_create(NAME,FILE,FLAG) (ssi_pcast (NAME, Factory::Create (NAME::GetCreateName (), FILE, FLAG)))
#define ssi_factory_create(NAME,FILE,FLAG) (ssi_pcast (NAME, Factory::Create (NAME::GetCreateName (), FILE, FLAG)))

class Factory {

friend class APIGenerator;

public:

	typedef bool (register_dll_fptr_t)(const ssi_char_t *dllpath, FILE *logfile, IMessage *message);
	typedef IObject *(create_fptr_t)(const ssi_char_t *name, const ssi_char_t *file, bool auto_free);
	//typedef bool (*register_fptr_t)(const ssi_char_t *, IObject::create_fptr_t);		
	typedef bool (*register_fptr_from_dll_t)(Factory *factory, FILE *logfile, IMessage *message);	

public:		

	static Factory *GetFactory () {
		return _factory;
	}
	static void SetFactory (Factory *factory) {
		_factory = factory;
	}
 	
	static bool Register (const ssi_char_t *name, 
		IObject::create_fptr_t create_fptr) {
		return _factory->register_h (name, create_fptr);
	}
	static bool RegisterDLL (const ssi_char_t *dllpath,
		FILE *logfile = 0,
		IMessage *message = 0) {
		return _factory->registerDLL (dllpath, logfile, message);
	}
	static bool ExportDlls (const ssi_char_t *target_dir) {
		return _factory->exportDlls (target_dir);
	}
	
	static IObject *Factory::Create (const ssi_char_t *name, 
		const ssi_char_t *file = 0,
		bool auto_free = true) {		
		return _factory->create (name, file, auto_free);
	}
	static void ClearObjects () {
		_factory->clearObjects ();
	}
	static void Clear () {
		_factory->clear ();
	}

	static ITheEventBoard *GetEventBoard (const ssi_char_t *file = 0) {
		return _factory->getEventBoard (file);
	}
	static ITheFramework *GetFramework (const ssi_char_t *file = 0) {
		return _factory->getFramework (file);
	}
	static IThePainter *GetPainter (const ssi_char_t *file = 0) {
		return _factory->getPainter (file);
	}	

	static ssi_size_t AddString (const ssi_char_t *str) {
		return _factory->addString (str);
	}
	static const ssi_char_t *GetString (ssi_size_t id) {
		return _factory->getString (id);
	}
	static ssi_size_t GetStringId (const ssi_char_t *str) {
		return _factory->getStringId (str);
	}
	static ssi_size_t GetUniqueId () {
		return _factory->getUniqueId ();
	}

	static void SetLogLevel (int level) {
		ssi_log_level = level;
	}	

protected:

	Factory ();
	virtual ~Factory ();

	static ssi_char_t *ssi_log_name;
	static int ssi_log_level;

	static Factory *_factory;

	// register

	bool register_h (const ssi_char_t *name, 
		IObject::create_fptr_t create_fptr);		
	bool registerDLL (const ssi_char_t *dllpath,
		FILE *logfile = 0,
		IMessage *message = 0);
	bool exportDlls (const ssi_char_t *target_dir);

	typedef std::map<String, HMODULE> dll_handle_map_t;
	typedef std::pair<String, HMODULE> dll_handle_pair_t;
	dll_handle_map_t _dll_handle_map;

	// create

	IObject *Factory::create (const ssi_char_t *name, 
		const ssi_char_t *file = 0,
		bool auto_free = true);

	typedef std::vector<IObject *> object_vector_t;
	object_vector_t _objects;

	typedef std::map<String, IObject::create_fptr_t> object_create_map_t;
	typedef std::pair<String, IObject::create_fptr_t> object_create_pair_t;
	object_create_map_t _object_create_map;

	// clean up

	void clearObjects ();
	void clear ();
	
	// singletons

	ITheEventBoard *getEventBoard (const ssi_char_t *file = 0);
	ITheFramework *getFramework (const ssi_char_t *file = 0);
	IThePainter *getPainter (const ssi_char_t *file = 0);

	ITheEventBoard *_board;
	ITheFramework *_frame;
	IThePainter *_painter;

	// string functions

	ssi_size_t addString (const ssi_char_t *str);
	const ssi_char_t *getString (ssi_size_t id); 
	ssi_size_t getStringId (const ssi_char_t *str);
	ssi_size_t getUniqueId ();

	Mutex _mutex;
	ssi_size_t _id_counter;
	ssi_char_t *_strings[SSI_FACTORY_STRINGS_MAX];
	ssi_size_t _strings_counter;
};

}

#endif
