/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Network layer for Wormux.
 *****************************************************************************/

// Standard header, only needed for the following method
#ifdef WIN32
#  include <winsock2.h>
#else
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <resolv.h>
#  include <errno.h>
#  include <unistd.h>
#endif

//-----------------------------------------------------------------------------
#include <SDL_thread.h>
#include <SDL_timer.h>
#include <WORMUX_error.h>
#include <WORMUX_i18n.h>
#include <WORMUX_index_svr_msg.h>
#include <WORMUX_network.h>
#include <WORMUX_socket.h>

#include <iostream>
#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif
//-----------------------------------------------------------------------------

static const std::string WORMUX_VERSION = PACKAGE_VERSION;

bool WNet::sdlnet_initialized = false;

//-----------------------------------------------------------------------------

void WNet::Init()
{
  if (sdlnet_initialized) {
    return;
  }

  if (SDLNet_Init()) {
    Error("Failed to initialize network library! (SDL_Net)");
    exit(1);
  }
  sdlnet_initialized = true;

  std::cout << "o " << _("Network initialization") << std::endl;
}

void WNet::Quit()
{
  if (!sdlnet_initialized) {
    return;
  }

  SDLNet_Quit();
  sdlnet_initialized = false;
}


//-----------------------------------------------------------------------------

#ifdef WIN32
# define SOCKET_PARAM    char
#else
typedef int SOCKET;
# define SOCKET_PARAM    void
# define SOCKET_ERROR    (-1)
# define INVALID_SOCKET  (-1)
# define closesocket(fd) close(fd)

// For Mac OS X
#ifndef AI_NUMERICSERV
#define AI_NUMERICSERV 0
#endif

#endif

// static method
connection_state_t WNet::GetError()
{
#ifdef WIN32
  int code = WSAGetLastError();
  switch (code)
  {
  case WSAECONNREFUSED: return CONN_REJECTED;
  case WSAEINPROGRESS:
  case WSAETIMEDOUT: return CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", code);
    return CONN_BAD_SOCKET;
  }
#else
  switch(errno)
  {
  case ECONNREFUSED: return CONN_REJECTED;
  case EINPROGRESS:
  case ETIMEDOUT: return CONN_TIMEOUT;
  default:
    fprintf(stderr, "Generic network error of code %i\n", errno);
    return CONN_BAD_SOCKET;
  }
#endif
}

// static method
#ifdef WIN32

static connection_state_t WIN32_CheckHost(const std::string &host, int prt)
{
  struct hostent* hostinfo;
  hostinfo = gethostbyname(host.c_str());
  if( ! hostinfo )
    return CONN_BAD_HOST;

  SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
  if( fd == INVALID_SOCKET )
    return CONN_BAD_SOCKET;

  // Set the timeout
  int timeout = 5000; //ms

  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
  {
    fprintf(stderr, "Setting receive timeout on socket failed\n");
    return CONN_BAD_SOCKET;
  }

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
  {
    fprintf(stderr, "Setting send timeout on socket failed\n");
    return CONN_BAD_SOCKET;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(prt);
  addr.sin_addr.s_addr = inet_addr(inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list));

  if( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR )
  {
    return WNet::GetError();
  }
  closesocket(fd);
  return CONNECTED;
}

#else

