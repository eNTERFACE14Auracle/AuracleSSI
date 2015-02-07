// Transformer.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/26
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

#include "frame/Transformer.h"
#include "frame/TheFramework.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

int Transformer::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;
ssi_char_t *Transformer::ssi_log_name = "transform_";

Transformer::Transformer (int buffer_id_in, 
	ITransformer *transformer,
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	ssi_time_t buffer_capacity) :
	_buffer_id_in (buffer_id_in), 	
	_transformer (transformer),
	_xtra_stream_num (0),
	_xtra_stream_ids (0),
	_xtra_streams (0),
	_frame (0) {

	init (frame_size, delta_size, buffer_capacity);
}

Transformer::Transformer (int buffer_id_in, 
	int xtra_buffer_num,
	int *xtra_buffer_ids,
	ITransformer *transformer,
	ssi_size_t frame_size,
	ssi_size_t delta_size,
	ssi_time_t buffer_capacity) :
	_buffer_id_in (buffer_id_in), 
	_transformer (transformer),
	_xtra_stream_num (xtra_buffer_num),
	_xtra_stream_ids (0),
	_xtra_streams (0),
	_frame (0) {

	_xtra_stream_ids = new int[_xtra_stream_num];
	for (ssi_size_t i = 0; i < _xtra_stream_num; i++) {
		_xtra_stream_ids[i] = xtra_buffer_ids[i];
	}

	init (frame_size, delta_size, buffer_capacity);
}

void Transformer::init (ssi_size_t frame_size,
	ssi_size_t delta_size,
	ssi_time_t buffer_capacity) {

	_frame = ssi_pcast (TheFramework, Factory::GetFramework ());

	// set thread name
	ssi_char_t *thread_name = ssi_strcat ("transform@", _transformer->getName ());
	Thread::setName (thread_name);
	delete[] thread_name;

	// first check if in input buffer is valid
	SSI_ASSERT (_frame->IsBufferInUse (_buffer_id_in));
	
	const void * meta;
	ssi_size_t meta_size;

	// forward main input meta data
	meta = _frame->GetMetaData (_buffer_id_in, meta_size);
	if (meta) {
		_transformer->setMetaData (meta_size, meta);
	}

	// forward meta data of xtra inputs
	for(ssi_size_t i = 0; i < _xtra_stream_num; i++)
	{
		meta = _frame->GetMetaData (_xtra_stream_ids[i], meta_size);
		if (meta) {
			_transformer->setMetaData (meta_size, meta);
		}
	}

	// figure out properties of input signal
	ssi_time_t sample_rate_in;
	ssi_size_t sample_dimension_in, sample_bytes_in;
	ssi_type_t sample_type_in;
	_frame->GetSampleRate (_buffer_id_in, sample_rate_in);
	_frame->GetSampleDimension (_buffer_id_in, sample_dimension_in);
	_frame->GetSampleBytes (_buffer_id_in, sample_bytes_in);
	_frame->GetSampleType (_buffer_id_in, sample_type_in);

	// figure out properties of output signal
	ssi_size_t sample_bytes_out = _transformer->getSampleBytesOut (sample_bytes_in);
	ssi_size_t sample_dimension_out = _transformer->getSampleDimensionOut (sample_dimension_in);
	ssi_type_t sample_type_out = _transformer->getSampleTypeOut (sample_type_in);

	_sample_number_frame = frame_size;
	_frame_size = _sample_number_frame / sample_rate_in;
	_sample_number_delta = delta_size;
	_delta_size = _sample_number_delta / sample_rate_in;
	_sample_number_in = _sample_number_frame + _sample_number_delta;
	_sample_number_out = _transformer->getSampleNumberOut (_sample_number_frame);	

	// now add output buffer
	ssi_time_t sample_rate_out = (ssi_cast (double, _sample_number_out) / ssi_cast (double, _sample_number_frame)) * sample_rate_in;
	_buffer_id_out = _frame->AddBuffer (sample_rate_out, 
		sample_dimension_out, 
		sample_bytes_out,
		sample_type_out,
		buffer_capacity);
	meta = _transformer->getMetaData (meta_size);
	if (meta) {
		_frame->SetMetaData (_buffer_id_out, meta_size, meta);
	}

	// temporal arrays
	ssi_stream_init (_stream_in, 0, sample_dimension_in, sample_bytes_in, sample_type_in, sample_rate_in);
	ssi_stream_init (_stream_out, 0, sample_dimension_out, sample_bytes_out, sample_type_out, sample_rate_out);

	// init additional input streams
	if (_xtra_stream_num > 0) {
		_xtra_streams = new ssi_stream_t[_xtra_stream_num];
		ssi_size_t byte = 0, dim = 0;
		ssi_time_t sr = 0;
		ssi_type_t type = SSI_UNDEF;
		for (ssi_size_t i = 0; i < _xtra_stream_num; i++) {

			// figure out properties of extra input signals
			_frame->GetSampleBytes (_xtra_stream_ids[i], byte);
			_frame->GetSampleDimension (_xtra_stream_ids[i], dim);
			_frame->GetSampleType (_xtra_stream_ids[i], type);
			_frame->GetSampleRate (_xtra_stream_ids[i], sr);			

			// init streams
			ssi_stream_init (_xtra_streams[i], 0, dim, byte, type, sr);
		}
	}

	// add consumer to framework
	if (_frame->IsAutoRun ()) {
		_frame->AddRunnable (this);
	}
}

