// PaintData.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/06
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

#ifndef SSI_GRAPHIC_PAINTDATA_H
#define SSI_GRAPHIC_PAINTDATA_H

#include "graphic/GraphicLibCons.h"
#include "graphic/PaintObject.h"

namespace ssi {

template<class T>
class PaintData : public PaintObject {
	
public:

	PaintData (unsigned int dimension, PAINT_TYPE type);
	virtual ~PaintData ();

	void paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object);
	void rescale ();

	void setData (const void *data, ssi_size_t size, ssi_time_t time = 0);

	void setLineWidth (int width);
	void setLineStyle (int style);
	void setPointSize (int size);
	void setLineColor (unsigned short r, unsigned short g, unsigned short b);
	void setFillColor (unsigned short r, unsigned short g, unsigned short b);

	void setFixedMinMax (const T* data_min, const T* data_max);

	void setLabel(const char* label);

private:

	void paintAsSignal (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsImage (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsPath (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsScatter (HDC hdc, RECT rect, Colormap *colormap);

	PAINT_TYPE type;
	float *data_scaled;
	Mutex data_scaled_mutex;
	unsigned int dimension, size;
	T *data_min;
	T *data_max;
	bool fixed_min_max;

	void createPen ();
	void createBrush ();

	void init ();

	int lineWidth;
	int lineStyle;
	int pointSize;
	COLORREF lineColor;
	HPEN pen;
	COLORREF fillColor;
	HBRUSH brush;

	char label[255];
};

}

#endif
