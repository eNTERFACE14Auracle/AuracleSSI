// Evaluation.cpp
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

#include "model/Evaluation.h"
#include "model/ModelTools.h"
#include "ioput/file/File.h"
#include "model/ISSelectUser.h"
#include "model/ISSelectSample.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *Evaluation::ssi_log_name = "evaluation";
int Evaluation::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;

int Evaluation::_default_class_id = 0;
bool Evaluation::_allow_unclassified = true;

Evaluation::Evaluation () 
	: _n_classes (0),
	_n_unclassified (0),
	_n_classified (0),
	_n_total (0),
	_result_vec (0),
	_result_vec_ptr (0),
	_conf_mat_ptr (0),
	_conf_mat_data (0),
	_trainer (0),
	_class_names (0),
	_fselmethod (0),
	_pre_fselmethod (0),
	_n_pre_feature (0),	
	_preproc_mode (false),
	_n_streams_refs (0),
	_stream_refs (0) {
}

Evaluation::~Evaluation () {

	destroy_conf_mat ();
}

void Evaluation::init_conf_mat (ISamples &samples) {

	_n_classes = samples.getClassSize ();

	// store class names
	_class_names = new ssi_char_t *[_n_classes];
	for (ssi_size_t i = 0; i < _n_classes; i++) {
		_class_names[i] = ssi_strcpy (samples.getClassName (i));
	}

	// allocate confussion matrix
	_conf_mat_ptr = new ssi_size_t *[_n_classes];
	_conf_mat_data = new ssi_size_t[_n_classes * _n_classes];
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		_conf_mat_ptr[i] = _conf_mat_data + i*_n_classes;
	}

	// set all elements in the confussion matrix to zero
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		for (ssi_size_t j = 0; j < _n_classes; ++j) {
			_conf_mat_ptr[i][j] = 0;
		}
	}

	_n_unclassified = 0;
	_n_classified = 0;	
}

void Evaluation::destroy_conf_mat () {

	// delete confussion matrix
	delete[] _conf_mat_ptr;
	_conf_mat_ptr = 0;
	delete[] _conf_mat_data;
	_conf_mat_data = 0;
	for (ssi_size_t i = 0; i < _n_classes; i++) {
		delete[] _class_names[i];
		_class_names[i] = 0;
	}
	delete[] _class_names; 
	_class_names = 0;
	_n_classes = 0;	
	_n_total = 0;
	delete[] _result_vec;
	_result_vec = 0;
	_result_vec_ptr = 0;
}

void Evaluation::eval (Trainer &trainer, ISamples &samples) {

	// init confussion matrix
	_trainer = &trainer;
	destroy_conf_mat ();
	init_conf_mat (samples);

	_n_total = samples.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	// call helper function
	eval_h (samples);
}

void Evaluation::eval_h (ISamples &samples) {	

	// walk through sample list and test trainer against each sample
	samples.reset ();
	const ssi_sample_t *sample = 0;
	ssi_size_t index, real_index;
	while (sample = samples.next ()) {
		real_index = sample->class_id;
		*_result_vec_ptr++ = real_index;
		if (_trainer->forward (sample->num, sample->streams, index)) {
			*_result_vec_ptr++ = index;
			_conf_mat_ptr[real_index][index]++;
			_n_classified++;
		} else if (!_allow_unclassified) {
			index = _default_class_id;
			*_result_vec_ptr++ = index;
			_conf_mat_ptr[real_index][index]++;
			_n_classified++;
		} else {
			*_result_vec_ptr++ = SSI_ISAMPLES_GARBAGE_CLASS_ID;
			_n_unclassified++;
		}
	}
}

