/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include <libxml++/libxml++.h>
#include "team/team.h"
#include "team/team_config.h"
#include "team/teams_list.h"
//-----------------------------------------------------------------------------
#include "character/character.h"
#include "character/body_list.h"
#include "include/action.h"
#include "game/config.h"
#include "network/network.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include "team/team_energy.h"
#include <algorithm>
#include <iostream>
#include "network/randomsync.h"

//-----------------------------------------------------------------------------
TeamsList *TeamsList::singleton = NULL;

TeamsList *TeamsList::GetInstance()
{
  if (singleton == NULL) {
    singleton = new TeamsList();
  }
  return singleton;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TeamsList::TeamsList():
  full_list(),
  playing_list(),
  selection(),
  active_team(playing_list.end())
{
  LoadList();
}

TeamsList::~TeamsList()
{
  /* The teamslist was never built... nothing to delete.
   * FIXME This is needed because we are lead to delete things even if they
   * were not created completely. IMHO, this reflects the fact that the object
   * life time is not well known...
   * Actually, this is not that bad whereas free(NULL) is accepted... but it
   * remains spurious. */
  if (!singleton)
    return;

  Clear();
  for(full_iterator it = full_list.begin(); it != full_list.end(); ++it)
    delete (*it);
  full_list.clear();
  singleton = NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::NextTeam ()
{
  Team* next = GetNextTeam();
  GetTeamsList().SetActive (next->GetId());
  Action a(Action::ACTION_GAMELOOP_NEXT_TEAM, next->GetId());
  Network::GetInstance()->SendAction(&a);
}

//-----------------------------------------------------------------------------

Team* TeamsList::GetNextTeam()
{
  // Next team
  std::vector<Team*>::iterator it=active_team;
  do
  {
    ++it;
    if (it == playing_list.end()) it = playing_list.begin();
  } while ((**it).NbAliveCharacter() == 0);
  return (*it);
}


//-----------------------------------------------------------------------------

Team& TeamsList::ActiveTeam()
{
  ASSERT (active_team != playing_list.end());
  return **active_team;
}

//-----------------------------------------------------------------------------

void TeamsList::LoadOneTeam(const std::string &dir, const std::string &team_name)
{
  // Skip '.', '..' and hidden files
  if (team_name[0] == '.') return;

  // Is it a directory ?
  if (!IsFolderExist(dir+team_name)) return;

  // Add the team
  try {
    full_list.push_back(new Team(dir, team_name));
    std::cout << ((1<full_list.size())?", ":" ") << team_name;
    std::cout.flush();
  }

  catch (char const *error) {
    std::cerr << std::endl
              << Format(_("Error loading team :")) << team_name <<":"<< error
              << std::endl;
    return;
  }

  catch (const xmlpp::exception &e) {
    std::cerr << std::endl
              << Format(_("Error loading team :")) << team_name << std::endl
              << e.what() << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::LoadList()
{
  playing_list.clear() ;

  std::cout << "o " << _("Load teams:");

  const Config * config = Config::GetInstance();

  // Load Wormux teams
  std::string dirname = config->GetDataDir() + PATH_SEPARATOR + "team" + PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    Error (Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }

  // Load personal teams
  dirname = config->GetPersonalDir() + "team" + PATH_SEPARATOR;
  f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneTeam(dirname, name);
    CloseFolder(f);
  } else {
    std::cerr << std::endl
      << Format(_("Cannot open personal teams directory (%s)!"), dirname.c_str())
      << std::endl;
  }

  full_list.sort(compareTeams);

  // We need at least 2 teams
  if (full_list.size() < 2)
    Error(_("You need at least two valid teams !"));

  // Default selection
  std::list<uint> nv_selection;
  nv_selection.push_back (0);
  nv_selection.push_back (1);
  ChangeSelection (nv_selection);

  std::cout << std::endl;
  InitList(Config::GetInstance()->AccessTeamList());
}

//-----------------------------------------------------------------------------

void TeamsList::LoadGamingData()
{
  std::sort(playing_list.begin(), playing_list.end(), compareTeams); // needed to fix bug #9820
  active_team = playing_list.begin();

  iterator it=playing_list.begin(), end=playing_list.end();

  // Load the data of all teams
  for (; it != end; ++it) (**it).LoadGamingData();
}

void TeamsList::RandomizeFirstPlayer()
{
  active_team = playing_list.begin();
  int skip = randomSync.GetLong(0, playing_list.size() - 1);
  for(int i = 0; i < skip; i++)
    active_team++;
}

//-----------------------------------------------------------------------------

void TeamsList::UnloadGamingData()
{
  body_list.FreeMem();
  iterator it=playing_list.begin(), end = playing_list.end();

  // Unload the data of all teams
  for (; it != end; ++it) (**it).UnloadGamingData();
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindById (const std::string &id, int &pos)
{
  full_iterator it=full_list.begin(), end = full_list.end();
  int i=0;
  for (; it != end; ++it, ++i)
  {
    if ((*it)->GetId() == id)
    {
      pos = i;
      return (*it);
    }
  }
  pos = -1;
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindByIndex (uint index)
{
  full_iterator it=full_list.begin(), end = full_list.end();
  uint i=0;
  for (; it != end; ++it, ++i)
  {
    if (i == index)
      return (*it);
  }
  ASSERT (false);
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindPlayingByIndex (uint index)
{
  ASSERT(index < playing_list.size());
  return playing_list[index];
}

//-----------------------------------------------------------------------------

Team* TeamsList::FindPlayingById(const std::string &id, int &index)
{
  iterator it = playing_list.begin(), end = playing_list.end();
  index=0;
  for (; it != end; ++it, ++index)
  {
    if ((*it) -> GetId() == id)
      return *it;
  }

  index = -1;
  ASSERT(false);
  return NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::InitList (const std::list<ConfigTeam> &lst)
{
  Clear();
  std::list<ConfigTeam>::const_iterator it=lst.begin(), end=lst.end();
  for (; it != end; ++it) {
    AddTeam (*it, false);
  }
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::InitEnergy()
{
  // Looking at team with the greatest energy
  // (in case teams does not have same amount of character)
  iterator it=playing_list.begin(), end = playing_list.end();
  uint max = 0;
  for (; it != end; ++it)
  {
    if( (**it).ReadEnergy() > max)
      max = (**it).ReadEnergy();
  }

  // Init each team's energy bar
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    (**it).InitEnergy (max);
  }

  // Initial ranking
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    uint rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    uint rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() == (**it).ReadEnergy() )
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
    if( (**it).energy.status != EnergyStatusWait) {
      waiting = false;
      break;
    }
  }

  // one of the energy bar is changing ?
  if(!waiting) {
    status = EnergyStatusOK;

    // change an energy bar value ?
    for (it=playing_list.begin(); it != end; ++it) {
      if( (**it).energy.status == EnergyStatusValueChange) {
        status = EnergyStatusValueChange;
        break;
      }
    }

    // change a ranking ?
    for (it=playing_list.begin(); it != end; ++it) {
      if( (**it).energy.status == EnergyStatusRankChange
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
  if(status != EnergyStatusOK || waiting) {
    it=playing_list.begin();
    for (; it != end; ++it) {
      (**it).energy.status = status;
    }
  }

  // Actualisation des valeurs (pas d'actualisation de l'affichage)
  for (it=playing_list.begin(); it != end; ++it) {
    (**it).UpdateEnergyBar();
    RefreshSort();
  }
}
//-----------------------------------------------------------------------------

void TeamsList::RefreshSort ()
{
  iterator it=playing_list.begin(), end = playing_list.end();
  uint rank;

  // Find a ranking without taking acount of the equalities
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    rank = 0;
    iterator it2=playing_list.begin();
    for (; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.rank_tmp = rank;
  }

  // Fix equalities
  it=playing_list.begin();
  for (; it != end; ++it)
  {
    rank = (**it).energy.rank_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != end; ++it2)
    {
      if((it != it2)
          && (**it2).ReadEnergy() == (**it).ReadEnergy() )
        ++rank;
    }
    (**it).energy.NewRanking(rank);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::ChangeSelection (const std::list<uint>& nv_selection)
{
  selection = nv_selection;

  selection_iterator it=selection.begin(), end = selection.end();
  playing_list.clear();
  for (; it != end; ++it) playing_list.push_back (FindByIndex(*it));
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

bool TeamsList::IsSelected (uint index)
{
  selection_iterator pos = std::find (selection.begin(), selection.end(), index);
  return pos != selection.end();
}

void TeamsList::Clear()
{
  selection.clear();
  playing_list.clear();
}

//-----------------------------------------------------------------------------

void TeamsList::AddTeam (const ConfigTeam &the_team_cfg, bool generate_error)
{
  int pos;
  Team *the_team = FindById (the_team_cfg.id, pos);
  if (the_team != NULL) {

    // set the player name and number of characters
    the_team->SetPlayerName(the_team_cfg.player_name);
    the_team->SetNbCharacters(the_team_cfg.nb_characters);

    selection.push_back (pos);
    playing_list.push_back (the_team);

  } else {
    std::string msg = Format(_("Can't find team %s!"), the_team_cfg.id.c_str());
    if (generate_error)
      Error (msg);
    else
      std::cout << "! " << msg << std::endl;
  }
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::UpdateTeam (const ConfigTeam &the_team_cfg, bool generate_error)
{
  int pos;
  Team *the_team = FindById (the_team_cfg.id, pos);
  if (the_team != NULL) {

    // set the player name and number of characters
    the_team->SetPlayerName(the_team_cfg.player_name);
    the_team->SetNbCharacters(the_team_cfg.nb_characters);

  } else {
    std::string msg = Format(_("Can't find team %s!"), the_team_cfg.id.c_str());
    if (generate_error)
      Error (msg);
    else
      std::cout << "! " << msg << std::endl;
  }
}

//-----------------------------------------------------------------------------

void TeamsList::DelTeam (const std::string &id)
{
  int pos;
  Team *equipe = FindById (id, pos);
  ASSERT(equipe != NULL);

  selection_iterator it = find(selection.begin(), selection.end(), (uint)pos);

  if (it != selection.end()) {
    selection.erase(it);
  }

  iterator playing_it = find(playing_list.begin(), playing_list.end(), equipe);

  if (playing_it != playing_list.end()) {
    playing_list.erase(playing_it);
  }

  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::SetActive(const std::string &id)
{
  iterator it = playing_list.begin(),
  end = playing_list.end();
  for (; it != end; ++it)
  {
    Team &team = **it;
    if (team.GetId() == id)
    {
      active_team = it;
      ActiveTeam().PrepareTurn();
      return;
    }
  }
  Error (Format(_("Can't find team %s!"), id.c_str()));
}

//-----------------------------------------------------------------------------

Team& ActiveTeam()
{
  return GetTeamsList().ActiveTeam();
}

//-----------------------------------------------------------------------------

Character& ActiveCharacter()
{
  return ActiveTeam().ActiveCharacter();
}

//-----------------------------------------------------------------------------

bool compareTeams(const Team *a, const Team *b)
{
  return a->GetName() < b->GetName();
}

//-----------------------------------------------------------------------------
