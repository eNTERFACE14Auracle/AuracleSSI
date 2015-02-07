// SSI_Global.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/30
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

#include "SSI_Cons.h"

bool ssi_log_file_on = false;
FILE *ssiout = stdout;
ssi::IMessage *ssimsg = 0;
ssi_size_t ssi_tic_start = 0;

ssi_char_t *SSI_TYPE_NAMES[SSI_TYPE_NAME_SIZE] = {
	"UNDEF",	
	"CHAR",
	"UCHAR",
	"SHORT",
	"USHORT",
	"INT",
	"UINT",
	"LONG",
	"ULONG",
	"FLOAT",
	"DOUBLE",
	"LDOUBLE",
	"STRUCT",
	"IMAGE",
	"BOOL"
};

ssi_char_t *SSI_ESTATE_NAMES[SSI_ESTATE_NAME_SIZE] = {
	"COMPLETED",
	"CONTINUED"
};

ssi_char_t *SSI_ETYPE_NAMES[SSI_ETYPE_NAME_SIZE] = {
	"UNDEF",
	"EMPTY",
	"STRING",
	"NTUPLE",
	"FLOATS"
};

ssi_char_t *SSI_OBJECT_NAMES[SSI_OBJECT_NAME_SIZE] = {
	"OBJECT",
	"PROVIDER",
	"CONSUMER",
	"TRANSFORMER",
	"FEATURE",
	"FILTER",
	"TRIGGER",
	"MODEL",
	"MODEL_CONTINUOUS",
	"FUSION",
	"SENSOR",
	"SELECTION"
};

ssi_char_t *SSI_FILE_TYPE_STREAM = ".stream";
ssi_char_t *SSI_FILE_TYPE_FEATURE = ".feat";
ssi_char_t *SSI_FILE_TYPE_WAV = ".wav";
ssi_char_t *SSI_FILE_TYPE_AVI = ".avi";
ssi_char_t *SSI_FILE_TYPE_ANNOTATION = ".anno";
ssi_char_t *SSI_FILE_TYPE_OPTION = ".option";
ssi_char_t *SSI_FILE_TYPE_TRAINDEF = ".traindef";
ssi_char_t *SSI_FILE_TYPE_TRAINING = ".training";
ssi_char_t *SSI_FILE_TYPE_TRAINER = ".trainer";
ssi_char_t *SSI_FILE_TYPE_MODEL = ".model";
ssi_char_t *SSI_FILE_TYPE_FUSION = ".fusion";
ssi_char_t *SSI_FILE_TYPE_SAMPLES = ".samples";
ssi_char_t *SSI_FILE_TYPE_PIPELINE = ".pipeline";
ssi_char_t *SSI_FILE_TYPE_PCONFIG = ".pipeline-config";
ssi_char_t *SSI_FILE_TYPE_CHAIN = ".chain";
ssi_char_t *SSI_FILE_TYPE_CORPUS = ".corpus";
ssi_char_t *SSI_FILE_TYPE_EVENTS = ".events";
