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
 * Distant Computer handling
 *****************************************************************************/

#include <algorithm>  //std::find
#include <SDL_thread.h>
#include <WARMUX_distant_cpu.h>
#include <WARMUX_error.h>
#include <WARMUX_i18n.h>
#include <WARMUX_socket.h>
#include <WARMUX_debug.h>

static const int MAX_PACKET_SIZE = 250*1024;

DistantComputer::DistantComputer(WSocket* new_sock) :
  sock(new_sock),
  game_id(0),
  force_disconnection_called(false)
{
  WARMUX_ConnectHost(*this);
}

DistantComputer::DistantComputer(WSocket* new_sock, const std::string& nickname, uint _game_id, uint initial_player_id) :
  sock(new_sock),
  game_id(_game_id),
  force_disconnection_called(false)
{
  Player theplayer(initial_player_id, nickname);
  players.push_back(theplayer);

  WARMUX_ConnectHost(*this);
}

DistantComputer::DistantComputer(WSocket* new_sock, const std::string& nickname, uint initial_player_id) :
  sock(new_sock),
  game_id(0),
  force_disconnection_called(false)
{
  Player theplayer(initial_player_id, nickname);
  players.push_back(theplayer);

  WARMUX_ConnectHost(*this);
}

DistantComputer::~DistantComputer()
{
  WARMUX_DisconnectHost(*this);

  // This will call the needed player->Disconnect() for each player
  // This doesn't receive or send info from the network, so it's safe
  // to do this after WARMUX_DisconnectHost to not delay disconnection
  players.clear();

  delete sock;
}

void DistantComputer::AddPlayer(uint player_id)
{
  Player theplayer(player_id, _("Unnamed"));
  players.push_back(theplayer);
}

void DistantComputer::DelPlayer(uint player_id)
{
  std::list<Player>::iterator player_it;
  for (player_it = players.begin(); player_it != players.end(); player_it++) {
    if (player_it->GetId() == player_id) {
      players.erase(player_it);
      return;
    }
  }
}

Player* DistantComputer::GetPlayer(uint player_id)
{
  Player* player = NULL;

  std::list<Player>::iterator player_it;
  for (player_it = players.begin(); player_it != players.end(); player_it++) {
    if (player_it->GetId() == player_id)
      return &(*player_it);
  }

  return player;
}

bool DistantComputer::SocketReady() const
{
  return sock->IsReady();
}

// ReceiveData may return true with *data = NULL and *len = 0
// That means that client is still valid BUT there are not enough data CURRENTLY
bool DistantComputer::ReceiveData(char** data, size_t* len)
{
  return sock->ReceivePacket(data, len);
}

bool DistantComputer::SendData(const char* data, size_t len)
{
  return sock->SendBuffer(data, len);
}

std::string DistantComputer::GetAddress() const
{
  return sock->GetAddress();
}

std::vector<std::string> DistantComputer::GetNicknames() const
{
  std::vector<std::string> nicknames;
  std::list<Player>::const_iterator player;

  for (player = players.begin(); player != players.end(); player++) {
    nicknames.push_back(player->GetNickname());
  }

  if (nicknames.empty())
    nicknames.push_back(_("Unnamed"));

  return nicknames;
}

int DistantComputer::GetNumberOfPlayersWithState(Player::State state)
{
  int counter = 0;
  std::list<Player>::const_iterator player;
  for (player = players.begin(); player != players.end(); player++) {
    if (player->GetState() == state)
      counter++;
  }
  return counter;
}

const std::string DistantComputer::ToString() const
{
  const std::vector<std::string>& nicknames = GetNicknames();
  std::string str = nicknames[0];
  for (uint i=1; i<nicknames.size(); i++)
    str += ", " + nicknames[i];
  str += " (" + GetAddress() + ")";
  return str;
}

std::vector<uint> DistantComputer::GetCommonMaps(const std::list<DistantComputer*>& cpu)
{
  std::vector<uint> index_list;

  if (cpu.empty()) {
    MSG_DEBUG("action_handler.map", "No CPU, empty list\n");
    return index_list;
  }

  if (cpu.size() == 1) {
    index_list = cpu.front()->GetAvailableMaps();

    MSG_DEBUG("action_handler.map", "Getting front CPU list of size %u from %p\n",
              (uint)index_list.size(), cpu.front());
    return index_list;
  }

  std::list<DistantComputer*>::const_iterator first = cpu.begin();
  const std::vector<uint>& start_list = (*first)->GetAvailableMaps();
  for (uint n=0; n<start_list.size(); n++) {
    uint index = start_list[n];
    std::list<DistantComputer*>::const_iterator client = first;
    bool found = true;
    client++;
    for (; client != cpu.end(); client++) {
      const std::vector<uint>& other_list = (*client)->GetAvailableMaps();
      if (std::find(other_list.begin(), other_list.end(), index) == other_list.end()) {
        found = false;
        break;
      }
    }

    if (found)
      index_list.push_back(index);
  }

  MSG_DEBUG("action_handler.map", "List of size %u\n", (uint)index_list.size());
  return index_list;
}

std::vector<uint> DistantComputer::GetCommonTeams(const std::list<DistantComputer*>& cpu)
{
  std::vector<uint> index_list;

  if (cpu.empty()) {
    MSG_DEBUG("action_handler.teams", "No CPU, empty list\n");
    return index_list;
  }

  if (cpu.size() == 1) {
    index_list = cpu.front()->GetAvailableTeams();

    MSG_DEBUG("action_handler.teams", "Getting front CPU list of size %u from %p\n",
              (uint)index_list.size(), cpu.front());
    return index_list;
  }

  std::list<DistantComputer*>::const_iterator first = cpu.begin();
  const std::vector<uint>& start_list = (*first)->GetAvailableTeams();
  for (uint n=0; n<start_list.size(); n++) {
    uint index = start_list[n];
    std::list<DistantComputer*>::const_iterator client = first;
    bool found = true;
    client++;
    for (; client != cpu.end(); client++) {
      const std::vector<uint>& other_list = (*client)->GetAvailableTeams();
      if (std::find(other_list.begin(), other_list.end(), index) == other_list.end()) {
        found = false;
        break;
      }
    }

    if (found)
      index_list.push_back(index);
  }

  MSG_DEBUG("action_handler.teams", "List of size %u\n", (uint)index_list.size());
  return index_list;
}
