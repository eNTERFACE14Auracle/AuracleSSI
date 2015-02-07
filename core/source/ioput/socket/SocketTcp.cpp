// SocketTcp.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/09/11
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

#include "ioput/socket/SocketTcp.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

SocketTcp::SocketTcp () 
	: _recv_from_host (0),
	_socket (0),
	_client (0) {

	_recv_from_host = new ssi_char_t[IpEndpointName::ADDRESS_AND_PORT_STRING_LENGTH];
	_recv_from_host[0] = '\0';
}

SocketTcp::~SocketTcp () {

	if (_is_connected) {
		disconnect ();
	}

	delete[] _recv_from_host;
}

bool SocketTcp::connect () {

	if (_socket) {
		ssi_wrn ("trying to connect a socket that is already connected");
		return false;
	}

	_socket = new TcpSocket ();
	if (!_socket->create ()) {
		ssi_wrn ("could not create socket [tcp]");
		return false;
	}

	switch (_mode) {

		case SocketTcp::SEND: {

			if (!_socket->connect (_ip.address, _ip.port)) {
				ssi_wrn ("could not connect to server '%s' [tcp]", _ipstr);
				return false;
			}
			
			ssi_msg (SSI_LOG_LEVEL_BASIC, "streaming to '%s' [tcp]", _ipstr);

			break;
		}
		case SocketTcp::RECEIVE: {

			if (!_socket->listen (_ip.port)) {
				ssi_wrn ("listening on port %d failed [tcp]", _ip.port);
				return false;
			}

			ssi_msg (SSI_LOG_LEVEL_BASIC, "waiting for client at port %d", _ip.port);

			if (!_socket->accept (&_client)) {
				ssi_wrn ("could not accept a client [tcp]");
				return false;
			}			
			
			_client->getpeer (&_recv_from.address, &_recv_from.port);
			_recv_from.AddressAndPortAsString (_recv_from_host);

			ssi_msg (SSI_LOG_LEVEL_BASIC, "established connection to client '%s'", _recv_from_host);

			break;
		}
	}

	_is_connected = true;

	return true;
}

bool SocketTcp::disconnect () {

	if (!_socket) {
		ssi_wrn ("trying to disconnected a socket that is not connected");
		return false;
	} 

	_socket->shutdown (SD_BOTH);
	_socket->close();
	delete _client; _client = 0;
	delete _socket; _socket = 0;
	_is_connected = false;

	ssi_msg (SSI_LOG_LEVEL_BASIC, "closed [udp]", _ipstr);
	return true;
}

int SocketTcp::recv (void *ptr, ssi_size_t size, long timeout) {

	if (!_is_connected) {
		ssi_wrn ("socket not connected");
		return 0;
	}

	if (_mode != SocketTcp::RECEIVE) {
		ssi_wrn ("you cannot read from a sending socket");
		return 0;
	}

	int result = _client->recv  (ptr, size, timeout);

	if (result == SOCKET_ERROR) {
		ssi_wrn ("receive() failed");
		return 0;
	} else {
		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "received %d bytes", result);
	}

	return result;
}

int SocketTcp::send (const void *ptr, ssi_size_t size) {

	if (!_is_connected) {
		ssi_wrn ("socket not connected");
		return 0;
	}

	if (_mode != SocketTcp::SEND) {
		ssi_wrn ("you cannot write from a receiving socket");
		return 0;
	}

	int result;
	{
		Lock lock (_send_mutex);		
		result = _socket->send (ptr, size);
	}

	if (result == SOCKET_ERROR) {
		ssi_wrn ("send() failed\n");		
		return 0;
	} else {
		SSI_DBG (SSI_LOG_LEVEL_DEBUG, "sent %d bytes", result);
	}

	return result;
}

const char *SocketTcp::getRecvAddress () {

	return _recv_from_host;
}

}
