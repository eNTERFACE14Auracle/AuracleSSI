// ExEventArg.h
// author: Benjamin Hrzek <hrzek@arcor.de>
// created: 2009/12/01
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

#ifndef SSI_THREAD_EXEVENTARG_H
#define SSI_THREAD_EXEVENTARG_H

namespace ssi
{
	class ExEventArg
	{
		protected:
			char* m_Message;

		public:
			virtual ~ExEventArg() 
			{
				delete[] m_Message;
			}

			char* getMessage() const { return m_Message; }
	};
}

#endif