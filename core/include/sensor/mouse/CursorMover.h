// CursorMover.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2010/04/25
// Copyright (C) University of Augsburg

#ifndef SSI_CONSUMER_CURSORMOVER_H
#define SSI_CONSUMER_CURSORMOVER_H

#include "base/IConsumer.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class CursorMover : public IConsumer {

public:

	class Options : public OptionList {

	public:

		Options () : scale (false), flipv (true), fliph (false), indx (0), indy (1), minx (0), miny (0), maxx (0), maxy (0)  {

			addOption ("scale", &scale, 1, SSI_BOOL, "scale to screen size");
			addOption ("fliph", &fliph, 1, SSI_BOOL, "flip horizontally");	
			addOption ("flipv", &flipv, 1, SSI_BOOL, "flip vertically");	
			addOption ("minx", &minx, 1, SSI_REAL, "minimum value of x coordinate");	
			addOption ("miny", &miny, 1, SSI_REAL, "minimum value of y coordinate");	
			addOption ("maxx", &maxx, 1, SSI_REAL, "maximum value of x coordinate");	
			addOption ("maxy", &maxy, 1, SSI_REAL, "maximum value of y coordinate");	
			addOption ("indx", &indx, 1, SSI_INT, "dimension of x coordinate (0)");			
			addOption ("indy", &indy, 1, SSI_INT, "dimension of y coordinate (1)");		
			addOption ("skip", &skip, 1, SSI_REAL, "skip if x or y coordinate is equal to this value (0)");
		}

		bool scale; 
		bool flipv;
		bool fliph;
		ssi_size_t indx;
		ssi_size_t indy;
		ssi_real_t minx;
		ssi_real_t miny;
		ssi_real_t maxx;
		ssi_real_t maxy;
		ssi_real_t skip;
	};


	static const ssi_char_t *GetCreateName () { return "ssi_consumer_CursorMover"; };
	static IObject *Create (const ssi_char_t *file) { return new CursorMover (file); };
	~CursorMover ();

	IOptions *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "Controls mouse cursor."; };

	void consume_enter (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume (IConsumer::info consume_info,
		ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);
	void consume_flush (ssi_size_t stream_in_num,
		ssi_stream_t stream_in[]);

protected:

	CursorMover (const ssi_char_t *file = 0);
	ssi_char_t *_file;
	Options _options;

	int _max_x, _max_y;
	
};

}

#endif
