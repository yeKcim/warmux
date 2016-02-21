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

#include <algorithm>

#ifdef WIN32
#  ifndef NOMINMAX
#  define NOMINMAX 1 // Avoid Windows headers to define max()
#  endif
#  include <winsock2.h>
#else
#  ifdef GEKKO
#    include <ogcsys.h>
#    include <network.h>
#    define socket  net_socket
#    define bind    net_bind
#    define connect net_connect
#    define setsockopt net_setsockopt
#  else
#    include <sys/socket.h>
#    include <netdb.h>
#    include <netinet/in.h>
#    include <arpa/nameser.h>
#    include <resolv.h>
#  endif
#  include <errno.h>
#  include <unistd.h>
#endif

#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif

#include <sys/types.h>
#include <SDL_thread.h>
#include <SDL_timer.h>

#include <WARMUX_debug.h>
#include <WARMUX_distant_cpu.h>
#include <WARMUX_player.h>
#include <WARMUX_index_server.h>
#include "network/network.h"
#include "network/network_local.h"
#include "network/network_client.h"
#include "network/network_server.h"
#include "network/chatlogger.h"
//-----------------------------------------------------------------------------
#include "game/game_mode.h"
#include "game/game.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "map/maps_list.h"
#include "menu/network_menu.h"

//-----------------------------------------------------------------------------

#include "team/team.h"
#include "team/teams_list.h"

//-----------------------------------------------------------------------------

SDL_Thread* NetworkThread::thread = NULL;
bool NetworkThread::stop_thread = false;

int NetworkThread::ThreadRun(void* /*no_param*/)
{
  MSG_DEBUG("network.thread", "Thread created: %u", SDL_ThreadID());
  ReceiveActions();
  MSG_DEBUG("network.thread", "Thread finished: %u", SDL_ThreadID());
  return 0;
}

void NetworkThread::Start()
{
  thread = SDL_CreateThread(NetworkThread::ThreadRun, NULL);
  stop_thread = false;
}

void NetworkThread::Wait()
{
  if (thread != NULL && SDL_ThreadID() != SDL_GetThreadID(thread)) {
    SDL_WaitThread(thread, NULL);
  }

  thread = NULL;
  stop_thread = false;
}

void NetworkThread::ReceiveActions()
{
  char* buffer;
  size_t packet_size;
  Network* net = Network::GetInstance();
  std::list<DistantComputer*>::iterator dst_cpu;
  std::list<DistantComputer*>& cpu = net->GetRemoteHosts();

  // While the connection is up
  while (Continue()) {
    if (net->GetState() == WNet::NETWORK_PLAYING && cpu.empty()) {
      // If while playing everybody disconnected, just quit
      break;
    }

    //Loop while nothing is received
    while (Continue()) {
      IndexServer::GetInstance()->Refresh();

      // Check forced disconnections
      dst_cpu = cpu.begin();
      while (Continue() && dst_cpu != cpu.end()) {
        // Disconnection is in 2 phases to be handled by one thread
        if ((*dst_cpu)->MustBeDisconnected()) {
          net->CloseConnection(dst_cpu);
          dst_cpu = cpu.begin();
        }
        else
          dst_cpu++;
      }

      // List is now maybe empty
      if (cpu.empty() && net->IsClient()) {
        fprintf(stderr, "you are alone!\n");
        Stop();
        return; // We really don't need to go through the loops
      }

      net->WaitActionSleep();

      int num_ready = net->CheckActivity(100);
      // Means something is available
      if (num_ready>0)
        break;
      // Means an error
      else if (num_ready == -1) {
        //Spams a lot under windows without the errno check...
        fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());

        continue; //Or break?
      }
    }

    for (dst_cpu = cpu.begin();
         Continue() && dst_cpu != cpu.end();
         dst_cpu++) {
      // Check if this socket contains data to receive
      if ((*dst_cpu)->SocketReady()) {

        if (!(*dst_cpu)->ReceiveData(&buffer, &packet_size)) {
          // An error occured during the reception
          (*dst_cpu)->ForceDisconnection();
          continue;
        }

        if (!buffer && !packet_size) {
          // Client is valid but there is not yet enough data to read an action
          continue;
        }

#ifdef LOG_NETWORK
        if (fin != 0) {
          int tmp = 0xFFFFFFFF;
          write(fin, &packet_size, 4);
          write(fin, buffer, packet_size);
          write(fin, &tmp, 4);
        }
#endif

        Action* a = new Action(buffer, (*dst_cpu));
        free(buffer);

        MSG_DEBUG("network.traffic", "Received action %s",
                  ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());
        net->HandleAction(a, *dst_cpu);
      }
    }
  }
}

