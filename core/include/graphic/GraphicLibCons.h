// GraphicLibCons.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/01/04
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

#ifndef SSI_GRAPHIC_GRAPHICLIBCONS_H
#define	SSI_GRAPHIC_GRAPHICLIBCONS_H

#include "SSI_Cons.h"

#ifndef NOGDI
#ifndef _MAC
#include <winspool.h>
#ifdef INC_OLE1
#include <ole.h>
#else
#include <ole2.h>
#endif /* !INC_OLE1 */
#endif /* !MAC */
#include <commdlg.h>
#endif /* !NOGDI */

#include <gdiplus.h>

//#ifdef _MSC_VER 
//#pragma comment (lib, "gdiplus.lib")
//#endif

#include "thread/Thread.h"
#include "thread/Lock.h"

namespace ssi {

//! define for debug information
//#define _THEPAINTER_DEBUG

//! Maximum number of canvas objects handled by the painter
#define THEPAINTER_MAX_BUFFER 32
//! Window class name
#define THEPAINTER_WINDOW_CLASS_NAME "PAINTER"

enum PAINT_TYPE : unsigned char {
	PAINT_TYPE_SIGNAL = 0,
	PAINT_TYPE_PATH,
	PAINT_TYPE_SCATTER,
	PAINT_TYPE_IMAGE,
	PAINT_TYPE_VIDEOIMAGE
};

enum COLORMAP_TYPE : unsigned char {
	COLORMAP_TYPE_BLACKANDWHITE = 0,
	COLORMAP_TYPE_COLOR16,
	COLORMAP_TYPE_COLOR64,
	COLORMAP_TYPE_GRAY64
};

enum MOVE_EX_TYPE : unsigned char {
	MOVE_EX_TYPE_MOVE_AND_RESIZE = 0,
	MOVE_EX_TYPE_ONLY_MOVE,
	MOVE_EX_TYPE_ONLY_RESIZE
};

}

#endif

									
