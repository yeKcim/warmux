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

#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <map>
#include <string>

#include <WARMUX_action.h>
#include <WARMUX_distant_cpu.h>
#include <WARMUX_network.h>
#include <WARMUX_socket.h>
#include <WARMUX_singleton.h>
#include <WARMUX_types.h>
#include <WSERVER_config.h>
#include <WSERVER_debug.h>

class NetworkGame
{
  std::string      game_name;
  std::string      password;
  bool             game_started;
  std::list<DistantComputer*> cpulist;
  std::map<std::string, uint> name_index_map;
  std::string      selected_map;

  // To handle an idling game master
  DistantComputer* waited;
  uint             start_waiting;
  bool             warned;

  void SendAdminMessage(const std::string& message);
  void SendAction(const Action& a, DistantComputer* client, bool clt_as_rcver) const;
  void StartGame();
  void StopGame();

  void UpdateWaited();

public:
  NetworkGame(const std::string& game_name, const std::string& password);

  const std::string& GetName() const { return game_name; }
  const std::string& GetPassword() const { return password; }

  void AddCpu(DistantComputer* cpu);
  std::list<DistantComputer*>::iterator CloseConnection(std::list<DistantComputer*>::iterator closed);

  std::list<DistantComputer*>& GetCpus() { return cpulist; }
  const std::list<DistantComputer*>& GetCpus() const { return cpulist; }

  bool AcceptNewComputers() const { return !game_started && cpulist.size() < 8; }

  uint NextPlayerId() const;
  void ElectGameMaster();
  void ForwardPacket(const char *buffer, size_t len, DistantComputer* sender);
  void SendActionToAll(const Action& a) const { SendAction(a, NULL, false); }
  void SendActionToOne(const Action& a, DistantComputer* client) const { SendAction(a, client, true); }
  void SendActionToAllExceptOne(const Action& a, DistantComputer* client) const { SendAction(a, client, false); }

  void SendSingleAdminMessage(DistantComputer* client, const std::string& message);
  void CheckWaited();
  void ResetWaiting() { warned = false; waited = NULL; start_waiting = 0; }
  bool IsGameMaster(std::list<DistantComputer*>::const_iterator& cpu) const { return cpu == cpulist.begin(); }
  bool IsGameMaster(std::list<DistantComputer*>::iterator& cpu) { return cpu == cpulist.begin(); }
  void SendMapsList(DistantComputer *host);

  void InformDisconnection(DistantComputer *host);
};

class GameServer : public Singleton<GameServer>
{
  friend class Singleton<GameServer>;
  GameServer();

  uint max_nb_games;
  std::string game_name;
  std::string password;
  uint port;
  bool is_public;

  WSocket server_socket;
  WSocketSet* clients_socket_set;
  std::map<uint, NetworkGame> games; // list of the connected computer

  void CreateGame(uint game_id);
  void DeleteGame(std::map<uint, NetworkGame>::iterator gamelst_it);

  uint CreateGameIfNeeded();

  std::list<DistantComputer*>& GetCpus(uint game_id);
  const std::list<DistantComputer*>& GetCpus(uint game_id) const;

  bool HandShake(uint game_id, WSocket& client_socket, std::string& client_nickname, uint player_id);
  void WaitClients();
  void RejectIncoming();

  bool ConnectToIndexServer();
  bool RefreshConnexionToIndexServer();

public:
  bool ServerStart(uint port, uint max_nb_games, uint max_nb_clients,
		   const std::string& game_name, std::string& password,
		   bool _is_public);
  void RunLoop();

  NetworkGame& GetGame(uint game_id);
  const NetworkGame& GetGame(uint game_id) const;
};

uint Action_TimeStamp();
void WARMUX_ConnectHost(DistantComputer& host);
void WARMUX_DisconnectHost(DistantComputer& host);
void WARMUX_DisconnectPlayer(Player& player);

#endif
