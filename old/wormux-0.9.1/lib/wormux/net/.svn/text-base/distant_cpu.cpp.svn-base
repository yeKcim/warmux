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
 ******************************************************************************
 * Distant Computer handling
 *****************************************************************************/

#include <algorithm>  //std::find
#include <SDL_thread.h>
#include <WORMUX_distant_cpu.h>
#include <WORMUX_error.h>
#include <WORMUX_i18n.h>
#include <WORMUX_socket.h>

static const int MAX_PACKET_SIZE = 250*1024;

DistantComputer::DistantComputer(WSocket* new_sock) :
  sock(new_sock),
  game_id(0),
  force_disconnection_called(false)
{
  WORMUX_ConnectHost(*this);
}

DistantComputer::DistantComputer(WSocket* new_sock, const std::string& nickname, uint _game_id, uint initial_player_id) :
  sock(new_sock),
  game_id(_game_id),
  force_disconnection_called(false)
{
  Player theplayer(initial_player_id, nickname);
  players.push_back(theplayer);

  WORMUX_ConnectHost(*this);
}

DistantComputer::DistantComputer(WSocket* new_sock, const std::string& nickname, uint initial_player_id) :
  sock(new_sock),
  game_id(0),
  force_disconnection_called(false)
{
  Player theplayer(initial_player_id, nickname);
  players.push_back(theplayer);

  WORMUX_ConnectHost(*this);
}

DistantComputer::~DistantComputer()
{
  WORMUX_DisconnectHost(*this);

  // This will call the needed player->Disconnect() for each player
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

const std::list<Player>& DistantComputer::GetPlayers() const
{
  return players;
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

std::string DistantComputer::GetNicknames() const
{
  std::string nicknames;
  std::list<Player>::const_iterator player;

  for (player = players.begin(); player != players.end(); player++) {
    if (nicknames != "")
      nicknames += ", ";

    nicknames += player->GetNickname();
  }

  if (nicknames == "")
    nicknames = _("Unnamed");

  return nicknames;
}

uint DistantComputer::GetGameId() const
{
  return game_id;
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

void DistantComputer::ForceDisconnection()
{
  force_disconnection_called = true;
}

bool DistantComputer::MustBeDisconnected()
{
  return force_disconnection_called;
}

const std::string DistantComputer::ToString() const
{
  std::string str = GetAddress() + std::string(" (") + GetNicknames() + std::string(" )");
  return str;
}
