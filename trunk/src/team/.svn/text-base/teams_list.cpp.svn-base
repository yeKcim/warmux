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
 * Team handling
 *****************************************************************************/
#include <algorithm>
#include <iostream>

#include <WARMUX_file_tools.h>
#include <WARMUX_team_config.h>

#include "character/character.h"
#include "character/body_list.h"
#include "game/config.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "team/team.h"
#include "team/team_energy.h"
#include "team/teams_list.h"
#include "tool/ansi_convert.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TeamsList::TeamsList():
  full_list(),
  playing_list(),
  selection(),
  groups(),
  active_group(groups.end())
{
  LoadList();
}

TeamsList::~TeamsList()
{
  UnloadGamingData();
  Clear();
  for (full_iterator it = full_list.begin(); it != full_list.end(); ++it)
    delete (*it);
  full_list.clear();
  groups.clear();
}

//-----------------------------------------------------------------------------

void TeamsList::NextTeam()
{
  ActiveCharacter().StopPlaying();

  Team* next = GetNextTeam();
  SetActive(next->GetId());

  next->NextCharacter(true);

  printf("\nPlaying character : %i %s\n", ActiveCharacter().GetCharacterIndex(), ActiveCharacter().GetName().c_str());
  printf("Playing team : %i %s\n", ActiveCharacter().GetTeamIndex(), ActiveTeam().GetName().c_str());
  printf("Alive characters: %i / %i\n\n", ActiveTeam().NbAliveCharacter(),ActiveTeam().GetNbCharacters());
}

//-----------------------------------------------------------------------------

Team* TeamsList::GetNextTeam()
{
  // Next group
  GroupList::iterator git = active_group;
  std::vector<Team*>::iterator it;

  do {
    ++git;
    if (git == groups.end())
      git = groups.begin();

    it = git->second.active_team;
    do {
      ++it;
      if (it == git->second.end())
        it = git->second.begin();
    } while (!(*it)->NbAliveCharacter() && it != git->second.active_team);
  } while (git != active_group && !(*it)->NbAliveCharacter());
  
  return (*it);
}


//-----------------------------------------------------------------------------

Team& TeamsList::ActiveTeam()
{
  return **(active_group->second.active_team);
}

//-----------------------------------------------------------------------------

bool TeamsList::LoadOneTeam(const std::string &dir, const std::string &team_name)
{
  // Skip '.', '..' and hidden files
  if (team_name[0] == '.' || team_name == "SVN~1")
    return false;

  // Is it a directory ?
  if (!DoesFolderExist(dir+team_name))
    return false;

  // Add the team
  std::string real_name = ANSIToUTF8(dir, team_name);
  std::string error;
  Team *team = Team::LoadTeam(dir, real_name, error);
  if (team) {
    full_list.push_back(team);
    std::cout << ((1<full_list.size())?", ":" ") << real_name;
    std::cout.flush();
    return true;
  }

  std::cerr << std::endl
    << Format(_("Error loading team :")) << real_name <<":"<< error
    << std::endl;
  return false;
}

//-----------------------------------------------------------------------------

void TeamsList::LoadList()
{
  playing_list.clear();

  std::cout << "o " << _("Load teams:");

  const Config * config = Config::GetInstance();

  // Load Warmux teams
  std::string dirname = config->GetDataDir() + "team" PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *name;
    bool search_file = false;
    while ((name = FolderSearchNext(f, search_file)) != NULL)
      LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    Error(Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }

  // Load personal teams
  dirname = config->GetPersonalDataDir() + "team" PATH_SEPARATOR;
  f = OpenFolder(dirname);
  if (f) {
    bool search_files = false;
    const char *name;
    while ((name = FolderSearchNext(f, search_files)) != NULL)
      LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    std::cerr << std::endl
      << Format(_("Cannot open personal teams directory (%s)!"), dirname.c_str())
      << std::endl;
  }

  full_list.sort(compareTeams);

  // We need at least 2 teams
  if (full_list.size() < 2)
    Error(_("You need at least two valid teams!"));

  // Default selection
  std::list<uint> nv_selection;
  nv_selection.push_back(0);
  nv_selection.push_back(1);
  ChangeSelection(nv_selection);

  std::cout << std::endl;
  InitList(Config::GetInstance()->AccessTeamList());
}

//-----------------------------------------------------------------------------

void TeamsList::LoadGamingData(WeaponsList * weapons_list)
{
  //std::sort(playing_list.begin(), playing_list.end(), compareTeams); // needed to fix bug #9820

  iterator it=playing_list.begin(), end=playing_list.end();

  // Load the data of all teams
  for (; it != end; ++it) {
    (*it)->LoadGamingData(weapons_list);
  }

  groups.clear();
  for (it=playing_list.begin(); it != end; ++it) {
    groups[ (*it)->GetGroup() ].push_back(*it);
    if ((*it)->IsLocalAI())
      (*it)->LoadAI();
  }
}

