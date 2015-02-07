// SSI_Tools.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/30
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

#ifndef SSI_TOOLS_H
#define SSI_TOOLS_H

#include "r250.h"

// type convert function
SSI_INLINE static bool ssi_name2type (const ssi_char_t *name, ssi_type_t &type) {

	bool found = false;
	type = SSI_UNDEF;

	for (ssi_size_t i = 0; i < SSI_TYPE_NAME_SIZE; i++) {
		type = ssi_cast (ssi_type_t, i);
		if (strcmp (name, SSI_TYPE_NAMES[i]) == 0) {
			found = true;
			break;
		}
	}

	return found;
}

SSI_INLINE static bool ssi_name2type (const ssi_char_t *name, ssi_etype_t &type) {

	bool found = false;
	type = SSI_ETYPE_UNDEF;

	for (ssi_size_t i = 0; i < SSI_ETYPE_NAME_SIZE; i++) {
		type = ssi_cast (ssi_etype_t, i);
		if (strcmp (name, SSI_ETYPE_NAMES[i]) == 0) {
			found = true;
			break;
		}
	}

	return found;
}

SSI_INLINE static bool ssi_name2type (const ssi_char_t *name, ssi_object_t &type) {

	bool found = false;
	type = SSI_OBJECT;

	for (ssi_size_t i = 0; i < SSI_OBJECT_NAME_SIZE; i++) {
		type = ssi_cast (ssi_object_t, i);
		if (strcmp (name, SSI_OBJECT_NAMES[i]) == 0) {
			found = true;
			break;
		}
	}

	return found;
}

SSI_INLINE static ssi_size_t ssi_type2bytes (ssi_type_t &type) {
	switch (type) {
		case SSI_CHAR:
			return sizeof (char);
		case SSI_UCHAR:
			return sizeof (unsigned char);
		case SSI_SHORT:
			return sizeof (short);
		case SSI_USHORT:
			return sizeof (unsigned char);
		case SSI_INT:
			return sizeof (int);
		case SSI_UINT:
			return sizeof (unsigned int);
		case SSI_LONG:
			return sizeof (long);
		case SSI_ULONG:
			return sizeof (unsigned long);
		case SSI_FLOAT:
			return sizeof (float);
		case SSI_DOUBLE:
			return sizeof (double);	
		case SSI_BOOL:
			return sizeof (bool);
		default:
			ssi_err ("unsupported sample type");
			return 0;
	}
}

template <class T>
static ssi_type_t ssi_gettype (char dummy) {
	return SSI_CHAR;
}
template <class T>
static ssi_type_t ssi_gettype (unsigned char dummy) {
	return SSI_UCHAR;
}
template <class T>
static ssi_type_t ssi_gettype (short dummy) {
	return SSI_SHORT;
}
template <class T>
static ssi_type_t ssi_gettype (unsigned short dummy) {
	return SSI_USHORT;
}
template <class T>
static ssi_type_t ssi_gettype (int dummy) {
	return SSI_INT;
}
template <class T>
static ssi_type_t ssi_gettype (unsigned int dummy) {
	return SSI_INT;
}
template <class T>
static ssi_type_t ssi_gettype (long dummy) {
	return SSI_LONG;
}
template <class T>
static ssi_type_t ssi_gettype (unsigned long dummy) {
	return SSI_ULONG;
}
template <class T>
static ssi_type_t ssi_gettype (float dummy) {
	return SSI_FLOAT;
}
template <class T>
static ssi_type_t ssi_gettype (double dummy) {
	return SSI_DOUBLE;
}
template <class T>
static ssi_type_t ssi_gettype (long double dummy) {
	return SSI_LDOUBLE;
}

// string functions
SSI_INLINE static void ssi_strcpy (ssi_char_t *dst, const ssi_char_t *src) {
	while (*dst++ = *src++)
		;
}
SSI_INLINE static ssi_char_t *ssi_strcpy (const ssi_char_t *string) {
	if (!string) {
		return 0;
	}
	ssi_char_t *result = new ssi_char_t[strlen (string) + 1];
	strcpy (result, string);
	return result;
}
SSI_INLINE static char **ssi_strcpy (ssi_size_t number, const ssi_char_t *const *string) {
	ssi_char_t **result = new ssi_char_t *[number];
	for (ssi_size_t i = 0; i < number; i++) {
		result[i] = ssi_strcpy (string[i]);
	}
	return result;
}
SSI_INLINE static ssi_char_t *ssi_strcat (const ssi_char_t *string1, const ssi_char_t *string2) {
	ssi_char_t *result = new ssi_char_t[strlen (string1) + strlen (string2) + 1];
	sprintf (result, "%s%s", string1, string2);
	return result;
}
SSI_INLINE static ssi_char_t *ssi_strcat (const ssi_char_t *string1, const ssi_char_t *string2, const ssi_char_t *string3) {
	ssi_char_t *result = new ssi_char_t[strlen (string1) + strlen (string2) + strlen (string3) + 1];
	sprintf (result, "%s%s%s", string1, string2, string3);
	return result;
}
SSI_INLINE static ssi_char_t *ssi_strrepl (const ssi_char_t *str, const ssi_char_t *search, const ssi_char_t *replace)
{
	char *ret, *r;
	const char *p, *q;
	size_t oldlen = strlen(search);
	size_t count, retlen, newlen = strlen(replace);

	if (oldlen != newlen) {
		for (count = 0, p = str; (q = strstr(p, search)) != NULL; p = q + oldlen)
			count++;
		/* this is undefined if p - str > PTRDIFF_MAX */
		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else
		retlen = strlen(str);

	ret = new ssi_char_t[retlen+1];	

	for (r = ret, p = str; (q = strstr(p, search)) != NULL; p = q + oldlen) {
		/* this is undefined if q - p > PTRDIFF_MAX */
		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, replace, newlen);
		r += newlen;
	}
	strcpy(r, p);

	return ret;
}
SSI_INLINE ssi_size_t ssi_strlen (const ssi_char_t *str) {
	return ssi_cast (ssi_size_t, strlen (str));
}
SSI_INLINE void ssi_strtrim (ssi_char_t *str) {
    ssi_char_t * p = str;
    ssi_size_t l = ssi_strlen(p);

    while(isspace(p[l - 1])) {
		p[--l] = 0;
		if (l <= 0) {
			return; // empty line
		}
	}
    while(* p && isspace(* p)) ++p, --l;

    memmove(str, p, l + 1);
}
SSI_INLINE char *ssi_wchar2char (const wchar_t *src) {
	size_t len = wcslen (src);
	char *result = new char[len+1];
	wcstombs (result, src, len);
	result[len] = '\0';	
	return result;
}
SSI_INLINE wchar_t *ssi_char2wchar (const char *src) {
	size_t len = strlen (src)+1;
	wchar_t *result = new wchar_t[len];
    mbstowcs (result, src, len);
	return result;
}


