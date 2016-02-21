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
 ******************************************************************************/

#include <WORMUX_action.h>
#include <WORMUX_error.h>
#include <WORMUX_i18n.h>
#include <WORMUX_index_server.h>
#include <WSERVER_config.h>
#include <server.h>

NetworkGame::NetworkGame(const std::string& _game_name, const std::string& _password) :
  game_name(_game_name), password(_password), game_started(false)
{
}

const std::string& NetworkGame::GetName() const
{
  return game_name;
}

const std::string& NetworkGame::GetPassword() const
{
  return password;
}

void NetworkGame::AddCpu(DistantComputer* cpu)
{
  cpulist.push_back(cpu);

  std::string msg = Format("[Game %s] New client connected: %s - total: %zd",
			   game_name.c_str(), cpu->ToString().c_str(), cpulist.size());

  DPRINT(INFO, "%s", msg.c_str());

  SendAdminMessage(msg);
}

std::list<DistantComputer*>& NetworkGame::GetCpus()
{
  return cpulist;
}

const std::list<DistantComputer*>& NetworkGame::GetCpus() const
{
  return cpulist;
}

bool NetworkGame::AcceptNewComputers() const
{
  if (game_started || cpulist.size() >= 4) {
    return false;
  }

  return true;
}

std::list<DistantComputer*>::iterator
NetworkGame::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  std::list<DistantComputer*>::iterator it;
  DistantComputer *host = *closed;

  it = cpulist.erase(closed);

  DPRINT(INFO, "[Game %s] Client disconnected: %s - total: %zd", game_name.c_str(),
	 host->ToString().c_str(), cpulist.size());

  delete host;

  return it;
}

uint NetworkGame::NextPlayerId() const
{
  uint player_id = 1;

  std::list<DistantComputer*>::const_iterator it;
  std::list<Player>::const_iterator player;
  for (it = cpulist.begin(); it != cpulist.end(); it++) {

    const std::list<Player>& players = (*it)->GetPlayers();

    for (player = players.begin(); player != players.end(); player++) {
      if (player_id <= player->GetId()) {
	player_id = player->GetId() + 1;
      }
    }
  }
  return player_id;
}

void NetworkGame::ElectGameMaster()
{
  if (cpulist.empty())
    return;

  DistantComputer* host = cpulist.front();

  DPRINT(INFO, "[Game %s] New game master: %s", game_name.c_str(),
	 host->ToString().c_str());

  Action a(Action::ACTION_NETWORK_SET_GAME_MASTER);
  SendActionToOne(a, host);
}

void NetworkGame::SendAdminMessage(const std::string& message)
{
  Action a(Action::ACTION_CHAT_MESSAGE);
  a.Push(std::string("***"));
  a.Push(message);
  SendActionToAll(a);
}

// Send Messages
void NetworkGame::SendActionToAll(const Action& a) const
{
  SendAction(a, NULL, false);
}

void NetworkGame::SendActionToOne(const Action& a, DistantComputer* client) const
{
  SendAction(a, client, true);
}

void NetworkGame::SendActionToAllExceptOne(const Action& a, DistantComputer* client) const
{
  SendAction(a, client, false);
}

// if (client == NULL) sending to every clients
// if (clt_as_rcver) sending only to client 'client'
// if (!clt_as_rcver) sending to all EXCEPT client 'client'
void NetworkGame::SendAction(const Action& a, DistantComputer* client, bool clt_as_rcver) const
{
  char *packet;
  int size;

  a.WriteToPacket(packet, size);
  ASSERT(packet != NULL);

  if (clt_as_rcver) {
    ASSERT(client);
    client->SendData(packet, size);
  } else {

    for (std::list<DistantComputer*>::const_iterator it = cpulist.begin();
	 it != cpulist.end(); it++) {

      if ((*it) != client) {
	(*it)->SendData(packet, size);
      }
    }
  }

  free(packet);
}

