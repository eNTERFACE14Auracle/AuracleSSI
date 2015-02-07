// OptionList.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/03/17
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

#include "ioput/option/OptionList.h"
#include "ioput/file/FilePath.h"

namespace ssi {

ssi_char_t *OptionList::ssi_log_name = "options___";
const char OptionList::SEPARATOR = ',';

OptionList::OptionList () {
}

OptionList::~OptionList () {

	std::vector<ssi_option_t *>::iterator iter;
	for (iter = _list.begin (); iter != _list.end (); iter++) {
		delete[] (*iter)->help;
		delete[] (*iter)->name;
		delete *iter;
	}
};

ssi_option_t *OptionList::getOption (const char *name) {

	ssi_option_t *option = 0;

	bool flag = false;
	std::vector<ssi_option_t *>::iterator iter;
	for (iter = _list.begin (); iter != _list.end (); iter++) {
		if (strcmp ((*iter)->name, name) == 0) {
			option = *iter;
			break;
		}
	}

	return option;
}

bool OptionList::addOption (const char *name, 
	void *ptr, 
	ssi_size_t num, 
	ssi_type_t type, 
	const char *help) {
	
	if (OptionList::getOption (name)) {
		ssi_wrn ("an option with name '%s' already exists", name);
		return false;
	}

	ssi_option_t *option = new ssi_option_t;
	
	option->name = Strcpy (name);
	option->help = Strcpy (help);
	option->ptr = ptr;
	option->num = num;
	option->type = type;

	_list.push_back (option);

	return true;
}

bool OptionList::setOptionValue (const char *name, 
	void *ptr) {

	ssi_option_t *option = getOption (name);
	if (!option) {
		return false;
	}

	memcpy (option->ptr, ptr, option->num * GetTypeSize (option->type));

	return true;
}

bool OptionList::getOptionValue (const char *name, 
	void *ptr) {

	ssi_option_t *option = getOption (name);
	if (!option) {
		return false;
	}

	memcpy (ptr, option->ptr, option->num * GetTypeSize (option->type));

	return true;
}

ssi_option_t *OptionList::getOption (ssi_size_t index) {

	if (index >= getSize ()) {
		return 0;
	}

	return _list.at (index);
}

inline unsigned int OptionList::getSize () {

	return ssi_cast (ssi_size_t, _list.size ());
}

inline char *OptionList::Strcpy (const char *string) {
	char *result = new char[strlen (string) + 1];
	ssi_strcpy (result, string);
	return result;
}

void OptionList::print (FILE *file) {
	
	std::vector<ssi_option_t *>::iterator iter;
	for (iter = _list.begin (); iter != _list.end (); iter++) {
		ssi_char_t *value = OptionList::ToString (*(*iter));
		fprintf (file, "%s:%s -> %s [%s]\n", (*iter)->name, SSI_TYPE_NAMES[(*iter)->type], value, (*iter)->help);						
		delete[] value;
	}
}

inline ssi_size_t OptionList::GetTypeSize (ssi_type_t type) {

	switch (type) {
		case SSI_BOOL:
			return sizeof (bool);
		case SSI_CHAR:
			return sizeof (char);
		case SSI_INT:
			return sizeof (int);
		case SSI_UINT:
			return sizeof (unsigned int);
		case SSI_FLOAT:
			return sizeof (float);
		case SSI_DOUBLE:
			return sizeof (double);
	}

	return 0;
}

FILE *OptionList::OpenXML (const ssi_char_t *filepath) {

	FilePath fp (filepath);
	ssi_char_t *filepath_with_ext = 0;
	if (strcmp (fp.getExtension (), SSI_FILE_TYPE_OPTION) != 0) {
		filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_OPTION);
	} else {
		filepath_with_ext = ssi_strcpy (filepath);
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "load options from '%s'", filepath_with_ext);
		
	FILE *file = fopen (filepath_with_ext, "r");
	if (!file) {
		ssi_wrn ("could not open file '%s'", filepath_with_ext);
	}
	
	delete[] filepath_with_ext;
	return file;
}

bool OptionList::LoadXML (const ssi_char_t *filepath, IOptions &list) {

	FILE *file = OpenXML (filepath);
	bool success = false;
	if (file) {
		success = OptionList::LoadXML (file, list);			
		fclose (file);	
	}
	return success;
}

bool OptionList::SaveXML (const ssi_char_t *filepath, IOptions &list) {

	FilePath fp (filepath);
	ssi_char_t *filepath_with_ext = 0;
	if (strcmp (fp.getExtension (), SSI_FILE_TYPE_OPTION) != 0) {
		filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_OPTION);
	} else {
		filepath_with_ext = ssi_strcpy (filepath);
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "save options to '%s'", filepath_with_ext);

	FILE *file = fopen (filepath_with_ext, "w");
	if (!file) {
		ssi_wrn ("could not open file '%s'", filepath_with_ext);
		delete[] filepath_with_ext;
		return false;
	}
	if (!OptionList::SaveXML (file, list)) {	
		delete[] filepath_with_ext;
		return false;
	}
	fclose (file);	

	delete[] filepath_with_ext;

	return true;
}