void TeamsList::RandomizeFirstPlayer()
{
  MSG_DEBUG("random.get", "TeamList::RandomizeFirstPlayer()");
  int skip = RandomSync().GetInt(0, groups.size()-1);

  for (GroupList::iterator git = groups.begin(); git != groups.end(); ++git) {
    if (!(skip--))
      active_group = git;

    TeamGroup& g = git->second;
    int skip2 = RandomSync().GetInt(1, g.size());
    g.active_team = g.begin();
    while (--skip2)
     g.active_team++;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::UnloadGamingData()
{
  groups.clear();

  // Iterate over all teams not just he playing ones
  // in order to unload leaver teams.
  full_iterator it=full_list.begin(), end = full_list.end();

  // Unload the data of all teams
  for (; it != end; ++it)
    (**it).UnloadGamingData();

  // Run this now as no reference from Character are left
  BodyList::GetRef().FreeMem();
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindById(const std::string &id, int &pos)
{
  full_iterator it=full_list.begin(), end = full_list.end();
  int i=0;
  for (; it != end; ++it, ++i) {
    if ((*it)->GetId() == id) {
      pos = i;
      return (*it);
    }
  }
  pos = -1;
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindByIndex(uint index)
{
  if (full_list.size() < index+1) {
    ASSERT(false);
    return NULL;
  }

  full_iterator it = full_list.begin(), end = full_list.end();
  uint i=0;
  for (; it != end; ++it, ++i) {
    if (i == index)
      return (*it);
  }

  return NULL;
}

//-----------------------------------------------------------------------------

Team* TeamsList::FindPlayingById(const std::string &id, int &index)
{
  iterator it = playing_list.begin(), end = playing_list.end();
  index=0;
  for (; it != end; ++it, ++index) {
    if ((*it) -> GetId() == id)
      return *it;
  }

  index = -1;
  ASSERT(false);
  return NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::InitList(const std::list<ConfigTeam> &lst)
{
  Clear();
  std::list<ConfigTeam>::const_iterator it=lst.begin(), end=lst.end();
  for (; it != end; ++it)
    AddTeam(*it, true, false);
}

//-----------------------------------------------------------------------------

void TeamsList::InitEnergy()
{
  // Looking at team with the greatest energy
  // (in case teams does not have same amount of character)
  iterator it = playing_list.begin(), end = playing_list.end();
  uint max = 0;
  for (; it != end; ++it) {
    if ((**it).ReadEnergy() > max)
      max = (**it).ReadEnergy();
  }

  // Init each team's energy bar
  it=playing_list.begin();
  for (; it != end; ++it)
    (**it).InitEnergy(max);

  // Initial ranking
  it=playing_list.begin();
  for (; it != end; ++it) {
    uint rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2) {
      if (it != it2 && (**it2).ReadEnergy() > (**it).ReadEnergy())
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }
  it=playing_list.begin();
  for (; it != end; ++it) {
    uint rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2) {
      if (it != it2 && (**it2).ReadEnergy() == (**it).ReadEnergy())
        ++rank;
    }
    (**it).energy.SetRanking(rank);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::RefreshEnergy()
{
  // In the order of the priorit :
  // - finish current action
  // - change a teams energy
  // - change ranking
  // - prepare energy bar for next event

  iterator it=playing_list.begin(), end = playing_list.end();
  energy_t status;

  bool waiting = true; // every energy bar are waiting

  for (; it != end; ++it) {
    if ((**it).energy.status != EnergyStatusWait) {
      waiting = false;
      break;
    }
  }

  // one of the energy bar is changing ?
  if (!waiting) {
    status = EnergyStatusOK;

    // change an energy bar value ?
    for (it=playing_list.begin(); it != end; ++it) {
      if ((**it).energy.status == EnergyStatusValueChange) {
        status = EnergyStatusValueChange;
        break;
      }
    }

    // change a ranking ?
    for (it=playing_list.begin(); it != end; ++it) {
      if ((**it).energy.status == EnergyStatusRankChange
          && ((**it).energy.IsMoving() || status == EnergyStatusOK)) {
        status = EnergyStatusRankChange;
        break;
      }
    }
  }
  else {
    // every energy bar are waiting
    // -> set state ready for a new event
    status = EnergyStatusOK;
  }

  // Setting event to process in every energy bar
  if (status != EnergyStatusOK || waiting) {
    it = playing_list.begin();
    for (; it != end; ++it) {
      (**it).energy.status = status;
    }
  }

  // Actualisation des valeurs (pas d'actualisation de l'affichage)
  for (it=playing_list.begin(); it != end; ++it) {
    (**it).UpdateEnergyBar();
  }

  RefreshSort();
}
//-----------------------------------------------------------------------------

void TeamsList::RefreshSort()
{
  iterator it=playing_list.begin(), end = playing_list.end();
  uint rank;

  // Find a ranking without taking acount of the equalities
  it = playing_list.begin();
  for (; it != end; ++it) {
    rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2) {
      if (it != it2 && (**it2).ReadEnergy() > (**it).ReadEnergy())
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }

  // Fix equalities
  it = playing_list.begin();
  for (; it != end; ++it) {
    rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2) {
      if (it != it2 && (**it2).ReadEnergy() == (**it).ReadEnergy())
        ++rank;
    }
    (**it).energy.NewRanking(rank);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::ChangeSelection(const std::list<uint>& nv_selection)
{
  selection = nv_selection;

  selection_iterator it=selection.begin(), end = selection.end();
  playing_list.clear();
  for (; it != end; ++it)
    playing_list.push_back(FindByIndex(*it));
}

//-----------------------------------------------------------------------------

bool TeamsList::IsSelected(uint index)
{
  selection_iterator pos = std::find(selection.begin(), selection.end(), index);
  return pos != selection.end();
}

//-----------------------------------------------------------------------------

void TeamsList::AddTeam(Team* the_team, int pos, const ConfigTeam &the_team_cfg,
                        bool is_local)
{
  ASSERT(the_team != NULL);

  the_team->SetRemote(!is_local);
  UpdateTeam(the_team, the_team_cfg);

  selection.push_back(pos);
  playing_list.push_back(the_team);
}

void TeamsList::AddTeam(const ConfigTeam &the_team_cfg, bool is_local,
                        bool generate_error)
{
  MSG_DEBUG("team", "%s, local: %d\n", the_team_cfg.id.c_str(), is_local);

  int pos;
  Team *the_team = FindById(the_team_cfg.id, pos);
  if (the_team != NULL) {
    AddTeam(the_team, pos, the_team_cfg, is_local);
  } else {
    std::string msg = Format(_("Can't find team %s!"), the_team_cfg.id.c_str());
    if (generate_error)
      Error (msg);
    else
      std::cout << "! " << msg << std::endl;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::UpdateTeam(Team* the_team, const ConfigTeam &the_team_cfg)
{
  ASSERT(the_team);

  // set the player name and number of characters
  the_team->SetPlayerName(the_team_cfg.player_name);
  the_team->SetNbCharacters(the_team_cfg.nb_characters);
  the_team->SetAIName(the_team_cfg.ai);
  the_team->SetGroup(the_team_cfg.group);
}

Team* TeamsList::UpdateTeam(const std::string& old_team_id,
                            const ConfigTeam &the_team_cfg)
{
  int pos;
  Team *the_team = NULL;

  MSG_DEBUG("team", "%s/%s\n", old_team_id.c_str(), the_team_cfg.id.c_str());

  if (old_team_id == the_team_cfg.id) {
    // this is a simple update

    the_team = FindById(the_team_cfg.id, pos);
    if (the_team) {
      UpdateTeam(the_team, the_team_cfg);
    } else {
      Error(Format(_("Can't find team %s!"), the_team_cfg.id.c_str()));
    }
    return the_team;

  }

  // here we are replacing a team by another one
  Team *the_old_team = FindById(old_team_id, pos);
  if (!the_old_team) {
    Error(Format(_("Can't find team %s!"), old_team_id.c_str()));
    return NULL;
  }

  the_team = FindById(the_team_cfg.id, pos);
  if (!the_team) {
    Error(Format(_("Can't find team %s!"), old_team_id.c_str()));
    return NULL;
  }

  bool is_local = the_old_team->IsLocal();
  DelTeam(the_old_team);
  AddTeam(the_team, pos, the_team_cfg, is_local);
  return the_team;
}

//-----------------------------------------------------------------------------

void TeamsList::DelTeam(Team* the_team)
{
  uint pos = 0;

  ASSERT(the_team);

  MSG_DEBUG("team", "%s\n", the_team->GetId().c_str());

  the_team->SetDefaultPlayingConfig();

  selection_iterator it = std::find(selection.begin(), selection.end(), pos);

  if (it != selection.end())
    selection.erase(it);

  iterator playing_it = std::find(playing_list.begin(), playing_list.end(), the_team);

  ASSERT(playing_it != playing_list.end());

  if (playing_it != playing_list.end())
    playing_list.erase(playing_it);
}

void TeamsList::DelTeam(const std::string &id)
{
  int pos;
  Team *the_team = FindById(id, pos);

  DelTeam(the_team);
}

//-----------------------------------------------------------------------------

void TeamsList::SetActive(const std::string &id)
{
  for (GroupList::iterator git = groups.begin(); git != groups.end(); ++git) {
    for (TeamGroup::iterator it = git->second.begin(); it != git->second.end(); ++it) {
      if ((*it)->GetId() == id) {
        active_group = git;
        git->second.active_team = it;
        (*it)->PrepareTurn();
        return;
      }
    }
  }

  Error(Format(_("Can't find team %s!"), id.c_str()));
}

Character& ActiveCharacter()
{
  return ActiveTeam().ActiveCharacter();
}

bool compareTeams(const Team *a, const Team *b)
{
  return a->GetName() < b->GetName();
}