static connection_state_t POSIX_CheckHost(const std::string &host, int prt)
{
  connection_state_t s;
  int r;
  SOCKET sfd;
  char port[10];
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;      /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;  /* TCP protocol only */
  hints.ai_flags =  AI_NUMERICSERV; /* Service is a numeric port number */
  hints.ai_protocol = IPPROTO_TCP;

  snprintf(port, 10, "%d", prt);

  r = getaddrinfo(host.c_str(), port, &hints, &result);
  if (r != 0) {

    fprintf(stderr, "getaddrinfo returns %d\n", r);

    switch (r) {
    case EAI_ADDRFAMILY:
      fprintf(stderr, "The specified network host does not have any network addresses in the requested address family.\n");
      break;
    case EAI_AGAIN:
      fprintf(stderr, "The name server returned a temporary failure indication.  Try again later.\n");
      break;
    case EAI_BADFLAGS:
      fprintf(stderr, "ai_flags contains invalid flags.\n");
      break;
    case EAI_FAIL:
      fprintf(stderr, "The name server returned a permanent failure indication.\n");
      break;
    case EAI_FAMILY:
      fprintf(stderr, "The requested address family is not supported at all.\n");
      break;
    case EAI_MEMORY:
      fprintf(stderr, "Out of memory.\n");
      break;
    case EAI_NODATA:
      fprintf(stderr, "The specified network host exists, but does not have any network addresses defined.\n");
      break;
    case EAI_NONAME:
      fprintf(stderr, "The node or service is not known; or both node and service are NULL; "
	      "or AI_NUMERICSERV was specified in hints.ai_flags and  service  was  not  a  numeric port-number string.\n");
      break;
    case EAI_SERVICE:
      fprintf(stderr, "The requested service is not available for the requested socket type.  It may be available through another socket type.\n");
      break;
    case EAI_SOCKTYPE:
      fprintf(stderr, "The requested socket type is not supported at all.\n");
      break;
    case EAI_SYSTEM:
      fprintf(stderr, "Other system error, check errno for details.\n");
      break;
    }

    if (r == EAI_NONAME) {
      s = CONN_BAD_HOST;
      goto err_addrinfo;
    }

    s = CONN_BAD_SOCKET;
    goto err_addrinfo;
  }

  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully connect(2).
     If socket(2) (or connect(2)) fails, we (close the socket
     and) try the next address. */
  for (rp = result; rp != NULL; rp = rp->ai_next) {

    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (sfd == -1)
      continue;

    // Try to set the timeout
    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 5; // 5seconds timeout
    if (setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
      fprintf(stderr, "Setting receive timeout on socket failed\n");
      closesocket(sfd);
      continue;
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, (SOCKET_PARAM*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
      fprintf(stderr, "Setting send timeout on socket failed\n");
      closesocket(sfd);
      continue;
    }

    if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
      break; /* Success */

    closesocket(sfd);
  }

  if (rp == NULL) { /* No address succeeded */
    fprintf(stderr, "Could not connect\n");
    s = CONN_BAD_SOCKET;
    goto error;
  }

  closesocket(sfd);

  s = CONNECTED;

 error:
  freeaddrinfo(result); /* No longer needed */

 err_addrinfo:
  return s;
}
#endif

connection_state_t WNet::CheckHost(const std::string &host, int prt)
{
#ifdef WIN32
  return WIN32_CheckHost(host, prt);
#else
  return POSIX_CheckHost(host, prt);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

uint WNet::Batch(void* buffer, const int& nbr)
{
  ASSERT(sdlnet_initialized);

  // this is not cute, but we don't want an int -> uint conversion here
  Uint32 u_nbr = *((const Uint32*)&nbr);

  SDLNet_Write32(u_nbr, buffer);

  return sizeof(u_nbr);
}

unsigned int WNet::Batch(void* buffer, const std::string &str)
{
  uint size = str.size();
  Batch(buffer, size);
  memcpy(((char*)buffer)+sizeof(Uint32), str.c_str(), size);
  return sizeof(Uint32)+size;
}

void WNet::FinalizeBatch(void* data, size_t len)
{
  SDLNet_Write32(len, (void*)( ((char*)data)+4 ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool WNet::Server_HandShake(WSocket& client_socket,
			    const std::string& game_name,
			    const std::string& password,
			    std::string& nickname,
			    uint client_player_id,
			    bool client_will_be_master)
{
  bool ret = false;
  std::string version;
  std::string _password;

  // Adding the socket to a temporary socket set
  if (!client_socket.AddToTmpSocketSet())
    goto error_no_socket_set;

  // 1) Receive the version number
  if (!client_socket.ReceiveStr(version, 40))
    goto error;

  if (!client_socket.SendStr(WORMUX_VERSION))
    goto error;

  if (WORMUX_VERSION != version) {
    std::cerr << "Client disconnected: wrong version " << version.c_str()
	     << std::endl;
    goto error;
  }

  // 2) Check the password
  if (!client_socket.ReceiveStr(_password, 100))
    goto error;

  if (_password != password) {
    std::cerr << "Client disconnected: wrong password " << _password.c_str()
	      << std::endl;
    client_socket.SendInt(1);
    goto error;
  }

  // Server: password OK
  if (!client_socket.SendInt(0))
    goto error;

  // 3) Server: client will be master ?
  if (!client_socket.SendInt(client_will_be_master))
    goto error;

  // 4) Get nickname of the client
  if (!client_socket.ReceiveStr(nickname, 100))
    goto error;

  // 5) Send gamename to the client
  if (!client_socket.SendStr(game_name))
    goto error;

  // 6) Send its player id to the client
  if (!client_socket.SendInt(client_player_id))
    goto error;

  // Server: Handshake done successfully :)
  ret = true;

 error:
  client_socket.RemoveFromTmpSocketSet();
 error_no_socket_set:
  if (!ret) {
    std::cerr << "Server: HandShake with client has failed!" << std::endl;
  }
  return ret;
}

