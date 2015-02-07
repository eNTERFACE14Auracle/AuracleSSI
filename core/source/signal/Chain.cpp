// Chain.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/08/29
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

#include "signal/Chain.h"
#include "ioput/file/FilePath.h"
#include "frame/XMLPipeline.h"
#include "base/Factory.h"

namespace ssi {

Chain::VERSION DEFAULT_VERSION = Chain::V1;
ssi_char_t *Chain::ssi_log_name = "chain_____";

Chain::Chain (const ssi_char_t *file) 
	: _n_filters (0),
	_filters (0),
	_n_features (0),		
	_features (0),
	_feature_dim_out (0),
	_stream_tmp (0),
	//_file (0),
	_parsed (false),
	_meta_size (0),
	_meta_data (0) {

	_xmlpipe = ssi_create (XMLPipeline, 0, false);

	if (file && file[0] != '\0') {
		if (!parse (file)) {
			ssi_err ("could not parse chain '%s'", file);
		}
	}
}

Chain::~Chain () {

	release ();
	delete _xmlpipe;
}

void Chain::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {	

	// create temporary streams, init them and call transform_enter for each filter/feature
	_stream_tmp_num = 1 + _n_filters + (_n_features > 0 ? 1 : 0);
	_stream_tmp = new ssi_stream_t *[_stream_tmp_num];		
	_stream_tmp[0] = &stream_in; // first is always the input stream
	for (ssi_size_t i = 1; i < _stream_tmp_num - 1; i++) { // create temporary streams
		_stream_tmp[i] = new ssi_stream_t;
	}
	_stream_tmp[_stream_tmp_num-1] = &stream_out; // last is always the output stream
	for (ssi_size_t i = 0; i < _n_filters; i++) {
		if (i+1 < _stream_tmp_num-1) {
			ssi_stream_init (*_stream_tmp[i+1], 0, _filters[i]->getSampleDimensionOut (_stream_tmp[i]->dim), _filters[i]->getSampleBytesOut (_stream_tmp[i]->byte), _stream_tmp[i]->type, _stream_tmp[i]->sr);											
		}
		_filters[i]->transform_enter (*_stream_tmp[i], *_stream_tmp[i+1]);
	}
	if (_n_features > 0) {
		_feature_dim_out = new ssi_size_t[_n_features];
		_feature_dim_out_tot = _stream_tmp[_stream_tmp_num-1]->dim;
		for (ssi_size_t i = 0; i < _n_features; i++) {			
			_feature_dim_out[i] = _features[i]->getSampleDimensionOut (_stream_tmp[_stream_tmp_num-2]->dim);
			_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out[i];
			_features[i]->transform_enter (*_stream_tmp[_stream_tmp_num-2], *_stream_tmp[_stream_tmp_num-1]);
		}
		_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out_tot;
	}
}
	
void Chain::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	// adjust arrays
	_stream_tmp[0] = &stream_in; // first is always the input stream
	ssi_size_t sample_number = stream_in.num;
	for (ssi_size_t i = 1; i < _stream_tmp_num - 1; i++) {
		ssi_stream_adjust (*_stream_tmp[i], sample_number);			
	}
	_stream_tmp[_stream_tmp_num-1] = &stream_out; // last is always the output stream

	// transform data
	for (ssi_size_t i = 0; i < _n_filters; i++) {
		_filters[i]->transform (info, *_stream_tmp[i], *_stream_tmp[i+1]);			
	}
	if (_n_features > 0) {
		ssi_byte_t *ptr_tmp = _stream_tmp[_stream_tmp_num-1]->ptr;
		for (ssi_size_t i = 0; i < _n_features; i++) {
			_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out[i];				
			_features[i]->transform (info, *_stream_tmp[_stream_tmp_num-2], *_stream_tmp[_stream_tmp_num-1]);		
			_stream_tmp[_stream_tmp_num-1]->ptr += _stream_tmp[_stream_tmp_num-1]->byte * _feature_dim_out[i];
		}
		_stream_tmp[_stream_tmp_num-1]->ptr = ptr_tmp;
		_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out_tot;	
	}
}

