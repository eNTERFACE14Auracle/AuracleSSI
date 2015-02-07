// ExEvent.h
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

#ifndef SSI_THREAD_EXEVENT_H
#define SSI_THREAD_EXEVENT_H

#include <list>
#include "thread/ExEventHandler.h"

namespace ssi
{
	class ExEvent
	{
		private:
			std::list<BaseExEventHandler*> m_ExEventHandlers;

		public:
			~ExEvent() { m_ExEventHandlers.clear(); }

			ExEvent& operator+= (BaseExEventHandler* eh) 
			{ 
				m_ExEventHandlers.push_back(eh);
				return *this;
			}

			void operator() (void* obj, ExEventArg* arg) 
			{
				for(std::list<BaseExEventHandler*>::iterator it = m_ExEventHandlers.begin(); it != m_ExEventHandlers.end(); it++)
					(*(*it))(obj, arg);
			}
	};
}

#endif
