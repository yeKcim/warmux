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

#include <iostream>
#include "include/app.h"
#include "include/constant.h"
#include "results.h"
#include "character/character.h"
#include "character/damage_stats.h"
#include "team.h"
#include "macro.h"

TopCharacters::TopCharacters()
  : Violent(NULL)
  , violence(0)
  , Useful(NULL)
  , usefulness(0)
  , Useless(NULL)
  , uselessness(0xFFFFFFFFU)
  , Traitor(NULL)
  , treachery(0)
  , Clumsy(NULL)
  , clumsyness(0)
  , Accurate(NULL)
  , accuracy(0.0)
  , death_time(0)
{}


void TopCharacters::merge(const TopCharacters* other)
{
  // Most damage in one shot
  if (other->violence > violence)
  {
    violence   = other->violence;
    Violent    = other->Violent;
  }
  // Most damage oplayerall to other teams
  if (other->usefulness > usefulness)
  {
    usefulness = other->usefulness;
    Useful     = other->Useful;
  }
  // Least damage oplayerall to other teams
  if (other->uselessness < uselessness)
  {
    uselessness = other->uselessness;
    Useless     = other->Useless;
  }
  // Most damage oplayerall to his own team (but not itself)
  if (other->treachery > treachery)
  {
    treachery   = other->treachery;
    Traitor     = other->Traitor;
  }
  // Most damage to itself
  if (other->clumsyness > clumsyness)
  {
    clumsyness = other->clumsyness;
    Clumsy     = other->Clumsy;
  }
  // Most accurate
  if (other->accuracy > accuracy)
  {
    accuracy   = other->accuracy;
    Accurate   = other->Accurate;
  }
  // This player is the latest survivor ?
  if (other->death_time > death_time)
  {
    death_time = other->death_time;
  }
}

void TopCharacters::rankPlayer(const Character *player)
{
  TopCharacters           top;
  const DamageStatistics  *stats = player->GetDamageStats();

  // Build a pseudo top from that player
  top.Violent  = player;
  top.Useful   = player;
  top.Useless  = player;
  top.Traitor  = player;
  top.Clumsy   = player;
  top.Accurate = player;

  top.violence    = stats->GetMostDamage();
  top.accuracy    = stats->GetAccuracy();
  top.usefulness  = stats->GetOthersDamage();
  top.uselessness = stats->GetOthersDamage();
  top.treachery   = stats->GetFriendlyFireDamage();
  top.clumsyness  = stats->GetItselfDamage();
  top.accuracy    = stats->GetAccuracy();
  top.death_time  = stats->GetDeathTime();

  merge(&top);
}

TeamResults::TeamResults(const Team* _team, TopCharacters* _top)
  : team(_team)
  , top(_top)
{}

TeamResults::~TeamResults()
{
  if (top)
    delete top;
}

TeamResults* TeamResults::createTeamResults(Team* team)
{
  TopCharacters* top = new TopCharacters;

  // Search best/worst performers
  FOR_EACH_LIVING_AND_DEAD_CHARACTER(team, player)
  {
    top->rankPlayer(&(*(player)));
  }

  return new TeamResults(team, top);
}

TeamResults* TeamResults::createGlobalResults()
{
  TopCharacters* top = new TopCharacters;

  FOR_ALL_LIVING_AND_DEAD_CHARACTER(team, player)
  {
    top->rankPlayer(&(*(player)));
  }

  // We'll do as if NULL is for all teams
  //top->death_time = 0;
  return new TeamResults(NULL, top);
}

std::vector<TeamResults*>* TeamResults::createAllResults(void)
{
  TeamResults                *results;
  TopCharacters              *top = new TopCharacters;
  std::vector<TeamResults*>  *results_list = new std::vector<TeamResults*>;

  // Build results list
  FOR_EACH_TEAM(team)
  {
    results = TeamResults::createTeamResults(*team);

    results_list->push_back(results);

    top->merge(results->getTopCharacters());
  }

  // Add overall results to list
  results = new TeamResults(NULL, top);
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