void Evaluation::eval (IModel &model, ISamples &samples, ssi_size_t stream_index) {

	// init confussion matrix
	_trainer = 0;
	destroy_conf_mat ();
	init_conf_mat (samples);
	ssi_size_t n_classes = samples.getClassSize ();
	ssi_real_t *probs = new ssi_real_t[n_classes];

	_n_total = samples.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	samples.reset ();
	const ssi_sample_t *sample = 0;	
	while (sample = samples.next ()) {

		ssi_size_t real_index = sample->class_id;
		*_result_vec_ptr++ = real_index;
		if (model.forward (*sample->streams[stream_index], n_classes, probs)) {

			ssi_size_t max_ind = 0;
			ssi_real_t max_val = probs[0];
			for (ssi_size_t i = 1; i < n_classes; i++) {
				if (probs[i] > max_val) {
					max_val = probs[i];
					max_ind = i;
				}
			}

			*_result_vec_ptr++ = max_ind;
			_conf_mat_ptr[real_index][max_ind]++;
			_n_classified++;

		} else if (!_allow_unclassified) {
			ssi_size_t max_ind = _default_class_id;
			*_result_vec_ptr++ = max_ind;
			_conf_mat_ptr[real_index][max_ind]++;
			_n_classified++;
		} else {
			*_result_vec_ptr++ = SSI_ISAMPLES_GARBAGE_CLASS_ID;
			_n_unclassified++;
		}		
	}

	delete[] probs;
}

void Evaluation::eval (IFusion &fusion, ssi_size_t n_models, IModel **models, ISamples &samples) {

	// init confussion matrix
	_trainer = 0;
	destroy_conf_mat ();	
	init_conf_mat (samples);
	ssi_size_t n_classes = samples.getClassSize ();
	ssi_real_t *probs = new ssi_real_t[n_classes];

	_n_total = samples.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	samples.reset ();
	const ssi_sample_t *sample = 0;	
	while (sample = samples.next ()) {

		ssi_size_t real_index = sample->class_id;
		*_result_vec_ptr++ = real_index;
		if (fusion.forward (n_models, models, sample->num, sample->streams, n_classes, probs)) {

			ssi_size_t max_ind = 0;
			ssi_real_t max_val = probs[0];
			for (ssi_size_t i = 1; i < n_classes; i++) {
				if (probs[i] > max_val) {
					max_val = probs[i];
					max_ind = i;
				}
			}

			*_result_vec_ptr++ = max_ind;
			_conf_mat_ptr[real_index][max_ind]++;
			_n_classified++;

		} else if (!_allow_unclassified) {
			ssi_size_t max_ind = _default_class_id;
			*_result_vec_ptr++ = max_ind;
			_conf_mat_ptr[real_index][max_ind]++;
			_n_classified++;
		} else {
			*_result_vec_ptr++ = SSI_ISAMPLES_GARBAGE_CLASS_ID;
			_n_unclassified++;
		}	
	}

	delete[] probs;
}

void Evaluation::evalSplit (Trainer &trainer, ISamples &samples, ssi_real_t split) {

	if (split <= 0 || split >= 1) {
		ssi_err ("split must be a value between 0 and 1");
	}

	_trainer = &trainer;
	destroy_conf_mat ();
	init_conf_mat (samples);
	
	ssi_size_t *indices = new ssi_size_t[samples.getSize ()];
	ssi_size_t *indices_count_lab = new ssi_size_t[samples.getClassSize ()];
	ssi_size_t indices_count_all;

	indices_count_all = 0;
	for (ssi_size_t j = 0; j < samples.getClassSize (); j++) {
		indices_count_lab[j] = 0;
	}

	ssi_size_t label;
	ssi_size_t label_size;
	for (ssi_size_t j = 0; j < samples.getSize (); j++) {
		label = samples.get (j)->class_id;
		label_size = samples.getSize (label);
		if (++indices_count_lab[label] <= ssi_cast (ssi_size_t, label_size * split + 0.5f)) {
			indices[indices_count_all++] = j;			
		}
	}

	SampleList strain;
	SampleList stest;

	// split off samples
	ModelTools::SelectSampleList (samples, strain, stest, indices_count_all, indices);
	_n_total = stest.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	// train with remaining samples
	_trainer->release ();	
	if (_preproc_mode) {
		_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
	} else if (_fselmethod) {
		_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
	}
	_trainer->train (strain);		

	// test with remaining samples
	eval_h (stest);

	delete[] indices;
	delete[] indices_count_lab;

}

