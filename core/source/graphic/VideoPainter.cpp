// VideoPainter.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/04
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

#include "graphic/VideoPainter.h"
#include "base/Factory.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

const ssi_char_t *VideoPainter::ssi_log_name = "vidpainter";

VideoPainter::VideoPainter (const ssi_char_t *file)
	: _plot_id (-1),
	_plot (0),
	_file (0) {

	_painter = Factory::GetPainter ();
	
	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

VideoPainter::~VideoPainter () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void VideoPainter::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	if (_video_format.framesPerSecond != stream_in[0].sr) {		
		_video_format.framesPerSecond = stream_in[0].sr;
		ssi_wrn ("sample rate has been adjusted");
	}

	if (ssi_video_size (_video_format) != stream_in[0].byte) {
		ssi_err ("input stream doesn't match video format");
	}

	if (stream_in[0].dim != 1) {
		ssi_wrn ("supports only one dimension");
	}

	_plot = new PaintVideo (_video_format, _options.flip, _options.scale, _options.mirror, _options.maxValue);
	_plot_id = _painter->AddCanvas (_options.name);
	_painter->AddObject (_plot_id, _plot);

	if (_options.move[0] != 0) {
		if (_options.move[1] != 0) {
			_painter->MoveEX (_plot_id, _options.move[2], _options.move[3], _options.move[4] < 0 ? _video_format.widthInPixels : _options.move[4], _options.move[5] < 0 ? _video_format.heightInPixels : _options.move[5], true);
		} else {
			_painter->Move (_plot_id, _options.move[2], _options.move[3],  _options.move[4] < 0 ? _video_format.widthInPixels : _options.move[4], _options.move[5] < 0 ? _video_format.heightInPixels : _options.move[5]);
		}
	}

	if (_options.arrange[0] != 0) {
		_painter->Arrange (_options.arrange[1], _options.arrange[2], _options.arrange[3], _options.arrange[4], _options.arrange[5], _options.arrange[6]);
	}
}

void VideoPainter::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	_plot->setData (stream_in[0].ptr, stream_in[0].num, consume_info.time);
 	_painter->Paint (_plot_id);
}

void VideoPainter::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	_painter->Clear (_plot_id);
	delete _plot;
}

}
