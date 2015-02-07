// ThePainter.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/07
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

/* Offers some basic plotting functionality. */

#ifndef SSI_GRAPHIC_THEPAINTER_H
#define SSI_GRAPHIC_THEPAINTER_H

#include "base/IObject.h"
#include "base/IThePainter.h"
#include "ioput/option/OptionList.h"
#include "graphic/GraphicLibCons.h"
#include "graphic/CanvasThread.h"

namespace ssi {

class ThePainter : public IThePainter {

friend class Factory;

public: 

	class Options : public OptionList {

	public:
		Options () 
			: arrange (false) {

			apos[0] = 1;
			apos[1] = 1;
			apos[2] = 0;
			apos[3] = 0;
			apos[4] = 400;
			apos[5] = 400;

			addOption ("arrange", &arrange, 1, SSI_BOOL, "arrange windows");	
			addOption ("apos", &apos, 6, SSI_INT, "position of arranged windows [numh,numv,posx,posy,width,height]");
		}

		void Arrange (int numh, int numv, int x, int y, int width, int height) {
			arrange = true;
			apos[0] = numh;
			apos[1] = numv;
			apos[2] = x;
			apos[3] = y;
			apos[4] = width;
			apos[5] = height;
		}

		bool arrange;
		int apos[6];
	};

public:

	Options *getOptions () { return &_options; }
	static const ssi_char_t *GetCreateName () { return "ssi_object_ThePainter"; }
	const ssi_char_t *getName () { return GetCreateName(); }
	const ssi_char_t *getInfo () { return "Creates windows and handles paint operations."; }
	static IObject* Create(const char* file) { return new ThePainter (file); }

    void Clear ();
	int AddCanvas (const char* name, COLORMAP_TYPE colormap_type = COLORMAP_TYPE_COLOR64);
	int AddCanvas (const char* name, const unsigned char *colormap, unsigned int n_colors);
    bool RemCanvas (int canvas_id);
	bool Clear (int canvas_id);
    bool AddObject (int canvas_id, PaintObject *object);
	bool Paint (int canvas_id);
	bool Move (int canvas_id, int x, int y, int width, int height);
	bool MoveEX (int canvas_id, int x, int y, int width, int height, bool paramsDescribeClientRect = false, MOVE_EX_TYPE moveAndOrResize = MOVE_EX_TYPE_MOVE_AND_RESIZE);

	//   __________________________________________
	//  |(0,0)                        numh=3       |
	//  |     (start_x,start_y)       numv=2       |
	//  |      \/_________width___________         |
    //  |      |        |        |        |        |
	//  |      |        |        |        |        |
	//  |      |________|________|________|height  |
	//  |      |        |        |        |        |
	//  |      |        |        |        |        |
	//  |      |________|________|________|        |
	//  |                                          |
	//  |__________________________________________|
    //
	void Arrange (int numh, int numv, int start_x, int start_y, int width, int height);
	void MoveConsole (int x, int y, int width, int height);

private:

    ThePainter (const ssi_char_t *file);
    ~ThePainter ();
	ssi_char_t *_file;
	Options _options;

	// pointer to array of canvas and _mutex
    CanvasThread *_canvas[THEPAINTER_MAX_BUFFER];
	Mutex _mutex[THEPAINTER_MAX_BUFFER];

	ULONG_PTR _gdiplus;
};

}

#endif // _THEPAINTER_H
