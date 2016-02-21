/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#include "network/network_server.h"
//-----------------------------------------------------------------------------
#include <SDL_thread.h>
#include "include/action_handler.h"
#include "include/constant.h"
#include "game/game_mode.h"
#include "tool/debug.h"
#include "network/distant_cpu.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif

//-----------------------------------------------------------------------------

NetworkServer::NetworkServer(const std::string& password) : Network(password)
{
#ifdef LOG_NETWORK
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

void NetworkServer::HandleAction(Action* a, DistantComputer* sender) const
{
  // Repeat the packet to other clients:
  if (a->GetType() != Action::ACTION_NETWORK_CHANGE_STATE
      && a->GetType() != Action::ACTION_NETWORK_CHECK_PHASE2
      && a->GetType() != Action::ACTION_CHAT_MESSAGE)
  {
    char* packet;
    int packet_size;
    a->WritePacket(packet, packet_size);

    for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
         client != cpu.end();
         client++)
      if (*client != sender)
      {
        (*client)->SendDatas(packet, packet_size);
      }
    free(packet);
  }

  ActionHandler::GetInstance()->NewAction(a, false);
}

bool NetworkServer::HandShake(TCPsocket& client_socket)
{
  int r;
  bool ret = false;
  std::string version;
  std::string _password;

  MSG_DEBUG("network", "server: Handshake!");

  // Adding the socket to a temporary socket set
  SDLNet_SocketSet tmp_socket_set = SDLNet_AllocSocketSet(1);
  SDLNet_TCP_AddSocket(tmp_socket_set, client_socket);

  // 1) Receive the version number
  MSG_DEBUG("network", "Server: waiting for client version number");

  r = Network::ReceiveStr(tmp_socket_set, client_socket, version);
  if (r) {
    std::cerr << "Error " << r << " when receiving version number"
	      << std::endl;
    goto error;
  }

  MSG_DEBUG("network", "Server: sending version number to client");

  Network::Send(client_socket, Constants::WORMUX_VERSION);

  if (Constants::WORMUX_VERSION != version) {
    std::cerr << "Client disconnected: wrong version " << version.c_str()
	     << std::endl;
    goto error;
  }

  // 2) Check the password
  MSG_DEBUG("network", "Server: waiting for password");

  r = Network::ReceiveStr(tmp_socket_set, client_socket, _password);
  if (r)
    goto error;

  if (_password != GetPassword()) {
    std::cerr << "Client disconnected: wrong password " << _password.c_str()
	      << std::endl;
    Network::Send(client_socket, 1);
    goto error;
  }
  MSG_DEBUG("network", "Server: password OK");

  Network::Send(client_socket, 0);

  MSG_DEBUG("network", "server: Handshake done successfully :)");
  ret = true;

 error:
  if (!ret) {
    std::cerr << "Server: HandShake with client has failed!" << std::endl;
  }
  SDLNet_TCP_DelSocket(tmp_socket_set, client_socket);
  SDLNet_FreeSocketSet(tmp_socket_set);
  return ret;
}

void NetworkServer::WaitActionSleep()
{
  if (server_socket)
  {
    // Check for an incoming connection
    TCPsocket incoming = SDLNet_TCP_Accept(server_socket);
    if (incoming)
    {
      if (!HandShake(incoming))
 	return;

      DistantComputer * client = new DistantComputer(incoming);
      cpu.push_back(client);

      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_NETWORK_CONNECT,
							 client->GetAddress()));
      printf("New client connected\n");
      if (GetNbConnectedPlayers() >= max_nb_players)
        RejectIncoming();
    }
    SDL_Delay(100);
  }
}
//-----------------------------------------------------------------------------

connection_state_t NetworkServer::ServerStart(const std::string &port)
{
  Init();

  // The server starts listening for clients
  MSG_DEBUG("network", "Start server on port %s", port.c_str());

  cpu.clear();
  // Convert port number (std::string port) into SDL port number format:
  int prt = strtol(port.c_str(), NULL, 10);

  if (SDLNet_ResolveHost(&ip,NULL,(Uint16)prt) != 0)
  {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return CONN_BAD_PORT;
  }

  max_nb_players = GameMode::GetInstance()->max_teams;

  // Open the port to listen to
  if (!AcceptIncoming()) {
    return CONN_BAD_PORT;
  }
  printf("\nConnected\n");
  socket_set = SDLNet_AllocSocketSet(GameMode::GetInstance()->max_teams);
  thread = SDL_CreateThread(Network::ThreadRun, NULL);
  MSG_DEBUG("network", "Thread %u created by thread %u\n", SDL_GetThreadID(thread), SDL_ThreadID());
  return CONNECTED;
}

std::list<DistantComputer*>::iterator
NetworkServer::CloseConnection(std::list<DistantComputer*>::iterator closed)
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
  ASSERT(IsServer());
  if (!server_socket) return;
  SDLNet_TCP_Close(server_socket);
  server_socket = NULL;
  printf("\noo Stop listening\n");
}

void NetworkServer::SetMaxNumberOfPlayers(uint _max_nb_players)
{
  if (_max_nb_players <= GameMode::GetInstance()->max_teams) {
    max_nb_players = _max_nb_players;
  } else {
    max_nb_players = GameMode::GetInstance()->max_teams;
  }
}

uint NetworkServer::GetNbConnectedPlayers() const
{
  return cpu.size() + 1;
}

uint NetworkServer::GetNbInitializedPlayers() const
{
  uint r = 0;

  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_INITIALIZED)
      r++;
  }

  return r;
}

uint NetworkServer::GetNbReadyPlayers() const
{
  uint r = 0;

  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_READY)
      r++;
  }

  return r;
}

uint NetworkServer::GetNbCheckedPlayers() const
{
  uint r = 0;

  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_CHECKED)
      r++;
  }

  return r;
}
