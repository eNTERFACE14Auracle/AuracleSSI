// FileProvider.h
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

#pragma once

#ifndef SSI_BASE_ITHEPAINTER_H
#define SSI_BASE_ITHEPAINTER_H

#include "base/IObject.h"
#include "graphic/GraphicLibCons.h"
#include "graphic/CanvasThread.h"

namespace ssi {
	
class IThePainter : public IObject {

public:

	virtual ~IThePainter () {};

	virtual int AddCanvas(const char* name, COLORMAP_TYPE colormap_type = COLORMAP_TYPE_COLOR64) = 0;
	virtual int AddCanvas(const char* name, const unsigned char *colormap, unsigned int n_colors) = 0;
	virtual bool RemCanvas(int convas_id) = 0;
	virtual bool Clear(int canvas_id) = 0;
	virtual bool AddObject(int canvas_id, PaintObject *object) = 0;
	virtual bool Paint(int canvas_id) = 0;
	virtual bool Move(int canvas_id, int x, int y, int width, int height) = 0;
	virtual bool MoveEX (int canvas_id, int x, int y, int width, int height, bool paramsDescribeClientRect = false, MOVE_EX_TYPE moveAndOrResize = MOVE_EX_TYPE_MOVE_AND_RESIZE) = 0;
	virtual void Arrange (int numh, int numv, int start_x, int start_y, int width, int height) = 0;
	virtual void MoveConsole (int x, int y, int width, int height) = 0;
	virtual void Clear () = 0;

};

}

#endif
