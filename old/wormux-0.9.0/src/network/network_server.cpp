/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#include <SDL_thread.h>
#include <WORMUX_socket.h>
#include "include/action_handler.h"
#include "network/network_server.h"
#include "tool/string_tools.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif

//-----------------------------------------------------------------------------

NetworkServer::NetworkServer(const std::string& game_name, const std::string& password) :
  Network(game_name, password),
  port(0)
{
#ifdef LOG_NETWORK
  fin = open("./network_server.in", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
  fout = open("./network_server.out", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
#endif
  game_master_player = true;
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::HandleAction(Action* a, DistantComputer* sender)
{
  // Repeat the packet to other clients
  SendActionToAllExceptOne(*a, sender);

  ActionHandler::GetInstance()->NewAction(a, false);
}

uint NetworkServer::NextPlayerId() const
{
  uint player_id = GetPlayer().GetId() + 1;

  const std::list<DistantComputer*>& hosts = LockRemoteHosts();
  std::list<DistantComputer*>::const_iterator it;
  std::list<Player>::const_iterator player;
  for (it = hosts.begin(); it != hosts.end(); it++) {

    const std::list<Player>& players = (*it)->GetPlayers();

    for (player = players.begin(); player != players.end(); player++) {
      if (player_id <= player->GetId()) {
	player_id = player->GetId() + 1;
      }
    }
  }
  UnlockRemoteHosts();

  return player_id;
}

bool NetworkServer::HandShake(WSocket& client_socket, std::string& nickname, uint player_id) const
{
  return WNet::Server_HandShake(client_socket, GetGameName(), GetPassword(), nickname, player_id, false);
}

void NetworkServer::WaitActionSleep()
{
  if (server_socket.IsConnected()) {

    // Check for an incoming connection
    WSocket* incoming = server_socket.LookForClient();
    if (incoming) {
      std::string nickname;
      uint player_id = NextPlayerId();

      if (!HandShake(*incoming, nickname, player_id))
 	return;

      socket_set->AddSocket(incoming);

      DistantComputer* client = new DistantComputer(incoming, nickname, player_id);
      SendInitialGameInfo(client, player_id);
      AddRemoteHost(client);

      if (GetNbPlayersConnected() >= max_nb_players)
        RejectIncoming();
    }
  }
}

void NetworkServer::RejectIncoming()
{
  server_socket.Disconnect();
}

//-----------------------------------------------------------------------------

connection_state_t NetworkServer::StartServer(const std::string &net_port, uint _max_nb_players)
{
  WNet::Init();

  // The server starts listening for clients
  printf("o Starting server on port %s...\n", net_port.c_str());

  std::list<DistantComputer*>& hosts = LockRemoteHosts();
  hosts.clear();
  UnlockRemoteHosts();

  // Convert port number (std::string port) into SDL port number format:
  if (!str2int(net_port, port)) {
    return CONN_BAD_PORT;
  }

  // Open the port to listen to
  if (!server_socket.AcceptIncoming(port)) {
    return CONN_BAD_PORT;
  }

  printf("o Server successfully started\n");
  max_nb_players = _max_nb_players;
  socket_set = WSocketSet::GetSocketSet(max_nb_players);
  if (!socket_set) {
    return CONN_REJECTED;
  }

  NetworkThread::Start();

  SetState(WNet::NETWORK_MENU_INIT);
  return CONNECTED;
}


void NetworkServer::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  RemoveRemoteHost(closed);

  if (GetNbPlayersConnected() == max_nb_players)
  {
    // A new player will be able to connect, so we reopen the socket
    // For incoming connections
    printf("Allowing new connections\n");
    server_socket.AcceptIncoming(port);
  }
}

void NetworkServer::SetMaxNumberOfPlayers(uint _max_nb_players)
{
  max_nb_players = _max_nb_players;
}
