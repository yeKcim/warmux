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
 * Network basic layer for Wormux.
 *****************************************************************************/

#ifndef WORMUX_NETWORK_H
#define WORMUX_NETWORK_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <string>
#include <WORMUX_types.h>
//-----------------------------------------------------------------------------

const std::string WORMUX_NETWORK_PORT = "3826";
const uint WORMUX_NETWORK_PORT_INT = 3826;

typedef enum
{
  CONNECTED,
  CONN_BAD_HOST,
  CONN_BAD_PORT,
  CONN_BAD_SOCKET,
  CONN_REJECTED,
  CONN_TIMEOUT,
  CONN_WRONG_PASSWORD,
  CONN_WRONG_VERSION
} connection_state_t;

class WSocket;

class WNet
{
public:
  typedef enum {
    NO_NETWORK,
    NETWORK_MENU_INIT,
    NETWORK_MENU_OK,
    NETWORK_LOADING_DATA,
    NETWORK_READY_TO_PLAY,
    NETWORK_PLAYING,
    NETWORK_NEXT_GAME
  } net_game_state_t;

private:
  static bool sdlnet_initialized;

public:
  static void Init();
  static void Quit();

  static connection_state_t CheckHost(const std::string &host, int prt);
  static connection_state_t GetError();

  // A batch consists in a msg id, a size, and the batch itself.
  // Size wasn't known yet, so write it now.
  static uint Batch(void* buffer, const int& nbr);
  static uint Batch(void* buffer, const std::string &str);
  static void FinalizeBatch(void* buffer, size_t len);

  static bool Server_HandShake(WSocket& client_socket,
			       const std::string& game_name,
			       const std::string& password,
			       std::string& client_nickname,
			       uint client_player_id,
			       bool client_will_be_master);
};

//-----------------------------------------------------------------------------
#endif
