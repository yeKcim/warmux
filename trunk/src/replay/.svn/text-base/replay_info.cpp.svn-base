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
 * Information on/for a replay file
 *****************************************************************************/

#include <string>
#include <SDL_net.h>
#include <time.h>

#include <WARMUX_i18n.h>

#include "include/constant.h"
#include "game/game_time.h"
#include "game/game_mode.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "map/maps_list.h"

#include "replay_info.h"

const uint32_t HEADER_MAGIC = 0x57524D58; /* WRMX */
const uint32_t DATA_MAGIC   = 0xDEADBEEF;

void Write32(FILE *out, uint32_t val)
{
  uint32_t render;
  SDLNet_Write32(val, &render);
  fwrite(&render, 1, 4, out);
}

uint32_t Read32(FILE *in)
{
  uint32_t render;
  (void)fread(&render, 1, 4, in);
  return SDLNet_Read32(&render);
}

ReplayInfo::ReplayInfo(time_t d, uint32_t dms)
  : valid(false)
  , date(d)
  , duration_ms(dms)
{
  teams.clear();
  map_id.clear(); map_id = _("Unknown.");
  comment.clear(); comment = _("No comment.");
};

ReplayInfo *ReplayInfo::ReplayInfoFromFile(FILE *in)
{
  char        temp[256+1];
  ReplayInfo *info = new ReplayInfo(0, 0);
  uint32_t    marker;

  info->last_error = _("Unspecified error or end of file");
  info->valid = false;

  // Header magic
  marker = Read32(in);                      // Header marker
  if (marker != HEADER_MAGIC) {
    info->last_error =
      Format(_("Bad header 0x%08X instead of 0x%08X"), marker, HEADER_MAGIC);
    return info;
  }

  // Version
  if (!fscanf(in, "%256[^\n]\n", temp)) return info;
  info->version = temp;
  if (ferror(in)) return info;

  // Time
  info->duration_ms = Read32(in);           // Duration
  info->date        = Read32(in);           // Return of time(NULL)
  if (ferror(in)) return info;

  // Comment
  if (!fscanf(in, "%256[^\n]\n", temp) || ferror(in)) return info;
  info->comment = temp;

  // map ID
  if (!fscanf(in, "%256[^\n]\n", temp) || ferror(in)) return info;
  info->map_id = temp;

  // Teams
  uint32_t num_teams = Read32(in);          // Number of teams
  if (num_teams > 8) {
    info->last_error = Format(_("Suspicious number of teams 0x%08X"), num_teams);
    return info;
  }
  while (num_teams) {
    ConfigTeam team_cfg;

    // Team No.i name
    if (!fscanf(in, "%256[^\n]\n", temp) || ferror(in))
      goto team_error;
    team_cfg.id = std::string(temp);

    // Player name for team No.i
    if (!fscanf(in, "%256[^\n]\n", temp) || ferror(in))
      goto team_error;
    team_cfg.player_name = std::string(temp);

    team_cfg.nb_characters = fgetc(in);
    if (ferror(in))
      goto team_error;

    // Nb characters for team ID No.i
    if (!fscanf(in, "%256[^\n]\n", temp) || ferror(in))
      goto team_error;
    team_cfg.ai = std::string(temp);

    team_cfg.group = fgetc(in);
    if (ferror(in))
      goto team_error;

    info->teams.push_back(team_cfg);
    num_teams--;
    continue;

team_error:
    info->last_error = _("End of file while parsing teams");
    return info;
  }

  // Game mode
  info->mode_info.allow_character_selection = Read32(in);
  info->mode_info.turn_duration = Read32(in);
  info->mode_info.duration_before_death_mode = Read32(in);
  info->mode_info.damage_per_turn_during_death_mode = Read32(in);
  info->mode_info.init_energy = Read32(in);
  info->mode_info.max_energy = Read32(in);
  info->mode_info.gravity = Read32(in);

  if (Read32(in) != DATA_MAGIC) {           // Data magic
    info->last_error = Format(_("Bad data marker 0x%08X instead of 0x%08X"), marker, DATA_MAGIC);
    return info;
  }

  info->valid = true;
  return info;
}

ReplayInfo *ReplayInfo::ReplayInfoFromCurrent(uint32_t duration, const char* comment)
{
  ReplayInfo *info    = new ReplayInfo(time(NULL), duration);

  info->version = Constants::WARMUX_VERSION; // Copy ?
  info->comment = (comment) ? comment : _("No comment.");
  info->map_id  = ActiveMap()->GetRawName();

  //Teams
  const std::vector<Team*>& plist = GetTeamsList().playing_list;
  for (uint i=0; i<plist.size(); i++) {
    ConfigTeam team_cfg = { plist[i]->GetId(), plist[i]->GetPlayerName(),
                            plist[i]->GetNbCharacters(), plist[i]->GetAIName(),
                            plist[i]->GetGroup() };
    info->teams.push_back(team_cfg);
  }

  // Game mode
  const GameMode * game_mode = GameMode::GetInstance();
  info->mode_info.allow_character_selection = game_mode->allow_character_selection;
  info->mode_info.turn_duration = game_mode->duration_turn;
  info->mode_info.duration_before_death_mode = game_mode->duration_before_death_mode;
  info->mode_info.damage_per_turn_during_death_mode = game_mode->damage_per_turn_during_death_mode;
  info->mode_info.init_energy = game_mode->character.init_energy;
  info->mode_info.max_energy = game_mode->character.max_energy;
  info->mode_info.gravity = (int)game_mode->gravity;

  // Everything ready
  info->valid   = true;
  return info;
}

bool
ReplayInfo::DumpToFile(FILE *out)
{
  if (!valid)
    return false;

  Write32(out, HEADER_MAGIC);               // Header magic
  fprintf(out, "%s\n", version.c_str());    // Version

  Write32(out, duration_ms);                // Duration
  Write32(out, date);                       // Date
  fprintf(out, "%s\n", comment.c_str());    // Comment

  fprintf(out, "%s\n", map_id.c_str());     // Mad ID

  Write32(out, teams.size());
  for (uint i=0; i<teams.size(); i++) {     // Team No.i
    fprintf(out, "%s\n", teams[i].id.c_str());
    fprintf(out, "%s\n", teams[i].player_name.c_str());
    fputc(teams[i].nb_characters, out);
    fprintf(out, "%s\n", teams[i].ai.c_str());
    fputc(teams[i].group, out);
  }

  //Game mode
  Write32(out, mode_info.allow_character_selection);
  Write32(out, mode_info.turn_duration);
  Write32(out, mode_info.duration_before_death_mode);
  Write32(out, mode_info.damage_per_turn_during_death_mode);
  Write32(out, mode_info.init_energy);
  Write32(out, mode_info.max_energy);
  Write32(out, mode_info.gravity);

  Write32(out, DATA_MAGIC);                 // Data magic
  return !ferror(out);
}
