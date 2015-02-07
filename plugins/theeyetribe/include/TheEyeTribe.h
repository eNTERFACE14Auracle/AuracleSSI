// eyetribe.h
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

// Provides eyetribe eyegaze tracking

#pragma once

#ifndef SSI_SENSOR_THEEYETRIBE_H
#define	SSI_SENSOR_THEEYETRIBE_H


#include "base/ISensor.h"
#include "base/IProvider.h"
#include "thread/Timer.h"
#include "thread/Thread.h"
#include "ioput/option/OptionList.h"
#include <gazeapi.h>
#include "event/EventAddress.h"

typedef struct _IplImage IplImage;

class CalibrationResultListener : public gtl::ICalibrationResultListener{
private:
    // ICalirationResultListener
    void on_calibration_changed(bool is_calibrated, gtl::CalibResult const & calib_result);
};

class TrackerStateListener : public gtl::ITrackerStateListener{
private:
    void on_tracker_connection_changed(int tracker_state);
    void on_screen_state_changed(gtl::Screen const & screen);
};

namespace ssi {

#define SSI_THEEYETRIBE_EYEGAZE_PROVIDER_NAME "EyeGaze"
/*#define SSI_THEEYETRIBE_TRACKER_PROVIDER_NAME "Tracker"
#define SSI_THEEYETRIBE_SCREEN_PROVIDER_NAME "Screen"
#define SSI_THEEYETRIBE_CALIBRATION_PROVIDER_NAME "Calibration"*/
//#define SSI_eyetribeRED_GAZEIMAGE_PROVIDER_NAME "GazeScene"
//#define SSI_eyetribeRED_EYESIMAGE_PROVIDER_NAME "EyesScene"

#define SSI_eyetribeRED_CHANNEL_NUM 1//3
#define SSI_eyetribeRED_EYEGAZE_CHANNEL_NUM 0
/*#define SSI_eyetribeRED_TRACKER_CHANNEL_NUM 1
#define SSI_eyetribeRED_SCREEN_CHANNEL_NUM 2
#define SSI_eyetribeRED_CALIBRATION_CHANNEL_NUM 3*/
//#define SSI_eyetribeRED_GAZEIMAGE_CHANNEL_NUM 1
//#define SSI_eyetribeRED_EYESIMAGE_CHANNEL_NUM 2

class TheEyeTribe : public ISensor, public Thread, public gtl::IGazeListener {

public:

    class EyeGazeChannel : public IChannel {

        friend class TheEyeTribe;

    public:
        EyeGazeChannel () {
            ssi_stream_init (stream, 0,14, sizeof (ssi_real_t), SSI_REAL, 0);
        }
        ~EyeGazeChannel () {
            ssi_stream_destroy (stream);
        }
        const ssi_char_t *getName () { return SSI_THEEYETRIBE_EYEGAZE_PROVIDER_NAME; }
        const ssi_char_t *getInfo () { return "Eyegaze"; }
        ssi_stream_t getStream () { return stream; }
    protected:
        ssi_stream_t stream;
    };

    /*class CalibrationChannel : public IChannel {

        friend class TheEyeTribe;

    public:
        CalibrationChannel () {
            ssi_stream_init (stream, 0,5+16*14, sizeof (ssi_size_t), SSI_REAL, 0);
        }
        ~CalibrationChannel () {
            ssi_stream_destroy (stream);
        }
        const ssi_char_t *getName () { return SSI_THEEYETRIBE_CALIBRATION_PROVIDER_NAME; }
        const ssi_char_t *getInfo () { return "Calibration"; }
        ssi_stream_t getStream () { return stream; }
    protected:
        ssi_stream_t stream;
    };

    class TrackerChannel : public IChannel {

        friend class TheEyeTribe;

    public:
        TrackerChannel () {
            ssi_stream_init (stream, 0,1, sizeof (ssi_size_t), SSI_INT, 0);
        }
        ~TrackerChannel () {
            ssi_stream_destroy (stream);
        }
        const ssi_char_t *getName () { return SSI_THEEYETRIBE_TRACKER_PROVIDER_NAME; }
        const ssi_char_t *getInfo () { return "Tracker"; }
        ssi_stream_t getStream () { return stream; }
    protected:
        ssi_stream_t stream;
    };

    class ScreenChannel : public IChannel {

        friend class TheEyeTribe;

    public:
        ScreenChannel () {
            ssi_stream_init (stream, 0,5, sizeof (ssi_size_t), SSI_REAL, 0);
        }
        ~ScreenChannel () {
            ssi_stream_destroy (stream);
        }
        const ssi_char_t *getName () { return SSI_THEEYETRIBE_SCREEN_PROVIDER_NAME; }
        const ssi_char_t *getInfo () { return "Screen"; }
        ssi_stream_t getStream () { return stream; }
    protected:
        ssi_stream_t stream;
    };*/

public:

    class Options : public OptionList {

