// WavHeader.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/07/23
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

#ifndef SSI_WAV_WAVHEADER_H
#define SSI_WAV_WAVHEADER_H

namespace ssi {

struct WavHeader {
   char rID[4];
   long int rLen;                                                                                                                           
   char wID[4];
   char fId[4];
   long int pcmHeaderLen;
   short int compressionTag;                                                      
   short int nChannels;
   long int nSamplesPerSec;                                                   
   long int nAvgBytesPerSec;                                                  
   short int nBlockAlign;                                                     
   short int nBitsPerSample;                                                  
};                                                                  
                                                                          
struct WavChunkHeader {                                                               
   char chunkID[4];
   int chunkLen;                                               
};

}


#endif