bool OptionList::SetOptionValueInPlace (const ssi_char_t *filepath, 
	const char *name, 
	const char *string) {

	FilePath fp (filepath);
	ssi_char_t *filepath_with_ext = 0;
	if (strcmp (fp.getExtension (), SSI_FILE_TYPE_OPTION) != 0) {
		filepath_with_ext = ssi_strcat (filepath, SSI_FILE_TYPE_OPTION);
	} else {
		filepath_with_ext = ssi_strcpy (filepath);
	}

	FILE *file = OpenXML (filepath_with_ext);
	bool success = false;
	if (file) {
		
		TiXmlDocument doc;
		doc.LoadFile (file);

		TiXmlElement *body = doc.FirstChildElement();	
		if (body && strcmp (body->Value (), "options") == 0) {
			TiXmlElement *item = body->FirstChildElement ();
			for ( item; item; item = item->NextSiblingElement()) {
				if (strcmp (item->Value (), "item") == 0 && strcmp (name, item->Attribute ("name")) == 0) {
					item->SetAttribute ("value", string);
					success = true;
					break;
				}
			}
		}

		fclose (file);	

		if (success) {
			doc.SaveFile (filepath_with_ext);
		} else {
			ssi_wrn ("setting option '%s' in file '%s' failed", name, filepath_with_ext);
		}
	}

	delete[] filepath_with_ext;
	return success;
}

bool OptionList::LoadXML (FILE *file, IOptions &list) {

	TiXmlDocument doc;
	doc.LoadFile (file);

	TiXmlElement *body = doc.FirstChildElement();	
	if (!body || strcmp (body->Value (), "options") != 0) {
		ssi_wrn ("tag <options> missing");
		return false;	
	}

	TiXmlElement *item = body->FirstChildElement ();
	for ( item; item; item = item->NextSiblingElement()) {

		if (strcmp (item->Value (), "item") != 0) {
			ssi_wrn ("tag <%s> ignored", item->Value ());
			continue;
		}

		const char *name = item->Attribute ("name");
		ssi_option_t *o = list.getOption (name);
		if (o) {						
			ssi_type_t type = SSI_UNDEF;
			ssi_name2type (item->Attribute ("type"), type);
			if (o->type != ssi_cast (ssi_type_t, type)) {
				ssi_err ("incompatible option type ('%s')", name);
			}
			int num = 0;
			item->QueryIntAttribute ("num", &num);
			if (o->num != ssi_cast (ssi_size_t, num)) {
				ssi_err ("incompatible option size ('%s')", name);
			}			
			OptionList::FromString (item->Attribute ("value"), *o);		
		} else {
			ssi_wrn ("option '%s' ignored", name);
			continue;
		}
	}


	return true;
}

bool OptionList::LoadBinary (FILE *file, IOptions &list) {

	std::vector<ssi_option_t *>::iterator iter;
	for (ssi_size_t i = 0; i < list.getSize (); i++) {

		ssi_option_t &option = *list.getOption (i);
		ssi_size_t res = ssi_cast (ssi_size_t, fread (option.ptr, ssi_type2bytes(option.type), option.num, file));
		if(res != option.num)
			ssi_wrn ("fread() failed");
	}
	return true;
}

bool OptionList::SaveXML (FILE *file, IOptions &list) {

	TiXmlDocument doc;

	TiXmlDeclaration head ("1.0", "", "");
	doc.InsertEndChild (head);
	TiXmlElement body ("options");	
	std::vector<ssi_option_t *>::iterator iter;
	for (ssi_size_t i = 0; i < list.getSize (); i++) {
		TiXmlElement item ("item" );
		ssi_option_t &option = *list.getOption (i);
		item.SetAttribute ("name", option.name);								
		item.SetAttribute ("type", SSI_TYPE_NAMES[option.type]);		
		item.SetAttribute ("num", option.num);
		char *value = OptionList::ToString (option);
		item.SetAttribute ("value", value);	
		delete[] value;
		item.SetAttribute ("help", option.help);
		body.InsertEndChild (item);
	}
	doc.InsertEndChild (body);
	doc.SaveFile (file);

	return true;
}

bool OptionList::SaveBinary (FILE *file, IOptions &list) {

	std::vector<ssi_option_t *>::iterator iter;
	for (ssi_size_t i = 0; i < list.getSize (); i++) {

		ssi_option_t &option = *list.getOption (i);
		ssi_size_t res = ssi_cast (ssi_size_t, fwrite (option.ptr, ssi_type2bytes(option.type), option.num, file));
		if(res != option.num)
			ssi_wrn ("fwrite() failed");
	}
	return true;
}

bool OptionList::setOptionValueFromString (const char *name, 
	const char *string) {

	ssi_option_t *option = getOption (name);
	if (!option) {
		ssi_wrn ("could not find an option with name '%s'", name);
		return false;
	}

	return FromString (string, *option);
}

