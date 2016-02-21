/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
 * Network server layer for Wormux.
 *****************************************************************************/

#include "network_server.h"
//-----------------------------------------------------------------------------
#include "include/action_handler.h"
#include "game/game_mode.h"
#include "tool/debug.h"
#include "tool/i18n.h"
#include "distant_cpu.h"

#if defined(DEBUG) && not defined(WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

//-----------------------------------------------------------------------------

NetworkServer::NetworkServer()
{
#if defined(DEBUG) && not defined(WIN32)
  fin = open("./network_server.in", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
  fout = open("./network_server.out", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
#endif
}

NetworkServer::~NetworkServer()
{
  SDLNet_TCP_Close(server_socket);
}

void NetworkServer::SendChatMessage(const std::string& txt)
{
  if (txt == "") return;
  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHAT_MESSAGE, nickname + std::string("> ") + txt));
}

void NetworkServer::ReceiveActions()
{
  char* packet;

  while (ThreadToContinue())
  {
    if (state == NETWORK_PLAYING && cpu.size() == 0)
    {
      // If while playing everybody disconnected, just quit
      break;
    }

    while (SDLNet_CheckSockets(socket_set, 100) == 0 && ThreadToContinue()) //Loop while nothing is received
      if (server_socket)
	{
	  // Check for an incoming connection
	  TCPsocket incoming;
	  incoming = SDLNet_TCP_Accept(server_socket);
	  if (incoming)
	    {
	      cpu.push_back(new DistantComputer(incoming));
	      printf("New client connected\n");
	      if (GetNbConnectedPlayers() >= max_nb_players)
		RejectIncoming();
	    }
	  SDL_Delay(100);
	}

    std::list<DistantComputer*>::iterator dst_cpu;
    for (dst_cpu = cpu.begin();
	 dst_cpu != cpu.end() && ThreadToContinue();
	 dst_cpu++)
    {
      if((*dst_cpu)->force_disconnect)
      {
        dst_cpu = CloseConnection(dst_cpu);
        continue;
      }
        
      if((*dst_cpu)->SocketReady()) // Check if this socket contains data to receive
      {
        // Read the size of the packet
        int packet_size = (*dst_cpu)->ReceiveDatas(packet);
        if( packet_size <= 0) {
          dst_cpu = CloseConnection(dst_cpu);
          continue;
        }

#if defined(DEBUG) && not defined(WIN32)
	if (fin != 0) {
	  int tmp = 0xFFFFFFFF;
	  write(fin, &packet_size, 4);
	  write(fin, packet, packet_size);
	  write(fin, &tmp, 4);
	}
#endif

        Action* a = new Action(packet, (*dst_cpu));
        MSG_DEBUG("network.traffic","Received action %s",
		  ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());

        // Repeat the packet to other clients:
        if (a->GetType() != Action::ACTION_RULES_SEND_VERSION
	    && a->GetType() != Action::ACTION_NETWORK_CHANGE_STATE
	    && a->GetType() != Action::ACTION_CHAT_MESSAGE)
	  for (std::list<DistantComputer*>::iterator client = cpu.begin();
	       client != cpu.end();
	       client++)
	    if (client != dst_cpu)
	      {
		(*client)->SendDatas(packet, packet_size);
	      }
	
           ActionHandler::GetInstance()->NewAction(a, false);
           free(packet);
      }
    }
  }
}

//-----------------------------------------------------------------------------

Network::connection_state_t NetworkServer::ServerStart(const std::string &port)
{
  Init();

  // The server starts listening for clients
  MSG_DEBUG("network", "Start server on port %s", port.c_str());

  cpu.clear();
  // Convert port number (std::string port) into SDL port number format:
  int prt;
  sscanf(port.c_str(),"%i",&prt);

  if (SDLNet_ResolveHost(&ip,NULL,(Uint16)prt) != 0)
  {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return Network::CONN_BAD_PORT;
  }

  max_nb_players = GameMode::GetInstance()->max_teams;

  // Open the port to listen to
  if (!AcceptIncoming()) {
    return Network::CONN_BAD_PORT;
  }
  printf("\nConnected\n");
  socket_set = SDLNet_AllocSocketSet(GameMode::GetInstance()->max_teams);
  thread = SDL_CreateThread(Network::ThreadRun, NULL);
  return Network::CONNECTED;
}

std::list<DistantComputer*>::iterator NetworkServer::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  printf("Client disconnected\n");
  delete *closed;
  if (GetNbConnectedPlayers() == max_nb_players)
  {
    // A new player will be able to connect, so we reopen the socket
    // For incoming connections
    printf("Allowing new connections\n");
    AcceptIncoming();
  }

  return cpu.erase(closed);
}

bool NetworkServer::AcceptIncoming()
{
  if (!IsServer()) return false;

  server_socket = SDLNet_TCP_Open(&ip);
  if (!server_socket)
  {
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return false;
  }
  printf("\nStart listening");
  return true;
}

void NetworkServer::RejectIncoming()
{
  assert(IsServer());
  if (!server_socket) return;
  SDLNet_TCP_Close(server_socket);
  server_socket = NULL;
  printf("\nStop listening");
}

void NetworkServer::SetMaxNumberOfPlayers(uint _max_nb_players)
{
  if (_max_nb_players <= GameMode::GetInstance()->max_teams) {
    max_nb_players = _max_nb_players;
  } else {
    max_nb_players = GameMode::GetInstance()->max_teams;
  }
}

const uint NetworkServer::GetNbConnectedPlayers() const
{
  return cpu.size() + 1;
}

const uint NetworkServer::GetNbInitializedPlayers() const
{
  uint r = 0;

  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::INITIALIZED)
      r++;
  }

  return r;
}

const uint NetworkServer::GetNbReadyPlayers() const
{
  uint r = 0;
  
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {    
    if ((*client)->GetState() == DistantComputer::READY)
      r++;
  }
  
  return r;
}
