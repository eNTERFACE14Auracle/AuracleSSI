// PaintSignal.cpp
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

#include "graphic/PaintSignal.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

#include "float.h"


namespace ssi {

template PaintSignal<char>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<unsigned char>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<short>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<unsigned short>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<int>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<unsigned int>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<long>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<unsigned long>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<float>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template PaintSignal<double>::PaintSignal (ssi_time_t sample_rate, ssi_size_t sample_dimension, ssi_time_t window_size, PaintSignalType::TYPE type, ssi_size_t, ssi_size_t);
template <class T>
PaintSignal<T>::PaintSignal (ssi_time_t sample_rate_,
	ssi_size_t dimension_, 
	ssi_time_t window_size_,
	PaintSignalType::TYPE type_,
	ssi_size_t x_ind_,
	ssi_size_t y_ind_)
	: data_buffer_scaled (0), 
	data_buffer (0),
	scale (0),
	scale_tot (0),
	data_buffer_position (0),
	sample_rate (sample_rate_),
	sample_dimension (dimension_),
	window_size (window_size_),
	sample_number_total (0), 
	data_min (0), 
	data_min_tot (0),
	data_max (0), 
	data_max_tot (0),
	time_in_s (0), 
	fixed_min_max (false), 
	pen (0),
	axisPen (0),
	brush (0),
	reset_min_max (true),
	discrete_plot (window_size_ <= 0),
	type (type_),
	x_ind (x_ind_),
	y_ind (y_ind_),
	axis_precision (3) {

	if (sample_rate == 0) {
		ssi_err ("sample rate must not be zero");
	}

	init ();
}

template <class T>
void PaintSignal<T>::init () {

	// create buffer (empty in discrete case)
	sample_number_total_real = sample_number_total = 0;
	data_buffer_size_real = data_buffer_size = 0;
	data_buffer_scaled = 0;
	data_buffer = 0;
	scale = new float[sample_dimension];
	data_min = new T[sample_dimension];
	data_max = new T[sample_dimension];

	// create default pen and brush
	lineStyle = PS_SOLID;
	lineWidth = 1;
	pointSize = 5;
	lineColor = RGB (0, 255, 0);
	axisColor = RGB (255, 255, 255);
	fillColor = RGB (0, 255, 0);
	backColor = RGB (0, 0, 0);
	createPen ();
	createAxisPen ();
	createBrush ();
	createBackBrush ();
}

template <class T>
PaintSignal<T>::~PaintSignal () {

	::DeleteObject (pen);
	::DeleteObject (axisPen);
	::DeleteObject (brush);

	delete[] data_buffer;
	delete[] data_buffer_scaled;
	delete[] data_min;
	delete[] data_max;
	delete[] scale;
}

template void PaintSignal<char>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<unsigned char>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<short>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<unsigned short>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<int>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<unsigned int>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<long>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<unsigned long>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<float>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template void PaintSignal<double>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time);
template<class T>
void PaintSignal<T>::setData (const void *data, ssi_size_t sample_number, ssi_time_t time) {	

	bool scale_change = false;

	// in continuous case init buffer at first call
	if (!discrete_plot && data_buffer == 0) {

		sample_number_total_real = sample_number_total = ssi_cast (ssi_size_t, sample_rate * window_size + 0.5);
		data_buffer_size_real = data_buffer_size = sample_dimension * sample_number_total_real;
		data_buffer_scaled = new float[data_buffer_size_real];	
		data_buffer = new T[data_buffer_size_real];				

		T *dstptr = data_buffer;
		float *dstptr_scaled = data_buffer_scaled;
		const T *srcptr = ssi_pcast (const T, data);
		for (unsigned int i = 0; i < sample_number_total_real; i++) {
			for (unsigned int j = 0; j < sample_dimension; j++) {
				*dstptr++ = srcptr[j];
				*dstptr_scaled++ = 0;
			}
		}
	}

	// in continuous case check if new data fits in buffer
	if (!discrete_plot && sample_number > sample_number_total) {
		ssi_wrn ("crop sample number from %u to %u", sample_number, sample_number_total);
		sample_number = sample_number_total;		
	}

	// in discrete case check if it is neccessary to adjust the buffer size
	if (discrete_plot) {
		if (sample_number > sample_number_total_real) {

			sample_number_total_real = sample_number;
			data_buffer_size_real = sample_number_total_real * sample_dimension;
			delete[] data_buffer;
			data_buffer = new T[data_buffer_size_real];
			delete[] data_buffer_scaled;
			data_buffer_scaled = new float[data_buffer_size_real];	

			T *dstptr = data_buffer;
			float *dstptr_scaled = data_buffer_scaled;
			const T *srcptr = ssi_pcast (const T, data);
			for (unsigned int i = 0; i < sample_number; i++) {
				for (unsigned int j = 0; j < sample_dimension; j++) {
					*dstptr++ = srcptr[j];
					*dstptr_scaled++ = 0;
				}
			}
		}
		sample_number_total = sample_number;
		data_buffer_size = sample_number_total * sample_dimension;
		data_buffer_position = 0;
	}

	if (!fixed_min_max) {

		// initialize min, max arrays if necessary
		if (reset_min_max) {			
			const T *dataptr = ssi_pcast (const T, data);
			for (ssi_size_t i = 0; i < sample_dimension; i++) {
				data_min[i] = *dataptr;
				data_max[i] = *dataptr++;
			}
			reset_min_max = false;
		}

		// adjust min, max arrays
		const T *dataptr = ssi_pcast (const T, data);
		for (unsigned int i = 0; i < sample_number; i++) {
			for (unsigned int j = 0; j < sample_dimension; j++) {
				if (data_min[j] > *dataptr) {
					data_min[j] = *dataptr;
					scale_change = true;
				} else if (data_max[j] < *dataptr) {
					data_max[j] = *dataptr;
					scale_change = true;
				}
				dataptr++;
			}
		}

		if (type == PaintSignalType::AUDIO) {	
			for (unsigned int j = 0; j < sample_dimension; j++) {
				if (myabs<T> (data_min[j]) > myabs<T> (data_max[j])) {
					data_max[j] = myabs<T> (data_min[j]);
				}
				data_min[j] = 0 - myabs<T> (data_max[j]);			
			}
		}

		// find total min, max
		data_min_tot = data_min[0];
		data_max_tot = data_max[0];
		for (unsigned int i = 1; i < sample_dimension; i++) {
			if (data_min_tot > data_min[i]) {
				data_min_tot = data_min[i];
			} else if (data_max_tot < data_max[i]) {
				data_max_tot = data_max[i];
			}
		}		
		scale_tot = ssi_cast (float, data_max_tot - data_min_tot);		
	}

	// adjust scale array
	for (unsigned int i = 0; i < sample_dimension; i++) {
		scale[i] = ssi_cast (float, data_max[i] - data_min[i]);
	}

	{

		Lock lock (data_buffer_mutex);	

		// update time
		time_in_s = time + sample_number / sample_rate;

		// fill in new data
		const T *srcptr = ssi_pcast (const T, data);
		T *dstptr = data_buffer + data_buffer_position;
		float *dstptr_scaled = data_buffer_scaled + data_buffer_position;
		T value;
		for (unsigned int i = 0; i < sample_number; i++) {
			for (unsigned int j = 0; j < sample_dimension; j++) {
				value = *srcptr++;
				*dstptr++ = value;
				if (type == PaintSignalType::IMAGE) {
					*dstptr_scaled++ = ssi_cast (float, value - data_min_tot) / scale_tot;
				} else {
					*dstptr_scaled++ = ssi_cast (float, value - data_min[j]) / scale[j];
				}
				++data_buffer_position;
				if (data_buffer_position == data_buffer_size) {
					data_buffer_position = 0;
					dstptr_scaled = data_buffer_scaled;
					dstptr = data_buffer;
				}
			}
		}

		// if scale has changed do a rescale
		if (scale_change) {
			srcptr = data_buffer;
			dstptr_scaled = data_buffer_scaled;
			for (unsigned int i = 0; i < sample_number_total; i++) {
				for (unsigned int j = 0; j < sample_dimension; j++) {
					value = *srcptr++;					
					if (type == PaintSignalType::IMAGE) {
						*dstptr_scaled++ = ssi_cast (float, value - data_min_tot) / scale_tot;
					} else {
						*dstptr_scaled++ = ssi_cast (float, value - data_min[j]) / scale[j];
					}
				}
			}
		}
	}
}

