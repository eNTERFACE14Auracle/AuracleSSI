// PaintSignal.h
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

#ifndef SSI_GRAPHIC_PAINTSIGNAL_H
#define SSI_GRAPHIC_PAINTSIGNAL_H

#include "graphic/GraphicLibCons.h"
#include "graphic/PaintObject.h"

namespace ssi {

class PaintSignalType {
public:
	enum TYPE : unsigned char {
		SIGNAL = 0,
		IMAGE,
		AUDIO,
		PATH
	};
};

template<class T>
class PaintSignal : public PaintObject {
	
public:

	PaintSignal (ssi_time_t sample_rate,
		ssi_size_t sample_dimension, 
		ssi_time_t window_size,
		PaintSignalType::TYPE,
		ssi_size_t x_ind = 0,
		ssi_size_t y_ind = 1);
	virtual ~PaintSignal ();

	void paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object);
	void rescale ();

	void setData (const void *data, ssi_size_t sample_number, ssi_time_t time);

	void setLineWidth (int width);
	void setLineStyle (int style);
	void setPointSize (int size);
	void setLineColor (unsigned short r, unsigned short g, unsigned short b);
	void setAxisColor (unsigned short r, unsigned short g, unsigned short b);
	void setFillColor (unsigned short r, unsigned short g, unsigned short b);
	void setBackColor (unsigned short r, unsigned short g, unsigned short b);

	void setFixedMinMax (const T* data_min, const T* data_max);
	void setFixedMinMax (T data_min, T data_max);
	void setAxisPrecision (ssi_size_t precision);

private:

	void paintAsPath (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsSignal (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsImage (HDC hdc, RECT rect, Colormap *colormap);
	void paintAsAudio (HDC hdc, RECT rect, Colormap *colormap);
	void paintAxis (HDC hdc, RECT rect, Colormap *colormap);

	T *data_buffer;
	float *data_buffer_scaled;
	float *scale, scale_tot;
	Mutex data_buffer_mutex;
	ssi_size_t data_buffer_position, data_buffer_size, data_buffer_size_real;
	ssi_size_t sample_dimension, sample_number_total, sample_number_total_real;
	ssi_time_t sample_rate, window_size;
	T *data_min, data_min_tot;
	T *data_max, data_max_tot;
	ssi_time_t time_in_s;
	bool fixed_min_max;
	bool reset_min_max;
	bool discrete_plot;
	ssi_size_t x_ind;
	ssi_size_t y_ind;
	ssi_size_t axis_precision;

	void createPen ();
	void createAxisPen ();
	void createBrush ();
	void createBackBrush ();

	void init ();

	int lineWidth;
	int lineStyle;
	int pointSize;
	COLORREF lineColor;
	HPEN pen;
	COLORREF axisColor;
	HPEN axisPen;
	COLORREF fillColor;
	HBRUSH brush;
	COLORREF backColor;
	HBRUSH backBrush;

	PaintSignalType::TYPE type;
};

}

#endif
