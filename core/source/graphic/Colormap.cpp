// Colormap.cpp
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

#include "graphic/Colormap.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Colormap::Colormap (const unsigned char *colormap, unsigned int color_num_)
: color_num (color_num_) {

	load (colormap);
	brushes = new HBRUSH[color_num];
	for (unsigned int i = 0; i < color_num; i++) {
		brushes[i] = ::CreateSolidBrush (colors[i]);
	}
}

Colormap::Colormap (COLORMAP_TYPE type) {

	loadDefault (type);
	brushes = new HBRUSH[color_num];
	for (unsigned int i = 0; i < color_num; i++) {
		brushes[i] = ::CreateSolidBrush (colors[i]);
	}
}

void Colormap::load (const unsigned char *colormap) {

	colors = new COLORREF[color_num];
	COLORREF *colors_ptr = colors;
	unsigned char *dataptr = const_cast<unsigned char *> (colormap);
	for (unsigned int i = 0; i < color_num; i++) {
		*colors_ptr++ = RGB(*dataptr,*(dataptr+1),*(dataptr+2));
		dataptr += 3;
	}
}

void Colormap::loadDefault (COLORMAP_TYPE type) {

	switch (type) {

		case COLORMAP_TYPE_BLACKANDWHITE: {

			color_num = 2;
			colors = new COLORREF[color_num];
			COLORREF *colors_ptr = colors;
			*colors_ptr++ = RGB(0,0,0);
			*colors_ptr++ = RGB(255,255,255);
			break;
		}

		case COLORMAP_TYPE_COLOR16: {

			color_num = 16;
			colors = new COLORREF[color_num];
			COLORREF *colors_ptr = colors;
			*colors_ptr++ = RGB(0,0,0);
			*colors_ptr++ = RGB(128,0,0);
			*colors_ptr++ = RGB(0,128,0);
			*colors_ptr++ = RGB(0,0,128);
			*colors_ptr++ = RGB(128,0,128);
			*colors_ptr++ = RGB(0,128,128);
			*colors_ptr++ = RGB(192,192,192);
			*colors_ptr++ = RGB(128,128,128);
			*colors_ptr++ = RGB(255,0,0);
			*colors_ptr++ = RGB(0,255,0);
			*colors_ptr++ = RGB(255,255,0);
			*colors_ptr++ = RGB(0,0,255);
			*colors_ptr++ = RGB(255,0,255);
			*colors_ptr++ = RGB(0,255,255);
			*colors_ptr++ = RGB(255,255,255);
			break;
		}
		case COLORMAP_TYPE_COLOR64: {
		
			color_num = 64;
			colors = new COLORREF[color_num];
			COLORREF *colors_ptr = colors;
			*colors_ptr++ = RGB(0,0,143);
			*colors_ptr++ = RGB(0,0,159);
			*colors_ptr++ = RGB(0,0,175);
			*colors_ptr++ = RGB(0,0,191);
			*colors_ptr++ = RGB(0,0,207);
			*colors_ptr++ = RGB(0,0,223);
			*colors_ptr++ = RGB(0,0,239);
			*colors_ptr++ = RGB(0,0,255);
			*colors_ptr++ = RGB(0,15,255);
			*colors_ptr++ = RGB(0,31,255);
			*colors_ptr++ = RGB(0,47,255);
			*colors_ptr++ = RGB(0,63,255);
			*colors_ptr++ = RGB(0,79,255);
			*colors_ptr++ = RGB(0,95,255);
			*colors_ptr++ = RGB(0,111,255);
			*colors_ptr++ = RGB(0,127,255);
			*colors_ptr++ = RGB(0,143,255);
			*colors_ptr++ = RGB(0,159,255);
			*colors_ptr++ = RGB(0,175,255);
			*colors_ptr++ = RGB(0,191,255);
			*colors_ptr++ = RGB(0,207,255);
			*colors_ptr++ = RGB(0,223,255);
			*colors_ptr++ = RGB(0,239,255);
			*colors_ptr++ = RGB(0,255,255);
			*colors_ptr++ = RGB(15,255,239);
			*colors_ptr++ = RGB(31,255,223);
			*colors_ptr++ = RGB(47,255,207);
			*colors_ptr++ = RGB(63,255,191);
			*colors_ptr++ = RGB(79,255,175);
			*colors_ptr++ = RGB(95,255,159);
			*colors_ptr++ = RGB(111,255,143);
			*colors_ptr++ = RGB(127,255,127);
			*colors_ptr++ = RGB(143,255,111);
			*colors_ptr++ = RGB(159,255,95);
			*colors_ptr++ = RGB(175,255,79);
			*colors_ptr++ = RGB(191,255,63);
			*colors_ptr++ = RGB(207,255,47);
			*colors_ptr++ = RGB(223,255,31);
			*colors_ptr++ = RGB(239,255,15);
			*colors_ptr++ = RGB(255,255,0);
			*colors_ptr++ = RGB(255,239,0);
			*colors_ptr++ = RGB(255,223,0);
			*colors_ptr++ = RGB(255,207,0);
			*colors_ptr++ = RGB(255,191,0);
			*colors_ptr++ = RGB(255,175,0);
			*colors_ptr++ = RGB(255,159,0);
			*colors_ptr++ = RGB(255,143,0);
			*colors_ptr++ = RGB(255,127,0);
			*colors_ptr++ = RGB(255,111,0);
			*colors_ptr++ = RGB(255,95,0);
			*colors_ptr++ = RGB(255,79,0);
			*colors_ptr++ = RGB(255,63,0);
			*colors_ptr++ = RGB(255,47,0);
			*colors_ptr++ = RGB(255,31,0);
			*colors_ptr++ = RGB(255,15,0);
			*colors_ptr++ = RGB(255,0,0);
			*colors_ptr++ = RGB(239,0,0);
			*colors_ptr++ = RGB(223,0,0);
			*colors_ptr++ = RGB(207,0,0);
			*colors_ptr++ = RGB(191,0,0);
			*colors_ptr++ = RGB(175,0,0);
			*colors_ptr++ = RGB(159,0,0);
			*colors_ptr++ = RGB(143,0,0);
			*colors_ptr++ = RGB(127,0,0);
			break;
		}
		case COLORMAP_TYPE_GRAY64: {

			color_num = 64;
			colors = new COLORREF[color_num];
			COLORREF *colors_ptr = colors;
			*colors_ptr++ = RGB(	0	,	0	,	0	);
			*colors_ptr++ = RGB(	4	,	4	,	4	);
			*colors_ptr++ = RGB(	8	,	8	,	8	);
			*colors_ptr++ = RGB(	12	,	12	,	12	);
			*colors_ptr++ = RGB(	16	,	16	,	16	);
			*colors_ptr++ = RGB(	20	,	20	,	20	);
			*colors_ptr++ = RGB(	24	,	24	,	24	);
			*colors_ptr++ = RGB(	28	,	28	,	28	);
			*colors_ptr++ = RGB(	32	,	32	,	32	);
			*colors_ptr++ = RGB(	36	,	36	,	36	);
			*colors_ptr++ = RGB(	40	,	40	,	40	);
			*colors_ptr++ = RGB(	45	,	45	,	45	);
			*colors_ptr++ = RGB(	49	,	49	,	49	);
			*colors_ptr++ = RGB(	53	,	53	,	53	);
			*colors_ptr++ = RGB(	57	,	57	,	57	);
			*colors_ptr++ = RGB(	61	,	61	,	61	);
			*colors_ptr++ = RGB(	65	,	65	,	65	);
			*colors_ptr++ = RGB(	69	,	69	,	69	);
			*colors_ptr++ = RGB(	73	,	73	,	73	);
			*colors_ptr++ = RGB(	77	,	77	,	77	);
			*colors_ptr++ = RGB(	81	,	81	,	81	);
			*colors_ptr++ = RGB(	85	,	85	,	85	);
			*colors_ptr++ = RGB(	89	,	89	,	89	);
			*colors_ptr++ = RGB(	93	,	93	,	93	);
			*colors_ptr++ = RGB(	97	,	97	,	97	);
			*colors_ptr++ = RGB(	101	,	101	,	101	);
			*colors_ptr++ = RGB(	105	,	105	,	105	);
			*colors_ptr++ = RGB(	109	,	109	,	109	);
			*colors_ptr++ = RGB(	113	,	113	,	113	);
			*colors_ptr++ = RGB(	117	,	117	,	117	);
			*colors_ptr++ = RGB(	121	,	121	,	121	);
			*colors_ptr++ = RGB(	125	,	125	,	125	);
			*colors_ptr++ = RGB(	130	,	130	,	130	);
			*colors_ptr++ = RGB(	134	,	134	,	134	);
			*colors_ptr++ = RGB(	138	,	138	,	138	);
			*colors_ptr++ = RGB(	142	,	142	,	142	);
			*colors_ptr++ = RGB(	146	,	146	,	146	);
			*colors_ptr++ = RGB(	150	,	150	,	150	);
			*colors_ptr++ = RGB(	154	,	154	,	154	);
			*colors_ptr++ = RGB(	158	,	158	,	158	);
			*colors_ptr++ = RGB(	162	,	162	,	162	);
			*colors_ptr++ = RGB(	166	,	166	,	166	);
			*colors_ptr++ = RGB(	170	,	170	,	170	);
			*colors_ptr++ = RGB(	174	,	174	,	174	);
			*colors_ptr++ = RGB(	178	,	178	,	178	);
			*colors_ptr++ = RGB(	182	,	182	,	182	);
			*colors_ptr++ = RGB(	186	,	186	,	186	);
			*colors_ptr++ = RGB(	190	,	190	,	190	);
			*colors_ptr++ = RGB(	194	,	194	,	194	);
			*colors_ptr++ = RGB(	198	,	198	,	198	);
			*colors_ptr++ = RGB(	202	,	202	,	202	);
			*colors_ptr++ = RGB(	206	,	206	,	206	);
			*colors_ptr++ = RGB(	210	,	210	,	210	);
			*colors_ptr++ = RGB(	215	,	215	,	215	);
			*colors_ptr++ = RGB(	219	,	219	,	219	);
			*colors_ptr++ = RGB(	223	,	223	,	223	);
			*colors_ptr++ = RGB(	227	,	227	,	227	);
			*colors_ptr++ = RGB(	231	,	231	,	231	);
			*colors_ptr++ = RGB(	235	,	235	,	235	);
			*colors_ptr++ = RGB(	239	,	239	,	239	);
			*colors_ptr++ = RGB(	243	,	243	,	243	);
			*colors_ptr++ = RGB(	247	,	247	,	247	);
			*colors_ptr++ = RGB(	251	,	251	,	251	);
			*colors_ptr++ = RGB(	255	,	255	,	255	);
			break;
		}

		default:
			ssi_err ("unkown colormap type");
	}
}

Colormap::~Colormap () {

	for (unsigned int i = 0; i < color_num; i++) {
		::DeleteObject (brushes[i]);
	}
	delete[] brushes;
	delete[] colors;
}

COLORREF Colormap::getColor (unsigned int index) {

	if (index < 0) {
		index = 0;
	} else if (index >= color_num) {
		index = color_num - 1;
	}
	return colors[index];
}

HBRUSH Colormap::getBrush (unsigned int index) {

	if (index < 0) {
		index = 0;
	} else if (index >= color_num) {
		index = color_num - 1;
	}
	return brushes[index];
}

COLORREF Colormap::getColor (float value) {

	return getColor (ssi_cast (unsigned int, 0.5f + value * (color_num-1)));
}

HBRUSH Colormap::getBrush (float value) {

	return getBrush (ssi_cast (unsigned int, 0.5f + value * (color_num-1)));
}

}
