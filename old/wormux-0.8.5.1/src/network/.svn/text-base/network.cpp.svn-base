/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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

#include <SDL_thread.h>
#include <SDL_timer.h>
#include <WORMUX_debug.h>
#include <WORMUX_distant_cpu.h>
#include <WORMUX_player.h>
#include <WORMUX_index_server.h>
#include "network/network.h"
#include "network/network_local.h"
#include "network/network_client.h"
#include "network/network_server.h"
#include "network/chatlogger.h"
//-----------------------------------------------------------------------------
#include "game/game_mode.h"
#include "game/game.h"
#include "graphic/video.h"
#include "include/action.h"
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif
//-----------------------------------------------------------------------------

// Standard header, only needed for the following method
#ifdef WIN32
#  include <winsock2.h>
#else
#  include <sys/socket.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <arpa/nameser.h>
#  include <resolv.h>
#  include <errno.h>
#  include <unistd.h>
#endif

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

void NetworkThread::Stop()
{
  stop_thread = true;
}

bool NetworkThread::Continue()
{
  return !stop_thread;
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

  while (Continue()) // While the connection is up
  {
    if (net->GetState() == WNet::NETWORK_PLAYING && cpu.empty())
    {
      // If while playing everybody disconnected, just quit
      break;
    }

    //Loop while nothing is received
    while (Continue())
    {
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
      else if (num_ready == -1)
      {
        fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
        continue; //Or break?
      }
    }

    for (dst_cpu = cpu.begin();
         Continue() && dst_cpu != cpu.end();
         dst_cpu++)
    {
      if((*dst_cpu)->SocketReady()) {// Check if this socket contains data to receive

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
  if (singleton == NULL) {
    singleton = new NetworkLocal();
    MSG_DEBUG("singleton", "Created singleton %p of type 'NetworkLocal'\n", singleton);
  }
  return singleton;
}

NetworkServer * Network::GetInstanceServer()
{
  if (singleton == NULL || !singleton->IsServer()) {
    return NULL;
  }
  return (NetworkServer*)singleton;
}

Network::Network(const std::string& _game_name, const std::string& passwd) :
  cpu(),
  game_name(_game_name),
  password(passwd),
  turn_master_player(false),
  game_master_player(false),
  state(WNet::NO_NETWORK),// useless value at beginning
  socket_set(NULL),
#ifdef LOG_NETWORK
  fout(0),
  fin(0),
#endif
  network_menu(NULL),
  sync_lock(false)
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
  }
}

//-----------------------------------------------------------------------------

std::list<DistantComputer*>& Network::GetRemoteHosts()
{
  return cpu;
}

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
  cpu.erase(host_it);
  delete *host_it;
  SDL_UnlockMutex(cpus_lock);
}

//-----------------------------------------------------------------------------

// Static method
void Network::Disconnect()
{
  // restore Windows title
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") + Constants::WORMUX_VERSION);

  // Flush all actions
  ActionHandler::GetInstance()->Flush();

  if (singleton != NULL) {
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

  DistantComputer* tmp;

  SDL_LockMutex(cpus_lock);
  std::list<DistantComputer*>::iterator client = cpu.begin();

  while (client != cpu.end()) {
    tmp = (*client);
    client = cpu.erase(client);
    delete tmp;
  }
  SDL_UnlockMutex(cpus_lock);

  if (socket_set != NULL) {
    delete socket_set;
  }
}

//-----------------------------------------------------------------------------
//----------------       Action handling methods   ----------------------------
//-----------------------------------------------------------------------------

// Send Messages
void Network::SendActionToAll(const Action& a) const
{
  MSG_DEBUG("network.traffic","Send action %s to all remote computers",
            ActionHandler::GetInstance()->GetActionName(a.GetType()).c_str());

  SendAction(a, NULL, false);
}

void Network::SendActionToOne(const Action& a, DistantComputer* client) const
{
  MSG_DEBUG("network.traffic","Send action %s to %s (%s)",
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

//-----------------------------------------------------------------------------

// Static method
bool Network::IsConnected()
{
  return (!GetInstance()->IsLocal() && NetworkThread::Continue());
}

//-----------------------------------------------------------------------------

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
  AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") +
						     Constants::WORMUX_VERSION + " - " +
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
    AppWormux::GetInstance()->video->SetWindowCaption( std::string("Wormux ") +
						       Constants::WORMUX_VERSION + " - " +
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

WNet::net_game_state_t Network::GetState() const
{
  return state;
}

void Network::SendNetworkState()
{
  ASSERT(!IsLocal());

  if (IsGameMaster()) {
    Action a(Action::ACTION_NETWORK_MASTER_CHANGE_STATE);
    a.Push(state);
    SendActionToAll(a);
  } else {
    Action a(Action::ACTION_NETWORK_CLIENT_CHANGE_STATE);
    a.Push(state);
    SendActionToAll(a);
  }
}

void Network::SetTurnMaster(bool master)
{
  MSG_DEBUG("network.turn_master", "turn_master: %d", master);
  turn_master_player = master;
}

bool Network::IsTurnMaster() const
{
  return turn_master_player;
}

void Network::SetGameMaster()
{
  MSG_DEBUG("network.game_master", "we are the new game master");
  game_master_player = true;
}

bool Network::IsGameMaster() const
{
  return game_master_player;
}

Player& Network::GetPlayer()
{
  return player;
}

const Player& Network::GetPlayer() const
{
  return player;
}

void Network::SetGameName(const std::string& _game_name)
{
  game_name = _game_name;
}

const std::string& Network::GetGameName() const
{
  return game_name;
}

const std::string& Network::GetPassword() const
{
  return password;
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

uint Network::GetNbHostsConnected() const
{
  return cpu.size();
}

uint Network::GetNbHostsInitialized() const
{
  uint r = 0;

  SDL_LockMutex(cpus_lock);
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_INITIALIZED)
      r++;
  }
  SDL_UnlockMutex(cpus_lock);

  return r;
}

uint Network::GetNbHostsReady() const
{
  uint r = 0;

  SDL_LockMutex(cpus_lock);
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_READY)
      r++;
  }
  SDL_UnlockMutex(cpus_lock);

  return r;
}

uint Network::GetNbHostsChecked() const
{
  uint r = 0;

  SDL_LockMutex(cpus_lock);
  for (std::list<DistantComputer*>::const_iterator client = cpu.begin();
       client != cpu.end();
       client++) {
    if ((*client)->GetState() == DistantComputer::STATE_CHECKED)
      r++;
  }
  SDL_UnlockMutex(cpus_lock);

  return r;
}
