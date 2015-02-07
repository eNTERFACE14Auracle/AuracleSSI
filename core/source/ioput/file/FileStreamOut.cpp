// FileStreamOut.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/09/27
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

#include "ioput/file/FileStreamOut.h"
#include "ioput/xml/tinyxml.h"
#include "ioput/file/FilePath.h"

namespace ssi {

int FileStreamOut::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;
ssi_char_t FileStreamOut::ssi_log_name[] = "fstrmout__";
File::VERSION FileStreamOut::DEFAULT_VERSION = File::V2;

FileStreamOut::FileStreamOut ()
	: _file_data (0),
	_file_info (0),
	_sample_count (0),
	_console (false),
	_path (0) {				
}

FileStreamOut::~FileStreamOut () {

	if (_file_data) {
		close ();
	}
}

bool FileStreamOut::open (ssi_stream_t &data, // data is not written!
	const ssi_char_t *path,
	File::TYPE type, 
	File::VERSION version) {

	return open (data, 0, 0, path, type, version);
}

bool FileStreamOut::open (ssi_stream_t &data, // data is not written!
	ssi_size_t n_meta, 
	const void *meta,
	const ssi_char_t *path,
	File::TYPE type, 
	File::VERSION version) {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "open stream file '%s'", path);	

	if (version < File::V2) {
		ssi_wrn ("version < V2 not supported");
		return false;
	}

	if (_file_info || _file_data) {
		ssi_wrn ("stream already open");
		return false;
	}

	if (path == 0 || path[0] == '\0') {
		_console = true;
	}

	if (_console) {
		
		_file_data = File::CreateAndOpen (type, File::WRITE, "");
		if (!_file_data) {
			ssi_wrn ("could not open console");
			return false;
		}

	} else {

		FilePath fp (path);
		ssi_char_t *path_info = 0;
		if (strcmp (fp.getExtension (), SSI_FILE_TYPE_STREAM) != 0) {
			path_info = ssi_strcat (path, SSI_FILE_TYPE_STREAM);
		} else {
			path_info = ssi_strcpy (path);
		}	
		_path = ssi_strcpy (path_info);

		_file_info = File::CreateAndOpen (File::ASCII, File::WRITE, path_info);
		if (!_file_info) {
			ssi_wrn ("could not open info file '%s'", path_info);
			return false;
		}
		
		ssi_sprint (_string, "<?xml version=\"1.0\" ?>\n<stream ssi-v=\"%d\">", version);
		_file_info->writeLine (_string);

		TiXmlElement info ("info" );	
		info.SetAttribute ("ftype", File::TYPE_NAMES[type]);
		info.SetDoubleAttribute ("sr", data.sr);
		info.SetAttribute ("dim", ssi_cast (int, data.dim));
		info.SetAttribute ("byte", ssi_cast (int, data.byte));
		info.SetAttribute ("type", SSI_TYPE_NAMES[data.type]);						
		info.Print (_file_info->getFile (), 1);
		_file_info->writeLine ("");

		if (n_meta > 0 && meta) {
			switch (data.type) {
				case SSI_IMAGE: {
					const ssi_video_params_t *params = ssi_pcast (const ssi_video_params_t, meta);
					TiXmlElement meta ("meta" );	
					meta.SetAttribute ("width", params->widthInPixels);
					meta.SetAttribute ("height", params->heightInPixels);
					meta.SetAttribute ("depth", params->depthInBitsPerChannel);
					meta.SetAttribute ("channels", params->numOfChannels);
					meta.SetAttribute ("flip", params->flipImage ? 1 : 0);
					meta.Print (_file_info->getFile (), 1);
					_file_info->writeLine ("");
					break;
				}
				default:
					ssi_wrn ("type '%s' doesn't support meta information", SSI_TYPE_NAMES[data.type]);
					break;
			}
		}
	
		ssi_char_t *path_data = ssi_strcat (path_info, "~");			
		_file_data = File::CreateAndOpen (type, File::WRITE, path_data);
		if (!_file_data) {
			ssi_wrn ("could not open data file '%s'", path_data);
			return false;
		}

		delete[] path_info;
		delete[] path_data;

	}

	_stream = data;
	_stream.num = 0;
	_stream.ptr = 0;
	_sample_count = 0;
	_last_time = 0;
	_last_byte = 0;

	return true;
};

bool FileStreamOut::write (ssi_stream_t &data, 
	bool continued) {

	if (data.byte != _stream.byte || data.dim != _stream.dim || data.type != _stream.type) {
		ssi_wrn ("stream is not compatible");
		return false;
	}

	if (!_console) {
		
		if (!continued) {
			if (_sample_count == 0) {
				ssi_sprint (_string, "\t<chunk from=\"%lf\" to=\"%lf\" byte=\"%u\" num=\"%u\"/>", data.time, data.time + data.num * (1.0/data.sr), _file_data->tell (), data.num);					
			} else {			
				_sample_count += data.num;
				ssi_sprint (_string, "\t<chunk from=\"%lf\" to=\"%lf\" byte=\"%u\" num=\"%u\"/>", _last_time, _last_time + _sample_count * (1.0/data.sr), _last_byte,_sample_count);					
				_sample_count = 0;
			}
			_file_info->writeLine (_string);		
		} else {
			if (_sample_count == 0) {
				_last_time = data.time;
				_last_byte = _file_data->tell ();		
			}
			_sample_count += data.num;
		}
	}

	if (data.num > 0) {
		if (_file_data->getType () == File::ASCII) {
			_file_data->setType (data.type);
			_file_data->write (data.ptr, data.dim, data.num * data.dim);
		} else {
			_file_data->write (data.ptr, sizeof (ssi_byte_t), data.tot);
		}
	}

	return true;
}

bool FileStreamOut::close () { 

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "close stream file '%s'", _path);

	if (!_console) {

		if (!_file_data->close ()) {
			ssi_wrn ("could not close data file '%s'", _path);
			return false;
		}

		if (_sample_count > 0) {	
			write (_stream, false);
		}

		_file_info->writeLine ("</stream>");

		if (!_file_info->close ()) {
			ssi_wrn ("could not close info file '%s'", _file_info->getPath ());
			return false;
		}
	}

	delete _file_data; _file_data = 0;
	delete _file_info; _file_info = 0;
	delete _path; _path = 0;

	return true;
};

}
