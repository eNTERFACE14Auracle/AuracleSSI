// OptionList.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/03/17
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

#ifndef SSI_IOPUT_OPTIONLIST_H
#define SSI_IOPUT_OPTIONLIST_H

#include "SSI_Cons.h"
#include "base/IOptions.h"
#include "ioput/xml/tinyxml.h"

namespace ssi {

class OptionList : public IOptions {

public:

	static const char SEPARATOR;

	OptionList ();	
	virtual ~OptionList ();

	bool addOption (const char *name, 
		void *ptr, 
		ssi_size_t num, 
		ssi_type_t type, 
		const char *help);

	bool getOptionValue (const char *name, 
		void *ptr);
	bool setOptionValue (const char *name, 
		void *ptr);	
	bool setOptionValueFromString (const char *name, 
		const char *string);	
	static bool SetOptionValueInPlace (const ssi_char_t *filename, 
		const char *name, 
		const char *string);
	static char *ToString (ssi_option_t &option);
	static bool FromString (const char *str, ssi_option_t &option);

	ssi_option_t *getOption (const char *name);
	ssi_option_t *getOption (ssi_size_t index);
	ssi_size_t getSize ();

	void print (FILE *file);

	static bool LoadXML (const ssi_char_t *filename, IOptions &list);		
	static bool SaveXML (const ssi_char_t *filename, IOptions &list);
	static bool LoadXML (FILE *file, IOptions &list);
	static bool LoadBinary (FILE *file, IOptions &list);
	static bool SaveXML (FILE *file, IOptions &list);	
	static bool SaveBinary (FILE *file, IOptions &list);	

protected:

	static char *ssi_log_name;

	std::vector<ssi_option_t *> _list;	

	static char *Strcpy (const char *string);
	static ssi_size_t GetTypeSize (ssi_type_t);
	static FILE *OpenXML (const ssi_char_t *filename);
};

}

#endif