template<class T>
void PaintSignal<T>::rescale () {

	if (!fixed_min_max) {
		reset_min_max = true;
	}
}

template <class T>
void PaintSignal<T>::paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object) {

	// nothing to plot
	if (!data_buffer)
		return;
	
	Lock lock (data_buffer_mutex);

	if (first_object) {
		::FillRect (hdc, &rect, backBrush);
	}
	switch (type) {
		case PaintSignalType::IMAGE:
			paintAsImage (hdc, rect, colormap);
			break;
		case PaintSignalType::SIGNAL:
			paintAsSignal (hdc, rect, colormap);
			break;
		case PaintSignalType::AUDIO:
			paintAsAudio (hdc, rect, colormap);
			break;
		case PaintSignalType::PATH:
			paintAsPath (hdc, rect, colormap);
			break;
	}
	if (last_object && type != PaintSignalType::PATH) {
		paintAxis (hdc, rect, colormap);
	}
}

template <class T>
void PaintSignal<T>::paintAxis (HDC hdc, RECT rect, Colormap *colormap) {

	ssi_size_t dim = type == PaintSignalType::IMAGE ? 1 : sample_dimension;

	ssi_char_t str[128];

	// get dim of the canvas
	int width = rect.right;
	int height_total = rect.bottom;
	int height = height_total / dim;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, axisPen);

	// draw
	for (ssi_size_t i = 0; i < dim; i++) {
	
		if (i > 0) {
			::MoveToEx (hdc, 0, height*i, 0);		
			::LineTo (hdc, width, height*i);
		}

		float min_val = ssi_cast (float, data_min[i]);
		float max_val = ssi_cast (float, data_max[i]);
		ssi_sprint (str, "%.*f", axis_precision, max_val);
		::TextOut (hdc, 0, i*height, str, (int) strlen (str));
		ssi_sprint (str, "%.*f", axis_precision, min_val);
		::TextOut (hdc, 0, (i+1)*height-15, str, (int) strlen (str));
		if (type == PaintSignalType::SIGNAL || type == PaintSignalType::AUDIO) {
			ssi_sprint (str, "%.*f", axis_precision, (float) data_buffer[(data_buffer_position < dim) ? (data_buffer_size - dim + i) : (data_buffer_position - dim + i)]);
			::TextOut (hdc, 0, i*height + (height>>1) - 15, str, (int) strlen (str));
		}
	}

	ssi_sprint (str, "%.3lf", time_in_s - sample_number_total / sample_rate);
	::TextOut (hdc, width-(((int) strlen (str)) * 7), 0, str, (int) strlen (str));
	ssi_sprint (str, "%.3lf", time_in_s);
	::TextOut (hdc, width-(((int) strlen (str)) * 7), height_total-15, str, (int) strlen (str));

	// switch to old pen
	::SelectObject(hdc,old);
}