//-----------------------------------------------------------------------------

int  Network::num_objects = 0;

Network * Network::GetInstance()
{
  if (!singleton) {
    singleton = new NetworkLocal();
    MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkLocal'\n", singleton);
  }
  return singleton;
}

NetworkServer * Network::GetInstanceServer()
{
  if (!singleton || !singleton->IsServer()) {
    return NULL;
  }
  return (NetworkServer*)singleton;
}

Network::Network(const std::string& _game_name, const std::string& passwd) :
  cpu(),
  game_name(_game_name),
  password(passwd),
  game_master_player(false),
  state(WNet::NO_NETWORK),// useless value at beginning
  socket_set(NULL),
#ifdef LOG_NETWORK
  fout(0),
  fin(0),
#endif
  network_menu(NULL)
{
  cpus_lock = SDL_CreateMutex();
  player.SetNickname(Player::GetDefaultNickname());
  num_objects++;
}
//-----------------------------------------------------------------------------

Network::~Network()
{
#ifdef LOG_NETWORK
  if (fin != 0)
    close(fin);
  if (fout != 0)
    close(fout);
#endif

  num_objects--;
  SDL_DestroyMutex(cpus_lock);
  if (num_objects == 0) {
    WNet::Quit();
    if (socket_set)
      fprintf(stderr, "Forgot to disconnect network at some point?");
  }
}

//-----------------------------------------------------------------------------

std::list<DistantComputer*>& Network::LockRemoteHosts()
{
  SDL_LockMutex(cpus_lock);
  return cpu;
}

const std::list<DistantComputer*>& Network::LockRemoteHosts() const
{
  SDL_LockMutex(cpus_lock);
  return cpu;
}

void Network::UnlockRemoteHosts() const
{
  SDL_UnlockMutex(cpus_lock);
}

void Network::AddRemoteHost(DistantComputer *host)
{
  SDL_LockMutex(cpus_lock);
  cpu.push_back(host);
  SDL_UnlockMutex(cpus_lock);
}

void Network::RemoveRemoteHost(std::list<DistantComputer*>::iterator host_it)
{
  SDL_LockMutex(cpus_lock);
  // A proper deference of the iterator must be obtained *before*
  // the iterator is removed from the list and has become invalid!
  DistantComputer *host =  *host_it;
  cpu.erase(host_it);
  // host can no longer receive packets, now disconnect it
  delete host;
  SDL_UnlockMutex(cpus_lock);
}


Player * Network::LockRemoteHostsAndGetPlayer(uint player_id)
{
  SDL_LockMutex(cpus_lock);
  Player * player = NULL;

  if (GetPlayer().GetId() == player_id)
    player = &(GetPlayer());

  std::list<DistantComputer*>::const_iterator it = cpu.begin();
  while (!player && (it != cpu.end())) {
    player = (*it)->GetPlayer(player_id);
    it++;
  }
  // Stay locked as the player must belong to a valid host while it gets processed.
  return player;
}

//-----------------------------------------------------------------------------

// Static method
void Network::Disconnect()
{
  // restore Windows title
  AppWarmux::GetInstance()->video->SetWindowCaption(std::string("WarMUX ") + Constants::WARMUX_VERSION);

  // Flush all actions
  ActionHandler::GetInstance()->Flush();

  if (singleton) {
    NetworkThread::Stop();
    singleton->DisconnectNetwork();
    delete singleton;
    ChatLogger::CloseIfOpen();
  }

  // Flush all actions
  ActionHandler::GetInstance()->Flush();
}

