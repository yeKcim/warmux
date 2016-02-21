/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

#include <algorithm>

#include <WARMUX_action.h>
#include <WARMUX_error.h>
#include <WARMUX_i18n.h>
#include <WARMUX_index_server.h>
#include "config.h"
#include "server.h"

NetworkGame::NetworkGame(const std::string& _game_name, const std::string& _password) :
  game_name(_game_name), password(_password), game_started(false), selected_map("dummy")
{
  ResetWaiting();
}

void NetworkGame::AddCpu(DistantComputer* cpu)
{
  cpulist.push_back(cpu);

  start_waiting = SDL_GetTicks();

  std::string msg = Format("[Game %s] New client connected: %s - total: %zd",
                           game_name.c_str(), cpu->ToString().c_str(), cpulist.size());

  DPRINT(INFO, "%s", msg.c_str());

  SendAdminMessage(msg);
}

std::list<DistantComputer*>::iterator
NetworkGame::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  std::list<DistantComputer*>::iterator it;
  DistantComputer *host = *closed;

  if (host == waited)
    ResetWaiting();

  it = cpulist.erase(closed);

  DPRINT(INFO, "[Game %s] Client disconnected: %s - total: %zd", game_name.c_str(),
         host->ToString().c_str(), cpulist.size());

  delete host;

  // Update list
  if (!cpulist.empty())
    SendMapsList(cpulist.front());

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

  SendMapsList(host);
}

void NetworkGame::SendMapsList(DistantComputer *host)
{
  std::vector<uint> index_list = DistantComputer::GetCommonMaps(cpulist);
  Action a(Action::ACTION_GAME_SET_MAP_LIST);

  a.Push(index_list.size());
  for (uint i=0; i<index_list.size(); i++) {
    std::map<std::string, uint>::const_iterator it = name_index_map.begin();
    while (it != name_index_map.end()) {
      if (it->second == index_list[i]) {
        a.Push(it->first);
        break;
      }
      ++it;
    }
  }

  SendActionToOne(a, host);
}

void NetworkGame::SendAdminMessage(const std::string& message)
{
  Action a(Action::ACTION_CHAT_MENU_MESSAGE);
  std::string msg = "***" + message;
  a.Push(0);
  a.Push(msg);
  SendActionToAll(a);
}

void NetworkGame::SendSingleAdminMessage(DistantComputer* client, const std::string& message)
{
  Action a(Action::ACTION_CHAT_MENU_MESSAGE);
  std::string msg = "***" + message;
  a.Push(0);
  a.Push(msg);
  SendActionToOne(a, client);
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

  // Reset any wait - task #6684
  ResetWaiting();

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
  ResetWaiting();
  game_started = false;
}

void NetworkGame::UpdateWaited()
{
  uint not_ready = 0;
  waited = NULL;

  std::list<DistantComputer*>::iterator it = cpulist.begin();
  while (it != cpulist.end()) {
    if ((int)(*it)->GetPlayers().size() != (*it)->GetNumberOfPlayersWithState(Player::STATE_INITIALIZED)) {
      not_ready++;
      waited = (*it);
    }
    ++it;
  }
  if (not_ready == 1) {
    start_waiting = SDL_GetTicks();
    DPRINT(INFO, "%p is the last not ready!\n", waited);
  }
}

void NetworkGame::CheckWaited()
{
  if (waited) {
    int wait = SDL_GetTicks()-start_waiting;
    if (wait > 30000 && !warned) {
      SendSingleAdminMessage(waited,
                             "Game waiting for you for more than 30s -"
                             " in 30s you'll get kicked!\n");
      warned = true;
    } else if (warned && wait>60000) {
      std::list<DistantComputer*>::iterator dst_cpu = std::find(cpulist.begin(), cpulist.end(), waited);
      if (dst_cpu != cpulist.end()) {
        bool was_master = IsGameMaster(dst_cpu);
        SendSingleAdminMessage(waited, "More than 60s of inactivity, you're out!\n");
        CloseConnection(dst_cpu);
        if (was_master)
          ElectGameMaster();
        ResetWaiting();
      }
    }
  }
}