    public:

        Options ()
            : sr (30)
        {
            setEventSender ("theeyetribe");
            setEventName ("calibration");

            addOption ("sr", &sr, 1, SSI_INT, "sample rate in hz");
            addOption ("sender_name", sender_name, SSI_MAX_CHAR, SSI_CHAR, "name of sender (if sent to event board)");
            addOption ("events_name", events_name, SSI_MAX_CHAR, SSI_CHAR, "name of event (if sent to event board)");
        }

        void setEventSender (const ssi_char_t *sender_name) {
            if (sender_name) {
                ssi_strcpy (this->sender_name, sender_name);
            }
        }

        void setEventName (const ssi_char_t *events_name) {
            if (events_name) {
                ssi_strcpy (this->events_name, events_name);
            }
        }

        ssi_char_t sender_name[SSI_MAX_CHAR];
        ssi_char_t events_name[SSI_MAX_CHAR];

        ssi_time_t sr;
    };

public:

    static const ssi_char_t *GetCreateName () { return "ssi_sensor_TheEyeTribe"; }
    static IObject *Create (const ssi_char_t *file) { return new TheEyeTribe (file); }
    ~TheEyeTribe ();
    TheEyeTribe::Options *getOptions () { return &_options; }
    const ssi_char_t *getName () { return GetCreateName (); }
    const ssi_char_t *getInfo () { return "eyetribe Red mobile Eyetracker"; }

    ssi_size_t getChannelSize () { return SSI_eyetribeRED_CHANNEL_NUM; }
    IChannel *getChannel (ssi_size_t index) {
        switch (index) {
        case SSI_eyetribeRED_EYEGAZE_CHANNEL_NUM:
            return &m_eyegaze_channel;
            break;
        /*case SSI_eyetribeRED_CALIBRATION_CHANNEL_NUM:
            return &m_calibration_channel;
            break;
        case SSI_eyetribeRED_TRACKER_CHANNEL_NUM:
            return &m_tracker_channel;
            break;
        case SSI_eyetribeRED_SCREEN_CHANNEL_NUM:
            return &m_screen_channel;
            break;*/
        return 0;
        }
    }
    bool setProvider (const ssi_char_t *name, IProvider *provider);
    bool connect ();
    bool start () { return Thread::start (); }
    bool stop () { return Thread::stop (); }
    void run ();
    bool disconnect ();

    void setLogLevel (int level) {
        ssi_log_level = level;
    }

    ssi_video_params_t getBWImageParams()
    {
        ssi_video_params_t vParam;
        ssi_video_params(vParam, 300, 240, _options.sr, SSI_VIDEO_DEPTH_8U, 3);
        vParam.flipImage = false;
        return vParam;
    }

    ssi_video_params_t getEyeImageParamsmobile()
    {
        ssi_video_params_t vParam;
        ssi_video_params(vParam, 496, 240, _options.sr, SSI_VIDEO_DEPTH_8U, 1);
        vParam.flipImage = false;
        return vParam;
    }

    ssi_video_params_t getEyeImageParams()
    {
        ssi_video_params_t vParam;
        ssi_video_params(vParam, 600, 200, _options.sr, SSI_VIDEO_DEPTH_8U, 1);
        vParam.flipImage = false;
        return vParam;
    }

    // Virtual functions inherited from IEventSender < IObject < ISensor
    //virtual void send_enter () {};
    virtual bool setEventListener (IEventListener *listener) /*{ return false; }*/;
    //virtual void send_flush () {};
    //virtual const ssi_char_t *getEventAddress () { return 0; };
    virtual const ssi_char_t *getEventAddress () {
        return _event_address.getAddress ();
    }

protected:

    // Event listenting
    ssi_event_t _event;
    IEventListener *_elistener;
    EventAddress _event_address;

    TheEyeTribe (const ssi_char_t *file = 0);
    TheEyeTribe::Options _options;
    ssi_char_t *_file;

    static ssi_char_t *ssi_log_name;
    int ssi_log_level;

    void processProvide ();

    Timer*                      m_timer;

    void on_gaze_data( gtl::GazeData const & gaze_data );
    gtl::GazeApi m_api;

    void setEyeGazeProvider (IProvider *eyegaze_provider);
    /*void setCalibrationProvider (IProvider *calibration_provider);
    void setTrackerProvider (IProvider *tracker_provider);
    void setScreenProvider (IProvider *screen_provider);*/
    EyeGazeChannel m_eyegaze_channel;
    /*CalibrationChannel m_calibration_channel;
    TrackerChannel m_tracker_channel;
    ScreenChannel m_screen_channel;*/
    IProvider *m_eyegaze_provider;
    /*IProvider *m_calibration_provider;
    IProvider *m_tracker_provider;
    IProvider *m_screen_provider;*/

    CalibrationResultListener ResultListener;
    TrackerStateListener TrackerListener;
};

}

#endif

