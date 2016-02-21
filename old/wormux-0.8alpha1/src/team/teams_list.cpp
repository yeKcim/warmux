/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Refresh des diff�entes �uipes.
 *****************************************************************************/

#include "teams_list.h"
//-----------------------------------------------------------------------------
#include "../character/body_list.h"
#include "../include/action_handler.h"
#include "../game/config.h"
#include "../tool/file_tools.h"
#include "../tool/i18n.h"
#include "team_energy.h"
#include <algorithm>
#include <iostream>

#if !defined(WIN32) || defined(__MINGW32__)
#include <dirent.h>
#include <sys/stat.h>
#endif
//-----------------------------------------------------------------------------
TeamsList teams_list;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TeamsList::TeamsList()
{}

TeamsList::~TeamsList()
{
  Clear();
  full_list.clear();
}

//-----------------------------------------------------------------------------

void TeamsList::NextTeam (bool begin_game)
{
  // Fin du tour pour l'�uipe active
  if (begin_game) return;
   
  // Passe �l'�uipe suivante
  std::vector<Team*>::iterator it=active_team;
  do
    {
      ++it;
      if (it == playing_list.end()) it = playing_list.begin();
    } while ((**it).NbAliveCharacter() == 0);
  ActionHandler::GetInstance()->NewAction(new Action(ACTION_CHANGE_TEAM, (**it).GetId()));
}

//-----------------------------------------------------------------------------

Team& TeamsList::ActiveTeam()
{
  assert (active_team != playing_list.end());
  return **active_team;
}

//-----------------------------------------------------------------------------

void TeamsList::LoadOneTeam(const std::string &dir, const std::string &team)
{
  // Skip '.', '..' and hidden files
  if (team[0] == '.') return;
  
#if !defined(WIN32) || defined(__MINGW32__)
  // Is it a directory ?
  struct stat stat_file;
  std::string filename = dir+team;
  if (stat(filename.c_str(), &stat_file) != 0) return;
  if (!S_ISDIR(stat_file.st_mode)) return;
#endif
	
  // Add the team
  Team * tmp = Team::CreateTeam (dir, team);
  if (tmp != NULL) {
    full_list.push_back(*tmp);
    std::cout << ((1<full_list.size())?", ":" ") << team;
    std::cout.flush();
  }
}

//-----------------------------------------------------------------------------

void TeamsList::LoadList()
{  
  playing_list.clear() ;
   
  std::cout << "o " << _("Load teams:");
  
  // Load Wormux teams
  std::string dirname = Config::GetInstance()->GetDataDir() + PATH_SEPARATOR + "team" + PATH_SEPARATOR;
#if !defined(WIN32) || defined(__MINGW32__)
  struct dirent *file;
  DIR *dir = opendir(dirname.c_str());
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL)  LoadOneTeam (dirname, file->d_name);
    closedir (dir);
  } else {
	Error (Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }
#else
  std::string pattern = dirname + "*.*";
  WIN32_FIND_DATA file;
  HANDLE file_search;
  file_search=FindFirstFile(pattern.c_str(),&file);
  if(file_search != INVALID_HANDLE_VALUE)
  {
    while (FindNextFile(file_search,&file))
	{
	  if(file.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	    LoadOneTeam(dirname,file.cFileName);
	}
  } else {
	Error (Format(_("Cannot open teams directory (%s)!"), dirname.c_str()));
  }
  FindClose(file_search);
#endif

  // Load personal teams
#if !defined(WIN32) || defined(__MINGW32__)
  dirname = Config::GetInstance()->GetPersonalDir() + PATH_SEPARATOR 
    + "team" + PATH_SEPARATOR;
  dir = opendir(dirname.c_str());
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL) LoadOneTeam (dirname, file->d_name);
    closedir (dir);
  }