void NetworkGame::StartGame()
{
  DPRINT(INFO, "[Game %s] started with %zd players", game_name.c_str(), cpulist.size());

  std::list<DistantComputer*>::iterator it;
  int i = 0;
  for (it = cpulist.begin(); it != cpulist.end(); it++, i++) {
    DPRINT(INFO, "[Game %s] \t\t %d) %s", game_name.c_str(), i, (*it)->ToString().c_str());
  }

  game_started = true;
}

void NetworkGame::StopGame()
{
  DPRINT(INFO, "[Game %s] finished with %zd players", game_name.c_str(), cpulist.size());
  game_started = false;
}

void NetworkGame::ForwardPacket(const char *buffer, size_t len, DistantComputer* sender)
{
  std::list<DistantComputer*>::iterator it;

  for (it = cpulist.begin(); it != cpulist.end(); it++) {

    if ((*it) != sender) {
      (*it)->SendData(buffer, len);
    }
  }

  if (sender == cpulist.front()) {
    Action a(buffer, sender);
    if (a.GetType() == Action::ACTION_NETWORK_MASTER_CHANGE_STATE) {
      int net_state = a.PopInt();
      if (net_state == WNet::NETWORK_LOADING_DATA) {
	StartGame();
      }
      else if (net_state == WNet::NETWORK_NEXT_GAME) {
	StopGame();
      }
    }
  }
}

GameServer::GameServer() :
  game_name("dedicated"),
  password(""),
  port(0),
  clients_socket_set(NULL)
{
}

void GameServer::CreateGame(uint game_id)
{
  char gamename_c_str[32];
  snprintf(gamename_c_str, 32, "%s-%d", game_name.c_str(), game_id);
  std::string gamename_str(gamename_c_str);

  NetworkGame netgame(gamename_str, password);
  games.insert(std::make_pair(game_id, netgame));

  DPRINT(INFO, "[Game %s] created", gamename_str.c_str());
}

void GameServer::DeleteGame(std::map<uint, NetworkGame>::iterator gamelst_it)
{
  DPRINT(INFO, "[Game %s] deleted", gamelst_it->second.GetName().c_str());
  games.erase(gamelst_it);
}

const NetworkGame& GameServer::GetGame(uint game_id) const
{
  std::map<uint, NetworkGame>::const_iterator gamelst_it;
  gamelst_it = games.find(game_id);

  if (gamelst_it == games.end()) {
    ASSERT(false);
  }
  return (gamelst_it->second);
}

NetworkGame& GameServer::GetGame(uint game_id)
{
  std::map<uint, NetworkGame>::iterator gamelst_it;
  gamelst_it = games.find(game_id);

  if (gamelst_it == games.end()) {
    ASSERT(false);
  }
  return (gamelst_it->second);
}

const std::list<DistantComputer*>& GameServer::GetCpus(uint game_id) const
{
  return GetGame(game_id).GetCpus();
}

std::list<DistantComputer*>& GameServer::GetCpus(uint game_id)
{
  return GetGame(game_id).GetCpus();
}

bool GameServer::ConnectToIndexServer()
{
  // Register the game to the index server
  if (!is_public) {
    IndexServer::GetInstance()->SetHiddenServer();
    return true;
  }

  std::string index_server_address;
  if (config.Get("index_server_address", index_server_address)) {
    int index_server_port = 9997;
    config.Get("index_server_port", index_server_port);

    DPRINT(INFO, "Connect to the index server on %s:%d. Use this option only for debugging!",
	   index_server_address.c_str(), index_server_port);
    IndexServer::GetInstance()->SetAddress(index_server_address.c_str(), index_server_port);
  }

  connection_state_t conn = IndexServer::GetInstance()->Connect(PACKAGE_VERSION);
  if (conn != CONNECTED) {
    if (conn == CONN_WRONG_VERSION) {
      fprintf(stderr,"%s", Format(_("Sorry, your version is not supported anymore. "
			       "Supported versions are %s. "
			       "You can download an updated version "
			       "from http://www.wormux.org/wiki/download.php"),
			     IndexServer::GetInstance()->GetSupportedVersions().c_str()).c_str());
    } else {
      fprintf(stderr, "ERROR: Fail to connect to the index server");
    }
    return false;
  }

  bool r = IndexServer::GetInstance()->SendServerStatus(game_name, password != "", port);
  if (!r) {
    char port_str[8];
    snprintf(port_str, 8, "%d", port);
    fprintf(stderr, "%s\n", Format(_("Error: Your server is not reachable from the internet. Check your firewall configuration: TCP Port %s must accept connections from the outside. If you are not directly connected to the internet, check your router configuration: TCP Port %s must be forwarded on your computer."), port_str, port_str).c_str());
    IndexServer::GetInstance()->Disconnect();
    return false;
  }

  return true;
}