void Chain::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	// destroy arrays and flush transformer
	_stream_tmp[0] = &stream_in; // first is always the input stream
	_stream_tmp[_stream_tmp_num-1] = &stream_out; // last is always the output stream
	for (ssi_size_t i = 0; i < _n_filters; i++) {
		_filters[i]->transform_flush (*_stream_tmp[i], *_stream_tmp[i+1]);
	}
	if (_n_features > 0) {
		for (ssi_size_t i = 0; i < _n_features; i++) {
			_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out[i];				
			_features[i]->transform_flush (*_stream_tmp[_stream_tmp_num-2], *_stream_tmp[_stream_tmp_num-1]);	
		}
		_stream_tmp[_stream_tmp_num-1]->dim = _feature_dim_out_tot;	
	}
	for (ssi_size_t i = 1; i < _stream_tmp_num - 1; i++) {
		ssi_stream_destroy (*_stream_tmp[i]);
		delete[] _stream_tmp[i];
	}

	delete[] _stream_tmp; _stream_tmp = 0;
	delete[] _feature_dim_out; _feature_dim_out = 0;
}

ssi_size_t Chain::getSampleDimensionOut (ssi_size_t sample_dimension_in) {
	return calc_sample_dimension_out (sample_dimension_in);
}

ssi_size_t Chain::getSampleNumberOut (ssi_size_t sample_number_in) {
	return calc_sample_number_out (sample_number_in);
}

ssi_size_t Chain::getSampleBytesOut (ssi_size_t sample_bytes_in) {
	return calc_sample_bytes_out (sample_bytes_in);
}

ssi_type_t Chain::getSampleTypeOut (ssi_type_t sample_type_in) {
	return calc_sample_type_out (sample_type_in);
}

ssi_size_t Chain::calc_sample_bytes_out (ssi_size_t sample_bytes_in) {

	ssi_size_t result = sample_bytes_in;

	if (_n_filters > 0) {
		for (ssi_size_t i = 0; i < _n_filters; i++) {
			result = _filters[i]->getSampleBytesOut (result);
		}
	} 
	if (_n_features > 0) {		
		ssi_size_t tmp = result;
		result = _features[0]->getSampleBytesOut (result);
		for (ssi_size_t i = 1; i < _n_features; i++) {
			if (result != _features[i]->getSampleBytesOut (tmp)) {
				ssi_err ("inconsistent chain: number of bytes do not agree");
			}
		}
	}

	return result;
}

ssi_size_t Chain::calc_sample_dimension_out (ssi_size_t sample_dimension_in) {

	ssi_size_t result = sample_dimension_in;

	if (_n_filters > 0) {
		for (ssi_size_t i = 0; i < _n_filters; i++) {
			result = _filters[i]->getSampleDimensionOut (result);
		}
	} 
	if (_n_features > 0) {
		ssi_size_t tmp = result;
		result = 0;
		for (ssi_size_t i = 0; i < _n_features; i++) {
			result += _features[i]->getSampleDimensionOut (tmp);
		}
	}

	return result;
}

ssi_size_t Chain::calc_sample_number_out (ssi_size_t sample_number_in) {

	ssi_size_t result = sample_number_in;

	if (_n_features > 0) {
		result = 1;
	}
		
	return result;
}

ssi_type_t Chain::calc_sample_type_out (ssi_type_t sample_type_in) {

	ssi_type_t result = sample_type_in;

	for (ssi_size_t i = 0; i < _n_filters; i++) {
		result = _filters[i]->getSampleTypeOut (result);
	}
	for (ssi_size_t i = 0; i < _n_features; i++) {
		result = _features[i]->getSampleTypeOut (result);
	}

	return result;
}

