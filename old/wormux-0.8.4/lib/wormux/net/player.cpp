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

#include <stdlib.h>
#ifdef WIN32
# include <windows.h> // DWORD, GetUserName
#endif

#include <WORMUX_error.h>
#include <WORMUX_i18n.h>
#include <WORMUX_player.h>

Player::Player(uint _player_id, const std::string& _nickname) : player_id(_player_id), nickname(_nickname)
{
}

Player::Player() : player_id(0), nickname(_("Unnamed"))
{
}

Player::~Player()
{
  Disconnect();
}

void Player::Disconnect()
{
  // It's up to the program using class Player to define WORMUX_DisconnectPlayer();
  WORMUX_DisconnectPlayer(*this);
}

void Player::SetId(uint _player_id)
{
  player_id = _player_id;
}

uint Player::GetId() const
{
  return player_id;
}

void Player::SetNickname(const std::string& _nickname)
{
  nickname = _nickname;
}

const std::string& Player::GetNickname() const
{
  return nickname;
}

void Player::UpdateNickname()
{
  std::string nick;

  std::map<const std::string, ConfigTeam>::iterator it;
  for (it = owned_teams.begin(); it != owned_teams.end(); it++) {
    if (nick != "")
      nick += "+";

    nick += it->second.player_name;
  }

  if (nick == "")
    nick = _("Unnamed");

  nickname = nick;
}

bool Player::AddTeam(const ConfigTeam& team_conf)
{
  std::pair<std::map<const std::string, ConfigTeam>::iterator, bool> r;
  r = owned_teams.insert(std::make_pair(team_conf.id, team_conf));
  if (!r.second) {
    ASSERT(false);
    return false;
  }

  UpdateNickname();

  return r.second;
}

bool Player::RemoveTeam(const std::string& team_id)
{
  size_t previous_size;
  previous_size = owned_teams.size();
  owned_teams.erase(team_id);

  if (previous_size == owned_teams.size()) {
    ASSERT(false);
    return false;
  }
  UpdateNickname();

  return true;
}

bool Player::UpdateTeam(const std::string& old_team_id, const ConfigTeam& team_conf)
{
  if (old_team_id == team_conf.id) {

    if (owned_teams.find(team_conf.id) == owned_teams.end()) {
      ASSERT(false);
      return false;
    }

    owned_teams[team_conf.id] = team_conf;
    UpdateNickname();

    return true;
  }

  if (!RemoveTeam(old_team_id))
    return false;

  if (!AddTeam(team_conf))
    return false;

  return true;
}

uint Player::GetNbTeams() const
{
  return owned_teams.size();
}

const std::map<const std::string, ConfigTeam>& Player::GetTeams() const
{
  return owned_teams;
}

//-----------------------------------------------------------------------------
// This is a Class method (static)
std::string Player::GetDefaultNickname()
{
  std::string s_nick;
  const char *nick = NULL;
#ifdef WIN32
  char  buffer[32];
  DWORD size = 32;
  if (GetUserName(buffer, &size))
    nick = buffer;
#else
  nick = getenv("USER");
#endif
  s_nick = (nick) ? nick : _("Unnamed");
  return s_nick;
}