static inline float mymax (float *data, ssi_size_t size, ssi_size_t dim, int old_ind, int new_ind) {	

	float *ptr = data + old_ind;
	float result = abs (*ptr);	
	while (old_ind != new_ind) {
		if (abs (*ptr) > result) {
			result = abs (*ptr);
		}
		ptr += dim;
		old_ind += dim;
		if (old_ind >= (int) size) {
			old_ind -= size;
			ptr = data + old_ind;
		}
	}

	return result;
}

template <class T>
static inline T myabs (T x) {
	return x < 0 ? 0-x : x;
}

static inline float mean (float *data, ssi_size_t size, ssi_size_t dim, int old_ind, int new_ind) {

	float result = 0;	

	ssi_size_t len = 0;
	float *ptr = data + old_ind;
	while (old_ind != new_ind) {
		result += *ptr;
		ptr += dim;
		old_ind += dim;
		++len;		
		if (old_ind >= (int) size) {
			old_ind -= size;
			ptr = data + old_ind;
		}
	}

	return result / len;
}


template <class T>
void PaintSignal<T>::paintAsPath (HDC hdc, RECT rect, Colormap *colormap) {

	// get dimension of the canvas
	int width = rect.right;
	int height = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// plot data
	float *data_scaledptr = data_buffer_scaled + data_buffer_position;
	float x = *(data_scaledptr + x_ind);
	float y = *(data_scaledptr + y_ind);
	data_scaledptr += sample_dimension;
	::MoveToEx (hdc, (int) (x * width), (int) ((1.0f - y) * height), 0);
	for (unsigned int i = sample_dimension; i < sample_number_total; i++) {
		if (data_buffer_position + i == data_buffer_size) {
			data_scaledptr = data_buffer_scaled;
		}
		x = *(data_scaledptr + x_ind);
		y = *(data_scaledptr + y_ind);
		data_scaledptr += sample_dimension;
		::LineTo (hdc, (int) (x * width), (int) ((1.0f - y) * height));
		::MoveToEx (hdc, (int) (x * width), (int) ((1.0f - y) * height), 0);
	}

	::SelectObject(hdc,old);
}

