// EventMonitor.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/10/14
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

#include "event/EventMonitor.h"
#include "ioput/file/FileTools.h"
#include "event/TheEventBoard.h"
#include "ioput/window/Monitor.h"
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

ssi_char_t *EventMonitor::ssi_log_name = "emonitor__";

EventMonitor::EventMonitor (const ssi_char_t *file)
	: _file (0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT),
	_monitor (0),
	_update_counter (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

EventMonitor::~EventMonitor () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void EventMonitor::listen_enter () {

	if (!_options.console && !_monitor) {
		_monitor = new Monitor ("events", _options.mpos[0], _options.mpos[1], _options.mpos[2], _options.mpos[3], _options.chars);
		_monitor->start ();
	}
	_update_counter = 0;
}

bool EventMonitor::update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {

	if (_update_counter * _options.update_ms >= time_ms) {
		return true;
	}
	_update_counter++;
	
	if (_options.console) {

		ssi_event_t *e = 0;
		ssi_size_t i = 0;
		ssi_print ("time=%u\n#   %-10.9s%-10.9s%-10.9s%-10.9s%-10.9s%-10.9s%-10.9s%-11.10s\r\n-------------------------------------------------------------------------------\n", time_ms, "type", "sender", "event", "time", "dur", "size", "state(gid)");
		while (e = events.next ()) {
			ssi_print ("%03u %-10.9s%-10.9s%-10.9s%-9u %-9u %-9u %s(%02u)\r\n", i++, SSI_ETYPE_NAMES[e->type], Factory::GetString (e->sender_id), Factory::GetString (e->event_id), e->time, e->dur, e->tot, e->state == SSI_ESTATE_CONTINUED ? "continued" : "completed", e->glue_id);
			if (!_options.all && i >= n_new_events) {
				break;
			}
		}

	} else {

		_monitor->clear ();		

		ssi_event_t *e = 0;
		ssi_size_t i = 0;
		ssi_sprint (_string, "time=%u\r\n#   %-10.9s%-10.9s%-10.9s%-10.9s%-10.9s%-10.9s%-11.10s\r\n-------------------------------------------------------------------------------\r\n", time_ms, "type", "sender", "event", "time", "dur", "size", "state(gid)");		
		_monitor->print (_string);

		while (e = events.next ()) {
			ssi_sprint (_string, "%03u %-10.9s%-10.9s%-10.9s%-9u %-9u %-9u %s(%02u)\r\n", i++, SSI_ETYPE_NAMES[e->type], Factory::GetString (e->sender_id), Factory::GetString (e->event_id), e->time, e->dur, e->tot, e->state == SSI_ESTATE_CONTINUED ? "continued" : "completed", e->glue_id);
			_monitor->print (_string);
			if (_options.detail) {
				switch (e->type) {
					case SSI_ETYPE_EMPTY:
						_monitor->print ("    <empty>\r\n");
						break;
					case SSI_ETYPE_STRING: {

						ssi_char_t *estr = ssi_pcast (ssi_char_t, e->ptr);	
						if (strlen (estr) > 100) {
							ssi_char_t estr_short[101];
							memcpy (estr_short, estr, 100);
							estr_short[100] = '\0';
							ssi_sprint (_string, "    %s...\r\n", estr_short);
						} else {
							ssi_sprint (_string, "    %s\r\n", estr);
						}
						_monitor->print (_string);
						break;
					}
					case SSI_ETYPE_FLOATS: {
							ssi_size_t n = e->tot / sizeof (ssi_real_t);
							ssi_real_t *p = ssi_pcast (ssi_real_t, e->ptr);							
							_monitor->print ("    ");
							for (ssi_size_t i = 0; i < n; i++) {
								ssi_sprint (_string, "%.2f ", p[i]);
								_monitor->print (_string);
							}
							_monitor->print ("\r\n");
						}
						break;				
					case SSI_ETYPE_NTUPLE: {
							ssi_size_t n = e->tot / sizeof (ssi_event_tuple_t);
							ssi_event_tuple_t *p = ssi_pcast (ssi_event_tuple_t, e->ptr);
							_monitor->print ("    ");
							for (ssi_size_t i = 0; i < n; i++) {
								ssi_sprint (_string, "%s:%.2f ", Factory::GetString (p[i].id), p[i].value);								
								_monitor->print (_string);
							}
							_monitor->print ("\r\n");
						}
						break;	
					default:
						_monitor->print ("    <unkown event type>\r\n");						
				}				
			}
			if (!_options.all && i >= n_new_events) {
				break;
			}
		}
		_monitor->show ();
	}

	return true;
}

void EventMonitor::listen_flush () {

	if (_monitor) {
		_monitor->close ();
		_monitor->stop ();
		delete _monitor;
		_monitor = 0;
	}
}

}
