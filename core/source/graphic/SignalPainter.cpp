// SignalPainter.cpp
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

#include "graphic/SignalPainter.h"
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

const ssi_char_t *SignalPainter::ssi_log_name = "sigpainter_";
int SignalPainter::ssi_log_level = SSI_LOG_LEVEL_DEFAULT;

SignalPainter::SignalPainter (const ssi_char_t *file)
	: _plot (0),
	_n_plots (0),
	_plot_id (0),		
	_stream_type (0),
	_file (0) {

	_painter = Factory::GetPainter ();

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

SignalPainter::~SignalPainter () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void SignalPainter::consume_enter (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	_n_plots = stream_in_num;
	_stream_type = new ssi_type_t[_n_plots];
	_plot_id = new int[_n_plots];
	_plot = new PaintObject *[_n_plots];

	for (ssi_size_t i = 0; i < _n_plots; i++) {

		_stream_type[i] = stream_in[i].type;	

		switch (_stream_type[i]) {
			case SSI_CHAR:
				_plot[i] = new PaintSignal<char> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<char>, _plot[i])->setFixedMinMax ((char) _options.fix[0], (char) _options.fix[1]);
				}					
				ssi_pcast (PaintSignal<char>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_UCHAR:
				_plot[i] = new PaintSignal<unsigned char> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<unsigned char>, _plot[i])->setFixedMinMax ((unsigned char) _options.fix[0], (unsigned char) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<unsigned char>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_SHORT:
				_plot[i] = new PaintSignal<short> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<short>, _plot[i])->setFixedMinMax ((short) _options.fix[0], (short) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<short>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_USHORT:
				_plot[i] = new PaintSignal<unsigned short> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<unsigned short>, _plot[i])->setFixedMinMax ((unsigned short) _options.fix[0], (unsigned short) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<unsigned short>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_INT:
				_plot[i] = new PaintSignal<int> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<int>, _plot[i])->setFixedMinMax ((int) _options.fix[0], (int) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<int>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_UINT:
				_plot[i] = new PaintSignal<unsigned int> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<unsigned int>, _plot[i])->setFixedMinMax ((unsigned int) _options.fix[0], (unsigned int) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<unsigned int>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_LONG:
				_plot[i] = new PaintSignal<long> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<long>, _plot[i])->setFixedMinMax ((long) _options.fix[0], (long) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<long>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_ULONG:
				_plot[i] = new PaintSignal<unsigned long> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<unsigned long>, _plot[i])->setFixedMinMax ((unsigned long) _options.fix[0], (unsigned long) _options.fix[1]);
				}			
				ssi_pcast (PaintSignal<unsigned long>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_FLOAT:
				_plot[i] = new PaintSignal<float> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<float>, _plot[i])->setFixedMinMax ((float) _options.fix[0], (float) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<float>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;
			case SSI_DOUBLE:
				_plot[i] = new PaintSignal<double> (stream_in[i].sr, stream_in[i].dim, _options.size, _options.type, _options.indx, _options.indy);
				if (!_options.autoscale) {
					ssi_pcast (PaintSignal<double>, _plot[i])->setFixedMinMax ((double) _options.fix[0], (double) _options.fix[1]);
				}				
				ssi_pcast (PaintSignal<double>, _plot[i])->setAxisPrecision(_options.axisPrecision);

				break;				
			default:
				ssi_err ("type not supported");
		}				

		ssi_char_t string[SSI_MAX_CHAR];
		if (_n_plots > 1) {
			ssi_sprint (string, "%s#%02d", _options.name, i);
		} else {
			ssi_sprint (string, "%s", _options.name);
		}
		_plot_id[i] = _painter->AddCanvas (string, _options.colormap);
		_painter->AddObject (_plot_id[i], _plot[i]);		

		if (_options.move[0] != 0) {
			if (_options.move[1] != 0) {
				_painter->MoveEX (_plot_id[i], _options.move[2], _options.move[3], _options.move[4], _options.move[5], true);
			} else {
				_painter->Move (_plot_id[i], _options.move[2], _options.move[3], _options.move[4], _options.move[5]);
			}
		}

		if (_options.arrange[0] != 0) {
			_painter->Arrange (_options.arrange[1], _options.arrange[2], _options.arrange[3], _options.arrange[4], _options.arrange[5], _options.arrange[6]);
		}
	}
}

void SignalPainter::consume (IConsumer::info consume_info,
	ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	for (ssi_size_t i = 0; i < _n_plots; i++) {
		if (_options.reset) {
			_plot[i]->rescale ();
		}
		_plot[i]->setData (stream_in[i].ptr, stream_in[i].num, consume_info.time);
 		_painter->Paint (_plot_id[i]);
	}
}

void SignalPainter::consume_flush (ssi_size_t stream_in_num,
	ssi_stream_t stream_in[]) {

	for (ssi_size_t i = 0; i < _n_plots; i++) {
		_painter->Clear (_plot_id[i]);
		delete _plot[i];
	}
	delete[] _plot_id; _plot_id = 0;
	delete[] _plot; _plot = 0;
	delete[] _stream_type; _stream_type = 0;
	_n_plots = 0;
}

}