bool GameServer::RefreshConnexionToIndexServer()
{
  if (!is_public)
    return true;

  // Try to reconnect to index server...
  if (!IndexServer::GetInstance()->IsConnected()) {
    bool r = ConnectToIndexServer();
    if (!r)
      return r;
  }

  IndexServer::GetInstance()->Refresh(true);

  return true;
}

bool GameServer::ServerStart(uint _port, uint _max_nb_games, uint max_nb_clients,
			     const std::string& _game_name, std::string& _password,
			     bool _is_public)
{
  max_nb_games = _max_nb_games;
  game_name = _game_name;
  password = _password;
  port = _port;
  is_public = _is_public;

  // Open the port to listen to
  if (!server_socket.AcceptIncoming(port)) {
    DPRINT(INFO, "Unable to use port %d", port);
    return false;
  }

  DPRINT(INFO, "Server successfully started on port %d", port);
  clients_socket_set = WSocketSet::GetSocketSet(max_nb_clients);
  if (!clients_socket_set) {
    DPRINT(INFO, "ERROR: Fail to allocate Socket Set");
    return false;
  }

  CreateGame(1);

  if (!ConnectToIndexServer())
    return false;

  return true;
}

bool GameServer::HandShake(uint game_id, WSocket& client_socket, std::string& nickname, uint player_id)
{
  bool client_will_be_master = false;
  if (GetCpus(game_id).empty())
    client_will_be_master = true;

  bool r = WNet::Server_HandShake(client_socket, GetGame(game_id).GetName(), GetGame(game_id).GetPassword(),
				  nickname, player_id, client_will_be_master);

  if (r && client_will_be_master)
    DPRINT(INFO, "[Game %s] %s (%s) will be game master", GetGame(game_id).GetName().c_str(),
	   nickname.c_str(), client_socket.GetAddress().c_str());

  return r;
}

void GameServer::RejectIncoming()
{
  server_socket.Disconnect();
}

uint GameServer::CreateGameIfNeeded()
{
  if (games.size() >= max_nb_games) {
    DPRINT(INFO, "Max number of games already reached : %d", max_nb_games);
    return 0;
  }

  uint game_id = 0;
  std::map<uint, NetworkGame>::const_iterator gamelst_it;
  for (gamelst_it = games.begin(); gamelst_it != games.end(); gamelst_it++) {

    if (gamelst_it->second.AcceptNewComputers()) {
      return gamelst_it->first;
    }

    if (gamelst_it->first > game_id)
      game_id = gamelst_it->first;
  }

  game_id++;
  CreateGame(game_id);

  return game_id;
}

void GameServer::WaitClients()
{
  if (!server_socket.IsConnected())
    return;

  // Create a new game if there is no more games accepting players
  uint game_id = CreateGameIfNeeded();
  if (!game_id)
    return;

  // Check for an incoming connection
  WSocket* incoming = server_socket.LookForClient();
  if (incoming) {

    DPRINT(INFO, "Connexion from %s\n", incoming->GetAddress().c_str());

    std::string client_nickname;
    uint player_id = GetGame(game_id).NextPlayerId();

    if (!HandShake(game_id, *incoming, client_nickname, player_id)) {
      incoming->Disconnect();
      return;
    }

    clients_socket_set->AddSocket(incoming);

    DistantComputer* client = new DistantComputer(incoming, client_nickname, game_id, player_id);
    GetGame(game_id).AddCpu(client);

    if (clients_socket_set->NbSockets() == clients_socket_set->MaxNbSockets())
      RejectIncoming();
  }
}

