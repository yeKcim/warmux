/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2006 Lawrence Azzoug.
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
 * Manage the end of game results of a team
 *****************************************************************************/

#include <sstream>
#include "include/app.h"
#include "include/constant.h"
#include "results.h"
#include "character/character.h"
#include "team.h"
#include "macro.h"
#include "tool/i18n.h"

TeamResults::TeamResults(const std::string& name,
			 const Surface* logo,
                         const Character* MV,
                         const Character* MUl,
                         const Character* MUs,
                         const Character* BT,
			 const Character* MS)
  : teamName(name),
    team_logo(logo),
    mostViolent(MV),
    mostUseful(MUl),
    mostUseless(MUs),
    biggestTraitor(BT),
    mostClumsy(MS)
{
}

TeamResults* TeamResults::createTeamResults(Team* team)
{
  uint most_violent = 0;
  uint most_useless = 0x0FFFFFFF;
  uint most_useful = 0;
  uint most_traitor = 0;
  uint most_clumsy = 0;

  const Character* MostViolent = NULL;
  const Character* MostUseful = NULL;
  const Character* MostUseless = NULL;
  const Character* BiggestTraitor = NULL;
  const Character* MostClumsy = NULL;

  // Search best/worst performers
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team, player)
  {
    // Most damage in one shot
    if (player->GetDamageStats().GetMostDamage() > most_violent)
    {
      most_violent = player->GetDamageStats().GetMostDamage();
      MostViolent  = &(*(player));
    }
    // Most damage oplayerall to other teams
    if (player->GetDamageStats().GetOthersDamage() > most_useful)
    {
      most_useful = player->GetDamageStats().GetOthersDamage();
      MostUseful  = &(*(player));
    }
    // Least damage oplayerall to other teams
    if (player->GetDamageStats().GetOthersDamage() < most_useless)
    {
      most_useless = player->GetDamageStats().GetOthersDamage();
      MostUseless  = &(*(player));
    }
    // Most damage oplayerall to his own team (but not itself)
    if (player->GetDamageStats().GetFriendlyFireDamage() > most_traitor)
    {
      most_traitor = player->GetDamageStats().GetFriendlyFireDamage();
      BiggestTraitor  = &(*(player));
    }
    // Most damage to itself
    if (player->GetDamageStats().GetItselfDamage() > most_clumsy)
    {
      most_clumsy = player->GetDamageStats().GetItselfDamage();
      MostClumsy = &(*(player));
    }
    
  }

  return new TeamResults(team->GetName()+" - "+team->GetPlayerName(),
			 &team->flag,
                         MostViolent,
                         MostUseful,
                         MostUseless,
                         BiggestTraitor,
			 MostClumsy);
}

TeamResults* TeamResults::createGlobalResults()
{
  uint most_violent = 0;
  uint most_useless = 0x0FFFFFFF;
  uint most_useful = 0;
  uint most_traitor = 0;
  uint most_clumsy = 0;
  const Character* MostViolent = NULL;
  const Character* MostUseful = NULL;
  const Character* MostUseless = NULL;
  const Character* BiggestTraitor = NULL;
  const Character* MostClumsy = NULL;

  FOR_ALL_LIVING_AND_DEAD_CHARACTER(team, player)
  {
    // Most damage in one shot
    if (player->GetDamageStats().GetMostDamage() > most_violent)
    {
      most_violent = player->GetDamageStats().GetMostDamage();
      MostViolent  = &(*player);
    }
    // Most damage oplayerall to other teams
    if (player->GetDamageStats().GetOthersDamage() > most_useful)
    {
      most_useful = player->GetDamageStats().GetOthersDamage();
      MostUseful  = &(*player); 
    }
    // Least damage oplayerall to other teams
    if (player->GetDamageStats().GetOthersDamage() < most_useless)
    {
      most_useless = player->GetDamageStats().GetOthersDamage();
      MostUseless  = &(*player); 
    }
    // Most damage oplayerall to his own team (but not to itself)
    if (player->GetDamageStats().GetFriendlyFireDamage() > most_traitor)
    {
      most_traitor = player->GetDamageStats().GetFriendlyFireDamage();
      BiggestTraitor = &(*player);
    }
    // Most damage to itself
    if (player->GetDamageStats().GetItselfDamage() > most_traitor)
    {
      most_clumsy = player->GetDamageStats().GetItselfDamage();
      MostClumsy  = &(*player); 
    }
    
  }

  // We'll do as if NULL is for all teams
  return new TeamResults(_("All teams"),
			 NULL,
                         MostViolent,
                         MostUseful,
                         MostUseless,
                         BiggestTraitor,
			 MostClumsy);
}

std::vector<TeamResults*>* TeamResults::createAllResults(void)
{
  TeamResults* results;
  std::vector<TeamResults*>* results_list = new std::vector<TeamResults*>;

  // Build results list
  FOR_EACH_TEAM(team)
  {
    results = TeamResults::createTeamResults(*team);

    results_list->push_back(results);
  }
 
  // Add overall results to list
  results = TeamResults::createGlobalResults();
  results_list->insert(results_list->begin(), results);

  return results_list;
}

void TeamResults::deleteAllResults(std::vector<TeamResults*>* results_list)
{
  // Build results list
  for (int i=results_list->size()-1; i==0; i--)
    delete ((*results_list)[i]);
 
  // Add overall results to list
  delete results_list;
}