template <class T>
void PaintSignal<T>::paintAsSignal (HDC hdc, RECT rect, Colormap *colormap) {

	// get sample_dimension of the canvas
	int width = rect.right;
	int height_total = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// calculate y scale factor
	float scale_y = (float) (sample_number_total) / width;
	int height = height_total / sample_dimension;
	int *offset = new int[sample_dimension];
	for (unsigned int i = 0; i < sample_dimension; i++) {
		offset[i] = height * i;
	}
	

	// plot data
	if (scale_y >= 1) {

		int old_y;
		int next_y;
		int old_ind, new_ind;
		float value;
		//ssi_size_t data_buffer_head = ((int) (data_buffer_position / scale_y)) / sample_dimension;
		int data_buffer_head = (int) data_buffer_position;
		for (unsigned int i = 0; i < sample_dimension; i++) {			
			bool found_data_buffer_head = false;
			//new_ind = (0 + i + data_buffer_position) % data_buffer_size;
			new_ind = i;
			next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[new_ind]) * height);
			::MoveToEx (hdc, 0, next_y, 0);	
			for (int x = 1; x < width; x++) {
				old_ind = new_ind;
				//new_ind = (data_buffer_position + sample_dimension * (int) (scale_y * x) + i) % data_buffer_size;
				new_ind = sample_dimension * (int) (scale_y * x) + i;
			
				if (!found_data_buffer_head && new_ind >= data_buffer_head) {
					//value = mean (data_buffer_scaled, data_buffer_size, sample_dimension, old_ind, new_ind);
					value = data_buffer_scaled[new_ind];
					old_y = next_y;
					next_y = offset[i] + (int) ((1.0f - value) * height);
					::LineTo (hdc, x, old_y);
					::MoveToEx (hdc, x, offset[i], 0);
					::SelectObject (hdc, axisPen);
					::LineTo (hdc, x, offset[i] + height);
					::SelectObject (hdc, pen);
					::MoveToEx (hdc, x, next_y, 0);
					found_data_buffer_head = true;
				} else {
					value = mean (data_buffer_scaled, data_buffer_size, sample_dimension, old_ind, new_ind);
					old_y = next_y;
					next_y = offset[i] + (int) ((1.0f - value) * height);
					::LineTo (hdc, x, next_y);
					::MoveToEx (hdc, x, next_y, 0);
				}
			}
		}

	} else {

		int old_y;
		int next_x, next_y;
		scale_y = 1 / scale_y;
		ssi_size_t data_buffer_head = data_buffer_position / sample_dimension;
		for (unsigned int i = 0; i < sample_dimension; i++) {
			next_x = 0;
			//next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[(data_buffer_position + i) % data_buffer_size]) * height);
			next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[i]) * height);
			::MoveToEx (hdc, next_x, next_y, 0);
			for (unsigned int y = 1; y < sample_number_total; y++) {
				next_x = (int) (scale_y * y);
				//next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[(data_buffer_position + sample_dimension*y+i) % data_buffer_size]) * height);
				old_y = next_y;
				next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[sample_dimension*y+i]) * height);
				if (y == data_buffer_head) {
					::LineTo (hdc, next_x, old_y);
					::MoveToEx (hdc, next_x, offset[i], 0);
					::SelectObject (hdc, axisPen);
					::LineTo (hdc, next_x, offset[i] + height);
					::SelectObject (hdc, pen);
					::MoveToEx (hdc, next_x, next_y, 0);
				} else {
					::LineTo (hdc, next_x, next_y);
					::MoveToEx (hdc, next_x, next_y, 0);
				}				
			}
		}

	}
	delete[] offset;

	// switch to old pen
	::SelectObject(hdc,old);
}