void GameServer::RunLoop()
{
 loop:
  while (true) {

    bool r = RefreshConnexionToIndexServer();
    if (!r && clients_socket_set->NbSockets() == 0) {
      fprintf(stderr, "Game server is no more connected to index server");
      return;
    }

    WaitClients();

    int num_ready = clients_socket_set->CheckActivity(100);

    if (num_ready == -1) { // Means an error
      fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
      continue; //Or break?
    } else if (num_ready == 0) {
      // nothing to do
      continue;
    }

    char *buffer;
    size_t packet_size;

    std::map<uint, NetworkGame>::iterator gamelst_it;
    std::list<DistantComputer*>::iterator dst_cpu;

    for (gamelst_it = games.begin(); gamelst_it != games.end(); gamelst_it++) {

      for (dst_cpu = gamelst_it->second.GetCpus().begin();
	   dst_cpu != gamelst_it->second.GetCpus().end();
	   dst_cpu++) {

	if ((*dst_cpu)->SocketReady()) {// Check if this socket contains data to receive

	  if (!(*dst_cpu)->ReceiveData(&buffer, &packet_size)) {
	    // An error occured during the reception

	    bool turn_master_lost = (dst_cpu == gamelst_it->second.GetCpus().begin());
	    dst_cpu = gamelst_it->second.CloseConnection(dst_cpu);

	    if (clients_socket_set->NbSockets() + 1 == clients_socket_set->MaxNbSockets()) {
	      // A new player will be able to connect, so we reopen the socket
	      // For incoming connections
	      DPRINT(INFO, "Allowing new connections (%d/%d)",
		     clients_socket_set->NbSockets(), clients_socket_set->MaxNbSockets());
	      server_socket.AcceptIncoming(port);
	    }

	    if (gamelst_it->second.GetCpus().size() != 0) {
	      if (turn_master_lost) {
		GetGame(gamelst_it->first).ElectGameMaster();
	      }
	    } else {
	      DeleteGame(gamelst_it);
	      goto loop;
	    }

	  } else if (buffer && packet_size) {
	    // buffer may be NULL and packet_size equal to zero if there is not yet
	    // enough data to read a packet.
	    GetGame(gamelst_it->first).ForwardPacket(buffer, packet_size, *dst_cpu);
	    free(buffer);
	  }
	}
      } // loop on distant cpu
    } // loop on games
  } // while (true)
}

uint Action_TimeStamp()
{
  return 0;
}

void WORMUX_ConnectHost(DistantComputer& host)
{
  std::string hostname = host.GetAddress();
  std::string nicknames = host.GetNicknames();

  Action a(Action::ACTION_INFO_CLIENT_CONNECT);
  ASSERT(host.GetPlayers().size() == 1);
  int player_id = host.GetPlayers().back().GetId();
  a.Push(player_id);
  a.Push(hostname);
  a.Push(nicknames);

  GameServer::GetInstance()->GetGame(host.GetGameId()).SendActionToAllExceptOne(a, &host);
}

void WORMUX_DisconnectHost(DistantComputer& host)
{
  std::string hostname = host.GetAddress();
  std::string nicknames = host.GetNicknames();

  Action a(Action::ACTION_INFO_CLIENT_DISCONNECT);
  a.Push(hostname);
  a.Push(nicknames);
  a.Push(int(host.GetPlayers().size()));

  std::list<Player>::const_iterator player_it;
  for (player_it = host.GetPlayers().begin(); player_it != host.GetPlayers().end(); player_it++) {
    a.Push(int(player_it->GetId()));
  }
  GameServer::GetInstance()->GetGame(host.GetGameId()).SendActionToAll(a); // host is already removed from the list
}

void WORMUX_DisconnectPlayer(Player& /*player*/)
{

}

