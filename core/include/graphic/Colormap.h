// Colormap.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/15
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

#ifndef SSI_GRAPHIC_COLORMAP_H
#define SSI_GRAPHIC_COLORMAP_H

#include "graphic/GraphicLibCons.h"

namespace ssi {

class IColormap
{
	public:
		virtual COLORREF getColor (unsigned int index)	= 0;
		virtual HBRUSH getBrush (unsigned int index)	= 0;
		virtual COLORREF getColor (float index)			= 0;
		virtual HBRUSH getBrush (float index)			= 0;
};

class Colormap : public IColormap {

public:

	Colormap (const unsigned char *colormap, unsigned int color_num);
	Colormap (COLORMAP_TYPE type);
	~Colormap ();

	COLORREF getColor (unsigned int index);
	HBRUSH getBrush (unsigned int index);
	COLORREF getColor (float index);
	HBRUSH getBrush (float index);

private:

	void loadDefault (COLORMAP_TYPE type);
	void load (const unsigned char *colormap);

	unsigned int color_num;
	HBRUSH *brushes;
	COLORREF *colors;
};

}

#endif // _THECOLORMAP_H
