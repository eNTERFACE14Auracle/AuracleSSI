// SignalPainter.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/02/08
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

#ifndef SSI_GRAPHIC_SIGNALPAINTER_H
#define SSI_GRAPHIC_SIGNALPAINTER_H

#include "base/IConsumer.h"
#include "base/IThePainter.h"
#include "graphic/PaintSignal.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class SignalPainter : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options ()
			: type (PaintSignalType::SIGNAL), size (0), colormap (COLORMAP_TYPE_COLOR64), indx (0), indy (1), reset (false), autoscale (true), axisPrecision(3) {

			fix[0] = 0.0;
			fix[1] = 1.0;
			name[0] = '\0';		
			memset (move, 0, 6 * sizeof (int));	
			memset (arrange, 0, 7 * sizeof (int));		
			addOption ("name", name, SSI_MAX_CHAR, SSI_CHAR, "plot name");
			addOption ("type", &type, 1, SSI_UCHAR, "plot type (0=signal, 1=image, 2=audio, 3=path)");					
			addOption ("size", &size, 1, SSI_DOUBLE, "displayed stream size in seconds (0 for dynamic)");
			addOption ("reset", &reset, 1, SSI_BOOL, "always reset y axis to local minimum and maximum, otherwise a global minium and maximum is used");
			addOption ("colormap", &colormap, 1, SSI_UCHAR, "plot type (0=black/white, 1=color16, 2=color64, 3=gray64)");	
			addOption ("move", move, 6, SSI_INT, "move window (on/off, accurate, top, left, width, height)");
			addOption ("arrange", arrange, 7, SSI_INT, "arrange windows (on/off, numh, numv, top, left, width, height)");
			addOption ("indx", &indx, 1, SSI_SIZE, "dimension of x coordinate (for path drawing only)");			
			addOption ("indy", &indy, 1, SSI_SIZE, "dimension of y coordinate (for path drawing only)");			
			addOption ("autoscale", &autoscale, 1, SSI_BOOL, "use autoscale (see fix)");			
			addOption ("fix", fix, 2, SSI_DOUBLE, "if autoscale if off set y axis to fix interval (fix[0] fix[1])");			
			addOption ("axisPrecision", &axisPrecision, 1, SSI_SIZE, "precision for axis values");							
		};

		void setName (const ssi_char_t *name) {
			ssi_strcpy (this->name, name);
		}
		void setMove (int top, int left, int width, int height, bool accurate = false) {
			move[0] = 1; move[1] = accurate ? 1 : 0; move[2] = top; move[3] = left; move[4] = width; move[5] = height;
		}
		void setArrange (int numh, int numv, int top, int left, int width, int height) {
			arrange[0] = 1; arrange[1] = numh; arrange[2] = numv; arrange[3] = top; arrange[4] = left; arrange[5] = width; arrange[6] = height;
		}

		ssi_char_t name[SSI_MAX_CHAR];
		PaintSignalType::TYPE type;
		ssi_time_t size;
		COLORMAP_TYPE colormap;		
		int move[6];
		int arrange[7];
		bool reset;
		ssi_size_t indx;
		ssi_size_t indy;
		bool autoscale;
		double fix[2];
		ssi_size_t axisPrecision;
	};

public: 

	static const ssi_char_t *GetCreateName () { return "ssi_consumer_SignalPainter"; };
	static IObject *Create (const ssi_char_t *file) { return new SignalPainter (file); };
	~SignalPainter ();
	
	SignalPainter::Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Viewer for signal streams."; };
	
	template<class T>
	PaintSignal<T> &getObject ();

	void setLogLevel (int level) {
		ssi_log_level = level;
	}

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

protected:

	SignalPainter (const ssi_char_t *file = 0);
	SignalPainter::Options _options;
	ssi_char_t *_file;

	IThePainter *_painter;
	ssi_size_t _n_plots;
	int *_plot_id;
	PaintObject **_plot;
	ssi_type_t *_stream_type;

	static const ssi_char_t *ssi_log_name;
	static int ssi_log_level;
};

template<class T>
PaintSignal<T> &SignalPainter::getObject () {

	if (plot) {

		switch (_stream_type) {
		
			case SSI_CHAR:
				return *ssi_pcast (PaintSignal<char>, _paint);
			case SSI_UCHAR:
				return *ssi_pcast (PaintSignal<unsigned char>, _paint);
			case SSI_SHORT:
				return *ssi_pcast (PaintSignal<short>, _paint);
			case SSI_USHORT:
				return *ssi_pcast (PaintSignal<unsigned short>, _paint);
			case SSI_INT:
				return *ssi_pcast (PaintSignal<int>, _paint);
			case SSI_UINT:
				return *ssi_pcast (PaintSignal<unsigned int>, _paint);
			case SSI_LONG:
				return *ssi_pcast (PaintSignal<long>, _paint);				
			case SSI_ULONG:
				return *ssi_pcast (PaintSignal<unsigned long>, _paint);
			case SSI_FLOAT:
				return *ssi_pcast (PaintSignal<float>, _paint);				
			case SSI_DOUBLE:
				return *ssi_pcast (PaintSignal<double>, _paint);
			default:
				ssi_err ("type not supported");
		}
	}

	return 0;
}

}

#endif
