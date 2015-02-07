// TcpSocket.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2009/11/02
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

#include "ioput/socket/ip/TcpSocket.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

TcpSocket::TcpSocket()
{
	m_sock  = INVALID_SOCKET;
	m_allow = INADDR_ANY;
	m_accept_callback = 0;
}

TcpSocket::~TcpSocket()
{
	close();
}

bool TcpSocket::create()
{
	m_sock = socket( AF_INET, SOCK_STREAM, 0 ) ;	

	return m_sock != INVALID_SOCKET;
}

bool TcpSocket::connect(DWORD ip, int port)
{
	SOCKADDR_IN sockaddr;

	if(m_sock == INVALID_SOCKET)
		return false;

	if(ip == 0 || ip == -1)
		return false;

	sockaddr.sin_family           = AF_INET;
	sockaddr.sin_port             = htons (port);
	sockaddr.sin_addr.S_un.S_addr = htonl (ip);
	
	if(::connect(m_sock, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0)
	{
		return false;
	}
	else
	{
		m_connected = true;
		return true;
	}
}

bool TcpSocket::connect(const char *hostname, int port)
{
	return connect(GetHostByName (hostname), port);
}

bool TcpSocket::shutdown(int how)
{
	if(m_sock != INVALID_SOCKET)
	{
		HANDLE hEvent = CreateEvent(0, FALSE, FALSE, 0);
		int ret;

		if(hEvent == 0)
			return false;

		WSAEventSelect(m_sock, hEvent, FD_CLOSE);

		// shutdown the socket. If it was connected then wait 5 seconds 
		// socket to properly close
		if((ret = ::shutdown(m_sock, how)) == 0)
		{
			WaitForSingleObject(hEvent, 3000);
		}

		//int ret = ::closesocket(m_sock);

		//m_sock      = INVALID_SOCKET;
		m_connected = false;

		CloseHandle(hEvent);

		return ret == 0;
	}
	else
	{
		return false;
	}
}



bool TcpSocket::close()
{
	SOCKET tmp_sock = m_sock;

	if(InterlockedExchange((PLONG)&m_sock, INVALID_SOCKET) != INVALID_SOCKET)
	{
		int ret = ::closesocket(tmp_sock);

		m_connected = false;

		return ret == 0;
	}
	else
	{
		return false;
	}
}

/*bool TcpSocket::close()
{
	m_connected = false;
	return ::shutdown(m_sock, how) == 0 ? true : false;
}*/

bool TcpSocket::create(SOCKET sock)
{
	if(m_sock == INVALID_SOCKET)
	{
		m_sock = sock;
		return true;
	}
	else
	{
		return false;
	}
}

bool TcpSocket::listen(int port, DWORD ip /*= INADDR_ANY*/)
{
	char			 localname[128];
	SOCKADDR_IN		 sockaddr;
	struct hostent * pent;

	if(m_sock == INVALID_SOCKET)
		return false;

	if(gethostname(localname, sizeof(localname)) != 0)
		return false;

	if((pent = gethostbyname(localname)) == 0)
		return false;

	sockaddr.sin_family				= AF_INET;
	sockaddr.sin_port				= htons(port);
	sockaddr.sin_addr.S_un.S_addr	= htonl(ip); 

	if(::bind(m_sock, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) != 0)
		return false;

	if(::listen(m_sock, 0) != 0)
		return false;
	
	return true;
}

bool TcpSocket::accept (TcpSocket **newsock, DWORD *clientip)
{
	SOCKET				new_sock;
	struct sockaddr_in	sockaddr;
	int					namelen;
	DWORD				sourceip;

	namelen = sizeof(sockaddr);

	if(m_sock == INVALID_SOCKET)
		return false;

	new_sock = ::accept (m_sock, (struct sockaddr *) &sockaddr, &namelen);

	if(new_sock == INVALID_SOCKET)
		return false;

	sourceip = ntohl(sockaddr.sin_addr.S_un.S_addr);
	
	if(m_allow != INADDR_ANY && sourceip != m_allow)
	{
		if(m_accept_callback)
			m_accept_callback(sourceip, m_accept_param);

		closesocket(new_sock);
		return false;
	}

	if(clientip != 0)
	{
		*clientip = sourceip;
	}

	*newsock = new TcpSocket(new_sock);

	return true;
}

int TcpSocket::select(long timeout)
{
	struct	fd_set  recv_set;
	struct	timeval tv ;
	
	tv.tv_sec  = timeout;
	tv.tv_usec = 0;
	
	FD_ZERO(&recv_set);
	FD_SET(m_sock, &recv_set);
	
	return ::select(0, &recv_set, 0, 0, &tv);
}

int TcpSocket::recv(void *buf, int num_bytes, long timeout)
{
	int recv_result;

	if(timeout != INFINITE)
	{
		int	result = select(timeout);
	
		if(result == 0)
			return 0;
	
		if(result == SOCKET_ERROR)
			return SOCKET_ERROR;
	}

	recv_result = ::recv(m_sock, (char *)buf, num_bytes, 0);

	//if(recv_result == 0)
	//	return 0;//SOCKET_BROKEN;

	return  recv_result;
}

int TcpSocket::recv_exact (void *buf, int num_bytes, long timeout)
{
	int len, received = 0;

	do
	{
		len = recv((BYTE *)buf + received, num_bytes - received, timeout);
		timeout = INFINITE;
		
		if(len > 0)
			received  += len;

	} while(len > 0 && received < num_bytes);

	return len > 0 ? num_bytes : len;
}

int TcpSocket::send (const void *buf, int num_bytes) {
	return ::send (m_sock, (char *) buf, num_bytes, 0);	
}

bool TcpSocket::connected()
{
	return m_connected;
}

bool TcpSocket::getpeer(DWORD *ip, int *port)
{
	struct sockaddr_in	sockaddr;
	int		namelen = sizeof(sockaddr);

	if(!getpeername(m_sock, (struct sockaddr *)&sockaddr, &namelen))
	{
		if(ip)   *ip   = ntohl(sockaddr.sin_addr.S_un.S_addr);
		if(port) *port = ntohs(sockaddr.sin_port);
		return true;
	}
	else
		return false;
}

bool TcpSocket::getself(DWORD *ip, int *port)
{
	return true;
}

void TcpSocket::allow(DWORD ip/*= INADDR_ANY*/, acceptproc ap /*=0*/, DWORD param)
{
	m_allow = ip;
	m_accept_callback = ap;
	m_accept_param    = param;
}

}
