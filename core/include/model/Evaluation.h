// Evaluation.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/01/12
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

/*
 
lets you evaluate a model
   
*/

#pragma once

#ifndef SSI_MODEL_EVALUATION_H
#define SSI_MODEL_EVALUATION_H

#include "Trainer.h"
#include "model/SampleList.h"

namespace ssi {

class Evaluation {

public:

	Evaluation ();
	~Evaluation ();

	// set feature selection method
	void setFselMethod (ISelection *fselmethod,
		ISelection *pre_fselmethod = 0,
		ssi_size_t n_pre_feature = 0) {
		_fselmethod = fselmethod;
		_pre_fselmethod = pre_fselmethod;
		_n_pre_feature = n_pre_feature;
	};
	// set pre-processing mode
	void setPreprocMode (bool toggle,
		ssi_size_t n_streams_refs,
		ssi_stream_t stream_refs[]) {
		_preproc_mode = toggle;
		_n_streams_refs = n_streams_refs;
		_stream_refs = stream_refs;
	}

	// eval using test set (it is assumed that model has already been trained)
	void eval (Trainer &trainer, ISamples &samples);
	void eval (IModel &model, ISamples &samples, ssi_size_t stream_index);
	void eval (IFusion &fusion, ssi_size_t n_models, IModel **models, ISamples &samples);

	// evaluation helper function
	void eval_h (ISamples &samples);

	// eval using the first split% for training and the rest for testing (split = ]0..1[)
	void evalSplit (Trainer &trainer, ISamples &samples, ssi_real_t split);

	// eval using k-fold (model is re-trained.. you may lose your old model!)
	void evalKFold (Trainer &trainer, ISamples &samples, ssi_size_t k);

	// eval using leave one out (model is re-trained.. you may lose your old model!)
	void evalLOO (Trainer &trainer, ISamples &samples);

	// eval using leave one user out (model is re-trained.. you may lose your old model!)
	void evalLOUO (Trainer &trainer, ISamples &samples);

	// print evaluation to file
	void print (FILE *file = stdout);
	void print_result_vec (FILE *file = stdout);

	// reset confussion matrix
	void release ();

	// #correct/#all per class
	ssi_real_t get_class_prob (ssi_size_t index);
	// mean of class probs
	ssi_real_t get_classwise_prob ();
	// relative mean of class probs
	ssi_real_t get_accuracy_prob ();
	// get confussion matrix
	ssi_size_t*const* get_conf_mat ();
	// get result vector
	const ssi_size_t *get_result_vec (ssi_size_t &size) { size = _n_total; return _result_vec; };

	// class size and names
	ssi_size_t get_class_size () {
		return _n_classes;
	}
	const ssi_char_t *get_class_name (ssi_size_t index) {
		return index >= _n_classes ? 0 : _class_names[index];
	}

	static void SetLogLevel (int level) {
		ssi_log_level = level;
	}

	ssi_size_t get_n_classified () {
		return _n_classified;
	}
	ssi_size_t get_n_unclassified () {
		return _n_unclassified;
	}

	static void SetHandlingOfUnclassifiedSamples (bool allow_unclassified, int default_class_id) {
		_allow_unclassified = allow_unclassified;
		_default_class_id = default_class_id;
	}

protected:

	// initialize confussion matrix
	void init_conf_mat (ISamples &samples);
	// destroy confussion matrix
	void destroy_conf_mat ();

	ssi_size_t **_conf_mat_ptr;
	ssi_size_t *_conf_mat_data;	
	ssi_size_t _n_unclassified;
	ssi_size_t _n_classified;
	ssi_size_t _n_total; // unclassified + classified
	ssi_size_t *_result_vec; // result vector, for each sample the real id and the classified id is stored, SSI_ISAMPLES_GARBAGE_CLASS_ID if unclassified	
	ssi_size_t *_result_vec_ptr;
	ssi_size_t _n_classes;
	ssi_char_t** _class_names;	

	Trainer *_trainer;	
	ISelection *_fselmethod;
	ISelection *_pre_fselmethod;
	ssi_size_t _n_pre_feature;
	bool _preproc_mode;
	ssi_size_t _n_streams_refs;
	ssi_stream_t *_stream_refs;

	static bool _allow_unclassified;
	static int _default_class_id;

	static ssi_char_t *ssi_log_name;
	static int ssi_log_level;
};

}

#endif