void Evaluation::evalKFold (Trainer &trainer, ISamples &samples, ssi_size_t k) {

	// init confussion matrix
	_trainer = &trainer;
	destroy_conf_mat ();
	init_conf_mat (samples);

	_n_total = samples.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;
	
	ssi_size_t *indices = new ssi_size_t[samples.getSize ()];
	ssi_size_t *indices_count_lab = new ssi_size_t[samples.getClassSize ()];
	ssi_size_t indices_count_all;

	for (ssi_size_t i = 0; i < k; ++i) {

		indices_count_all = 0;
		for (ssi_size_t j = 0; j < samples.getClassSize (); j++) {
			indices_count_lab[j] = 0;
		}

		ssi_size_t label;
		for (ssi_size_t j = 0; j < samples.getSize (); j++) {
			label = samples.get (j)->class_id;
			if (++indices_count_lab[label] % k == i) {
				indices[indices_count_all++] = j;			
			}
		}

		SampleList strain;
		SampleList stest;	
		// split off i'th fold
		ModelTools::SelectSampleList (samples, stest, strain, indices_count_all, indices);

		// train with i'th fold
		_trainer->release ();
		if (_fselmethod) {
			_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
		}
		if (_preproc_mode) {
			_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
		}
		_trainer->train (strain);

		// test with remaining samples
		eval_h (stest);
	}

	delete[] indices;
	delete[] indices_count_lab;
}

void Evaluation::evalLOO (Trainer &trainer, ISamples &samples) {

	_trainer = &trainer;
	destroy_conf_mat ();
	init_conf_mat (samples);
	ssi_size_t n_samples = samples.getSize ();

	_n_total = n_samples;
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	ssi_size_t itest  = 0;
	ssi_size_t *itrain = new ssi_size_t[n_samples - 1];
	for (ssi_size_t nsample = 0; nsample < n_samples - 1; ++nsample) {
		itrain[nsample] = nsample+1;
	}
	
	ISSelectSample strain (&samples);
	ISSelectSample stest (&samples);

	strain.setSelection  (n_samples-1, itrain);
	stest.setSelection (1, &itest);

	_trainer->release ();
	if (_fselmethod) {
		_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
	}
	if (_preproc_mode) {
		_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
	}
	_trainer->train (strain);
	eval_h (stest);		

	for (ssi_size_t nsample = 1; nsample < n_samples; ++nsample) {
		
		itrain[nsample-1] = nsample-1;
		itest = nsample;

		strain.setSelection  (n_samples-1, itrain);
		stest.setSelection (1, &itest);

		_trainer->release ();	
		if (_fselmethod) {
			_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
		}
		if (_preproc_mode) {
			_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
		}
		_trainer->train (strain);

		eval_h (stest);		
	}

	delete [] itrain;

}


void Evaluation::evalLOUO (Trainer &trainer, ISamples &samples){

	_trainer = &trainer;
	destroy_conf_mat ();
	init_conf_mat (samples);
	ssi_size_t n_users = samples.getUserSize ();

	_n_total = samples.getSize ();
	_result_vec = new ssi_size_t[2*_n_total];
	_result_vec_ptr = _result_vec;

	ssi_size_t itest  = 0;
	ssi_size_t *itrain = new ssi_size_t[n_users - 1];
	for (ssi_size_t nuser = 0; nuser < n_users - 1; ++nuser) {
		itrain[nuser] = nuser+1;
	}
	
	ISSelectUser strain (&samples);
	ISSelectUser stest (&samples);

	strain.setSelection  (n_users-1, itrain);
	stest.setSelection (1, &itest);

	_trainer->release ();
	if (_fselmethod) {
		_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
	}
	if (_preproc_mode) {
		_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
	}
	_trainer->train (strain);
	eval_h (stest);		

	for (ssi_size_t nuser = 1; nuser < n_users; ++nuser) {
		
		itrain[nuser-1] = nuser-1;
		itest = nuser;

		strain.setSelection  (n_users-1, itrain);
		stest.setSelection (1, &itest);

		_trainer->release ();
		if (_fselmethod) {
			_trainer->setSelection (strain, _fselmethod, _pre_fselmethod, _n_pre_feature);
		}
		if (_preproc_mode) {
			_trainer->setPreprocMode (_preproc_mode, _n_streams_refs, _stream_refs);
		}
		_trainer->train (strain);

		eval_h (stest);		
	}

	delete [] itrain;
}

