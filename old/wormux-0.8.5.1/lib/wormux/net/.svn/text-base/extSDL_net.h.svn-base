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
 * Functions extending SDL_Net
 *****************************************************************************/

#ifndef __EXTSDL_NET_H__
#define __EXTSDL_NET_H__

#include <SDL_net.h>

/*
  from SDL_net-1.2.7
  this file add SDLNet_TCP_Send_noBlocking function to sdl_net
  because the function SDLNet_TCP_Send is blocking
  only the MSG_NOWAIT flag was added
*/

// SDLNet_TCP_Send_noBlocking BLOCKS on some Operating system (such as WIN32)
// that do not define MSG_DONTWAIT
int SDLNet_TCP_Send_noBlocking(TCPsocket sock, const void *datap, int len);

/* SDLNet_SocketReady MUST have been called before
 * return false if an error happens and host should be disconnected
 *
 * WARNING: under WIN32, it returns the size of data that can be read with ONE
 * read(). It does not equal to the size of available data if there is lot
 * of data. (The maximum seems to be 8192)
 */
int SDLNet_TCP_NbBytesAvailable(TCPsocket sock);

#endif
