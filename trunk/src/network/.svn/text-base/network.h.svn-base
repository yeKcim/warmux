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
 ******************************************************************************
 * Network layer for Warmux.
 *****************************************************************************/

#ifndef NETWORK_H
#define NETWORK_H
//-----------------------------------------------------------------------------
#include <list>
#include <string>
#include <WARMUX_network.h>
#include <WARMUX_player.h>
#include <WARMUX_singleton.h>
#include <WARMUX_base.h>
//-----------------------------------------------------------------------------

// Use this debug to store network communication to a file
//#define LOG_NETWORK
// Factorize some declarations for Visual C++
#ifdef _MSC_VER
#  define S_IRUSR _S_IREAD
#  define S_IWUSR _S_IWRITE
#endif
#ifdef WIN32
#  define S_IRGRP 0
#  define O_SYNC  O_BINARY
#endif

// Some forward declarations
struct SDL_Thread;
class Action;
class DistantComputer;
class NetworkServer;
class NetworkMenu;
class WSocketSet;
class Team;

class NetworkThread
{
  static SDL_Thread* thread; // network thread, where we receive data from network
  static bool stop_thread;

  static void ReceiveActions();
  static int ThreadRun(void* no_param);
public:
  static void Start();
  static void Stop() { stop_thread = true; }

  static bool Continue() { return !stop_thread; }
  static void Wait();
};


class Network : public Singleton<Network>
{
  static int num_objects;
  static uint frames;

  std::list<DistantComputer*> cpu; // list of the connected computer
  SDL_mutex *cpus_lock;

  std::string game_name;
  std::string password;

  Player player;
  bool turn_master_player;

  void CheckOneHostTeams(Player& player, DistantComputer* new_host,
                         const std::vector<Team*>& local_list,
                         const std::vector<uint>& common_list);

protected:
  bool game_master_player;
  WNet::net_game_state_t state;

  Network(const std::string& game_name, const std::string& password); // pattern singleton

  WSocketSet* socket_set;

#ifdef LOG_NETWORK
  int fout;
  int fin;
#endif

  virtual void SendAction(const Action& a, DistantComputer* client, bool clt_as_rcver, bool lock=true) const;

  void DisconnectNetwork();

  void SetGameName(const std::string& _game_name) { game_name = _game_name; }

public:
  NetworkMenu* network_menu;

  virtual ~Network();

  static Network* GetInstance();
  static NetworkServer* GetInstanceServer(); // WARNING: return NULL if not server!!

  // Start a client
  static connection_state_t ClientStart(const std::string &host, const std::string &port,
                                        const std::string& password);

  // Start a server
  static connection_state_t ServerStart(const std::string &port,
                                        const std::string& game_name,
                                        const std::string& password);

  static void Disconnect();
  static bool IsConnected() { return !GetInstance()->IsLocal() && NetworkThread::Continue(); }

  virtual bool IsLocal() const { return false; }
  virtual bool IsServer() const { return false; }
  virtual bool IsClient() const { return false; }

  void SetGameMaster(); // useful when we re-electing a game master
  bool IsGameMaster() const { return game_master_player; }
  const std::string& GetGameName() const { return game_name; }
  const std::string& GetPassword() const { return password; }
  Player& GetPlayer() { return player; }
  const Player& GetPlayer() const { return player; }
  Player * LockRemoteHostsAndGetPlayer(uint player_id);

  std::list<DistantComputer*>& GetRemoteHosts() { return cpu; }

  std::list<DistantComputer*>& LockRemoteHosts();
  const std::list<DistantComputer*>& LockRemoteHosts() const;
  void UnlockRemoteHosts() const;
  void AddRemoteHost(DistantComputer *host);
  void RemoveRemoteHost(std::list<DistantComputer*>::iterator host_it);

  int CheckActivity(int timeout);
  virtual void HandleAction(Action* a, DistantComputer* sender) = 0;
  virtual void WaitActionSleep() = 0;
  virtual void CloseConnection(std::list<DistantComputer*>::iterator closed) = 0;

  // Action handling
  void SendActionToAll(const Action& action, bool lock=true) const;
  void SendActionToOne(const Action& action, DistantComputer* client, bool lock=true) const;
  void SendActionToAllExceptOne(const Action& action, DistantComputer* client, bool lock=true) const;

  // Manage network state
  void SetState(WNet::net_game_state_t state);
  WNet::net_game_state_t GetState() const { return state; }
  void SendNetworkState();

  bool IsTurnMaster() const;

  uint GetNbPlayersConnected() const;
  uint GetNbPlayersWithState(Player::State player_state) const;

  std::vector<uint> GetCommonMaps();
  void SendMapsList();

  std::vector<uint> GetCommonTeams();
  void SendTeamsList();

  bool HasPendingFrames() const { return frames; }
};

//-----------------------------------------------------------------------------
#endif
