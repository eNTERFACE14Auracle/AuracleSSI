// ssiwiimote.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/04/13
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

#ifndef SSI_SIGNAL_H
#define	SSI_SIGNAL_H

#include "base/Factory.h"
#include "signal/Clone.h"
#include "signal/Derivative.h"
#include "signal/Butfilt.h"
#include "signal/Energy.h"
#include "signal/Integral.h"
#include "signal/Intensity.h"
#include "signal/MFCC.h"
#include "signal/Spectrogram.h"
#include "signal/Functionals.h"
#include "signal/SNRatio.h"
#include "signal/DownSample.h"
#include "signal/Selector.h"
#include "signal/Normalize.h"
#include "signal/Chain.h"
#include "signal/MvgAvgVar.h"
#include "signal/MvgMinMax.h"
#include "signal/MvgNorm.h"
#include "signal/MvgPeakGate.h"
#include "signal/MvgDrvtv.h"
#include "signal/MvgConDiv.h"
#include "signal/MvgMedian.h"
#include "signal/Pulse.h"
#include "signal/Multiply.h"
#include "signal/Noise.h"
#include "signal/FFTfeat.h"
#include "signal/ConvPower.h"
#include "signal/Expression.h"
#include "signal/Limits.h"
#include "signal/Gate.h"
#include "signal/Cast.h"
#include "signal/Merge.h"
#include "signal/AudioActivity.h"
#include "signal/AudioIntensity.h"
#include "signal/AudioConvert.h"

#endif
