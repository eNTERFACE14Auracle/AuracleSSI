// Selector.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/21
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

#include "signal/Selector.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

Selector::Selector (const ssi_char_t *file) 
	: _cOffsetCount(0),
	_selectedIndexList (0),
	_offsetList (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

Selector::~Selector () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
	
	delete[] _offsetList;
}

void Selector::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	int maxbit = 0;

	delete[] _selectedIndexList;	
	_selectedIndexList = 0;

	if (!Parse (_options.indices)) 
		_cOffsetCount = 0;		
	else
	{
		maxbit = _selectedIndexList[0];

		//generate offset values
		_offsetList = new int[_cOffsetCount+1];

		_offsetList[0] = _selectedIndexList[0] * sizeof(float);
		for(ssi_size_t i = 1; i < _cOffsetCount; i++) {
			_offsetList[i] = (_selectedIndexList[i] - _selectedIndexList[i-1]) * sizeof(float); 
			if (maxbit < _selectedIndexList[i]) {
				maxbit = _selectedIndexList[i];
			}
		}

		//save one extra offset that closes the gap between last selected dimension and highest dimension in stream
		_offsetList[_cOffsetCount] = (stream_in.dim - _selectedIndexList[_cOffsetCount-1]) * sizeof(float);
	}

	if (maxbit >= ssi_cast (int, stream_in.dim)) {
		ssi_err ("selected index '%u' exceeds dimensions '%u'", maxbit, stream_in.dim);
	}
}

void Selector::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t num = stream_in.num;
	ssi_size_t dim = stream_in.dim;
	ssi_size_t byte = stream_in.byte;

	ssi_byte_t *ptr_in = stream_in.ptr;
	ssi_byte_t *ptr_out = stream_out.ptr;

	if(0 == _cOffsetCount)
		memcpy(ptr_out, ptr_in, stream_out.tot);
	else
	{
		for (ssi_size_t i = 0; i < stream_out.num; i++) {
			for (ssi_size_t j = 0; j < _cOffsetCount; j++) {
				ptr_in += _offsetList[j];
				memcpy(ptr_out, ptr_in, sizeof(float));
				ptr_out += stream_out.byte;
			}
			ptr_in += _offsetList[_cOffsetCount];
		}
	}
}

void Selector::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	delete[] _selectedIndexList;
	_selectedIndexList = 0;
}

bool Selector::Parse (const ssi_char_t *indices) {
	
	if (!indices || indices[0] == '\0') {
		return false;
	}

	ssi_char_t string[SSI_MAX_CHAR];
	
	char *pch;
	strcpy (string, indices);
	pch = strtok (string, ", ");
	int index;

	std::vector<int> items;
	
	while (pch != NULL) {
		index = atoi (pch);
		items.push_back(index);
		pch = strtok (NULL, ", ");
	}

	if (_options.sort) {
		std::sort(items.begin(), items.end());		
	}

	_cOffsetCount = (ssi_size_t)items.size();
	_selectedIndexList = new int[_cOffsetCount];

	for(size_t i = 0; i < items.size(); i++)
		_selectedIndexList[i] = items[i];		

	return true;
}

}
