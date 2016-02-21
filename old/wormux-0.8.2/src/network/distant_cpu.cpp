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
 * Distant Computer handling
 *****************************************************************************/

#include <algorithm>  //std::find
#include "network/distant_cpu.h"
//-----------------------------------------------------------------------------
#include <SDL_mutex.h>
#include <SDL_thread.h>
#include "network/network.h"
#include "include/action.h"
#include "include/action_handler.h"
#include "include/constant.h"
#include "map/maps_list.h"
#include "menu/network_menu.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/debug.h"
//-----------------------------------------------------------------------------

static const int MAX_PACKET_SIZE = 250*1024;

DistantComputer::DistantComputer(TCPsocket new_sock) :
  sock_lock(SDL_CreateMutex()),
  sock(new_sock),
  owned_teams(),
  state(DistantComputer::STATE_ERROR),
  nickname("this is not initialized"),
  force_disconnect(false)
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

    MSG_DEBUG("network", "Server: Sending map information");

    Action a(Action::ACTION_MENU_SET_MAP);
    MapsList::GetInstance()->FillActionMenuSetMap(a);
    a.WriteToPacket(pack, size);
    SendDatas(pack, size);
    free(pack);

    MSG_DEBUG("network", "Server: Sending teams information");

    // Teams infos of already connected computers
    for(TeamsList::iterator team = GetTeamsList().playing_list.begin();
      team != GetTeamsList().playing_list.end();
      ++team)
    {
      Action b(Action::ACTION_MENU_ADD_TEAM, (*team)->GetId());
      b.Push((*team)->GetPlayerName());
      b.Push((int)(*team)->GetNbCharacters());
      b.WriteToPacket(pack, size);
      SendDatas(pack, size);
      free(pack);
    }
  }
}

DistantComputer::~DistantComputer()
{
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

    if (packet_size > MAX_PACKET_SIZE)
    {
        MSG_DEBUG("network", "packet is too big");
        return -1;
    }

    packet = (char*)malloc(packet_size);
    if (!packet)
    {
        MSG_DEBUG("network", "memory allocated failed");
        return -1;
    }
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

void DistantComputer::SetNickname(const std::string& _nickname)
{
  nickname = _nickname;
}

const std::string& DistantComputer::GetNickname() const
{
  return nickname;
}

bool DistantComputer::AddTeam(const std::string& team_id)
{
  int index = 0;
  Team * the_team = GetTeamsList().FindById(team_id, index);

  if (the_team) {
    owned_teams.push_back(team_id);
    return true;
  }

  force_disconnect = true;

  std::cerr << "Team "<< team_id << "does not exist!" << std::endl;
  ASSERT(false);
  return false;
}

bool DistantComputer::RemoveTeam(const std::string& team_id)
{
  std::list<std::string>::iterator it;
  it = find(owned_teams.begin(), owned_teams.end(), team_id);
  printf("size of owned teams: %d\n", (int)owned_teams.size());

  if (it != owned_teams.end()) {
    owned_teams.erase(it);
    return true;
  }

  force_disconnect = true;

  ASSERT(false);
  return false;
}

bool DistantComputer::UpdateTeam(const std::string& old_team_id, const std::string& team_id)
{
  if (old_team_id == team_id) // nothing to do !
    return true;

  if (!RemoveTeam(old_team_id))
    return false;

  if (!AddTeam(team_id))
    return false;

  return true;
}

void DistantComputer::SetState(DistantComputer::state_t _state)
{
  state = _state;
}

DistantComputer::state_t DistantComputer::GetState() const
{
  return state;
}
