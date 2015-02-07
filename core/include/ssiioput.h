// ssi.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/09/05
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

#ifndef SSI_IOPUT_H
#define	SSI_IOPUT_H

#include "base/Factory.h"
#include "ioput/file/FileWriter.h"
#include "ioput/file/FileReader.h"
#include "ioput/file/FileEventWriter.h"
#include "ioput/socket/SocketWriter.h"
#include "ioput/socket/SocketReader.h"
#include "ioput/socket/SocketEventWriter.h"
#include "ioput/socket/SocketEventReader.h"
#include "ioput/wav/WavReader.h"
#include "ioput/wav/WavWriter.h"
#include "ioput/wav/WavProvider.h"
#include "ioput/avi/AviReader.h"
#include "ioput/avi/AviWriter.h"

#endif
