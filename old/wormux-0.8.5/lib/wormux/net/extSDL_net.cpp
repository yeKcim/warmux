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

/******************************************************************************/

/******************************************************************************
 * The following code comes from XMoto
 * It has been fixed for Wormux (handles of errno == EAGAIN)
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
		len = send(sock->channel, (const char *) data, left, MSG_DONTWAIT);
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

// SDLNet_SocketReady MUST have been called before
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
