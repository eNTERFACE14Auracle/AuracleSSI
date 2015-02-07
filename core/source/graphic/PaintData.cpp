// PaintData.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/08
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

#include "graphic/PaintData.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

template PaintData<char>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<unsigned char>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<short>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<unsigned short>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<int>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<unsigned int>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<long>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<unsigned long>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<float>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template PaintData<double>::PaintData (unsigned int dimension_, PAINT_TYPE type_);
template <class T>
PaintData<T>::PaintData (unsigned int dimension_, 
	PAINT_TYPE type_)
	: type (type_), 
	data_scaled (0), 
	dimension (dimension_),
	size (0), 
	data_min (0), 
	data_max (0), 
	fixed_min_max (false), 
	pen (0),
	brush (0) {

	if ((type == PAINT_TYPE_PATH || type == PAINT_TYPE_SCATTER) && dimension != 2) {
		ssi_err ("if plot type is path or scatter dimension needs to be 2");
	}
	
	label[0] = '\0';

	init ();
}

template <class T>
void PaintData<T>::init () {

	// create default pen and brush
	lineStyle = PS_SOLID;
	lineWidth = 1;
	pointSize = 5;
	lineColor = RGB (0, 255, 0);
	fillColor = RGB (0, 255, 0);
	createPen ();
	createBrush ();
}

template <class T>
PaintData<T>::~PaintData () {

	if (pen)
		::DeleteObject (pen);
	if (brush)
		::DeleteObject (brush);

	if (data_scaled)
		delete[] data_scaled;
	if (data_min)
		delete[] data_min;
	if (data_max)
		delete[] data_max;
}

template void PaintData<char>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<unsigned char>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<short>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<unsigned short>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<int>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<unsigned int>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<long>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<unsigned long>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<float>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template void PaintData<double>::setData (const void *data, ssi_size_t size, ssi_time_t time);
template<class T>
void PaintData<T>::setData (const void *data, ssi_size_t size_, ssi_time_t time) {

	{

	Lock lock (data_scaled_mutex);

	if (!data_scaled) {
		// create data array
		data_scaled = new float[size_ * dimension];
	} else if (size_ > size) {
		// create larger data array
		delete[] data_scaled;
		data_scaled = new float[size_ * dimension];
	}

	size = size_;
	
	if (!fixed_min_max) {

		// initialize min, max arrays if necessary
		if (!data_min) {
			data_min = new T[dimension];
			data_max = new T[dimension];
			const T *dataptr = ssi_pcast (const T, data);
			for (unsigned int i = 0; i < dimension; i++) {
				data_min[i] = *dataptr;
				data_max[i] = *dataptr++;
			}
		}

		// adjust min, max arrays
		const T *dataptr = ssi_pcast (const T, data);
		for (unsigned int i = 0; i < size; i++) {
			for (unsigned int j = 0; j < dimension; j++) {
				if (data_min[j] > *dataptr) {
					data_min[j] = *dataptr;
				} else if (data_max[j] < *dataptr) {
					data_max[j] = *dataptr;
				}
				dataptr++;
			}
		}
	}

	// scale data
	const T *srcptr = ssi_pcast (const T, data);
	float *dstptr = data_scaled;
	switch (type) {
		case PAINT_TYPE_IMAGE: {
			// find total min, max
			T data_min_tot = data_min[0];
			T data_max_tot = data_max[0];
			for (unsigned int i = 1; i < dimension; i++) {
				if (data_min_tot > data_min[i]) {
					data_min_tot = data_min[i];
				} else if (data_max_tot < data_max[i]) {
					data_max_tot = data_max[i];
				}
			}
			float scale = ssi_cast (float, data_max_tot - data_min_tot);
			for (unsigned int i = 0; i < size * dimension; i++) {
				*dstptr++ = (float) (*srcptr++ - data_min_tot) / scale;
			}
			break;
		}
		case PAINT_TYPE_SIGNAL:
		case PAINT_TYPE_PATH: 
		case PAINT_TYPE_SCATTER: {
			float *scale = new float[dimension];
			for (unsigned int i = 0; i < dimension; i++) {
				scale[i] = ssi_cast (float, data_max[i] - data_min[i]);
			}
			for (unsigned int i = 0; i < size; i++) {
				for (unsigned int j = 0; j < dimension; j++) {
					*dstptr++ = ssi_cast (float, *srcptr++ - data_min[j]) / scale[j];
				}
			}
			delete[] scale;
			break;
		}
	}

	}
}

template<class T>
void PaintData<T>::rescale () {

	if (!fixed_min_max) {
		T *tmp = data_max;
		data_max = data_min;
		data_min = tmp;
	}
}