bool OptionList::FromString (const char *str, ssi_option_t &option) {

	char *token_begin = 0;
	char *token_end = ssi_ccast (char *, str);
	ssi_size_t token_pos = 0;	

	if (option.type == SSI_CHAR && option.num > 1) {
		size_t len = strlen (str) + 1;
		if (option.num < len) {
			ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
			return false;
		}
		memcpy (option.ptr, str, len);
		return true;
	}

	for (ssi_size_t i = 0; i < option.num; i++) {

		token_begin = token_end;
		while (*token_end != SEPARATOR && *token_end != '\0')
			++token_end;

		*token_end = '\0';
		switch (option.type) {
			case SSI_BOOL: {							
				if (strcmp (token_begin, "true") == 0 || strcmp (token_begin, "True") == 0) {
					*(ssi_pcast (bool, option.ptr) + i) = true;
				} else if (strcmp (token_begin, "false") == 0 || strcmp (token_begin, "False") == 0) {					
					*(ssi_pcast (bool, option.ptr) + i) = false;
				} else {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}				
				break;
			}
			case SSI_CHAR : {		
				/*int tmp;
				if (EOF == sscanf (token_begin, "%d", &tmp)) {
					ssi_err ("could not parse token");
				}
				*(ssi_pcast (char, option.ptr) + i) = tmp;*/
				*(ssi_pcast (char, option.ptr) + i) = *token_begin;
				break;
			}
			case SSI_UCHAR : {		
				unsigned int tmp;
				if (EOF == sscanf (token_begin, "%u", &tmp)) {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}
				*(ssi_pcast (unsigned char, option.ptr) + i) = tmp;
				break;
			}
		   case SSI_INT : {		
				if (EOF == sscanf (token_begin, "%d", ssi_pcast (int, option.ptr) + i)) {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}
				break;
			}
			case SSI_UINT : {		
				if (EOF == sscanf (token_begin, "%u", ssi_pcast (unsigned int, option.ptr) + i)) {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}
				break;
			}
			case SSI_FLOAT : {		
				if (EOF == sscanf (token_begin, "%e", ssi_pcast (float, option.ptr) + i)) {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}
				break;
			}
			case SSI_DOUBLE : {		
				if (EOF == sscanf (token_begin, "%le", ssi_pcast (double, option.ptr) + i)) {
					ssi_wrn ("could not parse token '%s' of option '%s'", str, option.name);
					return false;
				}
				break;
			}			
			default:
				ssi_wrn ("unkown option type '%d'", ssi_cast (int, option.type));
				return false;
		}

		++token_end;

		if ((token_end == '\0' && i != option.num) || (i == option.num && token_end != '\0'))  {
			ssi_wrn ("number of tokens does not match");
			return false;
		}
	}

	return true;
}

char *OptionList::ToString (ssi_option_t &option) {

	char *string_h = new char[1];
	string_h[0] = '\0';
	char *string = 0;
	char tmp[SSI_MAX_CHAR];	
	
	switch (option.type) {
		case SSI_BOOL: {
			bool *ptr = ssi_pcast (bool, option.ptr);
			sprintf (tmp, "%s", *ptr++ ? "true" : "false");
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (ssi_size_t i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%s", SEPARATOR, *ptr++ ? "true" : "false");
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}		
		case SSI_CHAR: {
			char *ptr = ssi_pcast (char, option.ptr);
			string = new char[option.num + 1];
			memcpy (string, option.ptr, option.num);
			string[option.num] = '\0';
			delete[] string_h;
			break;
		}	
		case SSI_UCHAR: {
			unsigned char *ptr = ssi_pcast (unsigned char, option.ptr);
			sprintf (tmp, "%u", ssi_cast (int, *ptr++));
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (ssi_size_t i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%d", SEPARATOR, ssi_cast (int, *ptr++));
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}
		case SSI_INT: {
			int *ptr = ssi_pcast (int, option.ptr);
			sprintf (tmp, "%d", *ptr++);
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (ssi_size_t i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%d", SEPARATOR, *ptr++);
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}			
		case SSI_UINT: {
			unsigned int *ptr = ssi_pcast (unsigned int, option.ptr);
			sprintf (tmp, "%u", *ptr++);
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (ssi_size_t i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%u", SEPARATOR, *ptr++);
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}
		case SSI_FLOAT: {
			float *ptr = ssi_pcast (float, option.ptr);
			sprintf (tmp, "%e", *ptr++);
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (ssi_size_t i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%e", SEPARATOR, *ptr++);
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}
		case SSI_DOUBLE: {
			double *ptr = ssi_pcast (double, option.ptr);
			sprintf (tmp, "%le", *ptr++);
			string = ssi_strcat (string_h, tmp);
			delete[] string_h;
			for (unsigned int i = 1; i < option.num; i++) {								
				string_h = ssi_strcpy (string);				
				delete[] string;
				sprintf (tmp, "%c%le", SEPARATOR, *ptr++);
				string = ssi_strcat (string_h, tmp);
				delete[] string_h;
			}
			break;
		}
		default:
			ssi_err ("type not supported");
	}

	return string;

}

}

