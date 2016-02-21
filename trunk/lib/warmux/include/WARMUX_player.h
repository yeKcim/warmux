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
 * A network player (local or remote)
 *****************************************************************************/

#ifndef WARMUX_PLAYER_H
#define WARMUX_PLAYER_H
//-----------------------------------------------------------------------------
#include <list>
#include <string>
#include <WARMUX_types.h>
#include <WARMUX_team_config.h>
//-----------------------------------------------------------------------------

class Player
{
  friend void WARMUX_DisconnectPlayer(Player& player);
public:
  typedef enum {
    STATE_ERROR,
    STATE_NOT_INITIALIZED,
    STATE_INITIALIZED,
    STATE_READY,
    STATE_CHECKED,
    STATE_NEXT_GAME
  } State;
private:
  uint player_id;
  std::string nickname;
  std::list<ConfigTeam> owned_teams;
  void UpdateNickname();
  std::list<ConfigTeam>::iterator FindTeamWithId(const std::string team_id);
  State state;
public:
  Player(uint player_id, const std::string& nickname);
  Player();
  ~Player() { Disconnect(); }
  // This is misleading: it is assumed this doesn't use the network, check ~DistantComputer()
  void Disconnect()
  {
    // It's up to the program using class Player to define WARMUX_DisconnectPlayer()
    WARMUX_DisconnectPlayer(*this);
  }

  void SetId(uint _player_id) { player_id = _player_id; }
  uint GetId() const { return player_id; }

  void SetNickname(const std::string& _nickname) { nickname = _nickname; }
  const std::string& GetNickname() const { return nickname; }

  bool AddTeam(const ConfigTeam& team_conf);
  bool RemoveTeam(const std::string& team_id);
  bool UpdateTeam(const std::string& old_team_id, const ConfigTeam& team_conf);

  uint GetNbTeams() const { return owned_teams.size(); }
  const std::list<ConfigTeam> & GetTeams() const { return owned_teams; }

  void SetState(State _state) { state = _state; }
  State GetState() const { return state; }

  static std::string GetDefaultNickname();
};

// It's up to the program using class Player to define WARMUX_DisconnectPlayer();
// Currently it is expected not to use the network: check ~DistantComputer()
extern void WARMUX_DisconnectPlayer(Player& player);

#endif