// create directory
SSI_INLINE static void ssi_mkdir (const ssi_char_t *dir) {
	//ssi_char_t *cmd = ssi_strcat ("mkdir ", dir);
	//system (cmd);
	//delete[] cmd;
	CreateDirectoryA (dir, NULL);
}

// check if file exist
SSI_INLINE static bool ssi_exists (const char *filename) {

    FILE* fp = NULL;
    fp = fopen (filename, "rb");
    if( fp != NULL ) {
        fclose (fp);
        return true;
    }
    return false;
}

// min and max function
#ifndef min
#define	min(a,b) ((a) > (b) ? (b) : (a))
#endif
#ifndef max
#define	max(a,b) ((a) > (b) ? (a) : (b))
#endif


// video & functions

SSI_INLINE void ssi_video_params (ssi_video_params_t &s,
	int widthInPixels = 0,
	int heightInPixels = 0,
	double framesPerSecond = 0,
	int depthInBitsPerChannel = 0,
	int numOfChannels = 0,
	GUID outputSubTypeOfCaptureDevice = SSI_GUID_NULL,
	bool useClosestFramerateOfGraph = false,
	bool flipImage = false,
	bool automaticGenerationOfGraph = true,
	int majorVideoType = SSI_VIDEO_MAJOR_TYPE_VIDEO_ONLY,	
	GUID reserved = SSI_GUID_NULL)
{
	s.widthInPixels = widthInPixels;
	s.heightInPixels = heightInPixels;
	s.depthInBitsPerChannel = depthInBitsPerChannel;
	s.numOfChannels = numOfChannels;
	s.framesPerSecond = framesPerSecond;
	s.useClosestFramerateForGraph = useClosestFramerateOfGraph;
	s.flipImage = flipImage;
	s.automaticGenerationOfGraph = automaticGenerationOfGraph;
	s.majorVideoType = majorVideoType;
	s.outputSubTypeOfCaptureDevice = outputSubTypeOfCaptureDevice;
	s.reserved = reserved;
}

// calculates stride from ssi_video_params_t
// (((image->width * image->nChannels * (image->depth & ~IPL_DEPTH_SIGN) + 7)/8)+ align - 1) & (~(align - 1));
#define ssi_fourcc(c1,c2,c3,c4) (((c1)&255) + (((c2)&255)<<8) + (((c3)&255)<<16) + (((c4)&255)<<24))
#define ssi_video_stride(params) ((((params.widthInPixels * params.numOfChannels * (params.depthInBitsPerChannel & ~SSI_VIDEO_DEPTH_SIGN) + 7) >> 3) + 3) & (~3))
#define ssi_video_size(params) (((((params.widthInPixels * params.numOfChannels * (params.depthInBitsPerChannel & ~SSI_VIDEO_DEPTH_SIGN) + 7) >> 3) + 3) & (~3)) * params.heightInPixels)

SSI_INLINE bool ssi_write_bmp (const char *path, BYTE *image, ssi_size_t n_bytes, ssi_size_t width, ssi_size_t height, ssi_size_t bits_per_pixel)
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	
	// andinitialize them to zero
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
	
	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + n_bytes;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
	
	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = bits_per_pixel;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	FILE *fp = fopen (path, "wb");
	if (!fp) {
		ssi_wrn ("could not open file '%s'", path);
		return false;
	}
	
	// write file header	
	fwrite (&bmfh, sizeof (BITMAPFILEHEADER), 1, fp);
	
	// write infoheader
	fwrite (&info, sizeof (BITMAPINFOHEADER), 1, fp);
	
	// write image data
	fwrite (image, n_bytes, 1, fp);
		
	// and close file
	fclose (fp);

	return true;
}

SSI_INLINE bool ssi_write_bmp (const char *path, BYTE *image, ssi_video_params_t params) {
	return ssi_write_bmp (path, 
		image, 
		ssi_video_size (params), 
		params.widthInPixels, 
		params.heightInPixels, 
		params.depthInBitsPerChannel * params.numOfChannels);
}