void Evaluation::print (FILE *file) {

	if (!_conf_mat_ptr) {
		ssi_wrn ("nothing to print");
		return;
	}
		
	ssi_size_t max_label_len = 0;
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		ssi_size_t len = ssi_cast (ssi_size_t, strlen (_class_names[i]));
		if (len > max_label_len) {
			max_label_len = len;
		}
	}
	File *tmp = File::Create (File::ASCII, File::WRITE, 0, file);
	tmp->setType (SSI_UINT);
	tmp->setFormat (" ", "6");	
	
	ssi_fprint (file, "#classes:      %u\n", _n_classes);
	ssi_fprint (file, "#total:        %u\n", _n_classified + _n_unclassified);
	ssi_fprint (file, "#classified:   %u\n", _n_classified);
	ssi_fprint (file, "#unclassified: %u\n", _n_unclassified);
	for (ssi_size_t i = 0; i < _n_classes; ++i) {			
		ssi_fprint (file, "%*s: ", max_label_len, _class_names[i]);
		tmp->write (_conf_mat_ptr[i], 0, _n_classes);
		ssi_fprint (file, "   -> %8.2f%%\n", 100*get_class_prob (i));
	}
	ssi_fprint (file, "   %*s  => %8.2f%% | %.2f%%\n", max_label_len + _n_classes * 7, "", 100*get_classwise_prob (), 100*get_accuracy_prob ());	
	delete tmp;

	fflush (file);
}

void Evaluation::print_result_vec (FILE *file) {

	if (!_result_vec) {
		ssi_wrn ("nothing to print");
		return;
	}
	ssi_size_t *ptr = _result_vec;
	for (ssi_size_t i = 0; i < _n_total; i++) {
		ssi_fprint (file, "%u %u\n", *ptr, *(ptr+1));
		ptr += 2;
	}
}

ssi_real_t Evaluation::get_class_prob (ssi_size_t index) {

	ssi_size_t sum = 0;
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		sum += _conf_mat_ptr[index][i];
	}
	ssi_real_t prob = sum > 0 ? ssi_cast (ssi_real_t, _conf_mat_ptr[index][index]) / ssi_cast (ssi_real_t, sum) : 0; 

	return prob;
}

ssi_real_t Evaluation::get_classwise_prob () {

	ssi_real_t prob = 0;
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		prob += get_class_prob (i);
	}

	return prob / _n_classes;
}

ssi_real_t Evaluation::get_accuracy_prob () {

	ssi_size_t sum_correct = 0;
	for (ssi_size_t i = 0; i < _n_classes; ++i) {
		sum_correct += _conf_mat_ptr[i][i];
	}

	ssi_real_t prob = _n_classified > 0 ? ssi_cast (ssi_real_t, sum_correct) / ssi_cast (ssi_real_t, _n_classified) : 0; 

	return prob;
}

ssi_size_t*const* Evaluation::get_conf_mat () {

	ssi_size_t **conf_ptr = _conf_mat_ptr;

	return conf_ptr;
}

void Evaluation::release () {

	destroy_conf_mat ();
	_trainer = 0;
}

}