// Protected method for client and server
void Network::DisconnectNetwork()
{
  NetworkThread::Wait();

  SDL_LockMutex(cpus_lock);
  std::list<DistantComputer*>::iterator client = cpu.begin();

  while (client != cpu.end()) {
    DistantComputer* tmp = (*client);
    // A proper deference of the iterator must be obtained *before*
    // the iterator is removed from the list and has become invalid!
    client = cpu.erase(client);
    delete tmp;
  }
  SDL_UnlockMutex(cpus_lock);

  if (socket_set) {
    delete socket_set;
    socket_set = NULL;
  }
}

//-----------------------------------------------------------------------------
//----------------       Action handling methods   ----------------------------
//-----------------------------------------------------------------------------

// Send Messages
void Network::SendActionToAll(const Action& a) const
{
  MSG_DEBUG("network.traffic", "Send action %s to all remote computers",
            ActionHandler::GetInstance()->GetActionName(a.GetType()).c_str());

  SendAction(a, NULL, false);
}

void Network::SendActionToOne(const Action& a, DistantComputer* client) const
{
  MSG_DEBUG("network.traffic", "Send action %s to %s",
            ActionHandler::GetInstance()->GetActionName(a.GetType()).c_str(),
            client->ToString().c_str());

  SendAction(a, client, true);
}

void Network::SendActionToAllExceptOne(const Action& a, DistantComputer* client) const
{
  MSG_DEBUG("network.traffic","Send action %s to all EXCEPT %s",
            ActionHandler::GetInstance()->GetActionName(a.GetType()).c_str(),
            client->ToString().c_str());

  SendAction(a, client, false);
}

// if (client == NULL) sending to every clients
// if (clt_as_rcver) sending only to client 'client'
// if (!clt_as_rcver) sending to all EXCEPT client 'client'
void Network::SendAction(const Action& a, DistantComputer* client, bool clt_as_rcver) const
{
  char* packet;
  int size;

  a.WriteToPacket(packet, size);
  ASSERT(packet != NULL);

#ifdef LOG_NETWORK
  if (fout != 0) {
    int tmp = 0xFFFFFFFF;
    write(fout, &size, 4);
    write(fout, packet, size);
    write(fout, &tmp, 4);
  }
#endif

  if (clt_as_rcver) {
    ASSERT(client);
    client->SendData(packet, size);
  } else {

    SDL_LockMutex(cpus_lock);
    for (std::list<DistantComputer*>::const_iterator it = cpu.begin();
         it != cpu.end(); it++) {

      if ((*it) != client) {
        (*it)->SendData(packet, size);
      }
    }
    SDL_UnlockMutex(cpus_lock);
  }

  free(packet);
}

// Static method
connection_state_t Network::ClientStart(const std::string& host,
                                        const std::string& port,
                                        const std::string& password)
{
  NetworkClient* net = new NetworkClient(password);
  MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkClient'\n", net);

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  const connection_state_t error = net->ClientConnect(host, port);

  if (error != CONNECTED) {
    // revert change if connection failed
    singleton = prev;
    delete net;
  } else if (prev != NULL) {
    delete prev;
  }
  AppWarmux::GetInstance()->video->SetWindowCaption(std::string("WarMUX ") +
                                                    Constants::WARMUX_VERSION + " - " +
                                                    _("Client mode"));
  return error;
}

//-----------------------------------------------------------------------------

// Static method
connection_state_t Network::ServerStart(const std::string& port, const std::string& game_name, const std::string& password)
{
  NetworkServer* net = new NetworkServer(game_name, password);
  MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkServer'\n", net);

  // replace current singleton
  Network* prev = singleton;
  singleton = net;

  // try to connect
  const connection_state_t error = net->StartServer(port, GameMode::GetInstance()->max_teams);

  if (error != CONNECTED) {
    // revert change
    singleton = prev;
    delete net;
  } else if (prev != NULL) {
    delete prev;
  }

  if (error == CONNECTED) {
    AppWarmux::GetInstance()->video->SetWindowCaption(std::string("WarMUX ") +
                                                      Constants::WARMUX_VERSION + " - " +
                                                      _("Server mode"));
  }

  return error;
}

