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
#include "../include/base.h" 
#include <vector>
#include <string>
#include <ClanLib/network.h> 
#include "../include/action.h" 
//-----------------------------------------------------------------------------

typedef struct s_lobby_info
{
} lobby_info;

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
	CL_NetSession *session;
	CL_SlotContainer slots;
	
	bool m_is_connected;
	bool m_is_server;
	bool m_is_client;

	// Server Connection
	CL_NetComputer server;
	
	CL_NetPacket make_packet(const Action &p_action);
	Action* make_action(CL_NetPacket &packet);
	
public:

	Network();
	~Network();
	void Init();
	
	CL_NetSession& GetSession();
	bool is_connected();
	bool is_local();
	bool is_server();
	bool is_client();
	
	void disconnect();
	
	void client_connect(const std::string &host, const std::string &port);
	void client_on_receive_lobby(CL_NetPacket&, CL_NetComputer&);
	void client_on_receive_action(CL_NetPacket&, CL_NetComputer&);
	void client_on_disconnect(CL_NetComputer &computer);
	
	void server_start(const std::string &port);
	void server_on_connect(CL_NetComputer &computer);
	void server_on_disconnect(CL_NetComputer &computer);
	void server_on_receive_lobby(CL_NetPacket&, CL_NetComputer&);
	void server_on_receive_action(CL_NetPacket&, CL_NetComputer&);
	
	// Send Messages
	void send_lobby(const lobby_info&);
	void send_action(const Action&);
};

extern Network network;
//-----------------------------------------------------------------------------
#endif
