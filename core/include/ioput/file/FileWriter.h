// FileWriter.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/08
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

#pragma once

#ifndef SSI_IOPUT_FILEWRITER_H
#define SSI_IOPUT_FILEWRITER_H

#include "base/IConsumer.h"
#include "ioput/file/FileStreamOut.h"
#include "ioput/option/OptionList.h"
#include "base/ITheFramework.h"

namespace ssi {

class FileWriter : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options ()
			: mode (File::WRITE), stream (true), version (File::DEFAULT_VERSION), type (File::BINARY) {

			path[0] = '\0';
			delim[0] = ' '; delim[1] = '\0';
			flags[0] = '\0';
			addOption ("path", path, SSI_MAX_CHAR, SSI_CHAR, "file path (empty for stdout)");
			addOption ("type", &type, 1, SSI_UCHAR, "file type (0=binary, 1=ascii)");		
			addOption ("mode", &mode, 1, SSI_UCHAR, "file access mode (1=w, 2=a, 3=r+, 4=w+, 5=a+)");
			addOption ("version", &version, 1, SSI_UCHAR, "file version (0=V0, 1=V1, 2=V2(xml))");
			addOption ("stream", &stream, 1, SSI_BOOL, "continuous stream mode");
			addOption ("delim", delim, SSI_MAX_CHAR, SSI_CHAR, "delimiter string");
			addOption ("flags", flags, SSI_MAX_CHAR, SSI_CHAR, "format flags");
		};

		void setPath (const ssi_char_t *path) {
			this->path[0] = '\0';
			if (path) {
				ssi_strcpy (this->path, path);
			}
		}
		void setFormat (const ssi_char_t *delim, const ssi_char_t *flags) {
			this->delim[0] = ' '; this->delim[1] = '\0';
			this->flags[0] = '\0';
			if (delim) {
				ssi_strcpy (this->delim, delim);
			}
			if (flags) {
				ssi_strcpy (this->flags, flags);
			}
		}

		ssi_char_t path[SSI_MAX_CHAR];
		File::TYPE type;
		File::MODE mode;
		File::VERSION version;
		bool stream;
		ssi_char_t delim[20];
		ssi_char_t flags[20];
	};

public: 	

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_FileWriter"; };
	static IObject *Create (const ssi_char_t *file) { return new FileWriter (file); };
	~FileWriter ();

	FileWriter::Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Stores streams to a file on disk."; };

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

	void setMetaData (ssi_size_t size, const void *meta) {
		_n_meta = size;
		if (_n_meta > 0) {
			delete[] _meta;
			_meta = new ssi_byte_t[size];
			memcpy (_meta, meta, size);
		}
	}

	void setLogLevel (int level) {
		ssi_log_level = level;
	}

protected:

	FileWriter (const ssi_char_t *file = 0);
	FileWriter::Options _options;
	ssi_char_t *_file;

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;

	ssi_size_t _n_meta;
	ssi_byte_t *_meta;
	FileStreamOut _out;

	File *_fileptr;
	ssi_size_t _total_sample_number;
	ssi_size_t _position;
	ssi_char_t _string[1024];
	bool _first_call;
	ITheFramework *_frame;
};

}

#endif
