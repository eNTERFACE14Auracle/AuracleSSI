// CameraReader.cpp
// author: Frank Jung <frank.jung@informatik.uni-augsburg.de>
// created: 2008/12/03
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

#include "sensor/camera/CameraReader.h"

#include <AtlBase.h>
#include <AtlConv.h>
#include <dshow.h>
#include <Ocidl.h>
#include <OAIDL.H>
#include <comutil.h>
#include <streams.h>
//#include <Qedit.h>
#include <initguid.h>
#include <list>
#include <commdlg.h>
#include <limits>

#include "sensor/camera/CameraTools.h"
#include "sensor/camera/iUAProxyForceGrabber.h"
#include "sensor/camera/UAProxyForceGrabber.h"

#include <iostream>
using namespace std;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

//#define FRANKYS_DEEP_DEBUG_FLAG

#ifndef CLSID_NullRenderer
DEFINE_GUID(CLSID_NullRenderer, 0xC1F400A4, 0x3F08, 0x11d3, 0x9F, 0x0B,
	    0x00, 0x60, 0x08, 0x03, 0x9E, 0x37);
#endif 

namespace ssi {

int CameraReader::ssi_log_level_static = SSI_LOG_LEVEL_DEFAULT;
static char ssi_log_name_static[] = "camreader_";
char *CameraReader::ssi_log_name = "camreader_";

CameraReader::CameraReader(const ssi_char_t *file)
	:_isComInitialized(false), _comInitCount(0), _pGraph(NULL), 
	_pControl(NULL), _pGrabInterface(NULL), _provider (0), _is_providing (false),
	_pFileDevice(NULL), _pFileLoadFilter(NULL), _pBasicVideo(NULL),
	_pGrabber(NULL), _pEvent(NULL), _pMediaSeek(NULL),
	_pFileSourceFilter(NULL), _pPin(NULL),
	_picData(NULL), _picDataTmp (0), _sizeOfPicData(0),
	_timer (0), ssi_log_level (SSI_LOG_LEVEL_DEFAULT),
	_file (0) {

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) 
    {
		if(hr == RPC_E_CHANGED_MODE)
		{
			ssi_msg (SSI_LOG_LEVEL_DETAIL, "Tried to reinitialize COM with different threading model!");
		}
		else
		{
			ssi_err ("Could not initialize COM library in connect()");
		}
    }
	else 
	{
		if(hr == S_FALSE)
		{
			ssi_msg (SSI_LOG_LEVEL_DETAIL, "COM was already initialized for this thread!");
		}
		_isComInitialized = true;
		_comInitCount++;
	}

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

CameraReader::~CameraReader()
{
	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}

	if (_isComInitialized == true) {	
		CoUninitialize();
		--_comInitCount;
	}
	
	delete [] _picData;
	delete [] _picDataTmp;

	_picData = 0;
	_picDataTmp = 0;
	_sizeOfPicData = 0;
}

bool CameraReader::setProvider (const ssi_char_t *name, IProvider *provider) {

	if (strcmp (name, SSI_CAMERAREADER_PROVIDER_NAME) == 0) {
		setProvider (provider);
		return true;
	}

	ssi_wrn ("unkown provider name '%s'", name);

	return false;
}

void CameraReader::setProvider (IProvider *provider) {

	if (!provider) {
		return;
	}
	_provider = provider;

	_cbIH.pInfoHeader = &_bmpIH;
	_cbIH.pDataOfBMP = NULL;
	if(_options.path[0] == '\0')
	{
		ssi_char_t szFile[MAX_PATH];
		OPENFILENAME fileName;
		ZeroMemory(&fileName, sizeof(OPENFILENAME));
		fileName.lStructSize = sizeof(OPENFILENAME);
		fileName.hwndOwner = NULL;
		fileName.nMaxFile = sizeof(szFile);
		fileName.lpstrFilter = "All Files\0*.*\0\0";
		fileName.nFilterIndex = 1;
		fileName.lpstrFileTitle = NULL;
		fileName.nMaxFileTitle = 0;
		fileName.lpstrInitialDir = NULL;
		fileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		while(!GetOpenFileName(&fileName))
		{
			_options.path[0] = '\0';
			//ssi_err("No File selected!");
		}

		size_t strLenVar = strnlen(fileName.lpstrFile, 1024);
		strcpy_s(_options.path, strLenVar+1, fileName.lpstrFile);
	}
	else
	{
		size_t strLenVar = strnlen(_options.path, 1024);
		if(strLenVar == 1024)
		{
			_options.path[0] = '\0';
			ssi_err("String that indicated the File-Name was not NULL-Terminated!!!");
		}
		strcpy_s(_options.path, strLenVar+1, _options.path);
	}

	if(_options.params.heightInPixels == 0 || _options.params.widthInPixels == 0 || _options.params.depthInBitsPerChannel == 0 || _options.params.numOfChannels == 0 || (std::abs(_options.forcefps) <= std::numeric_limits<ssi_time_t>::epsilon()))
	{
		if(!CameraTools::BuildAndDestroyGraphToDetermineFileVideoParams(_options.path, &_options.params))
		{
			ssi_err("Could not determine video file parameters");
		}
		_options.forcefps = _options.params.framesPerSecond;
	}
	else
	{
		memcpy(&_options.params, &_options.params, sizeof(ssi_video_params_t));
	}
	if(std::abs(_options.forcefps) > std::numeric_limits<ssi_time_t>::epsilon())
	{
		_options.forcefps = _options.forcefps;
	}
	_options.params.framesPerSecond = _options.forcefps;
	_options.params.flipImage = _options.flip;

	_provider->setMetaData (sizeof (_options.params), &_options.params);
	ssi_stream_init (_video_channel.stream, 0, 1, ssi_video_size (_options.params), SSI_IMAGE, _options.forcefps);
	provider->init (&_video_channel);


}

