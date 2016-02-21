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
 * Macros to manage teams.
 *****************************************************************************/

#ifndef TEAM_MACRO_H
#define TEAM_MACRO_H
//-----------------------------------------------------------------------------
#include "team/teams_list.h"
//-----------------------------------------------------------------------------

// Boucle pour chaque team
#define FOR_EACH_TEAM(team) \
  for (TeamsList::iterator team=GetTeamsList().playing_list.begin(), \
       end_of_each_team = GetTeamsList().playing_list.end(); \
       team != end_of_each_team; \
       ++team)

#define FOR_EACH_LIVING_AND_DEAD_CHARACTER(team, character) \
  for (Team::iterator character = (*(team)).begin(), \
       end_character = (*(team)).end(); \
       character != end_character; \
       ++character) \

#define FOR_EACH_CHARACTER(team,character) \
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team,character) \
  if (!character -> IsGhost())

#define FOR_EACH_LIVING_CHARACTER(team,character) \
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team,character) \
  if (!character -> IsDead())

#define FOR_ALL_LIVING_AND_DEAD_CHARACTER(team, character) \
  FOR_EACH_TEAM(team) \
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(*team,character)

#define FOR_ALL_CHARACTERS(team,character) \
  FOR_EACH_TEAM(team) \
  FOR_EACH_CHARACTER(*team,character)

#define FOR_ALL_LIVING_CHARACTERS(team,character) \
  FOR_EACH_TEAM(team) \
  FOR_EACH_LIVING_CHARACTER(*team,character)

#define FOR_ALL_LIVING_ENEMIES(shooter,team,character) \
  FOR_EACH_TEAM(team) \
  if (!(*team)->IsSameAs(shooter.GetTeam())) \
  FOR_EACH_LIVING_CHARACTER(*team,character)

//-----------------------------------------------------------------------------
#endif