template <class T>
void PaintData<T>::paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object) {
	
	Lock lock (data_scaled_mutex);

	switch (type) {
		case PAINT_TYPE_IMAGE:		
			paintAsImage (hdc, rect, colormap);
			break;
		case PAINT_TYPE_SIGNAL:
			if (first_object) {
				::FillRect (hdc, &rect, ssi_cast (HBRUSH, ::GetStockObject (BLACK_BRUSH)));
			}
			paintAsSignal (hdc, rect, colormap);
			break;
		case PAINT_TYPE_PATH:
			if (first_object) {			
				::FillRect (hdc, &rect, ssi_cast (HBRUSH, ::GetStockObject (BLACK_BRUSH)));
			}
			paintAsPath (hdc, rect, colormap);
			break;
		case PAINT_TYPE_SCATTER:
			if (first_object) {			
				::FillRect (hdc, &rect, ssi_cast (HBRUSH, ::GetStockObject (BLACK_BRUSH)));
			}
			paintAsScatter (hdc, rect, colormap);
			break;
	}
}

template <class T>
void PaintData<T>::paintAsImage (HDC hdc, RECT rect, Colormap *colormap) {

	// nothing to plot
	if (!data_scaled)
		return;

	// get dimension of the canvas
	int width = rect.right;
	int height = rect.bottom;

	// calculate scale factors
	float scale_x = ssi_cast (float, size) / width;
	float scale_y = ssi_cast (float, dimension) / height;

	// plot data_scaled
	if (scale_x >= 1) {
		if (scale_y >= 1) {

			// more data_scaled points than pixels in x and y direction

			// determine row indices
			int *x_inds = new int[width];
			for (int x = 0; x < width; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			// determine col indices
			int *y_inds = new int[height];
			for (int y = 0; y < height; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			// now print
			for (int x = 0; x < width; x++) {
				float *data_scaled_ptr = data_scaled + x_inds[x] * dimension;
				for (int y = 0; y < height; y++) {
					::SetPixel (hdc, x, y, colormap->getColor (*(data_scaled_ptr + y_inds[y])));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		} else {

			// more data_scaled points in x direction but less in y
			scale_y = 1 / scale_y;

			// determine row indices
			int *x_inds = new int[width];
			for (int x = 0; x < width; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			// determine col indices
			int *y_inds = new int[dimension+1];
			for (unsigned int y = 0; y < dimension; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			y_inds[dimension] = height;
			// now print
			RECT r;
			for (int x = 0; x < width; x++) {
				float *data_scaled_ptr = data_scaled + x_inds[x] * dimension;
				r.left = x;
				r.right = x+1;
				for (unsigned int y = 0; y < dimension; y++) {
					r.top = y_inds[y];
					r.bottom = y_inds[y+1];
					::FillRect (hdc, &r, colormap->getBrush (*(data_scaled_ptr + y)));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		}
	} else {
		
		if (scale_y >= 1) {

			// more data_scaled points than pixels in y direction but less in x
			scale_x = 1 / scale_x;

			// determine col indices
			int *x_inds = new int[size+1];
			for (unsigned int x = 0; x < size; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			x_inds[size] = width;
			// determine col indices
			int *y_inds = new int[height];
			for (int y = 0; y < height; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			// now print
			RECT r;
			for (unsigned int x = 0; x < size; x++) {
				float *data_scaled_ptr = data_scaled + x * dimension;
				r.left = x_inds[x];
				r.right = x_inds[x+1];
				for (int y = 0; y < height; y++) {
					r.top = y;
					r.bottom = y+1;
					::FillRect (hdc, &r, colormap->getBrush (*(data_scaled_ptr + y_inds[y])));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		} else {

			// less data_scaled points in x and y direction
			scale_x = 1 / scale_x;
			scale_y = 1 / scale_y;

			// determine col indices
			int *x_inds = new int[size+1];
			for (unsigned int x = 0; x < size; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			x_inds[size] = width;
			// determine col indices
			int *y_inds = new int[dimension+1];
			for (unsigned int y = 0; y < dimension; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			y_inds[dimension] = height;
			// now print
			RECT r;
			for (unsigned int x = 0; x < size; x++) {
				float *data_scaled_ptr = data_scaled + x * dimension;
				r.left = x_inds[x];
				r.right = x_inds[x+1];
				for (unsigned int y = 0; y < dimension; y++) {
					r.top = y_inds[y];
					r.bottom = y_inds[y+1];
					::FillRect (hdc, &r, colormap->getBrush (*(data_scaled_ptr + y)));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		}
	}
}

static inline float mean (float *data, int dim, int len) {

	float result = 0;	
	for (int i = 0; i < len; ++i) {
		result += *data;
		data += dim;
	}
	return result / len;
}

template <class T>
void PaintData<T>::paintAsSignal (HDC hdc, RECT rect, Colormap *colormap) {

	// nothing to plot
	if (!data_scaled)
		return;

	// get dimension of the canvas
	int width = rect.right;
	int height_total = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// calculate y scale factor
	float scale_y = (float) (size) / width;
	int height = height_total / dimension;
	int *offset = new int[dimension];
	for (unsigned int i = 0; i < dimension; i++) {
		offset[i] = height * i;
	}
	
	// plot data_scaled
	if (scale_y >= 1) {

		int next_y;
		int old_ind, new_ind;
		float value;
		for (unsigned int i = 0; i < dimension; i++) {			
			new_ind = 0 + i;
			::MoveToEx (hdc, 0, offset[i] + (int) ((1.0f - data_scaled[new_ind]) * height), 0);			
			for (int x = 1; x < width; x++) {
				old_ind = new_ind;
				new_ind = dimension * (int) (scale_y * x) + i;
				//value = data_scaled[new_ind];
				value = mean (data_scaled+old_ind, dimension, (new_ind - old_ind) / dimension);
				next_y = offset[i] + (int) ((1.0f - value) * height);			
				::LineTo (hdc, x, next_y);
				::MoveToEx (hdc, x, next_y, 0);
			}
		}

	} else {

		int next_x, next_y;
		scale_y = 1 / scale_y;
		for (unsigned int i = 0; i < dimension; i++) {
			::MoveToEx (hdc, 0, offset[i] + (int) ((1.0f - data_scaled[i]) * height), 0);
			for (unsigned int y = 1; y < size; y++) {
				next_x = (int) (scale_y * y);
				next_y = offset[i] + (int) ((1.0f - data_scaled[dimension*y+i]) * height);
				::LineTo (hdc, next_x, next_y);
				::MoveToEx (hdc, next_x, next_y, 0);
			}
		}

	}
	delete[] offset;

	::SelectObject(hdc,old);
}


template <class T>
void PaintData<T>::paintAsPath (HDC hdc, RECT rect, Colormap *colormap) {

	// nothing to plot
	if (!data_scaled)
		return;

	// get dimension of the canvas
	int width = rect.right;
	int height = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// plot data
	float *data_scaledptr = data_scaled;
	float x = *data_scaledptr++;
	float y = *data_scaledptr++;
	::MoveToEx (hdc, (int) (x * width), (int) ((1.0f - y) * height), 0);
	for (unsigned int i = 2; i < size; i++) {
		x = *data_scaledptr++;
		y = *data_scaledptr++;
		::LineTo (hdc, (int) (x * width), (int) ((1.0f - y) * height));
		::MoveToEx (hdc, (int) (x * width), (int) ((1.0f - y) * height), 0);
	}

	::SelectObject(hdc,old);
}

template <class T>
void PaintData<T>::paintAsScatter (HDC hdc, RECT rect, Colormap *colormap) {

	// nothing to plot
	if (!data_scaled)
		return;

	// get dimension of the canvas
	int width = rect.right;
	int height = rect.bottom;

	// set pen
	HGDIOBJ old_pen = ::SelectObject (hdc, pen);
	HGDIOBJ old_brush = ::SelectObject (hdc, brush);

	HFONT font = ::CreateFont(40,0,0,0,FW_NORMAL,0,0,0,0,0,0,2,0,"SYSTEM_FIXED_FONT");
	HFONT old_font =(HFONT)::SelectObject(hdc, font);

	// plot data
	float *data_scaledptr = data_scaled;
	int pointSize2 = pointSize >> 1;
	float x, y;
	int top, left;
	for (unsigned int i = 0; i < size; i++) {
		x = *data_scaledptr++;
		y = *data_scaledptr++;
		left = (int) (x * width) - pointSize2;
		top = (int) ((1.0f - y) * height) - pointSize2;
		::Ellipse (hdc, left, top, left+pointSize, top+pointSize);
	}

	//paint canvas label
	if(label[0] != '\0')
	{
		ssi_char_t str[255];
		ssi_sprint (str, "%s", label);
		//::TextOut (hdc, width-(((int) strlen (str)) * 7), 0, str, (int) strlen (str));
		::TextOut (hdc, width-(((int) strlen (str)) * 16), 0, str, (int) strlen (str));
	}

	::SelectObject (hdc, old_pen);
	::SelectObject (hdc, old_brush);
	::SelectObject (hdc, old_font);
}


template void PaintData<char>::setLineWidth (int width);
template void PaintData<unsigned char>::setLineWidth (int width);
template void PaintData<short>::setLineWidth (int width);
template void PaintData<unsigned short>::setLineWidth (int width);
template void PaintData<int>::setLineWidth (int width);
template void PaintData<unsigned int>::setLineWidth (int width);
template void PaintData<long>::setLineWidth (int width);
template void PaintData<unsigned long>::setLineWidth (int width);
template void PaintData<float>::setLineWidth (int width);
template void PaintData<double>::setLineWidth (int width);
template <class T>
void PaintData<T>::setLineWidth (int width) {

	lineWidth = width;
	createPen ();
}

template void PaintData<char>::setLineStyle (int width);
template void PaintData<unsigned char>::setLineStyle (int width);
template void PaintData<short>::setLineStyle (int width);
template void PaintData<unsigned short>::setLineStyle (int width);
template void PaintData<int>::setLineStyle (int width);
template void PaintData<unsigned int>::setLineStyle (int width);
template void PaintData<long>::setLineStyle (int width);
template void PaintData<unsigned long>::setLineStyle (int width);
template void PaintData<float>::setLineStyle (int width);
template void PaintData<double>::setLineStyle (int width);
template <class T>
void PaintData<T>::setLineStyle (int style) {

	lineStyle = style;
	createPen ();
}

template void PaintData<char>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned char>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<short>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned short>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<int>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned int>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<long>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned long>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<float>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<double>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintData<T>::setLineColor (unsigned short r, unsigned short g, unsigned short b) {

	lineColor = RGB (r,g,b);
	createPen ();
}

template void PaintData<char>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned char>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<short>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned short>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<int>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned int>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<long>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<unsigned long>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<float>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintData<double>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintData<T>::setFillColor (unsigned short r, unsigned short g, unsigned short b) {

	fillColor = RGB (r,g,b);
	createBrush ();
}

template void PaintData<char>::setPointSize (int size);
template void PaintData<unsigned char>::setPointSize (int size);
template void PaintData<short>::setPointSize (int size);
template void PaintData<unsigned short>::setPointSize (int size);
template void PaintData<int>::setPointSize (int size);
template void PaintData<unsigned int>::setPointSize (int size);
template void PaintData<long>::setPointSize (int size);
template void PaintData<unsigned long>::setPointSize (int size);
template void PaintData<float>::setPointSize (int size);
template void PaintData<double>::setPointSize (int size);
template <class T>
void PaintData<T>::setPointSize (int size) {

	pointSize = size;
}

template <class T>
void PaintData<T>::createPen () {
	if (pen)
		::DeleteObject (pen);
	pen = ::CreatePen (lineStyle, lineWidth, lineColor);
}

template <class T>
void PaintData<T>::createBrush () {
	if (brush)
		::DeleteObject (pen);
	brush = ::CreateSolidBrush (fillColor);
}

template void PaintData<char>::setFixedMinMax (const char* data_min_, const char* data_max_);
template void PaintData<unsigned char>::setFixedMinMax (const unsigned char* data_min_, const unsigned char* data_max_);
template void PaintData<short>::setFixedMinMax (const short* data_min_, const short* data_max_);
template void PaintData<unsigned short>::setFixedMinMax (const unsigned short* data_min_, const unsigned short* data_max_);
template void PaintData<int>::setFixedMinMax (const int* data_min_, const int* data_max_);
template void PaintData<unsigned int>::setFixedMinMax (const unsigned int* data_min_, const unsigned int* data_max_);
template void PaintData<long>::setFixedMinMax (const long* data_min_, const long* data_max_);
template void PaintData<unsigned long>::setFixedMinMax (const unsigned long* data_min_, const unsigned long* data_max_);
template void PaintData<float>::setFixedMinMax (const float* data_min_, const float* data_max_);
template void PaintData<double>::setFixedMinMax (const double* data_min_, const double* data_max_);
template<class T>
void PaintData<T>::setFixedMinMax (const T* data_min_, const T* data_max_) {

	if (!data_min) {
		data_min = new T[dimension];
		data_max = new T[dimension];
	}

	for (unsigned int i = 0; i < dimension; i++) {
		data_min[i] = data_min_[i];
		data_max[i] = data_max_[i];
	}

	fixed_min_max = true;
}

//Canvas label (shown in window)
template void PaintData<char>::setLabel (const char* label_);
template void PaintData<unsigned char>::setLabel (const char* label_);
template void PaintData<short>::setLabel (const char* label_);
template void PaintData<unsigned short>::setLabel (const char* label_);
template void PaintData<int>::setLabel (const char* label_);
template void PaintData<unsigned int>::setLabel (const char* label_);
template void PaintData<long>::setLabel (const char* label_);
template void PaintData<unsigned long>::setLabel (const char* label_);
template void PaintData<float>::setLabel (const char* label_);
template void PaintData<double>::setLabel (const char* label_);
template <class T>
void PaintData<T>::setLabel (const char* label_) {

	strcpy_s((char *)label, 255, label_);
}

}

