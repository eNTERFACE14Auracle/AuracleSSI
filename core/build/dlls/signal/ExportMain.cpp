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

#include "ssisignal.h"

#ifndef DLLEXP
#define DLLEXP extern "C" __declspec( dllexport )
#endif

DLLEXP bool Register (ssi::Factory *factory, FILE *logfile, ssi::IMessage *message) {

	ssi::Factory::SetFactory (factory);

	if (logfile) {
		ssiout = logfile;
	}
	if (message) {
		ssimsg = message;
	}

	bool result = true;
	
	result = ssi::Factory::Register (ssi::MFCC::GetCreateName (),ssi::MFCC::Create) && result;	
	result = ssi::Factory::Register (ssi::Energy::GetCreateName (),ssi::Energy::Create) && result;	
	result = ssi::Factory::Register (ssi::Intensity::GetCreateName (),ssi::Intensity::Create) && result;	
	result = ssi::Factory::Register (ssi::Functionals::GetCreateName (),ssi::Functionals::Create) && result;	
	result = ssi::Factory::Register (ssi::Clone::GetCreateName (),ssi::Clone::Create) && result;	
	result = ssi::Factory::Register (ssi::Derivative::GetCreateName (),ssi::Derivative::Create) && result;
	result = ssi::Factory::Register (ssi::Integral::GetCreateName (),ssi::Integral::Create) && result;
	result = ssi::Factory::Register (ssi::Butfilt::GetCreateName (),ssi::Butfilt::Create) && result;
	result = ssi::Factory::Register (ssi::IIR::GetCreateName (),ssi::IIR::Create) && result;
	result = ssi::Factory::Register (ssi::Spectrogram::GetCreateName (),ssi::Spectrogram::Create) && result;	
	result = ssi::Factory::Register (ssi::DownSample::GetCreateName (),ssi::DownSample::Create) && result;
	result = ssi::Factory::Register (ssi::Normalize::GetCreateName (),ssi::Normalize::Create) && result;
	result = ssi::Factory::Register (ssi::SNRatio::GetCreateName (),ssi::SNRatio::Create) && result;		
	result = ssi::Factory::Register (ssi::Selector::GetCreateName (),ssi::Selector::Create) && result;
	result = ssi::Factory::Register (ssi::Chain::GetCreateName (),ssi::Chain::Create) && result;
	result = ssi::Factory::Register (ssi::MvgAvgVar::GetCreateName (),ssi::MvgAvgVar::Create) && result;
	result = ssi::Factory::Register (ssi::MvgMinMax::GetCreateName (),ssi::MvgMinMax::Create) && result;
	result = ssi::Factory::Register (ssi::MvgNorm::GetCreateName (),ssi::MvgNorm::Create) && result;
	result = ssi::Factory::Register (ssi::MvgPeakGate::GetCreateName (),ssi::MvgPeakGate::Create) && result;
	result = ssi::Factory::Register (ssi::MvgDrvtv::GetCreateName (),ssi::MvgDrvtv::Create) && result;
	result = ssi::Factory::Register (ssi::MvgConDiv::GetCreateName (),ssi::MvgConDiv::Create) && result;
	result = ssi::Factory::Register (ssi::MvgMedian::GetCreateName (),ssi::MvgMedian::Create) && result;
	result = ssi::Factory::Register (ssi::Pulse::GetCreateName (),ssi::Pulse::Create) && result;
	result = ssi::Factory::Register (ssi::Multiply::GetCreateName (),ssi::Multiply::Create) && result;
	result = ssi::Factory::Register (ssi::Noise::GetCreateName (),ssi::Noise::Create) && result;
	result = ssi::Factory::Register (ssi::FFTfeat::GetCreateName (),ssi::FFTfeat::Create) && result;
	result = ssi::Factory::Register (ssi::ConvPower::GetCreateName (),ssi::ConvPower::Create) && result;
	result = ssi::Factory::Register (ssi::Expression::GetCreateName (),ssi::Expression::Create) && result;
	result = ssi::Factory::Register (ssi::Limits::GetCreateName (),ssi::Limits::Create) && result;
	result = ssi::Factory::Register (ssi::Gate::GetCreateName (),ssi::Gate::Create) && result;
	result = ssi::Factory::Register (ssi::Cast::GetCreateName (),ssi::Cast::Create) && result;
	result = ssi::Factory::Register (ssi::Merge::GetCreateName (),ssi::Merge::Create) && result;
	result = ssi::Factory::Register (ssi::AudioActivity::GetCreateName (),ssi::AudioActivity::Create) && result;
	result = ssi::Factory::Register (ssi::AudioIntensity::GetCreateName (),ssi::AudioIntensity::Create) && result;
	result = ssi::Factory::Register (ssi::AudioConvert::GetCreateName (),ssi::AudioConvert::Create) && result;

	return result;
}
