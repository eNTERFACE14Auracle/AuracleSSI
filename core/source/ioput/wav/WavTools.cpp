// WavTools.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/07/21
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

#include "ioput/wav/WavTools.h"
#include "ioput/file/FileTools.h"
#include "base/Factory.h"
#include "ioput/file/FilePath.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *WavTools::ssi_log_name = "wavtools__";

bool WavTools::ReadWavFile (const ssi_char_t *filename,
	ssi_stream_t &stream,
	bool convert2float) {

	File *file = File::CreateAndOpen (File::BINARY, File::READ, filename);
	bool result = ReadWavFile (*file, stream, convert2float);
	delete file;

	return result;
}

bool WavTools::ReadWavFile (File &file, 
	ssi_stream_t &stream,
	bool convert2float) {

	WavHeader header;
	WavChunkHeader chunk;

	// read header
	if (!WavTools::ReadWavHeader (file, header)) {
		return false;
	}
	if (!WavTools::ReadWavChunk (file, chunk)) {
		return false;
	}

	// read stream
	ssi_size_t sample_number = (chunk.chunkLen / (header.nBitsPerSample / 8) / header.nChannels);
	
	if (convert2float) {
		ssi_stream_init (stream, sample_number, header.nChannels, sizeof (float), SSI_FLOAT, header.nSamplesPerSec);
		ssi_size_t n_values = stream.num * stream.dim;
		short *buffer = new short[n_values];
		file.read (buffer, sizeof (short), n_values);
		float *dst = ssi_pcast (float, stream.ptr);
		short *src = buffer;
		for (ssi_size_t i = 0; i < n_values; i++) {
			*dst++ = ssi_cast (float, *src++) / 32768.0f;
		}
		delete[] buffer;
	} else {
		ssi_stream_init (stream, sample_number, header.nChannels, header.nBitsPerSample / 8, SSI_SHORT, header.nSamplesPerSec);
		file.read (stream.ptr, 1, stream.tot);	
	}

	return true;
}

bool WavTools::WriteWavFile (const ssi_char_t *filename,
	ssi_stream_t &stream) {

	File *file = File::CreateAndOpen (File::BINARY, File::WRITE, filename);
	bool result = WriteWavFile (*file, stream);
	delete file;
	return result;
}

bool WavTools::WriteWavFile (File &file, ssi_stream_t &stream) {

	if (!(stream.type == SSI_FLOAT ||stream.type == SSI_SHORT)) {
		ssi_wrn ("stream type '%s' not supported", SSI_TYPE_NAMES[stream.type]);
		return false;
	}

	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;         /* format type */
    wfx.nChannels = stream.dim;          /* number of channels (i.e. mono, stereo...) */
    wfx.nSamplesPerSec = ssi_cast (DWORD, stream.sr);     /* sample rate */    
	wfx.wBitsPerSample = sizeof (short) * 8;     /* number of bits per sample of mono stream */  
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);        /* block size of stream */
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;    /* for buffer estimation */
	wfx.cbSize = 0; 

	// write header
	if (!WavTools::WriteWavHeader (file, wfx, stream.num)) {
		return false;
	}
	if (!WavTools::WriteWavChunk (file, wfx, stream.num)) {
		return false;
	}

	// write stream
	if (stream.type == SSI_FLOAT) {

		ssi_size_t n = stream.num * stream.dim;
		short *tmp = new short[n];
		float *src = ssi_pcast (float, stream.ptr);
		short *dst = tmp;
		for (ssi_size_t i = 0; i < n; i++) {
			*dst++ = ssi_cast (short, *src++ * 32768.0f);
		}

		if (!file.write (tmp, 1, sizeof (short) * n)) {
			return false;
		}

		delete[] tmp;

	} else {			
		if (!file.write (stream.ptr, 1, stream.tot)) {			
			return false;
		}
	}	

	return true;
}



bool WavTools::ReadWavChunk (File &file,
	WavChunkHeader &chunk) {

	// skip chunks until a 'stream' chunk is found               
	char chunkID[5];
	chunkID[4] = 0;
	for (;;) {                                                                                                                                            
	   // read chunk header                                        
	   file.read (&chunk, sizeof (WavChunkHeader), 1);  						                                                               
	   // check chunk type                                         
	   for (int i = 0; i < 4; i++) {
		   chunkID[i] = chunk.chunkID[i];                    
	   }
	   if (strcmp (chunkID, "data") == 0) {
		   break;
	   }                                                           
	   // skip over chunk                                                                                             
	   file.seek (chunk.chunkLen, File::CURRENT);                 
	}

	return true;
}

