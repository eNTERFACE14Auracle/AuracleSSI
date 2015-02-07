// IOptions.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/03/01
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

#ifndef SSI_IOPTIONS_H
#define SSI_IOPTIONS_H

#include "SSI_Cons.h"

namespace ssi {

class IOptions {

public:

	virtual ~IOptions () {};

	virtual bool addOption (const char *name, 
		void *ptr, 
		ssi_size_t num, 
		ssi_type_t type, 
		const ssi_char_t *help) = 0;
	virtual bool getOptionValue (const char *name, 
		void *ptr) = 0;
	virtual bool setOptionValue (const char *name, 
		void *ptr) = 0;	
	virtual bool setOptionValueFromString (const char *name, 
		const char *string) = 0;	
	virtual ssi_option_t *getOption (const char *name) = 0;
	virtual ssi_option_t *getOption (ssi_size_t index) = 0;
	virtual ssi_size_t getSize () = 0;
};

}

#endif
