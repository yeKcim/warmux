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
 * Handle distant computers
 *****************************************************************************/

#ifndef DISTANT_CPU_H
#define DISTANT_CPU_H
//-----------------------------------------------------------------------------
#include <list>
#include <vector>
#include <string>
#include <WARMUX_player.h>
#include <WARMUX_types.h>
//-----------------------------------------------------------------------------

class WSocket;

class DistantComputer
{
private:
  WSocket* sock;
  uint game_id;

  // a remote computer may act as a relay for several players (this is true when it is a server)
  std::list<Player> players;
  std::vector<uint> maps;
  std::vector<uint> teams;
  bool force_disconnection_called;
public:
  DistantComputer(WSocket* new_sock);
  DistantComputer(WSocket* new_sock, const std::string& nickname, uint initial_player_id);
  DistantComputer(WSocket* new_sock, const std::string& nickname, uint game_id, uint initial_player_id);

  ~DistantComputer();

  bool SocketReady() const;

  // ReceiveData may return true with *data = NULL and *len = 0
  // That means that client is still valid BUT there are not enough data CURRENTLY
  bool ReceiveData(char** data, size_t* len);
  bool SendData(const char* data, size_t len);

  std::string GetAddress() const;
  std::vector<std::string> GetNicknames() const;

  void AddPlayer(uint player_id);
  void DelPlayer(uint player_id);

  Player* GetPlayer(uint player_id);
  const std::list<Player>& GetPlayers() const { return players; }
  std::list<Player>& GetPlayers() { return players; }

  uint GetGameId() const { return game_id; } 

  int GetNumberOfPlayersWithState(Player::State state);

  void ForceDisconnection() { force_disconnection_called = true; }
  bool MustBeDisconnected() { return force_disconnection_called; }

  const std::string ToString() const;

  std::vector<uint>& GetAvailableMaps() { return maps; };
  static std::vector<uint> GetCommonMaps(const std::list<DistantComputer*>&);

  std::vector<uint>& GetAvailableTeams() { return teams; };
  static std::vector<uint> GetCommonTeams(const std::list<DistantComputer*>&);
};

// It's up to the program using class DistantComputer to define WARMUX_[Dis]connectHost();
extern void WARMUX_ConnectHost(DistantComputer& cpu);
extern void WARMUX_DisconnectHost(DistantComputer& cpu);

#endif