// wav functions
SSI_INLINE WAVEFORMATEX ssi_create_wfx (ssi_time_t sample_rate,
	ssi_size_t sample_dimension,
	ssi_size_t sample_bytes) {

	WAVEFORMATEX wfx;

	wfx.nSamplesPerSec  = ssi_cast (unsigned int, sample_rate);		 /* sample rate */ 
    wfx.wBitsPerSample  = ssi_cast (unsigned short, sample_bytes) * 8;     /* sample size */ 
    wfx.nChannels       = ssi_cast (unsigned short, sample_dimension);   /* channels    */ 
    wfx.cbSize          = 0;
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nBlockAlign     = (wfx.wBitsPerSample * wfx.nChannels) >> 3; 
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec; 

	return wfx;
}

// matrix & functions

SSI_INLINE void ssi_matrix_init (ssi_matrix_t &matrix,
	ssi_size_t rows,
	ssi_size_t cols,
	ssi_size_t byte) {
	matrix.rows = rows;
	matrix.cols = cols;
	matrix.byte = byte;
	matrix.elems = rows * cols;
	matrix.tot = matrix.elems * byte;
	if (matrix.tot == 0) {
		matrix.ptr = 0;
	} else  {
		matrix.ptr = new ssi_byte_t[matrix.tot];	
	}
};
SSI_INLINE void ssi_matrix_clone (const ssi_matrix_t &from, ssi_matrix_t &to) {
	to = from;
	to.ptr = new ssi_byte_t[from.tot];
	memcpy (to.ptr, from.ptr, from.tot);	
};
SSI_INLINE void ssi_matrix_reset (ssi_matrix_t &matrix) {
	matrix.tot = matrix.elems = matrix.rows = matrix.cols = 0;
	delete[] matrix.ptr;
	matrix.ptr = 0;
};
SSI_INLINE void ssi_matrix_destroy (ssi_matrix_t &matrix) {	
	ssi_matrix_reset (matrix);
	matrix.byte = 0;
};

// stream struct & functions

