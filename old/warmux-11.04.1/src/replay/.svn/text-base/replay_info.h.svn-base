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
 *****************************************************************************
 * Replay engine for Warmux parading as network layer
 * @todo Evaluate rate generation and determine wheter gziping is interesting
 *****************************************************************************/

#ifndef REPLAY_INFO_H
#define REPLAY_INFO_H

#include <SDL.h>
#include <vector>
#include <string>

#include <WARMUX_base.h>
#include <WARMUX_team_config.h>

#include "game_mode_info.h"

class ReplayInfo
{
  bool        valid;

  std::string version;
  time_t      date;
  uint32_t    duration_ms;
  std::string comment;
  std::string map_id;

  std::vector<ConfigTeam> teams;

  std::string last_error;

  //Game mode
  GameModeInfo mode_info;

  // Initialization as unvalid info
  ReplayInfo(time_t d = 0, uint32_t dms = 0);
  // std::string and std::vector make the destruction job for us

  void DisplayError(const char* err);

public:
  // Get all info from file
  static ReplayInfo  *ReplayInfoFromFile(FILE *in);
  // Get all info from current game
  static ReplayInfo  *ReplayInfoFromCurrent(uint32_t duration, const char* comment = NULL);
  // Dump to file
  bool               DumpToFile(FILE *out);

  bool               IsValid() const { return valid; }
  const std::string& GetVersion() const { return version; }
  time_t             GetDate() const { return date; }
  uint32_t           GetMillisecondsDuration() const { return duration_ms; }
  const std::string& GetComment() const { return comment; }
  const std::string& GetMapId() const { return map_id; }
  const std::vector<ConfigTeam>& GetTeams() const { return teams; }
  const std::string& GetLastError() const { return last_error; }

  // Boring accessing
  const GameModeInfo* GetGameModeInfo() const { return &mode_info; }
};

void     Write32(FILE *out, uint32_t val);
uint32_t Read32(FILE *in);

#endif //REPLAY_INFO_H
