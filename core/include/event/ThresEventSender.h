// ThresEventSender.h
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

#ifndef SSI_EVENT_THRESHEVENTSENDER_H
#define SSI_EVENT_THRESHEVENTSENDER_H

#include "base/IConsumer.h"
#include "ioput/option/OptionList.h"
#include "event/EventAddress.h"
#include "thread/Lock.h"

namespace ssi {

class ThresEventSender : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options ()
			: hangin(0), hangout(0), mindur(0), maxdur(5.0), loffset(0), uoffset(0), skip(false), thres(0), thresout(0), hard(false), eall(false), eager(false), use_thresout(false) {

				setSender ("tsender");
				setEvent ("tevent");
				setThresId ("thres");
				setThresOutId ("thresout");

				addOption ("thres", &thres, 1, SSI_DOUBLE, "threshold to trigger into event");
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
				addOption ("thresid", thresid, SSI_MAX_CHAR, SSI_CHAR, "name of threshold tuple value");	
				addOption ("thresoutid", thresid, SSI_MAX_CHAR, SSI_CHAR, "name of threshold (out) tuple value");	
				addOption ("eall", &eall, 1, SSI_BOOL, "forward incomplete events to event board, otherwise only complete events are sent");	
				addOption ("eager", &eager, 1, SSI_BOOL, "send an event when the observation begins");
				addOption ("usethresout", &use_thresout, 1, SSI_BOOL, "use different threshold to trigger out of event");
				addOption ("thresout", &thresout, 1, SSI_DOUBLE, "threshold to trigger out of event");
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
		void setThresId (const ssi_char_t *thresid) {
			if (thresid) {
				ssi_strcpy (this->thresid, thresid);
			}
		}
		void setThresOutId (const ssi_char_t *thresid) {
			if (thresid) {
				ssi_strcpy (this->thresoutid, thresid);
			}
		}

		ssi_size_t hangin;
		ssi_size_t hangout;
		ssi_time_t mindur;
		ssi_time_t maxdur;
		ssi_time_t uoffset;
		ssi_time_t loffset;
		bool skip;
		double thres, thresout;
		bool use_thresout;
		bool hard;
		ssi_char_t sname[SSI_MAX_CHAR];
		ssi_char_t ename[SSI_MAX_CHAR];	
		ssi_char_t thresid[SSI_MAX_CHAR];	
		ssi_char_t thresoutid[SSI_MAX_CHAR];	
		bool eall;
		bool eager;
	};

public:

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_ThresEventSender"; };
	static IObject *Create (const ssi_char_t *file) { return new ThresEventSender (file); };
	~ThresEventSender ();

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Detects events if samples exceed a certain threshold."; };

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

	void listen_enter ();
	bool update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms);
	void listen_flush ();

protected:

	ThresEventSender (const ssi_char_t *file = 0);
	ThresEventSender::Options _options;
	ssi_char_t *_file;

	typedef bool (*check_fptr)(void *, void *);
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
	ssi_size_t _thres_string_id;
	ssi_size_t _thresout_string_id;

	bool update_h (IConsumer::info info);

	void *_thresin;
	void *_thresout;

	Mutex _thres_mutex;
	
	void set_thresin (double thresin, ssi_type_t type);
	void set_thresout (double thresout, ssi_type_t type);

	static bool check_char (void *ptr, void *thres) {
		return *ssi_pcast (char, ptr) > *ssi_pcast (char, thres);
	}
	static bool check_uchar (void *ptr, void *thres) {
		return *ssi_pcast (unsigned char, ptr) > *ssi_pcast (unsigned char, thres);
	}
	static bool check_short (void *ptr, void *thres) {
		return *ssi_pcast (short, ptr) > *ssi_pcast (short, thres);
	}
	static bool check_ushort (void *ptr, void *thres) {
		return *ssi_pcast (unsigned short, ptr) > *ssi_pcast (unsigned short, thres);
	}
	static bool check_int (void *ptr, void *thres) {
		return *ssi_pcast (int, ptr) > *ssi_pcast (int, thres);
	}
	static bool check_uint (void *ptr, void *thres) {
		return *ssi_pcast (unsigned int, ptr) > *ssi_pcast (unsigned int, thres);
	}
	static bool check_long (void *ptr, void *thres) {
		return *ssi_pcast (long, ptr) > *ssi_pcast (long, thres);
	}
	static bool check_ulong (void *ptr, void *thres) {
		return *ssi_pcast (unsigned long, ptr) > *ssi_pcast (unsigned long, thres);
	}
	static bool check_float (void *ptr, void *thres) {
		return *ssi_pcast (float, ptr) > *ssi_pcast (float, thres);
	}
	static bool check_double (void *ptr, void *thres) {
		return *ssi_pcast (double, ptr) > *ssi_pcast (double, thres);
	}
};

}

#endif