template <class T>
void PaintSignal<T>::paintAsAudio (HDC hdc, RECT rect, Colormap *colormap) {

	// get sample_dimension of the canvas
	int width = rect.right;
	int height_total = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// calculate y scale factor
	float scale_y = (float) (sample_number_total) / width;
	int height = height_total / sample_dimension;
	int *offset = new int[sample_dimension];
	for (unsigned int i = 0; i < sample_dimension; i++) {
		offset[i] = height * i;
	}

	// plot data
	if (scale_y >= 1) {

		int next_y;
		int old_ind, new_ind;
		float value;
		int data_buffer_head = (int) data_buffer_position;
		for (unsigned int i = 0; i < sample_dimension; i++) {			
			bool found_data_buffer_head = false;		
			//new_ind = (0 + i + data_buffer_position) % data_buffer_size;		
			new_ind = i;		
			for (int x = 0; x < width; x++) {
				old_ind = new_ind;
				//new_ind = (data_buffer_position + sample_dimension * (int) (scale_y * x) + i) % data_buffer_size;
				new_ind = sample_dimension * (int) (scale_y * x) + i;

				if (!found_data_buffer_head && new_ind >= data_buffer_head) {
					::MoveToEx (hdc, x, offset[i], 0);
					::SelectObject (hdc, axisPen);
					::LineTo (hdc, x, offset[i] + height);
					::SelectObject (hdc, pen);
					found_data_buffer_head = true;
				} else {
					value = mymax (data_buffer_scaled, data_buffer_size, sample_dimension, old_ind, new_ind);	
					next_y = offset[i] + (int) ((1.0f - value) * height);		
					::MoveToEx (hdc, x, next_y, 0);
					next_y = offset[i] + (int) (value * height);
					::LineTo (hdc, x, next_y);				
				}
			}
		}

	} else {

		int next_x, next_y;
		scale_y = 1 / scale_y;
		ssi_size_t data_buffer_head = data_buffer_position / sample_dimension;
		for (unsigned int i = 0; i < sample_dimension; i++) {
			for (unsigned int y = 1; y < sample_number_total; y++) {
				next_x = (int) (scale_y * y);
				if (y == data_buffer_head) {					
					::MoveToEx (hdc, next_x, offset[i], 0);
					::SelectObject (hdc, axisPen);
					::LineTo (hdc, next_x, offset[i] + height);
					::SelectObject (hdc, pen);					
				} else {
					//next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[(data_buffer_position + sample_dimension*y+i) % data_buffer_size]) * height);
					next_y = offset[i] + (int) ((1.0f - data_buffer_scaled[sample_dimension*y+i]) * height);
					::MoveToEx (hdc, next_x, next_y, 0);
					//next_y = offset[i] + (int) ((data_buffer_scaled[(data_buffer_position + sample_dimension*y+i) % data_buffer_size]) * height);
					next_y = offset[i] + (int) ((data_buffer_scaled[sample_dimension*y+i]) * height);
					::LineTo (hdc, next_x, next_y);
				}
			}
		}

	}
	delete[] offset;

	// switch to old pen
	::SelectObject(hdc,old);
}

