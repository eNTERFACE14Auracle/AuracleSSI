// eyetribe.cpp
// authors: Tobias Baur <baur@hcm-lab.de>, Alexis Rochette <alexis@sonixtrip.be>, Christian Frisson <christian.frisson@umons.ac.be>
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
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
#include "SSI_LeakWatcher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

gtl::GazeData mGazeData;
gtl::CalibResult mCalibResult;
bool mCalibChanged = false;
bool mTrackerConnectionChanged = false;
bool mScreenStateChanged = false;
int mTrackerState;
gtl::Screen mScreen;

namespace ssi {

ssi_char_t *TheEyeTribe::ssi_log_name = "TheEyeTribe___";

TheEyeTribe::TheEyeTribe (const ssi_char_t *file)
    :
      m_eyegaze_provider (0),
      /*m_calibration_provider (0),
            m_tracker_provider (0),
            m_screen_provider (0),*/
      m_timer (0),
      _file (0),
      _elistener(0),
      ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {


    if (file) {
        if (!OptionList::LoadXML (file, _options)) {
            OptionList::SaveXML (file, _options);
        }
        _file = ssi_strcpy (file);
    }

    ssi_event_init (_event, SSI_ETYPE_NTUPLE);
}

TheEyeTribe::~TheEyeTribe () {

    if (_file) {
        OptionList::SaveXML (_file, _options);
        delete[] _file;
    }
    m_api.remove_listener( *this );
    m_api.remove_listener((*this).ResultListener);
    m_api.remove_listener((*this).TrackerListener);
    //m_api.disconnect();

    ssi_event_destroy (_event);
}

bool TheEyeTribe::setProvider (const ssi_char_t *name, IProvider *provider) {

    if (strcmp (name, SSI_THEEYETRIBE_EYEGAZE_PROVIDER_NAME) == 0) {
        setEyeGazeProvider (provider);
        return  true;
    }
    /*else if (strcmp (name, SSI_THEEYETRIBE_CALIBRATION_PROVIDER_NAME) == 0) {
        setCalibrationProvider (provider);
        return  true;
    }
    else if (strcmp (name, SSI_THEEYETRIBE_TRACKER_PROVIDER_NAME) == 0) {
        setTrackerProvider (provider);
        return  true;
    }
    else if (strcmp (name, SSI_THEEYETRIBE_SCREEN_PROVIDER_NAME) == 0) {
        setScreenProvider (provider);
        return  true;
    }*/
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

/*void TheEyeTribe::setCalibrationProvider (IProvider *calibration_provider) {

    if (m_calibration_provider) {
        ssi_wrn ("calibration provider already set");
    }

    m_calibration_provider = calibration_provider;
    if (m_calibration_provider) {
        //m_eyegaze_channel.stream.sr = _options.sr;
        m_calibration_provider->init (&m_calibration_channel);
        ssi_msg (SSI_LOG_LEVEL_DETAIL, "calibration provider set");
    }
}

void TheEyeTribe::setTrackerProvider (IProvider *tracker_provider) {

    if (m_tracker_provider) {
        ssi_wrn ("tracker provider already set");
    }

    m_tracker_provider = tracker_provider;
    if (m_tracker_provider) {
        //m_eyegaze_channel.stream.sr = _options.sr;
        m_tracker_provider->init (&m_tracker_channel);
        ssi_msg (SSI_LOG_LEVEL_DETAIL, "tracker provider set");
    }
}

void TheEyeTribe::setScreenProvider (IProvider *screen_provider) {

    if (m_screen_provider) {
        ssi_wrn ("screen provider already set");
    }

    m_screen_provider = screen_provider;
    if (m_screen_provider) {
        //m_eyegaze_channel.stream.sr = _options.sr;
        m_screen_provider->init (&m_screen_channel);
        ssi_msg (SSI_LOG_LEVEL_DETAIL, "screen provider set");
    }
}*/

bool TheEyeTribe::connect () 
{
    // Connect to the server in push mode on the default TCP port (6555)
    if( m_api.connect( true ) )
    {
        // Enable GazeData notifications
        m_api.add_listener( *this );
        m_api.add_listener((*this).ResultListener);
        m_api.add_listener((*this).TrackerListener);
    }
    return true;
}

void TheEyeTribe::run () {

    if (m_eyegaze_provider)
    {

		ITheFramework *frame_work = Factory::GetFramework ();
        ssi_time_t _time_stamp = frame_work->GetStartTimeMs() + frame_work->GetElapsedTimeMs();

        std::vector<ssi_real_t> _eyegazes; /// gazeapi data in float otherwise noted
        _eyegazes.push_back( (float)(mGazeData.fix) ); ///< is fixated? (bool) // CF: move to another provider?

//        enum
//        {
//            GD_STATE_TRACKING_GAZE        = 1 << 0,
//            GD_STATE_TRACKING_EYES        = 1 << 1,
//            GD_STATE_TRACKING_PRESENCE    = 1 << 2,
//            GD_STATE_TRACKING_FAIL        = 1 << 3,
//            GD_STATE_TRACKING_LOST        = 1 << 4
//        };
        _eyegazes.push_back( (float)(mGazeData.state) ); ///< 32bit masked state integer

        _eyegazes.push_back( mGazeData.raw.x ); ///< raw gaze x coordinate in pixels
        _eyegazes.push_back( mGazeData.raw.y ); ///< raw gaze y coordinate in pixels
        _eyegazes.push_back( mGazeData.avg.x ); ///< smoothed x coordinate in pix
        _eyegazes.push_back( mGazeData.avg.y ); ///< smoothed y coordinate in pix
        _eyegazes.push_back( mGazeData.lefteye.psize ); ///< left eye pupil size
        _eyegazes.push_back( mGazeData.righteye.psize ); ///< right eye pupil size
        _eyegazes.push_back( mGazeData.lefteye.pcenter.x ); ///< left eye pupil x coordinate normalized
        _eyegazes.push_back( mGazeData.lefteye.pcenter.y ); ///< left eye pupil y coordinate normalized
        _eyegazes.push_back( mGazeData.righteye.pcenter.x ); ///< right eye pupil x coordinate normalized
        _eyegazes.push_back( mGazeData.righteye.pcenter.y ); ///< right eye pupil y coordinate normalized
        _eyegazes.push_back( (float)(mGazeData.time) ); ///< timestamp (int) // CF: move to another provider?
		_eyegazes.push_back( _time_stamp );
        m_eyegaze_provider->provide(ssi_pcast(ssi_byte_t, &_eyegazes[0]),1);
        //ssi_wrn ("mGazeData.lefteye.psize '%f'", mGazeData.lefteye.psize);
        //ssi_wrn ("mGazeData.avg.x '%f' mGazeData.avg.y '%f'", mGazeData.avg.x,mGazeData.avg.y);
    }

    if(/*m_calibration_provider && */mCalibChanged)
    {
        /*std::vector<ssi_real_t> _calibration; /// gazeapi data in float otherwise noted
        _calibration.push_back( (float)(mCalibResult.result) ); ///< was the calibration successful? (bool)
        _calibration.push_back( mCalibResult.deg );  ///< average error in degrees
        _calibration.push_back( mCalibResult.degl ); ///< average error in degs, left eye
        _calibration.push_back( mCalibResult.degr );  ///< average error in degs, right eye
        _calibration.push_back( (float)(mCalibResult.calibpoints.size()) ); ///< number of calibration points
        //5
        std::vector<gtl::CalibPoint> calibpoints = mCalibResult.calibpoints;
        for (std::vector<gtl::CalibPoint>::iterator calibpoint = calibpoints.begin(); calibpoint != calibpoints.end(); calibpoint++){
            gtl::CalibPoint cp = *calibpoint;
            _calibration.push_back( (float)(cp.state) ); ///< state of calibration point
            _calibration.push_back( cp.cp.x ); ///< x coordinate in pixels
            _calibration.push_back( cp.cp.y ); ///< y coordinate in pixels
            _calibration.push_back( cp.mecp.x ); ///< mean estimated x coord in pixels
            _calibration.push_back( cp.mecp.y ); ///< mean estimated y coord in pixels
            _calibration.push_back( cp.acd.ad ); ///< global accuracy in degrees
            _calibration.push_back( cp.acd.adl ); ///< left eye accuracy in degrees
            _calibration.push_back( cp.acd.adr ); ///< right eye accuracy in degrees
            _calibration.push_back( cp.mepix.mep ); ///< global mean error in pixels
            _calibration.push_back( cp.mepix.mepl ); ///< left eye mean error in pixels
            _calibration.push_back( cp.mepix.mepr ); ///< right eye mean error in pixels
            _calibration.push_back( cp.asdp.asd ); ///< global average std deviation in pixels
            _calibration.push_back( cp.asdp.asdl ); ///< left eye average std deviation in pixels
            _calibration.push_back( cp.asdp.asdr ); ///< right eye average std deviation in pixels
            //14
        }
        m_calibration_provider->provide(ssi_pcast(ssi_byte_t, &_calibration[0]),_calibration.size());*/

        // Get the timestamp of the current frame
        ssi_time_t _time_stamp = m_eyegaze_channel.getStream().time;

        ssi_size_t _event_size = 5+mCalibResult.calibpoints.size()*14;
        ssi_event_adjust(_event, _event_size * sizeof(ssi_event_tuple_t));

        ssi_event_tuple_t * event_tuple = ssi_pcast (ssi_event_tuple_t, _event.ptr);

        for (ssi_size_t i = 0; i < _event_size; i++) {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "%u", i);
            event_tuple[i].id = Factory::AddString(str);
        }

        ssi_size_t i = 0;

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "success");
            event_tuple[i].id = Factory::AddString(str);
        }
		event_tuple[i++].value = (float)(mCalibResult.result); ///< was the calibration successful? (bool)

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "avg_error_deg");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  mCalibResult.deg;  ///< average error in degrees

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "avg_error_left_deg");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  mCalibResult.degl; ///< average error in degs, left eye

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "avg_error_right_deg");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  mCalibResult.degr;  ///< average error in degs, right eye

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "calib_points");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  (float)(mCalibResult.calibpoints.size()); ///< number of calibration points
        //5

        ssi_size_t p = 0;
        std::vector<gtl::CalibPoint> calibpoints = mCalibResult.calibpoints;
        for (std::vector<gtl::CalibPoint>::iterator calibpoint = calibpoints.begin(); calibpoint != calibpoints.end(); calibpoint++){
            gtl::CalibPoint cp = *calibpoint;

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_state",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  (float)(cp.state); ///< state of calibration point

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_raw_x",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.cp.x; ///< x coordinate in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_raw_y",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.cp.y; ///< y coordinate in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_avg_x",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.mecp.x; ///< mean estimated x coord in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_avg_y",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.mecp.y; ///< mean estimated y coord in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_acc_deg",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.acd.ad; ///< global accuracy in degrees

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_acc_left_deg",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.acd.adl; ///< left eye accuracy in degrees

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_acc_right_deg",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.acd.adr; ///< right eye accuracy in degrees

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_mean_error_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.mepix.mep; ///< global mean error in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_mean_error_left_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.mepix.mepl; ///< left eye mean error in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_mean_error_right_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.mepix.mepr; ///< right eye mean error in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_stddev_error_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.asdp.asd; ///< global average std deviation in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_stddev_error_left_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.asdp.asdl; ///< left eye average std deviation in pixels

            {
                ssi_char_t str[SSI_MAX_CHAR];
                ssi_sprint (str, "point_%u_stddev_error_right_px",p);
                event_tuple[i].id = Factory::AddString(str);
            }
            event_tuple[i++].value =  cp.asdp.asdr; ///< right eye average std deviation in pixels

            //14
            p++;
        }

        // Create an adequate event structure
        // and add the right data to the event
        _event.time = ssi_cast (ssi_size_t, 1000 * _time_stamp + 0.5); //CF double-check the need of the extra time scaling
        _event.dur = ssi_cast (ssi_size_t, 0);
        _event.prob = ssi_cast (ssi_real_t, 1.0);
        _event.state = SSI_ESTATE_CONTINUED;
        _event.type = SSI_ETYPE_NTUPLE;

        // And add the event to the event pool
        _elistener->update (_event);

        mCalibChanged = false;
    }

    if(/*m_tracker_provider &&*/ mTrackerConnectionChanged){
        // mTrackerState
        /*
        //        std::string _state("unknown");
        //        switch (mTrackerState)
        //        {
        //		case gtl::ServerState::TRACKER_CONNECTED: _state = "connected"; break;
        //        case gtl::ServerState::TRACKER_NOT_CONNECTED: _state = "not connected"; break;
        //        case gtl::ServerState::TRACKER_CONNECTED_BADFW: _state = "bad fw"; break;
        //        case gtl::ServerState::TRACKER_CONNECTED_NOUSB3: _state = "no USB3"; break;
        //        case gtl::ServerState::TRACKER_CONNECTED_NOSTREAM: _state = "no stream"; break;
        //        }
        //        m_tracker_provider->provide(ssi_pcast(ssi_byte_t, _state),1);
        m_tracker_provider->provide(ssi_pcast(ssi_byte_t, mTrackerState),1);*/

        // Get the timestamp of the current frame
		ssi_time_t _time_stamp = m_eyegaze_channel.getStream().time;

        ssi_size_t _event_size = 1;
        ssi_event_adjust(_event, _event_size * sizeof(ssi_event_tuple_t));

		ssi_event_tuple_t * event_tuple = ssi_pcast (ssi_event_tuple_t, _event.ptr);

        for (ssi_size_t i = 0; i < _event_size; i++) {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "%u", i);
            event_tuple[i].id = Factory::AddString(str);
        }

        ssi_size_t i = 0;

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "tracker_state");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value = (float)(mTrackerState); ///< was the calibration successful? (bool)

        // Create an adequate event structure
        // and add the right data to the event
        _event.time = ssi_cast (ssi_size_t, 1000 * _time_stamp + 0.5); //CF double-check the need of the extra time scaling
        _event.dur = ssi_cast (ssi_size_t, 0);
        _event.prob = ssi_cast (ssi_real_t, 1.0);
        _event.state = SSI_ESTATE_CONTINUED;
        _event.type = SSI_ETYPE_NTUPLE;

        // And add the event to the event pool
        _elistener->update (_event);

        mTrackerConnectionChanged = false;
    }

    if(/*m_screen_provider &&*/ mScreenStateChanged){
        // mScreen
        /*std::vector<ssi_real_t> _screen; /// gazeapi data in float otherwise noted
        _screen.push_back( (float)(mScreen.screenindex) ); ///< Screen index (int)
        _screen.push_back( (float)(mScreen.screenresw) ); ///< Screen resolution width in pixels (int)
        _screen.push_back( (float)(mScreen.screenresh) ); ///< Screen resolution height in pixels (int)
        _screen.push_back( mScreen.screenpsyw ); ///< Screen physical width in meters
        _screen.push_back( mScreen.screenpsyh ); ///< Screen physical height in meters
        m_screen_provider->provide(ssi_pcast(ssi_byte_t, &_screen[0]),1);*/

        // Get the timestamp of the current frame
		ssi_time_t _time_stamp = m_eyegaze_channel.getStream().time;

        ssi_size_t _event_size = 5;
        ssi_event_adjust(_event, _event_size * sizeof(ssi_event_tuple_t));

		ssi_event_tuple_t * event_tuple = ssi_pcast (ssi_event_tuple_t, _event.ptr);

        for (ssi_size_t i = 0; i < _event_size; i++) {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "%u", i);
            event_tuple[i].id = Factory::AddString(str);
        }

        ssi_size_t i = 0;

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "screen_index");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  (float)(mScreen.screenindex); ///< Screen index (int)

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "screen_width_px");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  (float)(mScreen.screenresw); ///< Screen resolution width in pixels (int)

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "screen_height_px");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  (float)(mScreen.screenresh); ///< Screen resolution height in pixels (int)

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "screen_width_m");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  mScreen.screenpsyw; ///< Screen physical width in meters

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "screen_height_m");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value =  mScreen.screenpsyh; ///< Screen physical height in meters

        // Create an adequate event structure
        // and add the right data to the event
        _event.time = ssi_cast (ssi_size_t, 1000 * _time_stamp + 0.5); //CF double-check the need of the extra time scaling
        _event.dur = ssi_cast (ssi_size_t, 0);
        _event.prob = ssi_cast (ssi_real_t, 1.0);
        _event.state = SSI_ESTATE_CONTINUED;
        _event.type = SSI_ETYPE_NTUPLE;

        // And add the event to the event pool
        _elistener->update (_event);

        mScreenStateChanged = false;
    }

    if (!m_timer)
    {
        m_timer = new Timer (1/_options.sr, true);
    }

    m_timer->wait();
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
        mGazeData = gaze_data;
    }
}

bool TheEyeTribe::setEventListener (IEventListener *listener) {
    _elistener = listener;
    _event.sender_id = Factory::AddString (_options.sender_name);
    if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
        return false;
    }
    _event.event_id = Factory::AddString (_options.events_name);
    if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
        return false;
    }
    _event_address.setSender (_options.sender_name);
    _event_address.setEvents (_options.events_name);
    _event.prob = 1.0;
    return true;
}


}

void CalibrationResultListener::on_calibration_changed(bool is_calibrated, gtl::CalibResult const & calib_result)
{
    mCalibResult = calib_result;
    /*for (int i = 0; i < calib_result.calibpoints.size(); i++){
    }*/
    mCalibChanged = true;
}

void TrackerStateListener::on_tracker_connection_changed(int tracker_state)
{
    int mTrackerState = tracker_state;
    mTrackerConnectionChanged = true;
}

void TrackerStateListener::on_screen_state_changed(gtl::Screen const & screen)
{
    mScreen = screen;
    mScreenStateChanged = true;
}


