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
#include "../include/app.h"
#include "../include/constant.h"
#include "results.h"
#include "../character/character.h"
#include "team.h"
#include "macro.h"
#include "../tool/i18n.h"

TeamResults::TeamResults(const std::string& name,
			 const Surface* logo,
                         const Character* MV,
                         const Character* MUl,
                         const Character* MUs,
                         const Character* BT)
  : teamName(name),
    team_logo(logo),
    mostViolent(MV),
    mostUsefull(MUl),
    mostUseless(MUs),
    biggestTraitor(BT)
{
}

TeamResults* TeamResults::createTeamResults(Team* team)
{
  int         most_violent    = 0;
  int         most_useless    = 0x0FFFFFFF;
  int         most_usefull    = 0;
  int         most_traitor    = 0;
  const Character* MostViolent = NULL;
  const Character* MostUsefull = NULL;
  const Character* MostUseless = NULL;
  const Character* BiggestTraitor = NULL;

  // Search best/worst performers
  for (Team::const_iterator player = team->begin(),
       last_player = team->end();
       player != last_player;
       ++player) 
    //FOR_EACH_CHARACTER(team, player)
  {
    // Most damage in one shot
    if (player->GetMostDamage() > most_violent)
    {
      most_violent = player->GetMostDamage();
      MostViolent  = &(*(player));
    }
    // Most damage oplayerall to other teams
    if (player->GetOtherDamage() > most_usefull)
    {
      most_usefull = player->GetOtherDamage();
      MostUsefull  = &(*(player));
    }
    // Least damage oplayerall to other teams
    if (player->GetOtherDamage() < most_useless)
    {
      most_useless = player->GetOtherDamage();
      MostUseless  = &(*(player));
    }
    // Most damage oplayerall to his own team
    if (player->GetOwnDamage() > most_traitor)
    {
      most_traitor = player->GetOwnDamage();
      BiggestTraitor  = &(*(player));
    }
  }

  return new TeamResults(team->GetName()+" - "+team->GetPlayerName(),
			 &team->flag,
                         MostViolent,
                         MostUsefull,
                         MostUseless,
                         BiggestTraitor);
}

TeamResults* TeamResults::createGlobalResults(std::vector<TeamResults*>* list)
{
  int         most_violent    = 0;
  int         most_useless    = 0x0FFFFFFF;
  int         most_usefull    = 0;
  int         most_traitor    = 0;
  const Character* MostViolent = NULL;
  const Character* MostUsefull = NULL;
  const Character* MostUseless = NULL;
  const Character* BiggestTraitor = NULL;

  for (res_iterator result=list->begin(), last_result=list->end();
       result != last_result;
       ++result)
  {
    const Character* player;
    // Most damage in one shot
    player = (*(result))->getMostViolent();
    if(player == NULL) continue;
    if (player->GetMostDamage() > most_violent)
    {
      most_violent = player->GetMostDamage();
      MostViolent  = player;
    }
    // Most damage oplayerall to other teams
    if (player->GetOtherDamage() > most_usefull)
    {
      most_usefull = player->GetOtherDamage();
      MostUsefull  = player;
    }
    // Least damage oplayerall to other teams
    if (player->GetOtherDamage() < most_useless)
    {
      most_useless = player->GetOtherDamage();
      MostUseless  = player;
    }
    // Most damage oplayerall to his own team
    if (player->GetOwnDamage() > most_traitor)
    {
      most_traitor = player->GetOwnDamage();
      BiggestTraitor  = player;
    }
  }

  // We'll do as if NULL is for all teams
  return new TeamResults(_("All teams"),
			 NULL,
                         MostViolent,
                         MostUsefull,
                         MostUseless,
                         BiggestTraitor);
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
  results = TeamResults::createGlobalResults(results_list);
  results_list->push_back(results);

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

void TeamResults::RenderText(std::string& txt)
{
  std::ostringstream mv;
  std::ostringstream muf;
  std::ostringstream mul;
  std::ostringstream bt;
  if(mostViolent)
    mv << mostViolent->GetName() << " (" << mostViolent->GetMostDamage() << ").\n";
  else
    mv << _("Nobody!");

  if(mostUsefull)
    mv << mostUsefull->GetName() << " (" << mostUsefull->GetMostDamage() << ").\n";
  else
    mv << _("Nobody!");

  if(mostUseless)
    mv << mostUseless->GetName() << " (" << mostUseless->GetMostDamage() << ").\n";
  else
    mv << _("Nobody!");

  if(biggestTraitor)
    mv << biggestTraitor->GetName() << " (" << biggestTraitor->GetMostDamage() << ").\n";
  else
    mv << _("Nobody!");

  if (team_logo)
    txt += Format(_("Team %s results:\n"), teamName.c_str());
  else
    txt += _("All teams results:\n");
  txt += _("  Most violent  :  ") + mv.str();
  txt += _("  Most useful   :  ") + muf.str();
  txt += _("  Most useless  :  ") + mul.str();
  txt += _("  Most sold-out  :  ") + bt.str();
}
