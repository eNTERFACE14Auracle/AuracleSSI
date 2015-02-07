// Functionals.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/12/11
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

#include "signal/Functionals.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi
{

const ssi_size_t IFunctionals::FORMAT_SIZE = 11;
const ssi_char_t *IFunctionals::FORMAT_NAMES[FORMAT_SIZE] = {"mean","energy","std","min","max","range","minpos","maxpos","zeros","peaks","len"};

ssi_size_t IFunctionals::CountSetBits (ssi_bitmask_t _format) {

	ssi_bitmask_t n = _format & ALL;
	unsigned int count = 0;

    while (n) {
		count++ ;
        n &= (n - 1) ;
	}

	return count;
}

ssi_bitmask_t IFunctionals::Names2Format (const ssi_char_t *names) {
	
	if (!names || names[0] == '\0') {
		return ALL;
	}

	ssi_char_t string[SSI_MAX_CHAR];
	ssi_bitmask_t format = NONE;

	char *pch;
	strcpy (string, names);
	pch = strtok (string, ", ");
	while (pch != NULL) {
		format = format | Name2Format (pch);
		pch = strtok (NULL, ", ");
	}

	return format;
}

ssi_bitmask_t IFunctionals::Name2Format (const ssi_char_t *name) {

	ssi_bitmask_t format = NONE;

	for (ssi_size_t i = 0; i < FORMAT_SIZE; i++) {
		if (strcmp (name, FORMAT_NAMES[i]) == 0) {
			format = 1i64 << i;
			break;
		}
	}

	return format;
}

Functionals::Functionals (const ssi_char_t *file)
: _format (NONE), 
	_mean_val(0),
	_energy_val(0),
	_std_val(0),
	_min_val(0),
	_max_val(0),
	_min_pos(0),
	_max_pos(0),
	_zeros(0),
	_peaks(0),
	_left_val(0),
	_mid_val(0),
	_file (0),
	_delta (2) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Functionals::~Functionals () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void Functionals::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	_format = Names2Format (_options.names);
	_delta = _options.delta;

	ssi_size_t sample_dimension = stream_in.dim;

	_mean_val = new ssi_real_t[sample_dimension];
	_energy_val = new ssi_real_t[sample_dimension];
	_std_val = new ssi_real_t[sample_dimension];
	_min_val = new ssi_real_t[sample_dimension];
	_max_val = new ssi_real_t[sample_dimension];
	_min_pos = new ssi_size_t[sample_dimension];
	_max_pos = new ssi_size_t[sample_dimension];
	_zeros = new ssi_size_t[sample_dimension];
	_peaks = new ssi_size_t[sample_dimension];
	_left_val = new ssi_real_t[sample_dimension];
	_mid_val = new ssi_real_t[sample_dimension];
}

void Functionals::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = stream_in.num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);

	calc (sample_dimension, sample_number, srcptr, dstptr);
}

void Functionals::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	delete[] _mean_val;
	_mean_val = 0;
	delete[] _energy_val;
	_energy_val = 0;
	delete[] _std_val;
	_std_val = 0;
	delete[] _min_val;
	_min_val = 0;
	delete[] _max_val;
	_max_val = 0;
	delete[] _min_pos;
	_min_pos = 0;
	delete[] _max_pos;
	_max_pos = 0;
	delete[] _zeros;
	_zeros = 0;
	delete[] _peaks;
	_peaks = 0;
	delete[] _left_val;
	_left_val = 0;
	delete[] _mid_val;
	_mid_val = 0;
}

void Functionals::calc (ssi_size_t sample_dimension, 
	ssi_size_t sample_number, 
	ssi_real_t *srcptr, 
	ssi_real_t *&dstptr) {

	// calculate mean and std
	// for performance reasons std is calculated by
	// 
	// std = sqrt (sum (x*x)/n - mean^2))
	//
	
	bool first_call = true;
	for (ssi_size_t i = 0; i < sample_dimension; i++)
	{
		_val = *srcptr++;
		_mean_val[i] = _val;
		_energy_val[i] = _val * _val;
		_min_val[i] = _val;
		_max_val[i] = _val;
		_min_pos[i] = 0;
		_max_pos[i] = 0;
		_zeros[i] = 0;
		_peaks[i] = 0;
		_mid_val[i] = _val;
	}
	for (ssi_size_t i = 1; i < sample_number; i++)
	{
		for (ssi_size_t j = 0; j < sample_dimension; j++)
		{
			_val = *srcptr++;
			_mean_val[j] += _val;
			_energy_val[j] += _val * _val;
			if (_val < _min_val[j])
			{
				_min_val[j] = _val;
				_min_pos[j] = i;
			}
			else if (_val > _max_val[j])
			{
				_max_val[j] = _val;
				_max_pos[j] = i;
			}
			if ((i % _delta) == 0)
			{
				if (first_call)
				{
					first_call = false;
				}
				else
				{
					if ((_left_val[j] > 0 && _mid_val[j] < 0) || (_left_val[j] < 0 && _mid_val[j] > 0) )
					{
						_zeros[j]++;
					}
					if (_left_val[j] < _mid_val[j]  &&  _mid_val[j] > _val)
					{
						_peaks[j]++;
					}
				}
				_left_val[j] = _mid_val[j];
				_mid_val[j] = _val;
			}
		}
	}
	for (ssi_size_t i = 0; i < sample_dimension; i++)
	{
		_mean_val[i] /= sample_number;
		_energy_val[i] /= sample_number;
		_std_val[i] = sqrt (abs (_energy_val[i] - _mean_val[i] * _mean_val[i]));
	}

	if (MEAN & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _mean_val[i];
		}
	}
	if (ENERGY & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _energy_val[i];
		}
	}
	if (STD & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _std_val[i];
		}
	}
	if (MIN & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _min_val[i];
		}
	}
	if (MAX & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _max_val[i];
		}
	}
	if (RANGE & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = _max_val[i] - _min_val[i];
		}
	}
	if (MINPOS & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = static_cast<ssi_real_t> (_min_pos[i]) / sample_number;
		}
	}
	if (MAXPOS & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = static_cast<ssi_real_t> (_max_pos[i]) / sample_number;
		}
	}
	if (ZEROS & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = static_cast<ssi_real_t> (_zeros[i]) / sample_number;
		}
	}
	if (PEAKS & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = static_cast<ssi_real_t> (_peaks[i]) / sample_number;
		}
	}
	if (LEN & _format)
	{
		for (ssi_size_t i = 0; i < sample_dimension; i++)
		{
			*dstptr++ = static_cast<ssi_real_t> (sample_number);
		}
	}

}

const ssi_char_t *Functionals::getName (ssi_size_t index) {

	ssi_bitmask_t format = Names2Format (_options.names);
	
	for (ssi_size_t i = 0; i < FORMAT_SIZE; i++) {
		if (format & (1i64 << i)) {
			if (index-- == 0) {
				return FORMAT_NAMES[i];
			}
		}
	}

	return 0;
}

}
