/*=============================================================================
XMOTO

This file is part of XMOTO.

XMOTO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

XMOTO is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with XMOTO; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
=============================================================================*/

#include <errno.h>

/******************************************************************************
 * From SDLNet 1.2.7 - SDLnetsys.h
 ******************************************************************************/
/* Include system network headers */
#if defined(__WIN32__) || defined(WIN32)
#  define __USE_W32_SOCKETS
#  include <windows.h>
#  define __PRETTY_FUNCTION__ __FUNCTION__
#else /* UNIX */
#  ifdef __OS2__
#    include <types.h>
#    include <sys/ioctl.h>
#  endif
#  include <sys/time.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <netinet/in.h>
#  ifndef __BEOS__
#    include <arpa/inet.h>
#  endif
#  ifdef linux /* FIXME: what other platforms have this? */
#    include <netinet/tcp.h>
#  endif
#include <netdb.h>
#include <sys/socket.h>
#endif /* WIN32 */

/* System-dependent definitions */
#ifndef __USE_W32_SOCKETS
#  ifdef __OS2__
#    define closesocket     soclose
#  else  /* !__OS2__ */
#    define closesocket	close
#  endif /* __OS2__ */
#  define SOCKET	int
#  define INVALID_SOCKET	-1
#  define SOCKET_ERROR	-1
#endif /* __USE_W32_SOCKETS */

#include "extSDL_net.h"

static const int MAX_PACKET_SIZE = 4096;

/******************************************************************************/

/******************************************************************************
 * The following code comes from XMoto
 * It has been fixed for Wormux:
 *    - handles of errno == EAGAIN
 *    - packets are splitted if bigger than MAX_PACKET_SIZE (may be needed for WIN32)
 ******************************************************************************/

struct _TCPsocket {
	int ready;
	SOCKET channel;
	IPaddress remoteAddress;
	IPaddress localAddress;
	int sflag;
};

// SDLNet_TCP_Send_noBlocking BLOCKS on some Operating system (such as WIN32)
// that do not define MSG_DONTWAIT
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

int SDLNet_TCP_Send_noBlocking(TCPsocket sock, const void *datap, int len)
{
	const Uint8 *data = (const Uint8 *)datap;	/* For pointer arithmetic */
	int sent, left;

	/* Server sockets are for accepting connections only */
	if ( sock->sflag ) {
		SDLNet_SetError("Server sockets cannot send");
		return(-1);
	}

	/* Keep sending data until it's sent or an error occurs */
	left = len;
	sent = 0;
	errno = 0;
	do {
		/* From MSDN documentation about send():
		 *
		 * For message-oriented sockets (address family of AF_INET or
		 * AF_INET6, type of SOCK_DGRAM, and protocol of IPPROTO_UDP,
		 * for example), care must be taken not to exceed the maximum
		 * packet size of the underlying provider. The maximum message
		 * packet size for a provider can be obtained by calling
		 * getsockopt with the optname parameter set to SO_MAX_MSG_SIZE
		 * to retrieve the value of socket option. If the data is too
		 * long to pass atomically through the underlying protocol, the
		 * error  WSAEMSGSIZE is returned, and no data is transmitted.
		 *
		 * Let's say that MAX_PACKET_SIZE is small enough to be
		 * transmitted
		 */
		int tosend = left;
		if (tosend > MAX_PACKET_SIZE)
			tosend = MAX_PACKET_SIZE;

		len = send(sock->channel, (const char *) data, tosend, MSG_DONTWAIT);
		if ( len > 0 ) {
			sent += len;
			left -= len;
			data += len;
		}

	} while ( (left > 0) && ((len > 0) || (errno == EINTR) || (errno == EAGAIN)) );

	return(sent);
}

/******************************************************************************
 * The following code has been written especially for Wormux
 ******************************************************************************/

#ifndef WIN32
# include <sys/ioctl.h>
#endif

/* SDLNet_SocketReady MUST have been called before
 *
 * WARNING: under WIN32, it returns the size of data that can be read with ONE
 * read(). It does not equal to the size of available data if there is lot
 * of data. (The maximum seems to be 8192)
 */
int SDLNet_TCP_NbBytesAvailable(TCPsocket sock)
{
	int r;
	unsigned long nbbytes;

#ifndef WIN32
	r = ioctl(sock->channel, FIONREAD, &nbbytes);
	if (r) {
		int error = errno;
		fprintf(stderr, "ERROR at %s:%d - ioclt: %s (%d)", __PRETTY_FUNCTION__,
			__LINE__, strerror(error), error);
		return r;
	}
#else
	r = ioctlsocket(sock->channel, FIONREAD, &nbbytes);
	if (r) {
		fprintf(stderr, "ERROR at %s:%d - ioctlsocket: %d\n", __PRETTY_FUNCTION__, __LINE__, WSAGetLastError());
		return r;
	}
#endif

	return nbbytes;
}
