// ZeroEventSender.h
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

#ifndef SSI_EVENT_ZEROEVENTSENDER_H
#define SSI_EVENT_ZEROEVENTSENDER_H

#include "base/IConsumer.h"
#include "ioput/option/OptionList.h"
#include "event/EventAddress.h"

namespace ssi {

class ZeroEventSender : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options ()
			: hangin (0), hangout (0), mindur (0), maxdur (5.0), loffset (0), uoffset (0), skip (false), hard (false), eall (false), eager (false) {

				setSender ("zsender");
				setEvent ("zevent");							

				addOption ("hard", &hard, 1, SSI_BOOL, "consider all dimensions");
				addOption ("hangin", &hangin, 1, SSI_SIZE, "# of non-zero values before onset is detected");		
				addOption ("hangout", &hangout, 1, SSI_SIZE, "# of non-zero values before offset is detected");	
				addOption ("mindur", &mindur, 1, SSI_TIME, "minimum duration in seconds");	
				addOption ("maxdur", &maxdur, 1, SSI_TIME, "maximum duration in seconds");	
				addOption ("loffset", &loffset, 1, SSI_TIME, "lower offset in seconds (will be substracted from event start time)");	
				addOption ("uoffset", &uoffset, 1, SSI_TIME, "upper offset in seconds (will be added to event end time)");	
				addOption ("skip", &skip, 1, SSI_BOOL, "skip if max duration is exceeded");
				addOption ("sname", sname, SSI_MAX_CHAR, SSI_CHAR, "name of sender (if sent to event board)");
				addOption ("ename", ename, SSI_MAX_CHAR, SSI_CHAR, "name of event (if sent to event board)");	
				addOption ("eall", &eall, 1, SSI_BOOL, "forward incomplete events to event board, otherwise only complete events are sent");	
				addOption ("eager", &eager, 1, SSI_BOOL, "start event is sent as soon as possible event is triggered");				
		};

		void setSender (const ssi_char_t *sname) {			
			if (sname) {
				ssi_strcpy (this->sname, sname);
			}
		}
		void setEvent (const ssi_char_t *ename) {
			if (ename) {
				ssi_strcpy (this->ename, ename);
			}
		}

		ssi_size_t hangin;
		ssi_size_t hangout;
		ssi_time_t mindur;
		ssi_time_t maxdur;
		ssi_time_t uoffset;
		ssi_time_t loffset;
		bool skip;
		bool hard;
		ssi_char_t sname[SSI_MAX_CHAR];
		ssi_char_t ename[SSI_MAX_CHAR];	
		bool eall;
		bool eager;
	};

public:

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_ZeroEventSender"; };
	static IObject *Create (const ssi_char_t *file) { return new ZeroEventSender (file); };
	~ZeroEventSender ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Detects events by looking for non-zero samples."; };

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

	bool setEventListener (IEventListener *listener);
	const ssi_char_t *getEventAddress () {
		return _event_address.getAddress ();
	}

protected:

	ZeroEventSender (const ssi_char_t *file = 0);
	ZeroEventSender::Options _options;
	ssi_char_t *_file;

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;

	typedef bool (*check_fptr)(ssi_byte_t *);
	check_fptr _check;

	bool _trigger_on;
	ssi_time_t _trigger_start, _trigger_stop;
	ssi_size_t _hangover_in, _hangover_out, _counter_in, _counter_out;
	ssi_size_t _samples_max_dur, _counter_max_dur;
	ssi_time_t _min_dur, _max_dur;
	ssi_time_t _loffset, _uoffset;
	bool _skip_on_max_dur;

	IEventListener *_elistener;
	ssi_event_t _event;
	EventAddress _event_address;

	bool update_h (IConsumer::info info);

	static bool check_char (ssi_byte_t *ptr) {
		return *ssi_pcast (char, ptr) != 0;
	}
	static bool check_uchar (ssi_byte_t *ptr) {
		return *ssi_pcast (unsigned char, ptr) != 0;
	}
	static bool check_short (ssi_byte_t *ptr) {
		return *ssi_pcast (short, ptr) != 0;
	}
	static bool check_ushort (ssi_byte_t *ptr) {
		return *ssi_pcast (unsigned short, ptr) != 0;
	}
	static bool check_int (ssi_byte_t *ptr) {
		return *ssi_pcast (int, ptr) != 0;
	}
	static bool check_uint (ssi_byte_t *ptr) {
		return *ssi_pcast (unsigned int, ptr) != 0;
	}
	static bool check_long (ssi_byte_t *ptr) {
		return *ssi_pcast (long, ptr) != 0;
	}
	static bool check_ulong (ssi_byte_t *ptr) {
		return *ssi_pcast (unsigned long, ptr) != 0;
	}
	static bool check_float (ssi_byte_t *ptr) {
		return *ssi_pcast (float, ptr) != 0;
	}
	static bool check_double (ssi_byte_t *ptr) {
		return *ssi_pcast (double, ptr) != 0;
	}
};

}

#endif
