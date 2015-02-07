// FFMPEGReader.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2014/04/15
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

#include "FFMPEGReader.h"
#include "FFMPEGReaderClient.h"
#include "base/Factory.h"

namespace ssi {

ssi_char_t *FFMPEGReader::ssi_log_name = "ffread____";

FFMPEGReader::FFMPEGReader (const ssi_char_t *file)
	: _file (0),
	_video_buffer (0),
	_audio_buffer (0),
	_video_provider (0),
    _videoframestamp_provider (0),
	_audio_provider(0),
	_audio_sr (0),
	_mode (MODE::UNDEFINED),
	_client(0),
    _timer (0),
    _elistener(0)
{

	ssi_log_level = SSI_LOG_LEVEL_DEFAULT;

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
	
    _client = new FFMPEGReaderClient (this);

    ssi_event_init (_event, SSI_ETYPE_NTUPLE);
};

FFMPEGReader::~FFMPEGReader () {

	delete _video_buffer; _video_buffer = 0;
	delete _audio_buffer; _audio_buffer = 0;

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}

    ssi_event_destroy (_event);
};

bool FFMPEGReader::setProvider (const ssi_char_t *name, IProvider *provider) {
	if(strcmp (name,SSI_FFMPEGREADER_VIDEO_PROVIDER_NAME)==0)	{
		setVideoProvider(provider);		
		return true;
	}
    if(strcmp (name,SSI_FFMPEGREADER_VIDEOFRAMESTAMP_PROVIDER_NAME)==0)	{
        setVideoFrameStampProvider(provider);
        return true;
    }
	if(strcmp(name,SSI_FFMPEGREADER_AUDIO_PROVIDER_NAME)==0){
		setAudioProvider(provider);		
		return true;
	}
	ssi_wrn ("unkown provider name '%s'", name);
	return false;
};

void FFMPEGReader::setVideoProvider(IProvider *provider){

	if(_video_provider){
		ssi_wrn ("provider already set");
		return;
	}

	_video_provider = provider;	
	_mode = _mode == MODE::UNDEFINED ? MODE::VIDEO : MODE::AUDIOVIDEO;	

	if (_options.stream) {
		ssi_video_params (_video_format, _options.width, _options.height, _options.fps, 8, 3);
	} else {
		if (!_client->peekVideoFormat (_video_format)) {
			ssi_wrn ("could not determine video format, use default options");
			ssi_video_params (_video_format, _options.width, _options.height, _options.fps, 8, 3);
		}
	}
	
	_video_provider->setMetaData (sizeof (ssi_video_params_t), &_video_format);
	ssi_stream_init (_video_channel.stream, 0, 1, ssi_video_size (_video_format), SSI_IMAGE, _video_format.framesPerSecond);
	_video_provider->init (&_video_channel);	
};

void FFMPEGReader::setVideoFrameStampProvider(IProvider *provider){

    if(_videoframestamp_provider){
        ssi_wrn ("provider already set");
        return;
    }

    _videoframestamp_provider = provider;
    /*_mode = _mode == MODE::UNDEFINED ? MODE::VIDEO : MODE::AUDIOVIDEO;

    if (_options.stream) {
        ssi_video_params (_video_format, _options.width, _options.height, _options.fps, 8, 3);
    } else {
        if (!_client->peekVideoFormat (_video_format)) {
            ssi_wrn ("could not determine video format, use default options");
            ssi_video_params (_video_format, _options.width, _options.height, _options.fps, 8, 3);
        }
    }

    _video_provider->setMetaData (sizeof (ssi_video_params_t), &_video_format);
    ssi_stream_init (_video_channel.stream, 0, 1, ssi_video_size (_video_format), SSI_IMAGE, _video_format.framesPerSecond);
    _video_provider->init (&_video_channel);*/

    if (_videoframestamp_provider) {
        _videoframestamp_channel.stream.sr = _options.fps;
        _videoframestamp_provider->init (&_videoframestamp_channel);
        ssi_msg (SSI_LOG_LEVEL_DETAIL, "video frame stamp provider set");
    }
};

void FFMPEGReader::setAudioProvider(IProvider *provider){

	if (_audio_provider){
		ssi_wrn ("provider already set");
		return;
	}
	
	_audio_provider = provider;
	_mode = _mode == MODE::UNDEFINED ? MODE::AUDIO : MODE::AUDIOVIDEO;	

	if (_options.stream) {
		_audio_sr = _options.asr;
	} else {
		if (!_client->peekAudioFormat (_audio_sr)) {
			ssi_wrn ("could not determine audio format, use default options");
			_audio_sr = _options.asr;
		}
	}
	
	ssi_stream_init (_audio_channel.stream, 0, 1, sizeof(ssi_real_t), SSI_FLOAT, _audio_sr);
	_audio_provider->init(&_audio_channel);

};