template <class T>
void PaintSignal<T>::paintAsImage (HDC hdc, RECT rect, Colormap *colormap) {

	// get sample_dimension of the canvas
	int width = rect.right;
	int height = rect.bottom;

	// set pen
	HGDIOBJ old = ::SelectObject (hdc, pen);

	// calculate y scale factor
	float scale_x = ssi_cast (float, sample_number_total) / width;
	float scale_y = ssi_cast (float, sample_dimension) / height;

	// plot data_buffer_scaled
	if (scale_x >= 1) {
		if (scale_y >= 1) {

			// more data_buffer_scaled points than pixels in x and y direction

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
				float *data_scaled_ptr = data_buffer_scaled + x_inds[x] * sample_dimension;
				for (int y = 0; y < height; y++) {
					::SetPixel (hdc, x, y, colormap->getColor (*(data_scaled_ptr + y_inds[y])));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		} 
		
		else {

			// more data_buffer_scaled points in x direction but less in y
			scale_y = 1 / scale_y;

			// determine row indices
			int *x_inds = new int[width];
			for (int x = 0; x < width; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			// determine col indices
			int *y_inds = new int[sample_dimension+1];
			for (unsigned int y = 0; y < sample_dimension; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			y_inds[sample_dimension] = height;
			// now print
			RECT r;
			for (int x = 0; x < width; x++) {
				float *data_scaled_ptr = data_buffer_scaled + x_inds[x] * sample_dimension;
				r.left = x;
				r.right = x+1;
				for (unsigned int y = 0; y < sample_dimension; y++) {
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

			// more data_buffer_scaled points than pixels in y direction but less in x
			scale_x = 1 / scale_x;

			// determine col indices
			int *x_inds = new int[sample_number_total+1];
			for (unsigned int x = 0; x < sample_number_total; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			x_inds[sample_number_total] = width;
			// determine col indices
			int *y_inds = new int[height];
			for (int y = 0; y < height; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			// now print
			RECT r;
			for (unsigned int x = 0; x < sample_number_total; x++) {
				float *data_scaled_ptr = data_buffer_scaled + x * sample_dimension;
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

			// less data_buffer_scaled points in x and y direction
			scale_x = 1 / scale_x;
			scale_y = 1 / scale_y;

			// determine col indices
			int *x_inds = new int[sample_number_total+1];
			for (unsigned int x = 0; x < sample_number_total; x++) {
				x_inds[x] = (int) (scale_x * x);
			}
			x_inds[sample_number_total] = width;
			// determine col indices
			int *y_inds = new int[sample_dimension+1];
			for (unsigned int y = 0; y < sample_dimension; y++) {
				y_inds[y] = (int) (scale_y * y);
			}
			y_inds[sample_dimension] = height;
			// now print
			RECT r;
			for (unsigned int x = 0; x < sample_number_total; x++) {
				float *data_scaled_ptr = data_buffer_scaled + x * sample_dimension;
				r.left = x_inds[x];
				r.right = x_inds[x+1];
				for (unsigned int y = 0; y < sample_dimension; y++) {
					r.top = y_inds[y];
					r.bottom = y_inds[y+1];
					::FillRect (hdc, &r, colormap->getBrush (*(data_scaled_ptr + y)));
				}
			}
			delete[] x_inds;
			delete[] y_inds;

		}
	}




	// switch to old pen
	::SelectObject(hdc,old);
}


template void PaintSignal<char>::setLineWidth (int width);
template void PaintSignal<unsigned char>::setLineWidth (int width);
template void PaintSignal<short>::setLineWidth (int width);
template void PaintSignal<unsigned short>::setLineWidth (int width);
template void PaintSignal<int>::setLineWidth (int width);
template void PaintSignal<unsigned int>::setLineWidth (int width);
template void PaintSignal<long>::setLineWidth (int width);
template void PaintSignal<unsigned long>::setLineWidth (int width);
template void PaintSignal<float>::setLineWidth (int width);
template void PaintSignal<double>::setLineWidth (int width);
template <class T>
void PaintSignal<T>::setLineWidth (int width) {

	lineWidth = width;
	createPen ();
}

template void PaintSignal<char>::setLineStyle (int width);
template void PaintSignal<unsigned char>::setLineStyle (int width);
template void PaintSignal<short>::setLineStyle (int width);
template void PaintSignal<unsigned short>::setLineStyle (int width);
template void PaintSignal<int>::setLineStyle (int width);
template void PaintSignal<unsigned int>::setLineStyle (int width);
template void PaintSignal<long>::setLineStyle (int width);
template void PaintSignal<unsigned long>::setLineStyle (int width);
template void PaintSignal<float>::setLineStyle (int width);
template void PaintSignal<double>::setLineStyle (int width);
template <class T>
void PaintSignal<T>::setLineStyle (int style) {

	lineStyle = style;
	createPen ();
}

template void PaintSignal<char>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned char>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<short>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned short>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<int>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned int>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<long>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned long>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<float>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<double>::setLineColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintSignal<T>::setLineColor (unsigned short r, unsigned short g, unsigned short b) {

	lineColor = RGB (r,g,b);
	createPen ();
}

template void PaintSignal<char>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned char>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<short>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned short>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<int>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned int>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<long>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned long>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<float>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<double>::setAxisColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintSignal<T>::setAxisColor (unsigned short r, unsigned short g, unsigned short b) {

	axisColor = RGB (r,g,b);
	createAxisPen ();
}

template void PaintSignal<char>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned char>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<short>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned short>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<int>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned int>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<long>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned long>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<float>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<double>::setFillColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintSignal<T>::setFillColor (unsigned short r, unsigned short g, unsigned short b) {

	fillColor = RGB (r,g,b);
	createBrush ();
}

template void PaintSignal<char>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned char>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<short>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned short>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<int>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned int>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<long>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<unsigned long>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<float>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template void PaintSignal<double>::setBackColor (unsigned short r, unsigned short g, unsigned short b);
template <class T>
void PaintSignal<T>::setBackColor (unsigned short r, unsigned short g, unsigned short b) {

	backColor = RGB (r,g,b);
	createBackBrush ();
}


template void PaintSignal<char>::setPointSize (int size);
template void PaintSignal<unsigned char>::setPointSize (int size);
template void PaintSignal<short>::setPointSize (int size);
template void PaintSignal<unsigned short>::setPointSize (int size);
template void PaintSignal<int>::setPointSize (int size);
template void PaintSignal<unsigned int>::setPointSize (int size);
template void PaintSignal<long>::setPointSize (int size);
template void PaintSignal<unsigned long>::setPointSize (int size);
template void PaintSignal<float>::setPointSize (int size);
template void PaintSignal<double>::setPointSize (int size);
template <class T>
void PaintSignal<T>::setPointSize (int size) {

	pointSize = size;
}

template <class T>
void PaintSignal<T>::createPen () {
	
	::DeleteObject (pen);
	pen = ::CreatePen (lineStyle, lineWidth, lineColor);
}

template <class T>
void PaintSignal<T>::createAxisPen () {
	
	::DeleteObject (axisPen);
	axisPen = ::CreatePen (lineStyle, lineWidth, axisColor);
}

template <class T>
void PaintSignal<T>::createBrush () {
	
	::DeleteObject (brush);
	brush = ::CreateSolidBrush (fillColor);
}

template <class T>
void PaintSignal<T>::createBackBrush () {
	
	::DeleteObject (backBrush);
	backBrush = ::CreateSolidBrush (backColor);
}

template void PaintSignal<char>::setFixedMinMax (const char* data_min_, const char* data_max_);
template void PaintSignal<unsigned char>::setFixedMinMax (const unsigned char* data_min_, const unsigned char* data_max_);
template void PaintSignal<short>::setFixedMinMax (const short* data_min_, const short* data_max_);
template void PaintSignal<unsigned short>::setFixedMinMax (const unsigned short* data_min_, const unsigned short* data_max_);
template void PaintSignal<int>::setFixedMinMax (const int* data_min_, const int* data_max_);
template void PaintSignal<unsigned int>::setFixedMinMax (const unsigned int* data_min_, const unsigned int* data_max_);
template void PaintSignal<long>::setFixedMinMax (const long* data_min_, const long* data_max_);
template void PaintSignal<unsigned long>::setFixedMinMax (const unsigned long* data_min_, const unsigned long* data_max_);
template void PaintSignal<float>::setFixedMinMax (const float* data_min_, const float* data_max_);
template void PaintSignal<double>::setFixedMinMax (const double* data_min_, const double* data_max_);
template<class T>
void PaintSignal<T>::setFixedMinMax (const T* data_min_, const T* data_max_) {
	
	delete[] data_min;
	delete[] data_max;
	data_min = new T[sample_dimension];
	data_max = new T[sample_dimension];

	for (unsigned int i = 0; i < sample_dimension; i++) {
		data_min[i] = data_min_[i];
		data_max[i] = data_max_[i];
	}

	fixed_min_max = true;
}

template void PaintSignal<char>::setFixedMinMax (char data_min_, char data_max_);
template void PaintSignal<unsigned char>::setFixedMinMax (unsigned char data_min_, unsigned char data_max_);
template void PaintSignal<short>::setFixedMinMax (short data_min_, short data_max_);
template void PaintSignal<unsigned short>::setFixedMinMax (unsigned short data_min_, unsigned short data_max_);
template void PaintSignal<int>::setFixedMinMax (int data_min_, int data_max_);
template void PaintSignal<unsigned int>::setFixedMinMax (unsigned int data_min_, unsigned int data_max_);
template void PaintSignal<long>::setFixedMinMax (long data_min_, long data_max_);
template void PaintSignal<unsigned long>::setFixedMinMax (unsigned long data_min_, unsigned long data_max_);
template void PaintSignal<float>::setFixedMinMax (float data_min_, float data_max_);
template void PaintSignal<double>::setFixedMinMax (double data_min_, double data_max_);
template<class T>
void PaintSignal<T>::setFixedMinMax (T data_min_, T data_max_) {

	delete[] data_min;
	delete[] data_max;
	data_min = new T[sample_dimension];
	data_max = new T[sample_dimension];	

	for (unsigned int i = 0; i < sample_dimension; i++) {
		data_min[i] = data_min_;
		data_max[i] = data_max_;
	}

	fixed_min_max = true;
}

template void PaintSignal<char>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<unsigned char>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<short>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<unsigned short>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<int>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<unsigned int>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<long>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<unsigned long>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<float>::setAxisPrecision (ssi_size_t precision);
template void PaintSignal<double>::setAxisPrecision (ssi_size_t precision);
template<class T>
void PaintSignal<T>::setAxisPrecision (ssi_size_t precision) {

	axis_precision = precision;
}

}


