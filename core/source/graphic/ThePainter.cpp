// ThePainter.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/07
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

#include "graphic/ThePainter.h"


using namespace std;
#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

ssi_char_t *ssi_log_name = "painter___";

// the window handle
HINSTANCE hInstance = NULL;

// constructor
ThePainter::ThePainter (const ssi_char_t *file)
: _file (0) {

#ifdef _THEPAINTER_DEBUG
	cout << "Initialize painter.. ";
#endif

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	if(Gdiplus::GdiplusStartup(&_gdiplus, &gdiplusStartupInput, NULL) != Gdiplus::Ok)
	{
		ssi_err ("failed call to GdiplusStartup() in ThePainter constructor");
	}

	// get a window handle
	if (!hInstance) {
		hInstance = GetModuleHandle (NULL);
	}

	// register window class
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= Procedure;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= ::LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) ::GetStockObject (BLACK_BRUSH);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= THEPAINTER_WINDOW_CLASS_NAME;
	wcex.hIconSm		= 0;

	if(!RegisterClassEx(&wcex)) {
		ssi_err ("failed to register window (%d)", ::GetLastError ());
    }

    // an array with pointer to all canvas handled by the painter
	for (int i = 0; i < THEPAINTER_MAX_BUFFER; i++)
		_canvas[i] = NULL;

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

#ifdef _THEPAINTER_DEBUG
	cout << "ok" << endl;
#endif
}

// deconstructor
ThePainter::~ThePainter () {

	Clear();	

	// unregister window class
	if (!UnregisterClass (THEPAINTER_WINDOW_CLASS_NAME, hInstance)) {
		ssi_err ("failed to unregister window class (%d)", ::GetLastError ());
	}

	Gdiplus::GdiplusShutdown (_gdiplus);
	_gdiplus = NULL;

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

// shutdown function
void ThePainter::Clear () {

#ifdef _THEPAINTER_DEBUG
printf ("Shutting down painter ..\n");
#endif

    // destroy all active canvas
	for (int i = 0; i < THEPAINTER_MAX_BUFFER; i++) {
        if (!(_canvas[i])) continue;
		RemCanvas (i);
    }

#ifdef _THEPAINTER_DEBUG
printf ("Good bye!\n");
#endif
}

// add a new canvas
int ThePainter::AddCanvas (const char* name, COLORMAP_TYPE colormap_type) {

#ifdef _THEPAINTER_DEBUG
	cout << "Add new _canvas..";
#endif

   	// find a free canvas id
	int canvas_id = -1;
	for (int i = 0; i < THEPAINTER_MAX_BUFFER; i++) {
		// get _mutex for the buffer
		Lock lock (_mutex[i]);
		// check if _canvas is still available
		if (!_canvas[i]) {
			canvas_id = i;
			break;
		}
	}
	if (canvas_id == -1) {
#ifdef _THEPAINTER_DEBUG
{
	Lock lock (logmutex);
	cout << "error [all _canvas in use]";
}
#endif
		return -1;
	}

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// create thread
	_canvas[canvas_id] = new CanvasThread (hInstance, THEPAINTER_WINDOW_CLASS_NAME, name, new Colormap (colormap_type));
	// start thread
	_canvas[canvas_id]->start ();
	// arrange
	if (_options.arrange) {
		Arrange (_options.apos[0], _options.apos[1], _options.apos[2], _options.apos[3], _options.apos[4], _options.apos[5]);
	}

#ifdef _THEPAINTER_DEBUG
cout << "ok" << endl;
#endif

    return canvas_id;
}

// add a new canvas
int ThePainter::AddCanvas (const char* name, const unsigned char *colormap, unsigned int color_num) {

#ifdef _THEPAINTER_DEBUG
	cout << "Add new _canvas..";
#endif

   	// find a free canvas id
	int canvas_id = -1;
	for (int i = 0; i < THEPAINTER_MAX_BUFFER; i++) {
		// get mutex for the buffer
		Lock lock (_mutex[i]);
		// check if canvas is still available
		if (!_canvas[i]) {
			canvas_id = i;
			break;
		}
	}
	if (canvas_id == -1) {
#ifdef _THEPAINTER_DEBUG
{
	Lock lock (logmutex);
	cout << "error [all _canvas in use]";
}
#endif
		return -1;
	}

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// create thread
	_canvas[canvas_id] = new CanvasThread (hInstance, THEPAINTER_WINDOW_CLASS_NAME, name, new Colormap (colormap, color_num));
	// start thread
	_canvas[canvas_id]->start ();
	// arrange
	if (_options.arrange) {
		Arrange (_options.apos[0], _options.apos[1], _options.apos[2], _options.apos[3], _options.apos[4], _options.apos[5]);
	}

#ifdef _THEPAINTER_DEBUG
cout << "ok" << endl;
#endif

    return canvas_id;
}


// removes a canvas
bool ThePainter::RemCanvas (int canvas_id) {

#ifdef _THEPAINTER_DEBUG
cout << "Remove _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// now destroy canvas
	// we do this in three steps:
	// 1. destroying the canvas
	// 2. stopping the thread
	// 3. deleting the thread
	_canvas[canvas_id]->canvas->destroy ();
	_canvas[canvas_id]->stop ();
	delete _canvas[canvas_id];
	_canvas[canvas_id] = 0;

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}


// clears a canvas
bool ThePainter::Clear (int canvas_id) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// clear and update canvas
	_canvas[canvas_id]->canvas->painter->clear ();
	_canvas[canvas_id]->canvas->update ();

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}


