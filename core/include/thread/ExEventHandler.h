// BaseExEventHandler.h
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

/*
 * @Author	Benjamin Hrzek
 * @Date	Dec 2009
*/

#pragma once

#ifndef SSI_THREAD_EXEVENTHANDLER_H
#define SSI_THREAD_EXEVENTHANDLER_H

#include "thread/ExEventArg.h"

namespace ssi
{
	class BaseExEventHandler
	{
		public:
			virtual void operator() (void*, ExEventArg*) = 0;
	};

	template<typename T>
	class ExEventHandler : public BaseExEventHandler
	{
		private:
			typedef void (T::*Member)(void*, ExEventArg*);
			
			T*		m_Object;
			Member	m_MemberFunction;

		public:
			ExEventHandler(T* obj, Member m) : m_Object(obj), m_MemberFunction(m) {}

			void operator() (void* obj, ExEventArg* arg) { (((T*)m_Object)->*m_MemberFunction)(obj, arg); }
	};
}

#endif
