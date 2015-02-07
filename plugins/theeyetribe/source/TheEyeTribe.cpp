// eyetribe.cpp
// author: Tobias Baur <baur@hcm-lab.de>
// created: 2013/02/28
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

#include <stdio.h>
#include <string.h>
#include "TheEyeTribe.h"


#ifdef USE_SSI_LEAK_DETECTOR
#include "SSI_LeakWatcher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


namespace ssi {


	int mGazeX = 0;
	int mGazeY = 0;

	ssi_char_t *TheEyeTribe::ssi_log_name = "TheEyeTribe___";

	TheEyeTribe::TheEyeTribe (const ssi_char_t *file) 
		:
		m_eyegaze_provider (0),
		m_timer (0),
		_file (0),
		ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

		
		if (file) {
			if (!OptionList::LoadXML (file, _options)) {
				OptionList::SaveXML (file, _options);
			}
			_file = ssi_strcpy (file);
		}
	}

	TheEyeTribe::~TheEyeTribe () {

		if (_file) {
			OptionList::SaveXML (_file, _options);
			delete[] _file;
		}
		m_api.remove_listener( *this );
        //m_api.disconnect();
	  	}

bool TheEyeTribe::setProvider (const ssi_char_t *name, IProvider *provider) {

		if (strcmp (name, SSI_THEEYETRIBE_EYEGAZE_PROVIDER_NAME) == 0) {
			setEyeGazeProvider (provider);
			return  true;
		}
		else ssi_wrn ("unkown provider name '%s'", name);

		return false;
	
	}


void TheEyeTribe::setEyeGazeProvider (IProvider *eyegaze_provider) {

		if (m_eyegaze_provider) {
			ssi_wrn ("eyegaze provider already set");
		}

		m_eyegaze_provider = eyegaze_provider;
		if (m_eyegaze_provider) {
			m_eyegaze_channel.stream.sr = _options.sr;
			m_eyegaze_provider->init (&m_eyegaze_channel);
			ssi_msg (SSI_LOG_LEVEL_DETAIL, "eyegaze provider set");
		}
	}




bool TheEyeTribe::connect () 
{
	// Connect to the server in push mode on the default TCP port (6555)
    if( m_api.connect( true ) )
    {
        // Enable GazeData notifications
        m_api.add_listener( *this );
    }
	return true;
}

void TheEyeTribe::run () {

		if (m_eyegaze_provider) 
		{

			m_eyegazes[0] = mGazeX;
			m_eyegazes[1] = mGazeY;
			m_eyegaze_provider->provide(ssi_pcast(ssi_byte_t, m_eyegazes),1);
		}


		if (!m_timer) 
		{
			m_timer = new Timer (1/_options.sr, true);
		}
					
		m_timer->wait ();
	}

bool TheEyeTribe::disconnect () {
	
	
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "try to disconnect sensor...");
	
		delete m_timer; m_timer = 0;
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "sensor disconnected");
		return true;
	}	
	


void TheEyeTribe::on_gaze_data( gtl::GazeData const & gaze_data )
{
    if( gaze_data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE )
    {
        gtl::Point2D const & smoothedCoordinates = gaze_data.avg;
		mGazeX = smoothedCoordinates.x;
		mGazeY = smoothedCoordinates.y;
    }
}




}