#endif

  teams_list.full_list.sort(compareTeams);

  // We need at least 2 teams
  if (full_list.size() < 2)
    Error(_("You need at least two valid teams !"));

  // Default selection
  std::list<uint> nv_selection;
  nv_selection.push_back (0);
  nv_selection.push_back (1);
  ChangeSelection (nv_selection);

  std::cout << std::endl;
}

//-----------------------------------------------------------------------------

void TeamsList::LoadGamingData(uint how_many_characters)
{
  active_team = playing_list.begin();

  iterator it=playing_list.begin(), end=playing_list.end();

  // Load the data of all teams
  for (; it != end; ++it) (**it).LoadGamingData(how_many_characters);
}

//-----------------------------------------------------------------------------

void TeamsList::UnloadGamingData()
{ 
  body_list.FreeMem();
  iterator it=playing_list.begin(), end=playing_list.end();

  // Unload the data of all teams
  for (; it != end; ++it) (**it).UnloadGamingData();
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindById (const std::string &id, int &pos)
{
  full_iterator it=full_list.begin(), fin=full_list.end();
  int i=0;
  for (; it != fin; ++it, ++i) 
  {
    if (it -> GetId() == id) 
    {
      pos = i;
      return &(*it);
    }
  }
  pos = -1;
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindByIndex (uint index)
{
  full_iterator it=full_list.begin(), fin=full_list.end();
  uint i=0;
  for (; it != fin; ++it, ++i) 
  {
    if (i == index) return &(*it);
  }
  assert (false);
  return NULL;
}

//-----------------------------------------------------------------------------

Team *TeamsList::FindPlayingByIndex (uint index)
{
  assert(index < playing_list.size());
  return playing_list[index];
}

//-----------------------------------------------------------------------------

Team* TeamsList::FindPlayingById(const std::string &id, uint &index)
{
  iterator it = playing_list.begin(), end = playing_list.end();
  index=0;
  for (; it != end; ++it, ++index) 
  {
    if ((*it) -> GetId() == id) 
      return *it;
  }
  assert(false);
  return NULL;
}

//-----------------------------------------------------------------------------

void TeamsList::InitList (const std::list<std::string> &liste_nom)
{
  Clear();
  std::list<std::string>::const_iterator it=liste_nom.begin(), fin=liste_nom.end();
  for (; it != fin; ++it) AddTeam (*it, false);
  active_team = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::InitEnergy ()
{
  //On cherche l'�uipe avec le plus d'�ergie pour fixer le niveau max
  //(arrive dans le cas d'�uipe n'ayant pas le m�e nombre de vers)
  iterator it=playing_list.begin(), fin=playing_list.end();
  uint max = 0;
  for (; it != fin; ++it)
  {
    if( (**it).ReadEnergy() > max)
      max = (**it).ReadEnergy();
  }

  //Initialisation de la barre d'�ergie de chaque �uipe
  it=playing_list.begin();
  uint i = 0;
  for (; it != fin; ++it)
  {
    (**it).InitEnergy (max);
    ++i;
  }

  //Calcul du classement initial
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    uint classement = 0;
    iterator it2=playing_list.begin();
    for (; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++classement;
    }
    (**it).energy.classement_tmp = classement;
  }
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    uint classement = (**it).energy.classement_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).ReadEnergy() == (**it).ReadEnergy() )
        ++classement;
    }
    (**it).energy.FixeClassement(classement);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::RefreshEnergy ()
{
  //Dans l'ordre des priorit� :
  // - Terminer l'op�ation en cours
  // - On change la valeur de l'�ergie
  // - On change le classement
  // - On pr�are les jauges �l'�enement suivant
  
  iterator it=playing_list.begin(), fin=playing_list.end();
  energy_t status;

  bool en_attente = true; // Toute les jauges sont en attente

  for (; it != fin; ++it) {
    if( (**it).energy.status != EnergyStatusWait)
    {
      en_attente = false;
      break;
    }
  }

  //Une des jauge ��ute un ordre?
  if(!en_attente)
  {
    status = EnergyStatusOK;

    //Une des jauges change de valeur?
    for (it=playing_list.begin(); it != fin; ++it) {
      if( (**it).energy.status == EnergyStatusValueChange) {
        status = EnergyStatusValueChange;
        break;
      }
    }
  
    //Une des jauges change de classement?
    for (it=playing_list.begin(); it != fin; ++it) {
      if( (**it).energy.status == EnergyStatusClassementChange
      && ((**it).energy.IsMoving() || status == EnergyStatusOK)) {
        status = EnergyStatusClassementChange;
        break;
      }
    }
  }
  else {
    //Les jauges sont toutes en attente
    //->on les met OK pour un nouvel ordre
    status = EnergyStatusOK;
  }

  // On recopie l'ordre a donner aux jauges
  if(status != EnergyStatusOK || en_attente)
  {
    it=playing_list.begin();
    for (; it != fin; ++it) {
      (**it).energy.status = status;
    }
  }

  // Actualisation des valeurs (pas d'actualisation de l'affichage)
  for (it=playing_list.begin(); it != fin; ++it)
  {
    (**it).UpdateEnergyBar();
    RefreshSort();
  }
}
//-----------------------------------------------------------------------------