void NetworkGame::ForwardPacket(const char *buffer, size_t len, DistantComputer* sender)
{
  std::list<DistantComputer*>::iterator it;
  Action a(buffer, sender);

  if (Action::GetType(buffer) == Action::ACTION_NETWORK_CLIENT_CHANGE_STATE) {
    Action a(buffer, sender);
    int player_id = a.PopInt();
    WNet::net_game_state_t state = (WNet::net_game_state_t)a.PopInt();
    if (state == WNet::NETWORK_MENU_OK) {
      sender->GetPlayer(player_id)->SetState(Player::STATE_INITIALIZED);
      UpdateWaited();
    }
  }

  for (it = cpulist.begin(); it != cpulist.end(); it++) {
    if ((*it) != sender) {
      (*it)->SendData(buffer, len);
    }
  }

  if (Action::GetType(buffer) == Action::ACTION_GAME_SET_MAP_LIST) {
    // The game server is the only one having the full list of available maps
    Action a(buffer, sender);
    int num = a.PopInt();
    std::vector<uint>& list = a.GetCreator()->GetAvailableMaps();

    list.resize(num);
    // Check each map and add it to the pool of known map indices
    while (num--) {
      std::string map_name = a.PopString();
      std::map<std::string, uint>::const_iterator it = name_index_map.find(map_name);
      if (it == name_index_map.end()) {
        // Not present, increase size and add
        uint size = name_index_map.size();
        printf("Associated %s to %u\n", map_name.c_str(), size);
        name_index_map[map_name] = size;
        list[num] = size;
      } else {
        list[num] = it->second;
      }
    }
  } else if (sender == cpulist.front()) {
    if (Action::GetType(buffer) == Action::ACTION_NETWORK_MASTER_CHANGE_STATE) {
      // We need to track the game started/stopped state
      Action a(buffer, sender);
      int net_state = a.PopInt();
      if (net_state == WNet::NETWORK_LOADING_DATA) {
        StartGame();
      }
      else if (net_state == WNet::NETWORK_NEXT_GAME) {
        StopGame();
      }
    } else if (Action::GetType(buffer) == Action::ACTION_GAME_SET_MAP) {
      // We need to know the selected map
      Action a(buffer, sender);
      selected_map = a.PopString();
    }
  }
}

void NetworkGame::InformDisconnection(DistantComputer *host)
{
  Action a(Action::ACTION_INFO_CLIENT_DISCONNECT);
  a.Push(host->ToString());
  a.Push(int(host->GetPlayers().size()));

  std::list<Player>::const_iterator player_it;
  for (player_it = host->GetPlayers().begin(); player_it != host->GetPlayers().end(); player_it++) {
    a.Push(int(player_it->GetId()));
  }

  if (host == waited)
    ResetWaiting();


  SendActionToAll(a); // host is already removed from the list
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

  connection_state_t conn = IndexServer::GetInstance()->Connect(PACKAGE_VERSION);
  if (conn != CONNECTED) {
    if (conn == CONN_WRONG_VERSION) {
      fprintf(stderr,"%s\n", Format(_("Sorry, your version is not supported anymore. "
                               "Supported versions are %s. "
                               "You can download an updated version "
                               "from http://www.warmux.org/wiki/download.php"),
                             IndexServer::GetInstance()->GetSupportedVersions().c_str()).c_str());
    } else {
      fprintf(stderr, "ERROR: Fail to connect to the index server\n");
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
    }

    std::map<uint, NetworkGame>::iterator gamelst_it = games.begin();
    for (; gamelst_it != games.end(); gamelst_it++) {
      // task #6684 - check waited
      NetworkGame& game = gamelst_it->second;
      game.CheckWaited();

      if (num_ready == 0) {
        // nothing to do
        continue;
      }

      std::list<DistantComputer*>::iterator dst_cpu = game.GetCpus().begin();
      for (; dst_cpu != game.GetCpus().end(); dst_cpu++) {

        if ((*dst_cpu)->SocketReady()) {// Check if this socket contains data to receive
          char *buffer;
          size_t packet_size;

          if (!(*dst_cpu)->ReceiveData(&buffer, &packet_size)) {
            // An error occured during the reception

            bool turn_master_lost = game.IsGameMaster(dst_cpu);
            dst_cpu = game.CloseConnection(dst_cpu);

            if (clients_socket_set->NbSockets() + 1 == clients_socket_set->MaxNbSockets()) {
              // A new player will be able to connect, so we reopen the socket
              // For incoming connections
              DPRINT(INFO, "Allowing new connections (%d/%d)",
                     clients_socket_set->NbSockets(), clients_socket_set->MaxNbSockets());
              server_socket.AcceptIncoming(port);
            }

            if (!game.GetCpus().empty()) {
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

void WARMUX_ConnectHost(DistantComputer& host)
{
  Action a(Action::ACTION_INFO_CLIENT_CONNECT);
  ASSERT(host.GetPlayers().size() == 1);
  int player_id = host.GetPlayers().back().GetId();
  a.Push(player_id);
  a.Push(host.ToString());

  GameServer::GetInstance()->GetGame(host.GetGameId()).SendActionToAllExceptOne(a, &host);
}

void WARMUX_DisconnectHost(DistantComputer& host)
{
  GameServer::GetInstance()->GetGame(host.GetGameId()).InformDisconnection(&host);
}

void WARMUX_DisconnectPlayer(Player& /*player*/)
{

}

