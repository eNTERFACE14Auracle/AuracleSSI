// PaintVideo.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/28
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

#include "graphic/PaintVideo.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

PaintVideo::PaintVideo (ssi_video_params_t video_params_,
	bool flipForOutput,
	bool scaleToOutput,
	bool mirrorForOutput,
	double maxValue)
	: _image_out (0), 
	_image_out_tmp (0),
	size (0),  
	_video_in_params (video_params_), 
	_video_in_stride (ssi_video_stride (video_params_)),
	_video_in_size (ssi_video_size (video_params_)),
	_flipForOutput(flipForOutput),
	_scaleToOutput(scaleToOutput),
	_mirrorForOutput(mirrorForOutput),
	_maxValue(maxValue)
{

	_video_out_params.widthInPixels = _video_in_params.widthInPixels;
	_video_out_params.heightInPixels = _video_in_params.heightInPixels;
	_video_out_params.depthInBitsPerChannel = SSI_VIDEO_DEPTH_8U;
	if(_video_in_params.numOfChannels == 1) {
		_video_out_params.numOfChannels = 3;
	} else {
		_video_out_params.numOfChannels = _video_in_params.numOfChannels;
	}
	_video_out_stride = ssi_video_stride (_video_out_params);
	_video_out_size = ssi_video_size (_video_out_params);
}

PaintVideo::~PaintVideo ()
{
	delete[] _image_out; _image_out = 0;
	delete[] _image_out_tmp; _image_out_tmp = 0;
}

void PaintVideo::setData (const void *image_in, ssi_size_t size_, ssi_time_t time) 
{
	Lock lock (_mutex);

	if (size_ != 1) {
		ssi_wrn ("ignoring everything but first picture");
	}
	size = 1;

	const BYTE *p_image_in = reinterpret_cast<const BYTE *> (image_in);

	if (!_image_out) {
		_image_out = new BYTE[_video_out_size];
		_image_out_tmp = new BYTE[_video_out_size];
	} 

	switch (_video_in_params.depthInBitsPerChannel)
	{
		case SSI_VIDEO_DEPTH_8U:
		{
			switch (_video_in_params.numOfChannels)
			{				
				case 1:
					{
						BYTE *dstptr;
						const BYTE *srcptr;
						BYTE value;

						for (int i = 0; i < _video_in_params.heightInPixels; i++)
						{
							dstptr = _image_out + i * _video_out_stride;
							srcptr = p_image_in + i * _video_in_stride;
							for (int j = 0; j < _video_in_params.widthInPixels; j++)
							{
								value = *srcptr++;
								*dstptr++ = value;
								*dstptr++ = value;
								*dstptr++ = value;
							}
						}
						break;
					}
				case 3:
				case 4:
					{					
						memcpy (_image_out, p_image_in, _video_in_size);
						break;
					}

				default:
					ssi_err ("#channels '%d' not supported", _video_in_params.numOfChannels);
			}

			break;
		}

		case SSI_VIDEO_DEPTH_32F:
		{
			switch (_video_in_params.numOfChannels)
			{				
				case 1: {

					BYTE *dstptr;
					const float *srcptr;
					unsigned int value;
					for (int i = 0; i < _video_in_params.heightInPixels; i++)
					{
						dstptr = _image_out + i * _video_out_stride;
						srcptr = reinterpret_cast<const float *> (p_image_in + i * _video_in_stride);
						for (int j = 0; j < _video_in_params.widthInPixels; j++)
						{
							value = static_cast<BYTE> (*srcptr++ * 255);
							*dstptr++ = value;
							*dstptr++ = value;
							*dstptr++ = value;
						}
					}
					break;
				}
				default:
					ssi_err ("#channels '%d' not supported", _video_in_params.numOfChannels);
			}

			break;
		}

		case SSI_VIDEO_DEPTH_16U:
		{
			switch (_video_in_params.numOfChannels)
			{				
				case 1: {

					BYTE *dstptr;
					const short *srcptr;
					BYTE value;

					for (int i = 0; i < _video_in_params.heightInPixels; i++)
					{
						dstptr = _image_out + i * _video_out_stride;
						srcptr = reinterpret_cast<const short *> (p_image_in + i * _video_in_stride) + (_mirrorForOutput ? (_video_in_params.widthInPixels - 1) : 0);
						for (int j = 0; j < _video_in_params.widthInPixels; j++)
						{
							// Convert 16 bit value to 8 bit value or use maxValue for this
							value = static_cast<unsigned char> (float(*srcptr) * (255.0f / ((_maxValue > 0) ? _maxValue : 65535.0f)));
							*dstptr++ = value;
							*dstptr++ = value;
							*dstptr++ = value;
							if (_mirrorForOutput)
								srcptr--;
							else
								srcptr++;
						}
					}
					break;

				default:
					ssi_err ("#channels '%d' not supported", _video_in_params.numOfChannels);
				}
			}

			break;
		}

		default:
			ssi_err ("pixel depth '%d' not supported", _video_in_params.depthInBitsPerChannel);
	}

	// mirror & flip

	if(_flipForOutput)
	{
		ssi_size_t stride = _video_out_stride;		
		int height = _video_out_params.heightInPixels;
		BYTE *dstptr = _image_out_tmp + (height - 1) * stride;
		BYTE *srcptr = _image_out;
		for(int j = 0; j < height; ++j)
		{
			memcpy(dstptr, srcptr, stride);
			dstptr -= stride;
			srcptr += stride;
		}
		BYTE *tmp = _image_out;
		_image_out = _image_out_tmp;
		_image_out_tmp = tmp;
	}

	if (_mirrorForOutput)
	{
		BYTE *dstptr = 0;
		const BYTE *srcptr = 0;
		int height = _video_out_params.heightInPixels;
		int width = _video_out_params.widthInPixels;
		int stride = _video_out_stride;
		for(int j = 0; j < height; ++j)
		{
			dstptr = _image_out_tmp + j * stride;
			srcptr = _image_out + j * stride + (width - 1) * 3;
			for (int i = 0; i < width; ++i)
			{
				memcpy(dstptr, srcptr, 3);
				dstptr +=3;
				srcptr -=3;
			}
		}
		BYTE *tmp = _image_out;
		_image_out = _image_out_tmp;
		_image_out_tmp = tmp;
	}
}

