// ZeroEventSender.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/26
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

#include "event/ZeroEventSender.h"
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

ssi_char_t *ZeroEventSender::ssi_log_name = "zeroesend_";

ZeroEventSender::ZeroEventSender (const ssi_char_t *file)
: _hangover_in (0),
	_hangover_out (0),
	_samples_max_dur (0),
	_counter_max_dur (0),
	_max_dur (0),
	_min_dur (0),
	_loffset (0),
	_uoffset (0),
	_skip_on_max_dur (false),
	_file (0),
	_check (0),
	_elistener (0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

	ssi_event_init (_event, SSI_ETYPE_EMPTY);
}

ZeroEventSender::~ZeroEventSender () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}

	ssi_event_destroy (_event);
}

void ZeroEventSender::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	_trigger_on = false;
	_hangover_in = _options.hangin;
	_hangover_out = _options.hangout;
	_counter_in = _hangover_in;
	_counter_out = _hangover_out;
	_min_dur = _options.mindur;
	_max_dur = _options.maxdur;
	_loffset = _options.loffset;
	_uoffset = _options.uoffset;
	_skip_on_max_dur = _options.skip;
	_samples_max_dur = ssi_cast (ssi_size_t, _max_dur * stream_in[0].sr);
	
	switch (stream_in[0].type) {
		case SSI_CHAR:
			_check = &check_char;
			break;
		case SSI_UCHAR:
			_check = &check_uchar;
			break;
		case SSI_SHORT:
			_check = &check_short;
			break;
		case SSI_USHORT:
			_check = &check_ushort;
			break;
		case SSI_INT:
			_check = &check_int;
			break;
		case SSI_UINT:
			_check = &check_uint;
			break;
		case SSI_LONG:
			_check = &check_long;
			break;
		case SSI_ULONG:
			_check = &check_ulong;
			break;
		case SSI_FLOAT:
			_check = &check_float;
			break;
		case SSI_DOUBLE:
			_check = &check_double;
			break;		
		default:
			ssi_err ("unsupported sample type");
	}
}

bool ZeroEventSender::setEventListener (IEventListener *listener) {

	_elistener = listener;
	_event.sender_id = Factory::AddString (_options.sname);
	if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
		return false;
	}
	_event.event_id = Factory::AddString (_options.ename);
	if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
		return false;
	}

	_event_address.setSender (_options.sname);
	_event_address.setEvents (_options.ename);

	return true;
}

void ZeroEventSender::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	ssi_byte_t *dataptr = stream_in[0].ptr;
	ssi_size_t dim = stream_in[0].dim; 

	bool found_event;
	for (ssi_size_t i = 0; i < stream_in[0].num; i++) {

		found_event = _options.hard;
		for (ssi_size_t j = 0; j < stream_in[0].dim; j++) {
			if (_options.hard) {
				found_event = found_event && _check (dataptr);
			} else {
				found_event = found_event || _check (dataptr);
			}
			dataptr += stream_in[0].byte;
		}	

		if (!_trigger_on) {
			
			if (found_event) {

				// possible start of a new event
				if (_counter_in == _hangover_in) {

					// store start time and init max dur counter
					_trigger_start = consume_info.time + i / stream_in[0].sr;					
				}
				// check if event start is proved
				if (_counter_in-- == 0) {
				
					// signal that event start is now proved and init hangout and max counter
					_trigger_on = true;					
					_counter_out = _hangover_out;
					_counter_max_dur = _samples_max_dur - _hangover_in;

					if (_elistener && _options.eager) {
						_event.time = ssi_cast (ssi_size_t, 1000 * _trigger_start + 0.5);
						_event.dur = 0;		
						_event.state = SSI_ESTATE_CONTINUED;
						_event.glue_id = Factory::GetUniqueId ();
						_elistener->update (_event);		
					}

					SSI_DBG (SSI_LOG_LEVEL_DEBUG, "event started at %.2lfs", _trigger_start);
				}
				
			} else {

				// re-init hangin counter
				_counter_in = _hangover_in;

			}
		} else if (_trigger_on) {

			// check if max dur is reached
			if (--_counter_max_dur == 0) {
				// send event and reset start/stop time amd max counter
				_trigger_stop = consume_info.time + i / stream_in[0].sr;

				IConsumer::info info;
				info.dur = _trigger_stop-_trigger_start;
				info.time = _trigger_start;
				info.status = IConsumer::CONTINUED;

				update_h (info);

				_trigger_start = _trigger_stop;
				_counter_max_dur = _samples_max_dur;
			}

			if (!found_event) {

				// possible end of a new event
				if (_counter_out == _hangover_out) {

					// store end time
					_trigger_stop = consume_info.time + i / stream_in[0].sr;

				} 
				// check if event start is proved
				if (_counter_out-- == 0) {

					// event end is now proved and event is sent
					IConsumer::info info;
					info.dur = _trigger_stop-_trigger_start;
					info.time = _trigger_start;
					info.status = IConsumer::COMPLETED;

					update_h (info);

					// signal end of event and init hangin counter
					_trigger_on = false;
					_counter_in = _hangover_in;

					SSI_DBG (SSI_LOG_LEVEL_DEBUG, "event stopped at %.2lfs", _trigger_stop);
				}
			} else {
				// re-init hangin counter
				_counter_out = _hangover_out;
			}
		}
	}
}

bool ZeroEventSender::update_h (IConsumer::info info) {
	
	if (info.dur < _min_dur || info.dur <= 0.0) {
		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "skip event because duration too short (%.2lf@%.2lf)", info.dur, info.time);
		return false;
	}	

	if (info.dur > _max_dur + 0.000000001) {
		if (_skip_on_max_dur) {
			SSI_DBG (SSI_LOG_LEVEL_DEBUG, "skip event because duration too long (%.2lf@%.2lf)", info.dur, info.time);
			return false;
		}
		ssi_wrn ("crop duration from %.2lfs to %.2lfs", info.dur, _max_dur);
		info.time += info.dur - _max_dur;		
		info.dur = _max_dur;		
	}

	if (_elistener && (_options.eall || info.status == IConsumer::COMPLETED)) {

		_event.time = max (0, ssi_cast (ssi_size_t, 1000 * (info.time - _loffset) + 0.5));
		_event.dur = max (0, ssi_cast (ssi_size_t, 1000 * (info.dur + _uoffset) + 0.5));

		if (info.status == IConsumer::COMPLETED) {
			_event.state = SSI_ESTATE_COMPLETED;
		} else {
			_event.state = SSI_ESTATE_CONTINUED;
			if (_event.glue_id == SSI_FACTORY_UNIQUE_INVALID_ID) {
				_event.glue_id = Factory::GetUniqueId ();
			}
		}
		
		_elistener->update (_event);		

		if (_event.state = SSI_ESTATE_COMPLETED) {
			_event.glue_id = SSI_FACTORY_UNIQUE_INVALID_ID;
		}
	}

	return true;	
}

void ZeroEventSender::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {
}

}
