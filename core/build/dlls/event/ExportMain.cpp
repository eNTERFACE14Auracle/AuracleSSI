// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/03/11 
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

#ifndef DLLEXP
#define DLLEXP extern "C" __declspec( dllexport )
#endif

#include "ssievent.h"

DLLEXP bool Register (ssi::Factory *factory, FILE *logfile, ssi::IMessage *message) {

	ssi::Factory::SetFactory (factory);

	if (logfile) {
		ssiout = logfile;
	}
	if (message) {
		ssimsg = message;
	}

	bool result = true;

	result = ssi::Factory::Register (ssi::TheEventBoard::GetCreateName(), ssi::TheEventBoard::Create) && result;
	result = ssi::Factory::Register (ssi::EventMonitor::GetCreateName(), ssi::EventMonitor::Create) && result;	
	result = ssi::Factory::Register (ssi::FloatsEventSender::GetCreateName(), ssi::FloatsEventSender::Create) && result;
	result = ssi::Factory::Register (ssi::FunctionalsEventSender::GetCreateName(), ssi::FunctionalsEventSender::Create) && result;
	result = ssi::Factory::Register (ssi::ZeroEventSender::GetCreateName(), ssi::ZeroEventSender::Create) && result;
	result = ssi::Factory::Register (ssi::ThresEventSender::GetCreateName(), ssi::ThresEventSender::Create) && result;	
	result = ssi::Factory::Register (ssi::FixationEventSender::GetCreateName(), ssi::FixationEventSender::Create) && result;		
	result = ssi::Factory::Register (ssi::ThresClassEventSender::GetCreateName(), ssi::ThresClassEventSender::Create) && result;		
	result = ssi::Factory::Register (ssi::EventSlider::GetCreateName(), ssi::EventSlider::Create) && result;	

	return result;
}