void PaintVideo::rescale () 
{
}

void PaintVideo::paint (HDC hdc, RECT rect, Colormap *colormap, bool first_object, bool last_object) 
{

	paintAsVideoImage (hdc, rect);
}

void PaintVideo::paintAsVideoImage (HDC hdc, RECT rect)
{
	Lock lock (_mutex);

	if(!_image_out)
		return;
	

	Gdiplus::Bitmap *bitmap = 0;

	#ifdef USE_SSI_LEAK_DETECTOR
	#ifdef _DEBUG
		#undef new
	#endif
	#endif
	switch (_video_out_params.numOfChannels)
	{
		case 4:
			bitmap = new Gdiplus::Bitmap (_video_out_params.widthInPixels, _video_out_params.heightInPixels, _video_out_stride, PixelFormat32bppRGB, _image_out);
			break;
		case 1:
			bitmap = new Gdiplus::Bitmap (_video_out_params.widthInPixels, _video_out_params.heightInPixels, _video_out_stride, PixelFormat24bppRGB, _image_out);
			break;
		default:
			bitmap = new Gdiplus::Bitmap (_video_out_params.widthInPixels, _video_out_params.heightInPixels, _video_out_stride, PixelFormat24bppRGB, _image_out);
			break;
	}
	#ifdef USE_SSI_LEAK_DETECTOR
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
	#endif
	
	Gdiplus::Graphics graphics(hdc);
	graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeNearestNeighbor); //faster
	
	if(_scaleToOutput && (rect.right-rect.left != _video_out_params.widthInPixels || rect.bottom-rect.top != _video_out_params.heightInPixels))
	{
		graphics.DrawImage(bitmap, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
	}
	else
	{
		graphics.DrawImage(bitmap, rect.left, rect.top);
	}

	delete bitmap;
}


}

