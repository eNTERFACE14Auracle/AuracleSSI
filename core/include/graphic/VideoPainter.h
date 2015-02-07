// VideoPainter.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/10/07
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

#ifndef SSI_GRAPHIC_VIDEOPAINTER_H
#define SSI_GRAPHIC_VIDEOPAINTER_H

#include "base/IConsumer.h"
#include "base/IThePainter.h"
#include "graphic/PaintVideo.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class VideoPainter : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options ()
			: scale (true), flip (true), mirror (false), maxValue(-1) {

			name[0] = '\0';
			memset (move, 0, 6 * sizeof (int));			
			memset (arrange, 0, 7 * sizeof (int));	
			addOption ("name", name, SSI_MAX_CHAR, SSI_CHAR, "plot name");
			addOption ("flip", &flip, 1, SSI_BOOL, "flip video image");
			addOption ("mirror", &mirror, 1, SSI_BOOL, "mirror video image");
			addOption ("maxValue", &maxValue, 1, SSI_DOUBLE, "max pixel value for video image");
			addOption ("scale", &scale, 1, SSI_BOOL, "scale video image");
			addOption ("move", move, 6, SSI_INT, "move window (on/off, accurate, top, left, width, height)");
			addOption ("arrange", arrange, 7, SSI_INT, "arrange windows (on/off, numh, numv, top, left, width, height)");
		};

		void setName (const ssi_char_t *name) {
			ssi_strcpy (this->name, name);
		}
		void setMove (int top, int left, int width = -1, int height = -1, bool accurate = true) {
			move[0] = 1; move[1] = accurate ? 1 : 0; move[2] = top; move[3] = left; move[4] = width; move[5] = height;
		}
		void setArrange (int numh, int numv, int top, int left, int width, int height) {
			arrange[0] = 1; arrange[1] = numh; arrange[2] = numv; arrange[3] = top; arrange[4] = left; arrange[5] = width; arrange[6] = height;
		}
		ssi_char_t name[SSI_MAX_CHAR];
		bool flip;
		/*mirror currently only implemented for 16 bit single channel stream and 8 bit 3 channel*/
		bool mirror;
		/*maxvalue currently only implemented for 16 bit single channel stream and 8 bit 3 channel*/
		double maxValue;
		bool scale;
		int move[6];
		int arrange[7];
	};

public: 

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_VideoPainter"; };
	static IObject *Create (const ssi_char_t *file) { return new VideoPainter (file); };
	~VideoPainter ();
	
	VideoPainter::Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Viewer for video streams."; };

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

	void setVideoFormat (ssi_video_params_t video_format) { 
		_video_format = video_format;
	};
	void setMetaData (ssi_size_t size, const void *meta) {
		if (sizeof (_video_format) != size) {
			ssi_err ("invalid meta size");
		}
		memcpy (&_video_format, meta, size);
	}
	ssi_video_params_t getVideoFormat () { return _video_format; };
	PaintVideo &getObject () { return *_plot; };

protected:

	VideoPainter (const ssi_char_t *file = 0);
	VideoPainter::Options _options;
	ssi_char_t *_file;

	IThePainter *_painter;
	int _plot_id;
	PaintVideo *_plot;
	ssi_video_params_t _video_format;

	static const ssi_char_t *ssi_log_name;
	static const ssi_char_t *_name;
	static const ssi_char_t *_info;
};

}

#endif