bool Chain::parse (const ssi_char_t *filepath) {

	release ();

	if (filepath == 0 || filepath[0] == '\0') {
		ssi_wrn ("file is empty");
		return false;
	}

	FilePath fp (filepath);
	ssi_char_t *filepath_with_ext = 0;
	if (strcmp (fp.getExtension (), SSI_FILE_TYPE_CHAIN) != 0) {
		filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_CHAIN);
	} else {
		filepath_with_ext = ssi_strcpy (filepath);
	}

	if (!ssi_exists (filepath_with_ext)) {
		ssi_wrn ("file not found '%s", filepath_with_ext);
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_BASIC, "load '%s'", filepath_with_ext);

	TiXmlDocument doc;
	if (!doc.LoadFile (filepath_with_ext)) {
		ssi_wrn ("failed loading chain from file '%s'", filepath_with_ext);
		delete[] filepath_with_ext;
		return false;
	}

	TiXmlElement *body = doc.FirstChildElement();	
	if (!body || strcmp (body->Value (), "chain") != 0) {
		ssi_wrn ("tag <chain> missing");
		delete[] filepath_with_ext;
		return false;	
	}

	TiXmlElement *filter = body->FirstChildElement ("filter");
	if (filter) {
		if (!parseFilter (filter)) {
			ssi_wrn ("failed parsing <filter> tag");
			return false;
		}
	}

	TiXmlElement *feature = body->FirstChildElement ("feature");
	if (feature) {
		if (!parseFeature (feature)) {
			ssi_wrn ("failed parsing <feature> tag");
			return false;
		}
	}

	_parsed = feature || filter;
	if (!_parsed) {
		ssi_wrn ("parsing failed because no feature/filter were loaded");
	}

	return _parsed;
}

bool Chain::parseFilter (TiXmlElement *element) {

	int n_filters = 0;
	element->QueryIntAttribute ("size", &n_filters);

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "found %u filter", n_filters); 

	if (n_filters > 0) {
		_n_filters = n_filters;
		_filters = new IFilter *[_n_filters];
		for (ssi_size_t i = 0; i < _n_filters; i++) {
			_filters[i] = 0;
		}
		TiXmlElement *item = 0;
		for (ssi_size_t i = 0; i < _n_filters; i++) {
			if (i == 0) {
				item = element->FirstChildElement ("item");	
			} else {
				item = item->NextSiblingElement ("item");
			}
			if (!item) {
				ssi_wrn ("filter: failed parsing '%u'th <item> tag", i);
				return false;
			}
			IObject *object =_xmlpipe->parseObject (item, false);
			if (!object) {
				ssi_wrn ("filter: class not found");
				return false;
			}
			if (object->getType () != SSI_FILTER) {
				ssi_wrn ("filter: class is not a filter");
				return false;
			}
			IFilter *filter = ssi_pcast (IFilter, object);
			if (!filter) {
				ssi_wrn ("filter: failed loading filter object");
				return false;
			}

			ssi_msg (SSI_LOG_LEVEL_DETAIL, "load %u. filter '%s'", i+1, object->getName ()); 

			_filters[i] = filter;			
		}
	}

	return true;
}

bool Chain::parseFeature (TiXmlElement *element) {

	int n_features = 0;
	element->QueryIntAttribute ("size", &n_features);

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "found %u feature", n_features); 

	if (n_features > 0) {
		_n_features = n_features;
		_features = new IFeature *[_n_features];
		for (ssi_size_t i = 0; i < _n_features; i++) {
			_features[i] = 0;
		}
		TiXmlElement *item = 0;
		for (ssi_size_t i = 0; i < _n_features; i++) {
			if (i == 0) {
				item = element->FirstChildElement ("item");	
			} else {
				item = item->NextSiblingElement ("item");
			}
			if (!item) {
				ssi_wrn ("feature: failed parsing '%u'th <item> tag", i);
				return false;
			}
			IObject *object = _xmlpipe->parseObject (item, false);
			if (!object) {
				ssi_wrn ("filter: class not found");
				return false;
			}
			if (object->getType () != SSI_FEATURE) {
				ssi_wrn ("feature: class is not a feature");
				return false;
			}
			IFeature *feature = ssi_pcast (IFeature, object);
			if (!feature) {
				ssi_wrn ("feature: failed loading feature object");
				return false;
			}

			ssi_msg (SSI_LOG_LEVEL_DETAIL, "load %u. feature '%s'", i+1, object->getName ()); 

			_features[i] = feature;
		}
	}

	return true;
}

void Chain::release () {

	if (_parsed) {
		
		for (ssi_size_t i = 0; i < _n_filters; i++) {
			delete _filters[i];
		}
		delete[] _filters;
		_filters = 0;
		_n_filters = 0;

		for (ssi_size_t i = 0; i < _n_features; i++) {
			delete _features[i];
		}
		delete[] _features;
		_features = 0;
		_n_features = 0;

		_parsed = false;
	}
	
	delete[] _meta_data;
	_meta_data = 0;
	_meta_size = 0;
}

}
