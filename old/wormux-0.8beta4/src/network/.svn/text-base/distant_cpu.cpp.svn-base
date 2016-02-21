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

#ifdef _MSC_VER
#  include <algorithm>  //std::find
#endif
#include "network/distant_cpu.h"
//-----------------------------------------------------------------------------
#include <SDL_mutex.h>
#include <SDL_thread.h>
#include "network/network.h"
#include "include/action.h"
#include "include/action_handler.h"
#include "map/maps_list.h"
#include "menu/network_menu.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/debug.h"
//-----------------------------------------------------------------------------

DistantComputer::DistantComputer(TCPsocket new_sock) :
  sock_lock(SDL_CreateMutex()),
  sock(new_sock),
  owned_teams(),
  state(DistantComputer::STATE_ERROR),
  version_checked(false),
  force_disconnect(false),
  nickname("this is not initialized")
{
  packet_size = 0;
  packet_received = 0;
  packet = NULL;
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

    Action a(Action::ACTION_MENU_SET_MAP);
    MapsList::GetInstance()->FillActionMenuSetMap(a);
    a.WritePacket(pack, size);
    SendDatas(pack, size);
    free(pack);

    // Teams infos of already connected computers
    for(TeamsList::iterator team = GetTeamsList().playing_list.begin();
      team != GetTeamsList().playing_list.end();
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
  if (version_checked)
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_NETWORK_DISCONNECT, GetAddress()));
  if (force_disconnect)
    std::cerr << GetAddress() << " have been kicked" << std::endl;

  if (Network::GetInstance()->IsConnected())
  {
    for (std::list<std::string>::iterator team = owned_teams.begin();
         team != owned_teams.end();
         ++team)
    {
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_MENU_DEL_TEAM, *team));
    }
  }
  owned_teams.clear();

  SDLNet_TCP_Close(sock);
  SDLNet_TCP_DelSocket(Network::GetInstance()->socket_set, sock);
  SDL_DestroyMutex(sock_lock);
}

bool DistantComputer::SocketReady() const
{
  return SDLNet_SocketReady(sock);
}

int DistantComputer::ReceiveDatas(char* & buf)
{
  SDL_LockMutex(sock_lock);
  MSG_DEBUG("network","locked");

  if( packet_size == 0)
  {
    // Firstly, we read the size of the incoming packet
    Uint32 net_size;
    if (SDLNet_TCP_Recv(sock, &net_size, 4) <= 0)
    {
      SDL_UnlockMutex(sock_lock);
      return -1;
    }

    packet_size = (int)SDLNet_Read32(&net_size);
    NET_ASSERT(packet_size > 0)
    {
      // force_disconnect = true; // hum.. in this case we will assume it's a network error
      return -1;
    }

    packet = (char*)malloc(packet_size);
  }


  int sdl_received = 0;
  do
  {
    sdl_received = SDLNet_TCP_Recv(sock, packet + packet_received, packet_size - packet_received);
    if (sdl_received > 0)
    {
      MSG_DEBUG("network", "%i received", sdl_received);
      packet_received += sdl_received;
    }

    if (sdl_received < 0)
    {
      std::cerr << "Malformed packet" << std::endl;
      packet_received = 0;
      packet_size = 0;
      free(packet);
      packet = NULL;
      NET_ASSERT(false)
      {
        return -1;
      }
    }
  }
  while( sdl_received > 0 && packet_received != packet_size);

  SDL_UnlockMutex(sock_lock);
  MSG_DEBUG("network","unlocked");

  if( packet_received == packet_size )
  {
    int size = packet_size;
    buf = packet;
    packet = NULL;
    packet_size = 0;
    packet_received = 0;
    return size;
  }

  buf = NULL;
  return 0;
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

std::string DistantComputer::GetAddress()
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
    Team * tmp = GetTeamsList().FindById(name, index);
    if (tmp != NULL) 
    {
      tmp->SetRemote();
      
      Action* copy = new Action(Action::ACTION_MENU_ADD_TEAM, name);
      copy->Push( team->PopString() );
      copy->Push( team->PopInt() );
      ActionHandler::GetInstance()->NewAction(copy, false);
    } 
    else 
    {
      std::cerr << "Team "<< name << "does not exist!" << std::endl;
      ASSERT(false);
    }
  }
  else if(team->GetType() == Action::ACTION_MENU_DEL_TEAM)
  {
    std::list<std::string>::iterator it;
    it = find(owned_teams.begin(), owned_teams.end(), name);
    NET_ASSERT(it != owned_teams.end())
    {
      force_disconnect = true;
      return;
    }
    if (it != owned_teams.end()) 
    {
      owned_teams.erase(it);
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_MENU_DEL_TEAM, name), false);
    }
  }
  else
    ASSERT(false);
}

void DistantComputer::SendChatMessage(Action* a) const
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