void TeamsList::RefreshSort ()
{
  iterator it=playing_list.begin(), fin=playing_list.end();
  uint classement;
  
  //Cherche le classement sans tenir comte des �alit�
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    classement = 0;
    iterator it2=playing_list.begin();
    for (; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).ReadEnergy() > (**it).ReadEnergy() )
        ++classement;
    }
    (**it).energy.classement_tmp = classement;
  }

  //R�lage des �alit�
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    classement = (**it).energy.classement_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).ReadEnergy() == (**it).ReadEnergy() )
        ++classement;
    }
    (**it).energy.NouveauClassement(classement);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::ChangeSelection (const std::list<uint>& nv_selection)
{
  selection = nv_selection;

  selection_iterator it=selection.begin(), fin=selection.end();
  playing_list.clear();
  for (; it != fin; ++it) playing_list.push_back (FindByIndex(*it));
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

void TeamsList::AddTeam (const std::string &id, bool generate_error)
{
  int pos;
  Team *equipe = FindById (id, pos);
  if (equipe != NULL) {
    selection.push_back (pos);
    playing_list.push_back (equipe);
  } else {
    std::string msg = Format(_("Can't find team %s!"), id.c_str());
    if (generate_error)
      Error (msg);
    else
      std::cout << "! " << msg << std::endl;
  }
  active_team = playing_list.begin();
}
  
//-----------------------------------------------------------------------------

void TeamsList::DelTeam (const std::string &id)
{
    int pos;
    Team *equipe = FindById (id, pos);
    assert(equipe != NULL);
    
    selection.erase(find(selection.begin(),selection.end(),(uint)pos));
    playing_list.erase(find(playing_list.begin(),playing_list.end(),equipe));

   active_team = playing_list.begin();
}
  
//-----------------------------------------------------------------------------

void TeamsList::SetActive(const std::string &id)
{
	iterator
		it = playing_list.begin(),
		end = playing_list.end();
	for (; it != end; ++it)
	{
		Team &team = **it;
		if (team.GetId() == id)
		{
			active_team = it;
			return;
		}
	}
	Error (Format(_("Can't find team %s!"), id.c_str()));
}
  
//-----------------------------------------------------------------------------

Team& ActiveTeam()
{ return teams_list.ActiveTeam(); }

//-----------------------------------------------------------------------------

Character& ActiveCharacter()
{ return ActiveTeam().ActiveCharacter(); }

//-----------------------------------------------------------------------------

bool compareTeams(const Team& a, const Team& b)
{
  return a.GetName() < b.GetName();
}

//-----------------------------------------------------------------------------