// plots on a canvas
bool ThePainter::AddObject (int canvas_id, PaintObject *object) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->canvas->painter->add (object);

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

// updates a canvas
bool ThePainter::Paint (int canvas_id) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->canvas->update ();

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

// resize a canvas
bool ThePainter::Move (int canvas_id, int x, int y, int width, int height) {

#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->canvas->move (x, y, width, height);

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;
}

bool ThePainter::MoveEX (int canvas_id, int x, int y, int width, int height, bool paramsDescribeClientRect, MOVE_EX_TYPE moveAndOrResize)
{
	#ifdef _THEPAINTER_DEBUG
cout << "Plot on _canvas with ID " << canvas_id << ".. ";
#endif

    // first check if ID is valid
    if (canvas_id < 0 || canvas_id >= THEPAINTER_MAX_BUFFER) {
#ifdef _THEPAINTER_DEBUG
cout << "error [invalid _canvas ID " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// now check if canvas is in use
    if (!_canvas[canvas_id]) {
#ifdef _THEPAINTER_DEBUG
cout << "error [_canvas not in use: " << canvas_id << "]" << endl;
#endif
        return false;
    }

	// lock mutex for the canvas
	Lock lock (_mutex[canvas_id]);
	// plot and update
	_canvas[canvas_id]->canvas->moveEX (x, y, width, height, paramsDescribeClientRect, moveAndOrResize);

#ifdef _THEPAINTER_DEBUG
cout <<  "ok" << endl;
#endif

    return true;

}

void ThePainter::Arrange (int numh, int numv, int start_x, int start_y, int width, int height) {
		
	int next_x = start_x;
	int next_y = start_y;
	int numh_left = numh;
	int max_steps = numh * numv;
	int win_width = width / numh;
	int win_height = height / numv;

	for (int i = 0; i < THEPAINTER_MAX_BUFFER; i++) {

		if (max_steps <= 0) {
			break;
		}

		if (_canvas[i]) {
			// lock mutex for the canvas
			Lock lock (_mutex[i]);
			// move canvas
			if (!_canvas[i]->canvas->move (next_x, next_y, win_width, win_height)) {
				ssi_err ("failed to move window (%d)", ::GetLastError ());
			};
			numh_left--;
			max_steps--;
			if (numh_left > 0) {
				next_x += win_width;
			} else {
				next_x = start_x;
				next_y += win_height;
				numh_left = numh;
			}
		}
	}
}

void ThePainter::MoveConsole (int start_x, int start_y, int width, int height) {

	HWND hWnd = GetConsoleWindow();
	::MoveWindow(hWnd, start_x, start_y, width, height, true);
}

}
