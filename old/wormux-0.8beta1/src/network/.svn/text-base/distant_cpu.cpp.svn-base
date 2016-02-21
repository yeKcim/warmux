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
 * Distant Computer handling
 *****************************************************************************/

#include "distant_cpu.h"
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <SDL_thread.h>
#include "network.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "menu/network_menu.h"
#include "team/teams_list.h"
#include "tool/debug.h"
#include "tool/i18n.h"
//-----------------------------------------------------------------------------

DistantComputer::DistantComputer(TCPsocket new_sock) :
  sock_lock(SDL_CreateMutex()),
  sock(new_sock),
  owned_teams(),
  state(DistantComputer::ERROR),
  version_checked(false),
  force_disconnect(false),
  nickname("this is not initialized")
{
  SDLNet_TCP_AddSocket(Network::GetInstance()->socket_set, sock);

  // If we are the server, we have to tell this new computer
  // what teams / maps have already been selected
  if( Network::GetInstance()->IsServer() )
  {
    int size;
    char* pack;
    Action c(Action::ACTION_RULES_ASK_VERSION);
    c.WritePacket(pack, size);
    SendDatas(pack, size);
    free(pack);

    Action a(Action::ACTION_MENU_SET_MAP, ActiveMap().ReadName());
    a.WritePacket(pack, size);
    SendDatas(pack, size);
    free(pack);

    // Teams infos of already connected computers
    for(TeamsList::iterator team = teams_list.playing_list.begin();
      team != teams_list.playing_list.end();
      ++team)
    {
      Action b(Action::ACTION_MENU_ADD_TEAM, (*team)->GetId());
      b.Push((*team)->GetPlayerName());
      b.Push((int)(*team)->GetNbCharacters());
      b.WritePacket(pack, size);
      SendDatas(pack, size);
      free(pack);
    }
  }
}

DistantComputer::~DistantComputer()
{
  if(version_checked)
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_NETWORK_DISCONNECT, GetAdress()));
  if(force_disconnect)
    std::cerr << GetAdress() << " have been kicked" << std::endl;

  SDLNet_TCP_Close(sock);
  SDLNet_TCP_DelSocket(Network::GetInstance()->socket_set, sock);

  if(Network::GetInstance()->IsConnected())
  for(std::list<std::string>::iterator team = owned_teams.begin();
      team != owned_teams.end();
      ++team)
  {
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_MENU_DEL_TEAM, *team));
  }
  owned_teams.clear();

  SDL_DestroyMutex(sock_lock);
}

bool DistantComputer::SocketReady()
{
  return SDLNet_SocketReady(sock);
}

int DistantComputer::ReceiveDatas(char* & buf)
{
  SDL_LockMutex(sock_lock);
  MSG_DEBUG("network","locked");

  // Firstly, we read the size of the incoming packet
  Uint32 net_size;
  if (SDLNet_TCP_Recv(sock, &net_size, 4) <= 0)
  {
    SDL_UnlockMutex(sock_lock);
    return -1;
  }

  int size = (int)SDLNet_Read32(&net_size);
  net_assert(size > 0)
  {
    // force_disconnect = true; // hum.. in this case we will assume it's a network error
    return 0;
  }

  // Now we read the packet
  buf = (char*)malloc(size);

  int total_received = 0;
  while (total_received != size)
  {
    int received = SDLNet_TCP_Recv(sock, buf + total_received, size - total_received);
    if (received > 0)
    {
      MSG_DEBUG("network", "%i received", received);
      total_received += received;
    }

    if (received < 0)
    {
      std::cerr << "Malformed packet" << std::endl;
      total_received = received;
      net_assert(false)
      {
	return 0;
      }
    }
  }
  SDL_UnlockMutex(sock_lock);
  MSG_DEBUG("network","unlocked");
  return total_received;
}

void DistantComputer::SendDatas(char* packet, int size_tmp)
{
  SDL_LockMutex(sock_lock);
  MSG_DEBUG("network","locked");
  Uint32 size;
  SDLNet_Write32(size_tmp, &size);
  SDLNet_TCP_Send(sock,&size,4);
  SDLNet_TCP_Send(sock,packet, size_tmp);
  MSG_DEBUG("network","%i sent", 4 + size_tmp);

  SDL_UnlockMutex(sock_lock);
  MSG_DEBUG("network","unlocked");
}

std::string DistantComputer::GetAdress()
{
  IPaddress* ip = SDLNet_TCP_GetPeerAddress(sock);
  std::string address;
  const char* resolved_ip = SDLNet_ResolveIP(ip);
  if(resolved_ip)
    address = resolved_ip;
  else
    return "Unresolved address";
  return address;
}

void DistantComputer::ManageTeam(Action* team)
{
  std::string name = team->PopString();
  if(team->GetType() == Action::ACTION_MENU_ADD_TEAM)
  {
    owned_teams.push_back(name);

    int index = 0;
    Team * tmp = teams_list.FindById(name, index);
    tmp->SetRemote();

    Action* copy = new Action(Action::ACTION_MENU_ADD_TEAM, name);
    copy->Push( team->PopString() );
    copy->Push( team->PopInt() );
    ActionHandler::GetInstance()->NewAction(copy, false);
  }
  else if(team->GetType() == Action::ACTION_MENU_DEL_TEAM)
  {
    std::list<std::string>::iterator it;
    it = find(owned_teams.begin(), owned_teams.end(), name);
    net_assert(it != owned_teams.end())
    {
      force_disconnect = true;
      return;
    }
    owned_teams.erase(it);
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_MENU_DEL_TEAM, name), false);
  }
  else
    assert(false);
}

void DistantComputer::SendChatMessage(Action* a)
{
  std::string txt = a->PopString();
  if (txt == "") return;
  if(Network::GetInstance()->IsServer())
  {
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHAT_MESSAGE, nickname + "> "+txt));
  }
  else
  {
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHAT_MESSAGE, txt), false);
  }
}

void DistantComputer::SetState(DistantComputer::state_t _state) 
{
  state = _state;
}

DistantComputer::state_t DistantComputer::GetState() const
{
  return state;
}