bool CameraReader::connect()
{
	
	ssi_msg (SSI_LOG_LEVEL_BASIC, "try to connect sensor(file)...");
	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling InitFilterGraphManager...");

	HRESULT hr = CameraTools::InitFilterGraphManager(&_pGraph);
	if(SUCCEEDED(hr))
	{
		// ssi_msg (SSI_LOG_LEVEL_DETAIL, "...SUCCEEDED!");
	}
	else 
	{
		if(_isComInitialized == true)
		{
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("InitFilterGraphManager");
		return false;
	}
	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling QueryInterfaces...");
	hr = CameraTools::QueryInterfaces(_pGraph, &_pControl, NULL, NULL, NULL, &_pBasicVideo);
	if(SUCCEEDED(hr))
	{
		// ssi_msg (SSI_LOG_LEVEL_DETAIL, "...SUCCEEDED!");
	}
	else 
	{
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("QueryInterfaces");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling AddFilterToGraphByCLSID with CLSID_AsyncReader...");
	hr = CameraTools::AddFilterToGraphByCLSID(_pGraph, CLSID_AsyncReader, L"FileReader", &_pFileLoadFilter);
	if(SUCCEEDED(hr))
	{
		// ssi_msg (SSI_LOG_LEVEL_DETAIL, "...SUCCEEDED!");
	}
	else 
	{
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err("AddFilterToGraphByCLSID");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling QueryInterface with IID_IFileSourceFilter...");
	hr = _pFileLoadFilter->QueryInterface(IID_IFileSourceFilter, (LPVOID *) &_pFileSourceFilter);
	if(SUCCEEDED(hr))
	{
		//ssi_msg (SSI_LOG_LEVEL_DETAIL, "SUCCEEDED!");
	}
	else
	{
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FAILED!");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling IFileSourceFilter::Load...");
	wchar_t fileNameW[1024*2];
	if(!MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, _options.path, -1, fileNameW, MAX_PATH*2))
	{
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err("Character conversion to Unicode failed in BuildAndDestroyGraphToDetermineFileFPS");
		return false;
	}

	hr = _pFileSourceFilter->Load(fileNameW, NULL);	
	if(SUCCEEDED(hr))
	{
		//ssi_msg (SSI_LOG_LEVEL_DETAIL, "SUCCEEDED!" );
	}
	else
	{
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FAILED!");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling GetPin with AsyncFileLoader...");
	_pPin = CameraTools::GetFirstPin(_pFileLoadFilter, PINDIR_OUTPUT);
	if(_pPin)
	{
		//ssi_msg (SSI_LOG_LEVEL_DETAIL, "SUCCEEDED!" );
	}
	else
	{
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FAILED!");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling AddAndConnectFilterByCLSIDToGivenOutputPin with Capture and Grabber...");
	hr = CameraTools::AddAndConnectFilterByCLSIDToGivenOutputPin(CLSID_UAProxyForceGrabber, L"Grabber", _pGraph, _pPin);
	if(SUCCEEDED(hr))
	{
		//ssi_msg (SSI_LOG_LEVEL_DETAIL, "SUCCEEDED!" );
	}
	else
	{
		SafeReleaseFJ(_pPin);
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);

		if(_isComInitialized)
		{
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FAILED");
		return false;
	}

	SafeReleaseFJ(_pPin);


	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Trying to find Grabber by Name in Graph...");
	hr = _pGraph->FindFilterByName(L"Grabber", &_pGrabber);
	if(SUCCEEDED(hr))
	{
		#ifdef FRANKYS_DEEP_DEBUG_FLAG
		cout << "...SUCCEEDED!" << endl;
		#endif
	}
	else
	{
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);

		if(_isComInitialized)
		{
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FindFilterByName with UAProxyForceGrabber");
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling Get Pin with Grabber...");
	_pPin = CameraTools::GetFirstPin(_pGrabber, PINDIR_OUTPUT);
	if(_pPin != NULL)
	{
		#ifdef FRANKYS_DEEP_DEBUG_FLAG
		cout << "...SUCCEEDED!" << endl;
		#endif
	}
	else
	{
		
		SafeReleaseFJ(_pGrabber);
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);

		if(_isComInitialized)
		{
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("Get Output Pin of Grabber Device" );
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Try to Grabber-Interface..."); 
	hr = _pGrabber->QueryInterface(IID_IUAProxyForceGrabber, (LPVOID *)&_pGrabInterface);
	if(SUCCEEDED(hr))
	{
		#ifdef FRANKYS_DEEP_DEBUG_FLAG
		cout << "...SUCCEEDED!" << endl;
		#endif
	}
	else
	{
		cerr << "Query Grabber Interface" << endl;
		SafeReleaseFJ(_pPin);
		SafeReleaseFJ(_pGrabber);
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);

		if(_isComInitialized)
		{
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("Get Grabber Interface" );
		return false;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "Calling ConnectToNullRenderer...");
	hr = CameraTools::ConnectToNullRenderer(_pPin, _pGraph);
	if(SUCCEEDED(hr))
	{
		//ssi_msg_static (SSI_LOG_LEVEL_DETAIL, "SUCCEEDED!" );
	}
	else
	{
		SafeReleaseFJ(_pGrabInterface);
		SafeReleaseFJ(_pGrabber);
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("FAILED!");
		return false;
	}

	SafeReleaseFJ(_pPin);

	SSI_DBG (SSI_LOG_LEVEL_DEBUG, "Activating Framebuffering in Filter...");
	
	hr = _pGrabInterface->ToggleCallFrameBufferingForGrabbing(true);
	if(SUCCEEDED(hr))
	{
		// ssi_msg (SSI_LOG_LEVEL_DETAIL, "...SUCCEEDED!");
	}
	else
	{
		SafeReleaseFJ(_pGrabInterface);
		SafeReleaseFJ(_pGrabber);
		SafeReleaseFJ(_pFileLoadFilter);
		SafeReleaseFJ(_pFileSourceFilter);
		SafeReleaseFJ(_pBasicVideo);
		SafeReleaseFJ(_pControl);
		SafeReleaseFJ(_pGraph);
		if(_isComInitialized == true)
		{
			
			CoUninitialize();
			--_comInitCount;
		}
		ssi_err ("Activation of FrameBuffering for Grabbing");
		return false;
	}		

	ssi_msg (SSI_LOG_LEVEL_BASIC, "connected");
	if (_provider && ssi_log_level >= SSI_LOG_LEVEL_DETAIL) {
			ssi_print ("\
             width\t= %d\n\
             height\t= %d\n\
             frame\t= %.2lf\n\
             dim\t= %u\n\
             bytes\t= %u\n",
		_options.params.widthInPixels,
		_options.params.heightInPixels,
		_options.params.framesPerSecond,
		1, 
		ssi_video_size (_options.params));
	}

	_wait_event.block ();
	_is_providing = true;
	_timer = 0;
	_first_call = true;

	// set thread name
	ssi_char_t string[SSI_MAX_CHAR];
	ssi_sprint (string, "%s@%s", getName (), _options.path);
	Thread::setName (string);

	return true;
}

bool CameraReader::disconnect()
{
	ssi_msg (SSI_LOG_LEVEL_BASIC, "try to disconnect sensor(file)...");

	if(_pControl)
		_pControl->Stop();
	SafeReleaseFJ(_pGrabInterface);
	SafeReleaseFJ(_pGrabber);
	SafeReleaseFJ(_pFileLoadFilter);
	SafeReleaseFJ(_pFileSourceFilter);
	SafeReleaseFJ(_pBasicVideo);
	SafeReleaseFJ(_pControl);
	SafeReleaseFJ(_pGraph);

	delete _timer;	
	_timer = 0;
	_wait_event.block ();

	ssi_msg (SSI_LOG_LEVEL_BASIC, "sensor(file) disconnected");

	return true;
}

void CameraReader::run()
{
	HRESULT hr;

	if (_first_call) {

		ssi_msg (SSI_LOG_LEVEL_DETAIL, "Running Graph...");
	
		hr = _pControl->Run();
		if(!SUCCEEDED(hr))	{
			SafeReleaseFJ(_pGrabInterface);
			SafeReleaseFJ(_pGrabber);
			SafeReleaseFJ(_pFileLoadFilter);
			SafeReleaseFJ(_pFileSourceFilter);
			SafeReleaseFJ(_pBasicVideo);
			SafeReleaseFJ(_pControl);
			SafeReleaseFJ(_pGraph);
			if(_isComInitialized == true)
			{
			
				CoUninitialize();
				--_comInitCount;
			}
			ssi_err ("could not run graph");			
		}
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "run graph");
		_first_call = false;
	}

	bool flip = _options.flip;
	bool mirror = _options.mirror;

	int oldSizeOfPicData = _sizeOfPicData;
	
	hr = NOERROR;
	if (_is_providing) {
		hr = _pGrabInterface->GrabFrame(_picData, &_sizeOfPicData,(void*) &_cbIH);	
	}	

	switch(hr)
	{
		case E_PENDING:
			_wait_event.release ();
			break;

		case NOERROR:
			
			if (flip) {

				int stride = ssi_video_stride (_options.params);
				int height = _options.params.heightInPixels;
				int copyLength = _options.params.widthInPixels * 3;
				BYTE *dstptr = _picDataTmp + (height - 1) * stride;
				BYTE *srcptr = _picData;
				for(int j = 0; j < height; ++j)
				{
					memcpy(dstptr, srcptr, copyLength);
					dstptr -= stride;
					srcptr += stride;
				}
				BYTE *tmp = _picDataTmp;
				_picDataTmp = _picData;
				_picData = tmp;
			}

			if (mirror) {
				BYTE *dstptr = 0;
				const BYTE *srcptr = 0;
				int height = _options.params.heightInPixels;
				int width = _options.params.widthInPixels;
				int stride = ssi_video_stride (_options.params);
				for(int j = 0; j < height; ++j)
				{
					dstptr = _picDataTmp + j * stride;
					srcptr = _picData + j * stride + (width - 1) * 3;
					for (int i = 0; i < width; i++)
					{
						memcpy(dstptr, srcptr, 3);
						dstptr +=3;
						srcptr -=3;
					}
				}
				BYTE *tmp = _picData;
				_picData = _picDataTmp;
				_picDataTmp = tmp;
			}

			_is_providing = _provider->provide(reinterpret_cast<char *>(_picData), SSI_CAMERA_SAMPLES_PER_STEP_TO_BUFFER);
			//ssi_print ("_is_providing == %s\n", _is_providing ? "true" : "false");

			OAFilterState state;
			if (!_is_providing) {				
				_pControl->GetState (INFINITE, &state);
				if (state == State_Running) {
					ssi_msg (SSI_LOG_LEVEL_DETAIL, "pause graph");
					_pControl->Pause ();
				}
				Sleep (10);
				return;
			}	
			_pControl->GetState (INFINITE, &state);
			if (state == State_Paused) {
				ssi_msg (SSI_LOG_LEVEL_DETAIL, "restart graph");
				_pControl->Run ();
			}			

			if (!_options.best_effort_delivery) {

				if (!_timer) {
					_timer = new Timer (1.0 / _options.forcefps);
				}
				_timer->wait ();
			}	

			break;

		case E_FAIL:

			Sleep (10);
			break;

		case E_ABORT:

			if(oldSizeOfPicData != _sizeOfPicData)
			{				
				delete [] _picData; _picData = 0;
				delete [] _picDataTmp; _picDataTmp = 0;
				_picData = new BYTE[_sizeOfPicData];				
				_picDataTmp = new BYTE[_sizeOfPicData];				
			}
			break;

		case E_POINTER:

			if((_picData == NULL) && (_sizeOfPicData > 0))
			{
				_picData = new BYTE[_sizeOfPicData];				
				_picDataTmp = new BYTE[_sizeOfPicData];				
			}
			break;

		default:
			break;
	}
}

void CameraReader::wait () {
	_wait_event.wait ();
}

ssi_video_params_t CameraReader::getFormat (const ssi_char_t *filepath) { 
	ssi_video_params_t video_format;
	CameraTools::BuildAndDestroyGraphToDetermineFileVideoParams(filepath, &video_format);
	return video_format;
};

}
