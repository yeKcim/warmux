/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#ifndef NETWORK_H 
#define NETWORK_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <SDL_thread.h>
#include "../include/base.h" 
#include <vector>
#include <string>
#include "../include/action.h" 
//-----------------------------------------------------------------------------

class Network
{
public:
	typedef enum
	{
		NETWORK_NOT_CONNECTED,
		NETWORK_SERVER_INIT_GAME,
		NETWORK_WAIT_CLIENTS,
		NETWORK_WAIT_SERVER,
		NETWORK_WAIT_MAP,
		NETWORK_WAIT_TEAMS,
		NETWORK_PLAYING
	} network_state_t;
	network_state_t state;
		
private:
  TCPsocket socket;
  TCPsocket client;
  SDL_Thread* thread;
	
	bool m_is_connected;
	bool m_is_server;
	bool m_is_client;

	// Server Connection
	Action* make_action(Uint32* packet);

public:

	Network();
	~Network();
	void Init();
	
	bool is_connected();
	bool is_local();
	bool is_server();
	bool is_client();
	
	void disconnect();
	
	void client_connect(const std::string &host, const std::string &port);

	void server_start(const std::string &port);

   void SendAction(const Action &action);
   void ReceiveActions();
};

extern Network network;
//-----------------------------------------------------------------------------
#endif
