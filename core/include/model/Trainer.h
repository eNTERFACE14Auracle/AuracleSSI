// Trainer.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/27
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

#ifndef SSI_MODEL_TRAINER_H
#define SSI_MODEL_TRAINER_H

#include "base/IModel.h"
#include "base/IFusion.h"
#include "base/ISamples.h"
#include "base/ISelection.h"
#include "base/ITransformer.h"
#include "model/SampleList.h"
#include "ioput/xml/tinyxml.h"
#include "ioput/file/FileTools.h"
#include "ioput/file/FilePath.h"

namespace ssi {

class Trainer {

friend class EmoVoiceBayes;

public:

	enum VERSION {
		V1 = 1,		// original format
		V2 = 2,		// + user names
		V3 = 3,		// + relative model/fusion path + feature selection
		V4 = 4		// + transformer
	};
	static Trainer::VERSION DEFAULT_VERSION;

	Trainer ();
	Trainer (IModel *model, ssi_size_t stream_index = 0);
	Trainer (ssi_size_t n_models,
		IModel **models,
		IFusion *fusion);

	virtual ~Trainer ();

	bool setSelection (ssi_size_t n_streams,
		ssi_size_t *n_select,
		ssi_size_t **stream_select);
	bool setSelection (ISamples &samples,
		ISelection *fselmethod,
		ISelection *pre_fselmethod = 0,
		ssi_size_t n_pre_feature = 0);

	bool setTransformer (ssi_size_t n_streams,
		ITransformer **transformer,
		ssi_size_t *transformer_frame = 0,
		ssi_size_t *transformer_delta = 0);	

	bool setPreprocMode (bool toggle,
		ssi_size_t n_streams,
		ssi_stream_t stream_refs[]);
	bool preproc (ISamples &samples, 
		SampleList &samples_pre);

	bool train (ISamples &samples);

	bool forward (ssi_stream_t &stream,
		ssi_size_t &class_index);
	bool forward_probs (ssi_stream_t &stream,
		ssi_size_t class_num,
		ssi_real_t *class_probs);
	bool forward (ssi_size_t num,
		ssi_stream_t **streams,
		ssi_size_t &class_index);
	bool forward_probs (ssi_size_t num,
		ssi_stream_t **streams,
		ssi_size_t class_num,
		ssi_real_t *class_probs);
	bool cluster (ISamples &samples);

	void release ();
	void release_transformer ();
	void release_selection ();
	
	ssi_size_t getMetaSize ();
	bool getMetaData (ssi_size_t n_metas,
		ssi_real_t *metas);	

	static bool Load (Trainer &trainer,
		const ssi_char_t *filename);
	bool save (const ssi_char_t *filename, VERSION version = DEFAULT_VERSION);
	
	ssi_size_t getModelSize ();
	IModel *getModel (ssi_size_t index);
	IFusion *getFusion ();
	const ssi_char_t *getName ();
	const ssi_char_t *getInfo ();

	bool isTrained () { return _is_trained; };
	bool hasSelection () { return _has_selection; };
	ssi_size_t getStreamSize () { return _n_streams; };
	ssi_stream_t &getStreamRef (ssi_size_t stream_index) { return _stream_refs[stream_index]; };

	ssi_size_t getClassSize () { return _n_classes; } ;
	const ssi_char_t *getClassName (ssi_size_t class_index) { return _class_names[class_index]; };
	ssi_char_t *const *getClassNames () { return _class_names; };
	
	static void SetLogLevel (int level) {
		ssi_log_level = level;
	}

//protected:

	static ssi_char_t *ssi_log_name;
	static int ssi_log_level;

	void init_class_names (ISamples &samples);
	void free_class_names ();

	void init_user_names (ISamples &samples);
	void free_user_names ();

	bool train_h (ISamples &samples);
	static bool Load_V1to3 (Trainer &trainer, FilePath &fp, TiXmlElement *body, VERSION version);
	bool save_V1to3 (const ssi_char_t *filepath, TiXmlElement &body, VERSION version);
	static bool Load_V4 (Trainer &trainer, FilePath &fp, TiXmlElement *body);
	bool save_V4 (const ssi_char_t *filepath, TiXmlElement &body);

	bool _is_trained;
	bool _preproc_mode;
	ssi_size_t _n_streams;
	ssi_stream_t *_stream_refs;
	ssi_size_t _stream_index;

	bool _has_selection;
	ssi_size_t *_n_stream_select;
	ssi_size_t **_stream_select;

	bool _has_transformer;
	ITransformer **_transformer;
	ssi_size_t *_transformer_frame;
	ssi_size_t *_transformer_delta;

	ssi_size_t _n_models;
	IModel **_models;
	IFusion *_fusion;
	
	ssi_size_t _n_classes;
	ssi_char_t **_class_names;

	ssi_size_t _n_users;
	ssi_char_t **_user_names;

};

}

#endif
