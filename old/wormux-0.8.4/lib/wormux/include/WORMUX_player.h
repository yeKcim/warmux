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
 * A network player (local or remote)
 *****************************************************************************/

#ifndef WORMUX_PLAYER_H
#define WORMUX_PLAYER_H
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <map>
#include <string>
#include <WORMUX_types.h>
#include <WORMUX_team_config.h>
//-----------------------------------------------------------------------------

class Player
{
  friend void WORMUX_DisconnectPlayer(Player& player);

private:
  uint player_id;
  std::string nickname;
  std::map<const std::string, ConfigTeam> owned_teams;
  void UpdateNickname();

public:
  Player(uint player_id, const std::string& nickname);
  Player();
  ~Player();
  void Disconnect();

  void SetId(uint player_id);
  uint GetId() const;

  void SetNickname(const std::string& nickname);
  const std::string& GetNickname() const;

  bool AddTeam(const ConfigTeam& team_conf);
  bool RemoveTeam(const std::string& team_id);
  bool UpdateTeam(const std::string& old_team_id, const ConfigTeam& team_conf);

  uint GetNbTeams() const;
  const std::map<const std::string, ConfigTeam>& GetTeams() const;

  static std::string GetDefaultNickname();
};

// It's up to the program using class Player to define WORMUX_DisconnectPlayer();
extern void WORMUX_DisconnectPlayer(Player& player);

#endif