bool WavTools::ReadWavHeader (File &file,
	WavHeader &header) {

	char buffer[100];

	// read wav header
	file.read (&header, sizeof (WavHeader), 1);
                                                                           
	// check format of header                                                  
	for (int i = 0; i < 4; i++) {
		buffer[i] = header.rID[i];
	}
	buffer[4] = 0;                                                              
	if (strcmp (buffer, "RIFF")!=0) {
		ssi_err ("bad RIFF format");
		return false;
	}
	for (int i = 0; i < 4; i++) {
		buffer[i] = header.wID[i];
	}
	buffer[4] = 0;                                                              
	if (strcmp (buffer, "WAVE") !=0) {
		ssi_err ("bad WAVE format");
		return false;
	}                                                                            
	for (int i = 0; i < 3; i++) {
		buffer[i] = header.fId[i];
	}
	buffer[3] = 0;                                                              
	if (strcmp (buffer, "fmt") != 0) {
		ssi_err ("bad fmt format");
		return false;
	}
    if (header.compressionTag != 1) {
		ssi_err ("compression not supported");
		return false;
	}

	// ignore remaining header
	file.seek (header.pcmHeaderLen - (sizeof (WavHeader) - 20), File::CURRENT);

	return true;
}


bool WavTools::WriteWavHeader (File &file,
	WAVEFORMATEX audio_params,
	ssi_size_t sample_number) {

	WavHeader header;

	memcpy (header.rID, "RIFF", 4);
	header.rLen = sizeof (WavHeader) + sizeof (WavChunkHeader) + sample_number * audio_params.nChannels * audio_params.wBitsPerSample - 8;
	memcpy (header.wID, "WAVE", 4);
	header.compressionTag = 1;
	memcpy (header.fId, "fmt ", 4);
	header.nAvgBytesPerSec = audio_params.nAvgBytesPerSec;
	header.nBitsPerSample = audio_params.wBitsPerSample;
	header.nBlockAlign = audio_params.nBlockAlign;
	header.nChannels = audio_params.nChannels;
	header.nSamplesPerSec = audio_params.nSamplesPerSec;
	header.pcmHeaderLen = 16;

	return file.write (&header, sizeof (header), 1);;
	
}

bool WavTools::WriteWavChunk (File &file,
	WAVEFORMATEX audio_params,
	ssi_size_t sample_number) {

	WavChunkHeader chunk;
	memcpy (chunk.chunkID, "data", 4);
	chunk.chunkLen = sample_number * audio_params.nBlockAlign;

	return file.write (&chunk, sizeof (chunk), 1);
}

bool WavTools::ConvertVXtoWAV (const ssi_char_t *filename) {
	
	if (!ssi_exists (filename)) {
		return false;
	}
	ssi_stream_t stream;
	File *file = File::CreateAndOpen (File::BINARY, File::READ, filename);
	FileTools::ReadStreamFile (*file, stream);
	delete file;
	FilePath fp (filename);
	ssi_char_t *filename_wav = ssi_strcat (fp.getPath (), SSI_FILE_TYPE_WAV);
	File *wav_file = File::CreateAndOpen (File::BINARY, File::WRITE, filename_wav);
	if (!WavTools::WriteWavFile (*wav_file, stream)) {
		return false;
	}
	delete[] filename_wav;
	ssi_stream_destroy (stream);
	delete wav_file;	

	return true;
}

bool WavTools::ConvertWAVtoVX (const ssi_char_t *filename, 
	File::VERSION version) {

	if (!ssi_exists (filename)) {
		return false;
	}
	ssi_stream_t stream;	
	if (!WavTools::ReadWavFile (filename, stream)) {
		return false;
	}	
	FilePath fp (filename);
	ssi_char_t *filename_ssi = ssi_strcat (fp.getPath (), SSI_FILE_TYPE_STREAM);
	File *file = File::CreateAndOpen (File::BINARY, File::WRITE, filename_ssi);
	FileTools::WriteStreamFile (*file, stream, version);
	delete[] filename_ssi;
	ssi_stream_destroy (stream);
	delete file;

	return true;
}


bool WavTools::RepairWavFile (const ssi_char_t *path,
	ssi_stream_t &data) {

	File *file = File::CreateAndOpen (File::BINARY, File::READ, path);

	WavHeader header;
	WavChunkHeader chunk;

	// read header
	if (!WavTools::ReadWavHeader (*file, header)) {
		return false;
	}
	if (!WavTools::ReadWavChunk (*file, chunk)) {
		return false;
	}

	ssi_size_t from = file->tell ();
	file->seek (0, File::END);
	ssi_size_t to = file->tell ();
	file->seek (from, File::BEGIN);

	ssi_size_t sample_number = (to-from) / (header.nChannels * header.nBitsPerSample / 8);
	ssi_stream_init (data, sample_number, header.nChannels, header.nBitsPerSample / 8, SSI_SHORT, header.nSamplesPerSec);
	file->read (data.ptr, 1, data.tot);

	delete file;
	return true;
}
}