SSI_INLINE void ssi_stream_init (ssi_stream_t &stream,
	ssi_size_t num,
	ssi_size_t dim,
	ssi_size_t byte,
	ssi_type_t type,
	ssi_time_t sr,		
	ssi_time_t time = 0) {
	stream.num_real = stream.num = num;
	stream.dim = dim;
	stream.byte = byte;
	stream.tot_real = stream.tot = num*dim*byte;
	if (stream.tot_real > 0) {
		stream.ptr = new ssi_byte_t[stream.tot_real];
	} else {
		stream.ptr = 0;
	}
	stream.sr = sr;
	stream.time = time;
	stream.type = type;
};
SSI_INLINE int ssi_stream_adjust (ssi_stream_t &stream, ssi_size_t num) {
	if (num == stream.num)
		return 0;
	if (num < stream.num_real) {
		stream.num = num;
		stream.tot = num*stream.dim*stream.byte;
		return -1;
	}
	ssi_size_t new_num = num;
	ssi_size_t new_tot = num*stream.dim*stream.byte;
	ssi_byte_t *new_ptr = new ssi_byte_t[new_tot];
	memcpy (new_ptr, stream.ptr, stream.tot);
	delete[] stream.ptr;
	stream.ptr = new_ptr;
	stream.num_real = stream.num = new_num;
	stream.tot_real = stream.tot = new_tot;	
	return 1;
};
SSI_INLINE void ssi_stream_clone (const ssi_stream_t &from, ssi_stream_t &to) {
	to = from;
	if (from.tot_real > 0) {
		to.ptr = new ssi_byte_t[from.tot_real];
		memcpy (to.ptr, from.ptr, from.tot_real);
	} else {
		to.ptr = 0;
	}
};
SSI_INLINE void ssi_stream_select (const ssi_stream_t &from, ssi_stream_t &to, ssi_size_t n_dims, ssi_size_t *dims) {

	if (n_dims == 0) {
		ssi_stream_clone (from, to);
		return;
	}

	ssi_size_t byte = from.byte;
	ssi_size_t num = from.num;
	ssi_stream_init (to, num, n_dims, byte, from.type, from.sr, from.time);

	if (num > 0) {

		int *offsets = new int[n_dims+1];
		offsets[0] = dims[0] * byte;
		for (ssi_size_t i = 1; i < n_dims; i++) {
			offsets[i] = (ssi_cast (int, dims[i]) - ssi_cast (int, dims[i-1])) * byte;
		}
		offsets[n_dims] = (from.dim - dims[n_dims-1]) * byte;

		ssi_byte_t *ptrin = from.ptr;
		ssi_byte_t *ptrout = to.ptr;
		for (ssi_size_t nnum = 0; nnum < num; nnum++) {
			for (ssi_size_t ndim = 0; ndim < n_dims; ndim++) {
				ptrin += offsets[ndim];
				memcpy (ptrout, ptrin, byte);
				ptrout += byte;
			}
			ptrin += offsets[n_dims];
		}

		delete[] offsets;
	}
}
// copies (sub)stream, from sample 'num_from' to sample 'num_to'-1
// same like ssi_stream_copy (from, to, 0, from.num) is the same as ssi_stream_clone (from, to)
// except that unused bytes are not copied
SSI_INLINE void ssi_stream_copy (const ssi_stream_t &from, ssi_stream_t &to, ssi_size_t num_from, ssi_size_t num_to) {
	to = from;
	to.num_real = to.num = num_to - num_from;
	to.tot_real = to.tot = to.num_real * to.dim * to.byte;
	to.ptr = new ssi_byte_t[to.tot_real];
	memcpy (to.ptr, from.ptr + from.byte * from.dim * num_from, to.tot_real);
};
SSI_INLINE void ssi_stream_cat (const ssi_stream_t &from, ssi_stream_t &to) {

	// calculate required size
	ssi_size_t new_num = to.num + from.num;
	ssi_size_t new_tot = to.tot + from.tot;

	// check if to is large enough to fit from
	if (to.tot_real >= new_tot) {
		// append to end of to
		memcpy (to.ptr + to.tot, from.ptr, from.tot);
		to.num = new_num;
		to.tot = new_tot;
	} else {
	    // allocate new memory
		ssi_byte_t *new_mem = new ssi_byte_t[new_tot];
		// copy data
		memcpy (new_mem, to.ptr, to.tot);
		memcpy (new_mem + to.tot, from.ptr, from.tot);
		// delete old memory and assign new memory
		delete[] to.ptr;
		to.ptr = new_mem;
		to.num_real = to.num = new_num;
		to.tot_real = to.tot = new_tot;
	}
}
SSI_INLINE bool ssi_stream_compare (ssi_stream_t &a, ssi_stream_t &b) {
	return a.byte == b.byte && 
		   a.dim == b.dim &&
		   a.type == b.type;
}
SSI_INLINE void ssi_stream_reset (ssi_stream_t &stream) {
	stream.num_real = stream.num = 0;
	stream.tot_real = stream.tot = 0;
	delete[] stream.ptr;
	stream.ptr = 0;
};
SSI_INLINE void ssi_stream_destroy (ssi_stream_t &stream) {	
	ssi_stream_reset (stream);
	stream.dim = 0;
	stream.byte = 0;
	stream.ptr = 0;
	stream.sr = 0;
	stream.type = SSI_UNDEF;
};
SSI_INLINE void ssi_stream_info (const ssi_stream_t &stream, FILE *file) {
	ssi_fprint (file, "rate\t= %lf hz\n\
dim\t= %u\n\
bytes\t= %u\n\
num\t= %u (%u)\n\
dur\t= %lf s\n\
time\t %lf s\n\
type\t= %s\n",
stream.sr,
stream.dim,
stream.byte,
stream.num,
stream.num_real,
stream.num/stream.sr,
stream.time,
SSI_TYPE_NAMES[stream.type]);
}
SSI_INLINE void ssi_stream_zero (ssi_stream_t &stream) {
	memset (stream.ptr, 0, stream.tot_real);
}
SSI_INLINE void ssi_stream_print (const ssi_stream_t &stream, FILE *file) {
	switch (stream.type) {
		case SSI_CHAR: {
			char *ptr = ssi_pcast (char, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%d ", ssi_cast (int, *ptr++));
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_UCHAR: {
			unsigned char *ptr = ssi_pcast (unsigned char, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%d ", ssi_cast (int, *ptr++));
				}
				ssi_fprint (file, "\n");
				}
			break;
		}
		case SSI_SHORT: {
			short *ptr = ssi_pcast (short, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%d ", ssi_cast (int, *ptr++));
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_USHORT: {
			unsigned short *ptr = ssi_pcast (unsigned short, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%u ", ssi_cast (unsigned int, *ptr++));
				}
				ssi_fprint (file, "\n");
			}
			break;
		}	
		case SSI_INT: {
			int *ptr = ssi_pcast (int, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%d ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_UINT: {		
			unsigned int *ptr = ssi_pcast (unsigned int, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%u ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_LONG: {
			long *ptr = ssi_pcast (long, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%ld ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_ULONG: {
			unsigned long *ptr = ssi_pcast (unsigned long, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%lu ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}		
		case SSI_REAL: {
			float *ptr = ssi_pcast (float, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%f ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}					  
		case SSI_DOUBLE: {
			double *ptr = ssi_pcast (double, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%lf ", *ptr++);
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		case SSI_BOOL: {
			bool *ptr = ssi_pcast (bool, stream.ptr);
			for (ssi_size_t i = 0; i < stream.num; i++) {
				for (ssi_size_t j = 0; j < stream.dim; j++) {
					ssi_fprint (file, "%s ", *ptr++ ? "true" : "false");
				}
				ssi_fprint (file, "\n");
			}
			break;
		}
		default:
			ssi_err ("unsupported sample type");
	}
}

// event struct and functions
SSI_INLINE void ssi_event_init (ssi_event_t &e,
	ssi_etype_t type = SSI_ETYPE_UNDEF,
	ssi_size_t sender_id = SSI_FACTORY_STRINGS_INVALID_ID,
	ssi_size_t event_id = SSI_FACTORY_STRINGS_INVALID_ID,
	ssi_size_t time = 0,
	ssi_size_t dur = 0,	
	ssi_size_t tot = 0,	
	ssi_real_t prob = 1.0f,
	ssi_estate_t state = SSI_ESTATE_COMPLETED,
	ssi_size_t glue_id = SSI_FACTORY_UNIQUE_INVALID_ID) {
	e.sender_id = sender_id;
	e.event_id = event_id;
	e.time = time;
	e.dur = dur;
	e.type = type;
	e.tot_real = e.tot = tot;
	if (tot > 0) {
		e.ptr = new ssi_byte_t[tot];
	} else {
		e.ptr = 0;
	}
	e.prob = prob;
	e.state = state;
	e.glue_id = glue_id;
};
SSI_INLINE void ssi_event_reset (ssi_event_t &e) {	
	delete[] e.ptr; e.ptr = 0;
	e.tot_real = e.tot = 0;
}
SSI_INLINE void ssi_event_destroy (ssi_event_t &e) {
	ssi_event_reset (e);
	e.type = SSI_ETYPE_UNDEF;
	e.time = 0;
	e.dur = 0;	
	e.prob = 0;
	e.event_id = SSI_FACTORY_STRINGS_INVALID_ID;
	e.sender_id = SSI_FACTORY_STRINGS_INVALID_ID;
	e.state = SSI_ESTATE_COMPLETED;
	e.glue_id = SSI_FACTORY_UNIQUE_INVALID_ID;
}
SSI_INLINE int ssi_event_adjust (ssi_event_t &e, ssi_size_t tot) {
	if (tot == e.tot)
		return 0;
	if (tot < e.tot_real) {
		e.tot = tot;		
		return -1;
	}
	ssi_size_t new_tot = tot;	
	ssi_byte_t *new_ptr = new ssi_byte_t[new_tot];
	memcpy (new_ptr, e.ptr, e.tot);
	delete[] e.ptr;
	e.ptr = new_ptr;	
	e.tot_real = e.tot = new_tot;		
	return 1;
};
SSI_INLINE void ssi_event_clone (const ssi_event_t &from, ssi_event_t &to) {
	to = from;
	if (from.tot_real > 0) {
		to.ptr = new ssi_byte_t[from.tot_real];
		memcpy (to.ptr, from.ptr, from.tot_real);
	} else {
		to.ptr = 0;
	}
};
SSI_INLINE void ssi_event_copy (const ssi_event_t &from, ssi_event_t &to) {	
	to.time = from.time;
	to.dur = from.dur;
	to.prob = from.prob;
	to.sender_id = from.sender_id;
	to.event_id = from.event_id;
	to.type = from.type;
	to.state = from.state;
	to.glue_id = from.glue_id;
	if (from.tot > 0) {
		ssi_event_adjust (to, from.tot);
		memcpy (to.ptr, from.ptr, from.tot);
	} else {
		delete[] to.ptr;
		to.ptr = 0;
		to.tot = to.tot_real = 0;		 
	}
};

// sample struct and functions
SSI_INLINE void ssi_sample_create (ssi_sample_t &sample, ssi_size_t num, ssi_size_t user_id, ssi_size_t class_id, ssi_time_t time, ssi_real_t prob) {
	sample.num = num;
	if (num > 0) {
		sample.streams = new ssi_stream_t *[num];
		for (ssi_size_t i = 0; i < num; i++) {
			sample.streams[i] = 0;
		}
	} else {
		sample.streams = 0;
	}
	sample.user_id = user_id;
	sample.class_id = class_id;
	sample.time = time;
	sample.prob = prob;
}
SSI_INLINE void ssi_sample_clone (const ssi_sample_t &from, ssi_sample_t &to) {
	to = from;
	to.streams = new ssi_stream_t *[from.num];
	for (ssi_size_t i = 0; i < from.num; i++) {		
		if (from.streams[i]) {
			to.streams[i] = new ssi_stream_t;
			ssi_stream_clone (*from.streams[i], *to.streams[i]);
		} else {
			to.streams[i] = 0;
		}
	}
}
SSI_INLINE void ssi_sample_destroy (ssi_sample_t &sample) {
	for (ssi_size_t i = 0; i < sample.num; i++) {
		if (sample.streams[i]) {
			ssi_stream_destroy (*sample.streams[i]);		
			delete sample.streams[i];
		}
	}
	delete[] sample.streams;
	sample.streams = 0;
	sample.num = 0;	
}

// random

SSI_INLINE void ssi_random_seed (unsigned int seed = ssi_cast (unsigned int, time (NULL))) {
	//srand (seed);
	r250_init (seed);
}
SSI_INLINE double ssi_random () { // random number in interval [0..1]
	//return ssi_cast (double, rand ()) / RAND_MAX;
	return dr250 ();
}
SSI_INLINE double ssi_random_distr (double m, double s) { // random number drawn from distribution with mean m and standard deviation s
	return dr250_box_muller (m, s);
}
SSI_INLINE double ssi_random (double a, double b) { // random number in interval [a..b]
	return (ssi_random () * (b - a)) + a;
}
SSI_INLINE double ssi_random (double max) { // random number in interval [0..max]
	return ssi_random (0, max);
}
SSI_INLINE ssi_size_t ssi_random (ssi_size_t max) { // integer random number in interval [0..max]
	return ssi_cast (ssi_size_t, max * ssi_random () + 0.5);
}
SSI_INLINE void ssi_random_shuffle (ssi_size_t n, ssi_size_t *arr) { // randomly shuffles elements in array arr
	for (ssi_size_t i = 0; i < n; i++) {
		ssi_size_t r = ssi_random (n-1);
		ssi_size_t tmp = arr[i];
		arr[i] = arr[r];
		arr[r] = tmp;
	}
}

// math functions
SSI_INLINE void static ssi_power (ssi_size_t num, 
	ssi_size_t dim, 
	const ssi_real_t *srcptr, 
	ssi_real_t *dstptr) {

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		val = *srcptr++;	
		dstptr[i] = val * val;
	}
	for (ssi_size_t i = 1; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++;	
			dstptr[j] += val * val;
		}
	}
	for (ssi_size_t i = 0; i < dim; i++) {
		dstptr[i] = sqrt (dstptr[i] / num);
	}
}
SSI_INLINE void static ssi_power_thres (ssi_size_t num, 
	ssi_size_t dim, 
	ssi_real_t thres, 
	const ssi_real_t *srcptr, 
	ssi_real_t *dstptr) {

	ssi_size_t *counter = new ssi_size_t[dim];
	for (ssi_size_t i = 0; i < dim; i++) {
		counter[i] = 0;
	}

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		dstptr[i] = 0;		
	}
	for (ssi_size_t i = 0; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++;	
			if (val > thres) {
				dstptr[j] += val * val;
				++counter[j];
			}
		}
	}
	for (ssi_size_t i = 0; i < dim; i++) {
		if (counter[i] > 0) {
			dstptr[i] = sqrt (dstptr[i] / counter[i]);
		}
	}

	delete[] counter;
}
SSI_INLINE void static ssi_mean (ssi_size_t num, 
	ssi_size_t dim, 
	const ssi_real_t *srcptr, 
	ssi_real_t *dstptr) {

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		val = *srcptr++;	
		dstptr[i] = val;
	}
	for (ssi_size_t i = 1; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++;	
			dstptr[j] += val;
		}
	}
	for (ssi_size_t i = 0; i < dim; i++) {
		dstptr[i] /= num;
	}
}
SSI_INLINE void static ssi_var (ssi_size_t num, 
	ssi_size_t dim, 
	const ssi_real_t *srcptr, 
	ssi_real_t *dstptr) {

	ssi_real_t *mean = new ssi_real_t[dim];
	ssi_mean (num, dim, srcptr, mean);

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		val = *srcptr++ - mean[i];	
		dstptr[i] = val * val;
	}
	for (ssi_size_t i = 1; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++ - mean[j];	
			dstptr[j] += val * val;
		}
	}
	for (ssi_size_t i = 0; i < dim; i++) {
		dstptr[i] /= num;
	}

	delete[] mean;
}
SSI_INLINE void static ssi_minmax (ssi_size_t num, 
	ssi_size_t dim, 
	const ssi_real_t *srcptr, 
	ssi_real_t *minval,
	ssi_size_t *minpos,
	ssi_real_t *maxval,
	ssi_size_t *maxpos) {

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		val = *srcptr++;	
		minval[i] = val;
		minpos[i] = 0;
		maxval[i] = val;
		maxpos[i] = 0;
	}
	for (ssi_size_t i = 1; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++;	
			if (val < minval[j]) {
				minval[j] = val;
				minpos[j] = i;
			}
			if (val > maxval[j]) {
				maxval[j] = val;
				maxpos[j] = i;
			}
		}
	}
}

SSI_INLINE void static ssi_peak_count (ssi_size_t num, 
	ssi_size_t dim, 
	ssi_real_t thres,
	ssi_size_t hangover,
	const ssi_real_t *srcptr, 
	ssi_size_t *dstptr) {

	ssi_size_t *counter = new ssi_size_t[dim];
	for (ssi_size_t i = 0; i < dim; i++) {
		counter[i] = 0;
	}

	ssi_real_t val;
	for (ssi_size_t i = 0; i < dim; i++) {
		dstptr[i] = 0;		
	}
	for (ssi_size_t i = 0; i < num; i++) {
		for (ssi_size_t j = 0; j < dim; j++) {
			val = *srcptr++;	
			if (abs (val) > thres && counter[j] == 0) {
				++dstptr[j];
				counter[j] = hangover;
			}
			if (abs (val) <= thres && counter[j] > 0) {
				--counter[j];
			}
		}
	}

	delete[] counter;
}

// bitmask

SSI_INLINE ssi_size_t static ssi_CountSetBits (ssi_bitmask_t bitmask) {

	ssi_size_t count = 0;

    while (bitmask) {
		count++ ;
        bitmask &= (bitmask - 1) ;
	}

	return count;
}

SSI_INLINE int static ssi_FindHighestOrderBit (ssi_bitmask_t bitmask) {

	int position = -1;

	while (bitmask) {
		bitmask = bitmask >> 1;
		position++;
	}

	return position;
}

// array2string and parse functions

SSI_INLINE bool static ssi_array2string (ssi_size_t n_arr, ssi_size_t *arr, ssi_size_t n_string, ssi_char_t *string, ssi_char_t delim = ' ') {

	ssi_size_t count = 0;
	ssi_size_t len = 0;
	ssi_char_t tmp[64];

	if (n_arr == 0) {
		string[0] = '\0';
		return true;
	}

	ssi_sprint (string, "%u", arr[0]);
	count = ssi_cast (ssi_size_t, strlen (string));
	for (ssi_size_t i = 1; i < n_arr; i++) {
		ssi_sprint (tmp, "%c%u", delim, arr[i]);
		len = ssi_cast (ssi_size_t, strlen (tmp));
		if (count + len < n_string - 1) {
			memcpy (string + count, tmp, len);
		} else {
			ssi_wrn ("string too small to fit array");
			return false;
		}
		count += len;
	}

	string[count] = '\0';
	return true;
}


SSI_INLINE bool static ssi_array2string (ssi_size_t n_arr, ssi_real_t *arr, ssi_size_t n_string, ssi_char_t *string, ssi_char_t delim = ' ') {

	ssi_size_t count = 0;
	ssi_size_t len = 0;
	ssi_char_t tmp[64];

	if (n_arr == 0) {
		string[0] = '\0';
		return true;
	}

	ssi_sprint (string, "%f", arr[0]);
	count = ssi_cast (ssi_size_t, strlen (string));
	for (ssi_size_t i = 1; i < n_arr; i++) {
		ssi_sprint (tmp, "%c%f", delim, arr[i]);
		len = ssi_cast (ssi_size_t, strlen (tmp));
		if (count + len < n_string - 1) {
			memcpy (string + count, tmp, len);
		} else {
			ssi_wrn ("string too small to fit array");
			return false;
		}
		count += len;
	}

	string[count] = '\0';
	return true;
}

SSI_INLINE ssi_size_t static ssi_split_string_count (const ssi_char_t *string, ssi_char_t delim = ' ') {

	ssi_size_t len = ssi_cast (ssi_size_t, strlen (string));
	if (len == 0) {
		return 0;
	}

	ssi_size_t count = 1;
	ssi_char_t *ptr = ssi_ccast (ssi_char_t *, string);
	bool valid = false;
	while (*ptr != '\0') {
		if (*ptr++ == delim) {
			if (valid) {			
				count++;			
			}
			valid = false;
		} else {
			valid = true;		
		}
	}
	if (!valid) {
		count--;
	}

	return count;
}

SSI_INLINE bool static ssi_split_string (ssi_size_t n_arr, ssi_char_t **arr, const ssi_char_t *string, ssi_char_t delim = ' ') {

	if (ssi_split_string_count (string, delim) != n_arr) {
		ssi_wrn ("invalid array size");
		return false;
	}

	if (n_arr == 0) {
		return true;
	}

	ssi_char_t delim_s[2];
	delim_s[0] = delim;
	delim_s[1] = '\0';

	char *str = ssi_strcpy (string);
	char * pch = strtok (str, delim_s);
	for (ssi_size_t i = 0; i < n_arr; i++) {
		arr[i] = ssi_strcpy (pch);
		pch = strtok (NULL, delim_s);
	}
	delete[] str;

	return true;
}

SSI_INLINE ssi_size_t static ssi_string2array_count (const ssi_char_t *string, ssi_char_t delim = ' ') {

	ssi_size_t len = ssi_cast (ssi_size_t, strlen (string));
	if (len == 0) {
		return 0;
	}

	ssi_size_t count = 1;
	ssi_char_t *ptr = ssi_ccast (ssi_char_t *, string);
	while (*ptr != '\0') {
		if (*ptr++ == delim) {
			count++;
		}
	}

	return count;
}

SSI_INLINE bool static ssi_string2array (ssi_size_t n_arr, ssi_size_t *arr, const ssi_char_t *string, ssi_char_t delim = ' ') {

	if (ssi_string2array_count (string, delim) != n_arr) {
		ssi_wrn ("invalid array size");
		return false;
	}

	if (n_arr == 0) {
		return true;
	}
	
	sscanf (string, "%u", &arr[0]);
	ssi_char_t *ptr = ssi_ccast (ssi_char_t *, string);	
	ssi_size_t count = 1;
	ssi_size_t pos = 0;	
	while (*ptr != '\0') {
		if (*ptr++ == delim) {
			sscanf (string + pos + 1, "%u", &arr[count]);			
			count++;
		}
		pos++;
	}

	return true;
}

SSI_INLINE bool static ssi_string2array (ssi_size_t n_arr, ssi_real_t *arr, const ssi_char_t *string, ssi_char_t delim = ' ') {

	if (ssi_string2array_count (string, delim) != n_arr) {
		ssi_wrn ("invalid array size");
		return false;
	}

	if (n_arr == 0) {
		return true;
	}
	
	sscanf (string, "%f", &arr[0]);
	ssi_char_t *ptr = ssi_ccast (ssi_char_t *, string);	
	ssi_size_t count = 1;
	ssi_size_t pos = 0;	
	while (*ptr != '\0') {
		if (*ptr++ == delim) {
			sscanf (string + pos + 1, "%f", &arr[count]);			
			count++;
		}
		pos++;
	}

	return true;
}

SSI_INLINE int *ssi_parse_indices (const ssi_char_t *str, ssi_size_t &n_indices, bool sort = false, const ssi_char_t *delims = " ,") {

	n_indices = 0;

	if (!str || str[0] == '\0') {
		return 0;
	}

	ssi_char_t *string = ssi_strcpy (str);
	
	char *pch;
	strcpy (string, str);
	pch = strtok (string, delims);
	int index;

	std::vector<int> items;
	
	while (pch != NULL) {
		index = atoi (pch);
		items.push_back(index);
		pch = strtok (NULL, delims);
	}

	if (sort) {
		std::sort (items.begin(), items.end());		
	}

	n_indices = (ssi_size_t) items.size();
	int *indices = new int[n_indices];

	for(size_t i = 0; i < items.size(); i++) {
 		indices[i] = items[i];		
	}

	delete[] string;

	return indices;
}

template <typename tin>
SSI_INLINE static void ssi_cast2type (ssi_size_t n, tin* in, void* out, ssi_type_t type) {

	switch (type) 
	{
	case SSI_CHAR: 
		{
			char *pout = ssi_pcast(char, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(char, *in++); 
		} break;

	case SSI_UCHAR: 
		{
			unsigned char *pout = ssi_pcast(unsigned char, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(unsigned char, *in++); 
		} break;

	case SSI_SHORT:
		{
			short *pout = ssi_pcast(short, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(short, *in++); 
		} break;

	case SSI_USHORT: 
		{
			unsigned short *pout = ssi_pcast(unsigned short, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(unsigned short, *in++); 
		} break;

	case SSI_INT: 
		{
			int *pout = ssi_pcast(int, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(int, *in++); 
		} break;

	case SSI_UINT: 
		{
			unsigned int *pout = ssi_pcast(unsigned int, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(unsigned int, *in++); 
		} break;

	case SSI_LONG:
		{
			long *pout = ssi_pcast(long, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(long, *in++); 
		} break;

	case SSI_ULONG: 
		{
			unsigned long *pout = ssi_pcast(unsigned long, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(unsigned long, *in++); 
		} break;

	case SSI_FLOAT: 
		{
			float *pout = ssi_pcast(float, out);
			for (ssi_size_t i = 0; i < n; i++) 
				*pout++ = ssi_cast(float, *in++);
		} break;

	case SSI_DOUBLE: 
		{
			double *pout = ssi_pcast(double, out);
			for (ssi_size_t i = 0; i < n; i++)
				*pout++ = ssi_cast(double, *in++); 
		} break;

	case SSI_BOOL: 
		{
			bool *pout = ssi_pcast(bool, out);
			for (ssi_size_t i = 0; i < n; i++)
			{
				bool value = (*in++ != 0) ? true : false;
				*pout++ = value; 
			}
		} break;

	default:
		ssi_err ("unsupported sample type");
	}
}

SSI_INLINE static void ssi_cast2type (ssi_size_t n, void *pin, void *pout, ssi_type_t tin, ssi_type_t tout) {

	switch (tin) 
	{
	case SSI_CHAR:
		{
			char *src = (char *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_UCHAR:
		{
			unsigned char *src = (unsigned char *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_SHORT:
		{
			short *src = (short *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_USHORT:
		{
			unsigned short *src = (unsigned short *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_INT:
		{
			int *src = (int *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_UINT:
		{
			unsigned int *src = (unsigned int *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_LONG:
		{
			long *src = (long *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_ULONG:
		{
			unsigned long *src = (unsigned long *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_FLOAT:
		{
			float *src = (float *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_DOUBLE:
		{
			double *src = (double *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;

	case SSI_BOOL:
		{
			bool *src = (bool *) pin;
			ssi_cast2type (n, src, pout, tout);
		} break;
	}
}

// execute a process
// set 'wait_ms' >= 0 to wait that many milliseconds
// set 'wait_ms'  < 0 to wait until job is finished
SSI_INLINE bool ssi_execute (const ssi_char_t *exe, const ssi_char_t *args, int wait_ms, bool show_console = true)
{
	size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
	DWORD dwExitCode = 0;
	std::wstring sTempStr = L"";

	// check here to see if the exe even exists
	if (!ssi_exists (exe)) {
		ssi_wrn ("executable not found '%s'", exe);
		return false;
	}

	wchar_t *wstr = ssi_char2wchar (exe);
	std::wstring FullPathToExe (wstr);
	delete[] wstr;
	wstr = ssi_char2wchar (args != 0 ? args : "");
	std::wstring Parameters (wstr);
	delete[] wstr;
	
	// add a space to the beginning of the Parameters 
	if (Parameters.size() != 0)
	{
		if (Parameters[0] != L' ')
		{
			Parameters.insert(0,L" ");
		}
	}

	// The first parameter needs to be the exe itself
	sTempStr = FullPathToExe;
	iPos = sTempStr.find_last_of (L"\\");
	sTempStr.erase (0, iPos +1);
	Parameters = sTempStr.append (Parameters);

	// CreateProcessW can modify Parameters thus we allocate needed memory
	wchar_t * pwszParam = new wchar_t[Parameters.size () + 1];
	if (pwszParam == 0)
	{
		return false;
	}
	const wchar_t* pchrTemp = Parameters.c_str ();
	wcscpy_s(pwszParam, Parameters.size () + 1, pchrTemp);

	// CreateProcess API initialization
	STARTUPINFOW siStartupInfo;	
	PROCESS_INFORMATION piProcessInfo;
	memset (&siStartupInfo, 0, sizeof (siStartupInfo));
	memset(&piProcessInfo, 0, sizeof (piProcessInfo));
	siStartupInfo.cb = sizeof (siStartupInfo);

	if (!show_console) {
		siStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		siStartupInfo.wShowWindow = SW_HIDE;
	}

	if (CreateProcessW (const_cast<LPCWSTR> (FullPathToExe.c_str()),
		pwszParam, 0, 0, false,
		CREATE_NEW_CONSOLE, 0, 0,
		&siStartupInfo, &piProcessInfo) != false) {

		// Watch the process.
		dwExitCode = WaitForSingleObject (piProcessInfo.hProcess, wait_ms < 0 ? INFINITE : wait_ms);
	}
	else {

		// CreateProcess failed
		iReturnVal = GetLastError();

		wchar_t *strErrorMessage = NULL;    
		FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, (DWORD) iReturnVal, 0, (LPWSTR) &strErrorMessage, 0, NULL);	
		char *string = ssi_wchar2char (strErrorMessage);
		ssi_wrn ("%s", string);
		delete[] string;
	}

	// Free memory
	delete[]pwszParam;
	pwszParam = 0;

	// Release handles
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread);

	return true;
}

#endif