Transformer::~Transformer () {

	if (_xtra_stream_num > 0) {
		for (ssi_size_t i = 0; i < _xtra_stream_num; i++) {
			ssi_stream_destroy (_xtra_streams[i]);
		}		
	}
	delete[] _xtra_streams;
	delete[] _xtra_stream_ids;
}

void Transformer::enter () {

	if (_frame->IsInIdleMode ()) {
		_read_pos = 0;
	} else {
		_frame->GetCurrentWritePos (_buffer_id_in, _read_pos);
	}

	_transformer->transform_enter (_stream_in, _stream_out, _xtra_stream_num, _xtra_streams);

	ssi_stream_adjust (_stream_in, _sample_number_in);
	ssi_stream_adjust (_stream_out, _sample_number_out);

	ssi_msg (SSI_LOG_LEVEL_BASIC, "start '%s'", _transformer->getName ());
	if ( ssi_log_level >= SSI_LOG_LEVEL_BASIC) {
		ssi_print ("\
             frame\t= %.2lf\n\
             delta\t= %.2lf\n\
             id\t\t= %d -> %d\n\
             rate\t= %.2lf -> %.2lf\n\
             dim\t= %u -> %d\n\
             bytes\t= %u -> %d\n\
             type\t= %s -> %s\n",
			_frame_size,
			_delta_size,
			_buffer_id_in,
			_buffer_id_out,
			_stream_in.sr, 
			_stream_out.sr, 
			_stream_in.dim, 
			_stream_out.dim, 
			_stream_in.byte,
			_stream_out.byte,			
			SSI_TYPE_NAMES[_stream_in.type],
			SSI_TYPE_NAMES[_stream_out.type]);
	}
}


// TODO: clean up!
void Transformer::run () {

	// try to transform data
	// if an error occurs during receive operation
	// operation fails and error code is returned
	int status = transform ();

	// check if operation was successful
	// otherwise try to handle the error
	switch (status) {
		case TimeBuffer::SUCCESS:
			_read_pos += _sample_number_frame;
			break;
		case TimeBuffer::DATA_NOT_IN_BUFFER_YET:
			// data is not yet available
			// we return and hope that it will be available at next call..
			SSI_DBG (SSI_LOG_LEVEL_DEBUG, "data not in buffer yet '%s'", _transformer->getName ());
			return;
		case THEFRAMEWORK_ERROR:
			// framework error, probably framework is in idle mode
			// there is not much to do for us but wait..
			//SSI_DBG (SSI_LOG_LEVEL_DEBUG, "framework not running '%s'", _transformer->getName ());
			return;
		default:
			// well, something critical happend, probably the requested data is not available anymore
			// all we can do is to reset the timer and hope that we will succeed next time..
			ssi_wrn ("requested data not available (%s) '%s'", TimeBuffer::STATUS_NAMES[status], _transformer->getName ());
			_frame->GetCurrentWritePos (_buffer_id_in, _read_pos);
			_frame->SetCurrentSampleTime (_buffer_id_out, _read_pos / _stream_in.sr);
			return; 
	}	
}

int Transformer::transform () {

	int status;

	// get data from input buffer
	status = _frame->GetData (_buffer_id_in, _stream_in.ptr, _sample_number_in, _read_pos);

	// if receive operation failed return error code
	if (status != TimeBuffer::SUCCESS) {
		return status;
	}

	// get data from additional buffers
	for (ssi_size_t i = 0; i < _xtra_stream_num; i++) {
		ssi_time_t time = ssi_cast (ssi_time_t, _read_pos) / ssi_cast (ssi_time_t, _stream_in.sr);
		ssi_time_t duration = _frame_size + _delta_size;
		status = _frame->GetData (_xtra_stream_ids[i], _xtra_streams[i], time, duration);		
		if (status != TimeBuffer::SUCCESS) {
			return status;
		}
	}

	// transform data
	ITransformer::info tinfo;
	tinfo.delta_num = _sample_number_delta;
	tinfo.frame_num = _sample_number_frame;
	tinfo.time = _read_pos / _stream_in.sr;
	_transformer->transform (tinfo, _stream_in, _stream_out, _xtra_stream_num, _xtra_streams);
	
	// put result to output buffer
	_frame->PushData (_buffer_id_out, _stream_out.ptr, _sample_number_out);

	SSI_DBG (SSI_LOG_LEVEL_DEBUG, "transformed %u -> %u samples ('%s')", _sample_number_in, _sample_number_out, _transformer->getName ());

	return TimeBuffer::SUCCESS;
}

void Transformer::flush () {

	ssi_msg (SSI_LOG_LEVEL_BASIC, "stop '%s'", _transformer->getName ());

	ssi_stream_reset (_stream_in);
	ssi_stream_reset (_stream_out);
	for (ssi_size_t i = 0; i < _xtra_stream_num; i++) {	
		ssi_stream_reset (_xtra_streams[i]);
	}

	_transformer->transform_flush (_stream_in, _stream_out, _xtra_stream_num, _xtra_streams);
}



}

