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

#include "network.h"
//-----------------------------------------------------------------------------
#include "../include/action_handler.h"
//-----------------------------------------------------------------------------

#ifdef DEBUG
#define COUT_DBG std::cout<<"[Network] "
#define DBG_MSG
#include <iostream>
#endif

//-----------------------------------------------------------------------------
Network network;
//-----------------------------------------------------------------------------

Network::Network()
{
	m_is_connected = false;
	m_is_server = false;
	m_is_client = false;
	state = NETWORK_NOT_CONNECTED;
#ifdef CL
	session = NULL;
#endif
}

//-----------------------------------------------------------------------------

void Network::Init()
{
#ifdef CL
	if (session != NULL) return;
	session = new CL_NetSession("Wormux");
#endif
}

//-----------------------------------------------------------------------------

Network::~Network() 
{
#ifdef CL
	delete session;
#endif
}

//-----------------------------------------------------------------------------

#ifdef CL
CL_NetSession& Network::GetSession()
{
	assert (session != NULL);
	return *session;
}
#endif

//-----------------------------------------------------------------------------

void Network::disconnect() 
{
	if( m_is_server ) {
#ifdef CL
		GetSession().get_all().disconnect();
		GetSession().stop_listen();
#endif
	} else {
#ifdef CL
		server.disconnect();
#endif
	}
	m_is_connected = false;
	m_is_server = false;
	m_is_client = false;
}

//-----------------------------------------------------------------------------

void Network::client_connect(const std::string &host, const std::string& port) 
{
	Init();
#ifdef DBG_MSG
	COUT_DBG << "Client connect to " << host << ":" << port << std::endl;
#endif
	m_is_client = true;
	m_is_server = false;
	state = NETWORK_WAIT_SERVER;
#ifdef CL
	server = GetSession().connect(CL_IPAddress(host, port));
 	slots.connect(GetSession().sig_computer_disconnected(), this, &Network::client_on_disconnect);
	slots.connect(GetSession().sig_netpacket_receive("lobby"), this, &Network::client_on_receive_lobby);
	slots.connect(GetSession().sig_netpacket_receive("game"), this, &Network::client_on_receive_action);
#endif
	m_is_connected = true;
}

//-----------------------------------------------------------------------------

#ifdef CL
void Network::client_on_disconnect(CL_NetComputer &computer) 
{
#ifdef DBG_MSG
	COUT_DBG << "client_on_disconnect" << std::endl;
#endif
	m_is_connected = false;
	state = NETWORK_NOT_CONNECTED;
}

//-----------------------------------------------------------------------------

void Network::client_on_receive_lobby(CL_NetPacket &packet, 
		                              CL_NetComputer &computer)
{
}

//-----------------------------------------------------------------------------

void Network::client_on_receive_action(CL_NetPacket &packet, 
 		                              CL_NetComputer &computer) 
{
	Action *a = make_action(packet);
	ActionHandler::GetInstance()->NewAction(*a, false);
	delete a;
}
#endif //CL
//-----------------------------------------------------------------------------

void Network::server_start(const std::string &port) 
{
	Init();
#ifdef DBG_MSG
	COUT_DBG << "Start server on port " << port << "." << std::endl;
#endif
	m_is_connected = true;
	m_is_server = true;
	m_is_client = false;
	state = NETWORK_WAIT_CLIENTS;
#ifdef CL
	slots.connect(GetSession().sig_computer_connected(), this, &Network::server_on_connect);
 	slots.connect(GetSession().sig_computer_disconnected(), this, &Network::server_on_disconnect);
 	slots.connect(GetSession().sig_netpacket_receive("lobby"), this, &Network::server_on_receive_lobby);
 	slots.connect(GetSession().sig_netpacket_receive("game"), this, &Network::server_on_receive_action);
 	GetSession().start_listen(port);
#endif
}

//-----------------------------------------------------------------------------
#ifdef CL

void Network::server_on_connect(CL_NetComputer &computer) 
{
#ifdef DBG_MSG
  COUT_DBG << "Server_on_connect" << std::endl;
#endif
}

//-----------------------------------------------------------------------------
void Network::server_on_disconnect(CL_NetComputer &computer) 
{	
#ifdef DBG_MSG
  COUT_DBG << "Server_on_disconnect" << std::endl;
#endif
	state = NETWORK_NOT_CONNECTED;
}

//-----------------------------------------------------------------------------

void Network::server_on_receive_lobby(CL_NetPacket &packet, CL_NetComputer &computer) 
{
}

//-----------------------------------------------------------------------------

void Network::server_on_receive_action(CL_NetPacket &packet, CL_NetComputer &computer)
{
#ifdef DBG_MSG
	COUT_DBG << "Server received packet." << std::endl;
#endif
	// Add the Action to the stack
	Action *a = make_action(packet);
	ActionHandler::GetInstance()->NewAction(*a, false);
	delete a;

	// Resend to other computers the packet
	CL_NetGroup grp = GetSession().get_all();
	grp.remove( computer );
	grp.send("game", packet);
}
#endif //CL
//-----------------------------------------------------------------------------

// Send Messages
void Network::send_action(const Action &action) 
{
	if (!m_is_connected) return;

	if (m_is_server) {
#ifdef CL
	// Send to Clients
		CL_NetPacket p = make_packet(action);
		GetSession().get_all().send("game", p);
#endif
	} else {
#ifdef CL
	// Send to Server
		CL_NetPacket msg = make_packet(action);
		server.send("game",msg);
#endif
	}
}

//-----------------------------------------------------------------------------
#ifdef CL
CL_NetPacket Network::make_packet(const Action &action) 
{
	CL_NetPacket packet;
	action.Write (packet.output);
	return packet;
}

//-----------------------------------------------------------------------------

Action* Network::make_action(CL_NetPacket &packet) 
{
	CL_InputSource_NetPacket &input = packet.input;
	Action_t type = (Action_t)input.read_int32 ();
	
	switch(type)
	{
	case ACTION_MOVE_CHARACTER:
	case ACTION_SHOOT:
		return new ActionInt2(type, input);
		
	case ACTION_CHANGE_CHARACTER:
	case ACTION_CHANGE_WEAPON:
	case ACTION_WIND:
	case ACTION_SET_CHARACTER_DIRECTION:
		return new ActionInt(type, input);
		
	case ACTION_SET_GAME_MODE:
	case ACTION_SET_MAP:
	case ACTION_CHANGE_TEAM:
	case ACTION_NEW_TEAM:
	case ACTION_SEND_VERSION:
	case ACTION_SEND_TEAM:
		return new ActionString(type, input);

	case ACTION_WALK:
	case ACTION_MOVE_LEFT:
	case ACTION_MOVE_RIGHT:
	case ACTION_JUMP:
	case ACTION_SUPER_JUMP:
	case ACTION_UP:
	case ACTION_DOWN:
	case ACTION_CLEAR_TEAMS:
	case ACTION_START_GAME:
	case ACTION_ASK_VERSION:
		return new Action(type);

	default: 
		assert(false);
		return new Action(type);
	}
}
#endif //CL
//-----------------------------------------------------------------------------

bool Network::is_connected() { return m_is_connected; }
bool Network::is_local() { return !m_is_server && !m_is_client; }
bool Network::is_server() { return m_is_server; }
bool Network::is_client() { return m_is_client; }

//-----------------------------------------------------------------------------