//-----------------------------------------------------------------------------

void Network::SetState(WNet::net_game_state_t _state)
{
  MSG_DEBUG("network.state", "%d -> %d", state, _state);
  state = _state;
}

void Network::SendNetworkState()
{
  ASSERT(!IsLocal());

  if (IsGameMaster()) {
    Action a(Action::ACTION_NETWORK_MASTER_CHANGE_STATE);
    a.Push(state);
    SendActionToAll(a);
  } else {
    int player_id = Network::GetInstance()->GetPlayer().GetId();
    Action a(Action::ACTION_NETWORK_CLIENT_CHANGE_STATE);
    a.Push(player_id);
    a.Push(state);
    SendActionToAll(a);
  }
}

bool Network::IsTurnMaster() const
{
  return ActiveTeam().IsLocal();
}

void Network::SetGameMaster()
{
  MSG_DEBUG("network.game_master", "we are the new game master");
  game_master_player = true;
}

int Network::CheckActivity(int timeout)
{
  return socket_set->CheckActivity(timeout);
}

//-----------------------------------------------------------------------------

uint Network::GetNbPlayersConnected() const
{
  uint r = 0;

  SDL_LockMutex(cpus_lock);
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    r += (*client)->GetPlayers().size();
  }
  SDL_UnlockMutex(cpus_lock);

  return r;
}

uint Network::GetNbPlayersWithState(Player::State player_state) const
{
  uint counter = 0;

  SDL_LockMutex(cpus_lock);
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    counter += (*client)->GetNumberOfPlayersWithState(player_state);
  }
  SDL_UnlockMutex(cpus_lock);

  return counter;
}

std::vector<uint> Network::GetCommonMaps()
{
  SDL_LockMutex(cpus_lock);
  std::vector<uint> list = DistantComputer::GetCommonMaps(cpu);
  SDL_UnlockMutex(cpus_lock);

  return list;
}


void Network::SendMapsList()
{
  MapsList *map_list = MapsList::GetInstance();
  std::list<DistantComputer*> &list = GetRemoteHosts();
  if (list.empty())
    return;

  if (IsGameMaster()) {
    // We are the game master: the received list must be used to determine
    // the common list and inform *all* distant computers
    // Furthermore, there should be no additional integer for the currently selected
    std::vector<uint> common_list = GetCommonMaps();
    if (common_list.empty()) {
      // No host, create a ful list list for ourselves at least
      common_list.resize(map_list->lst.size());
      for (uint i=0; i<map_list->lst.size(); i++)
        common_list[i] = i;
    }
    MSG_DEBUG("action_handler.map", "Common list has now %u maps\n", common_list.size());

    int index = map_list->GetActiveMapIndex();
    if (map_list->IsRandom()) {
      index = common_list.size();
    } else if (std::find(common_list.begin(), common_list.end(), index) == common_list.end()) {
      // Not found, reset
      index = 0;
      map_list->SelectMapByIndex(0);
    }

    Action a(Action::ACTION_GAME_FORCE_MAP_LIST);

    a.Push(common_list.size());
    for (uint i=0; i<common_list.size(); i++)
      a.Push(map_list->lst[common_list[i]]->GetRawName());
    //map_list->FillActionMenuSetMap(a);

    SendActionToAll(a);

    if (network_menu) {
      // Apply locally: list and current active one
      network_menu->SetMapsCallback(common_list);
      // Calling this will send an action, check MapSelectionBox::ChangeMap
      network_menu->ChangeMapCallback();
    }

    return;
  }
  DistantComputer* host = list.front();
  MSG_DEBUG("action_handler.map", "Sending list to %p\n", host);
  Action a(Action::ACTION_GAME_SET_MAP_LIST);
  a.Push(map_list->lst.size());
  for (uint i=0; i<map_list->lst.size(); i++)
    a.Push(map_list->lst[i]->GetRawName());

  // We only send to game master, which should be the only one anyway
  SendActionToOne(a, host);
}
