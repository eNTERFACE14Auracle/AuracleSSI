// SocketTcp.h
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

#pragma once

#ifndef SSI_IOPUT_SOCKETTCP_H
#define SSI_IOPUT_SOCKETTCP_H

#include "ioput/socket/Socket.h"
#include "ioput/socket/ip/TcpSocket.h"
#include "ioput/socket/ip/IpEndpointName.h"
#include "thread/Lock.h"

#include <winsock2.h>
#ifdef _MSC_VER 
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Winmm.lib")
#endif

namespace ssi {

class SocketTcp : public Socket {

friend class Socket;

public:

	bool connect ();
	bool disconnect ();
	int recv (void *ptr, ssi_size_t size, long timeout_in_ms = INFINITE_TIMEOUT);
	int send (const void *ptr, ssi_size_t size);
	const char *getRecvAddress ();

protected:

	SocketTcp ();
	virtual ~SocketTcp ();

	TcpSocket *_client;
	TcpSocket *_socket;	
	ssi_char_t *_recv_from_host;
	IpEndpointName _recv_from;
	Mutex _send_mutex;
};

}

#endif