bool FFMPEGReader::connect () {
		
	if (!(_video_provider||_audio_provider)) {
		ssi_wrn ("provider not set");
		return false;
	};

	FFMPEGReaderClient::SetLogLevel (ssi_log_level);

	if (_mode != MODE::AUDIO) {
		_video_buffer = new FFMPEGVideoBuffer (_options.buffer, _video_format.framesPerSecond, ssi_video_size (_video_format));
		_video_buffer->reset ();
	}

	if (_mode != MODE::VIDEO) {
		if (_mode == MODE::AUDIOVIDEO) {
			_audio_buffer = new FFMPEGAudioBuffer (_options.buffer, _audio_channel.stream.sr, 1.0/_video_format.framesPerSecond);
		} else {
			_audio_buffer = new FFMPEGAudioBuffer (_options.buffer, _audio_channel.stream.sr, _options.ablock);
		}
		_audio_buffer->reset ();
	}

	_client->start ();

	return true;
};

void FFMPEGReader::enter () {

	ssi_char_t str[SSI_MAX_CHAR];
	ssi_sprint (str, "FFMPEGReader@%s", _options.url);
	Thread::setName (str);

	if (_mode == MODE::AUDIO) { 
		_timer = new Timer (_options.ablock);	
	} else {
		_timer = new Timer (1.0/_video_format.framesPerSecond);	
	}
}

void FFMPEGReader::run () {
	
	if (_video_provider) {
		
		ssi_size_t n_bytes;
		bool is_empty;
		ssi_byte_t *frame = _video_buffer->pop (n_bytes, is_empty);
		
		SSI_ASSERT (n_bytes == ssi_video_size (_video_format));
		
		bool result = _video_provider->provide (frame, 1);

		if (!_options.stream) {
			if (result) {			
				_video_buffer->pop_done ();
			}
		} else {			
			_video_buffer->pop_done ();			
		}			

		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "video buffer : %.2f%% ", _video_buffer->getFillState () * 100);

        /// Determine the current frame stamp

        int _codedframestamp = 0;
        int _displayframestamp = 0;
        if(_client){
            _codedframestamp = _client->_codedFrameNumber;
            _displayframestamp = _client->_displayFrameNumber;
        }

		ITheFramework *frame_work = Factory::GetFramework ();
        ssi_time_t _time_stamp = frame_work->GetStartTimeMs() + frame_work->GetElapsedTimeMs();

        //ssi_time_t _time_stamp = _video_channel.getStream().time;

        /// Send frame stamps through events

        // Get the timestamp of the current frame
        //ssi_time_t _time_stamp = stream.time;

        ssi_size_t _event_size = 3;
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
            ssi_sprint (str, "coded_frame");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value = (float)( _codedframestamp );

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "display_frame");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value = (float)( _displayframestamp );

        {
            ssi_char_t str[SSI_MAX_CHAR];
            ssi_sprint (str, "stamp");
            event_tuple[i].id = Factory::AddString(str);
        }
        event_tuple[i++].value = (float)( _time_stamp );

        // Create an adequate event structure
        // and add the right data to the event
        //_event.time = ssi_cast (ssi_size_t, 1000 * _time_stamp + 0.5); //CF double-check the need of the extra time scaling
        _event.dur = ssi_cast (ssi_size_t, 0);
        _event.prob = ssi_cast (ssi_real_t, 1.0);
        _event.state = SSI_ESTATE_CONTINUED;
        _event.type = SSI_ETYPE_NTUPLE;

        // And add the event to the event pool
        _elistener->update (_event);

        /// Send frame stamps through provider
        if (_videoframestamp_provider){
            std::vector<ssi_real_t> _videoframestamp_provided;
            _videoframestamp_provided.push_back(_codedframestamp);
            _videoframestamp_provided.push_back(_displayframestamp);
            _videoframestamp_provided.push_back(_time_stamp);
            bool result = _videoframestamp_provider->provide( ssi_pcast(ssi_byte_t, &_videoframestamp_provided[0]),1);
        }
	}

	if (_audio_provider) {

		ssi_size_t n_samples;
		bool is_empty;
		ssi_real_t *chunk = _audio_buffer->pop (n_samples, is_empty);

		bool result = _audio_provider->provide (ssi_pcast (ssi_byte_t, chunk), n_samples);
		if (!_options.stream) {
			if (result) {			
				_audio_buffer->pop_done (n_samples);
			}
		} else {			
			_audio_buffer->pop_done (n_samples);			
		}			

		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "audio buffer : %.2f%% ", _audio_buffer->getFillState () * 100);
	}

	_timer->wait ();
}

void FFMPEGReader::flush () {
	
	delete _timer; _timer = 0;
}

bool FFMPEGReader::disconnect () {

	_client->stop ();

	delete _client;_client=0;
	delete _timer; _timer = 0;

	return true;
};

bool FFMPEGReader::pushVideoFrame (ssi_size_t n_bytes, ssi_byte_t *frame) {

	/*SSI_ASSERT (n_bytes == ssi_video_size (_video_format));

	return _video_buffer->push (frame);	*/

	if (n_bytes == ssi_video_size (_video_format)){
		return _video_buffer->push (frame);	
	}
	else{
		ssi_wrn("FFMPEGReader::pushVideoFrame: n bytes received and video size are different");
		return false;
	}
}

bool FFMPEGReader::pushAudioChunk (ssi_size_t n_samples, ssi_real_t *chunk) {

	return _audio_buffer->push (n_samples, chunk);
}

bool FFMPEGReader::setEventListener (IEventListener *listener) {
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
