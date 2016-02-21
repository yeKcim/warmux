/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Refresh des différentes équipes.
 *****************************************************************************/

#include "teams_list.h"
//-----------------------------------------------------------------------------
#include "../include/action_handler.h"
#ifdef CL
#include "../network/network.h"
#endif
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

TeamsList::TeamsList()
{}

//-----------------------------------------------------------------------------

void TeamsList::NextTeam (bool debut_jeu)
{
  // Fin du tour pour l'équipe active
  if (debut_jeu) return;
#ifdef CL
  if (network.is_client()) return;
  ActiveTeam().FinTour();
#endif
   
  // Passe à l'équipe suivante
  std::vector<Team*>::iterator it=m_equipe_active;
  do
    {
      ++it;
      if (it == playing_list.end()) it = playing_list.begin();
    } while ((**it).NbAliveCharacter() == 0);
  action_handler.NewAction(ActionString(ACTION_CHANGE_TEAM, (**it).GetId()));
}

//-----------------------------------------------------------------------------

Team& TeamsList::ActiveTeam()
{
  assert (m_equipe_active != playing_list.end());
  return **m_equipe_active;
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
	
  // Add a new empty team
  Team nv_equipe;
  full_list.push_back(nv_equipe);

  // Try to load team 
  bool ok = full_list.back().Init (dir, team);

  // If fails, remove the team
  if (!ok)
  {
    full_list.pop_back();
	return;
  }
  std::cout << ((1<full_list.size())?", ":" ") << team;
  std::cout.flush();
}

//-----------------------------------------------------------------------------

void TeamsList::LoadList()
{  
  playing_list.clear() ;
   
  std::cout << "o " << _("Load teams:");
  
  // Load Wormux teams
  std::string dirname = Wormux::config.data_dir+"team"+PATH_SEPARATOR;
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
  dirname = Wormux::config.GetWormuxPersonalDir()+"team/";
  dir = opendir(dirname.c_str());
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL) LoadOneTeam (dirname, file->d_name);
    closedir (dir);
  }
#endif

  teams_list.full_list.sort(compareTeams);

  // On a au moins deux équipes ?
  if (full_list.size() < 2)
    Error(_("You need at least two valid teams !"));

  // Sélection bidon
  std::list<uint> nv_selection;
  nv_selection.push_back (0);
  nv_selection.push_back (1);
  ChangeSelection (nv_selection);

  std::cout << std::endl;
}

//-----------------------------------------------------------------------------

void TeamsList::Reset()
{
  m_equipe_active = playing_list.begin();

  // Commence par désactiver tous les vers
  iterator it=playing_list.begin(), fin=playing_list.end();
/*  for (; it != fin; ++it) 
  {
	Team &team = **it;
    Team::iterator ver=team.begin(), dernier_ver=team.end();
    for (; ver != dernier_ver; ++ver) (*ver).StopPlaying();
  } 
*/
  // Reset de toutes les équipes
  for (it=playing_list.begin(); it != fin; ++it) (**it).Reset();
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

void TeamsList::InitList (const std::list<std::string> &liste_nom)
{
  Clear();
  std::list<std::string>::const_iterator it=liste_nom.begin(), fin=liste_nom.end();
  for (; it != fin; ++it) AddTeam (*it, false);
  m_equipe_active = playing_list.begin();
}

//-----------------------------------------------------------------------------

void TeamsList::InitEnergy ()
{
  //On cherche l'équipe avec le plus d'énergie pour fixer le niveau max
  //(arrive dans le cas d'équipe n'ayant pas le même nombre de vers)
  iterator it=playing_list.begin(), fin=playing_list.end();
  uint max = 0;
  for (; it != fin; ++it)
  {
    if( (**it).LitEnergie() > max)
      max = (**it).LitEnergie();
  }

  //Initialisation de la barre d'énergie de chaque équipe
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
      && (**it2).LitEnergie() > (**it).LitEnergie() )
        ++classement;
    }
    (**it).energie.classement_tmp = classement;
  }
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    uint classement = (**it).energie.classement_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).LitEnergie() == (**it).LitEnergie() )
        ++classement;
    }
    (**it).energie.FixeClassement(classement);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::RefreshEnergy ()
{
  //Dans l'ordre des priorités :
  // - Terminer l'opération en cours
  // - On change la valeur de l'énergie
  // - On change le classement
  // - On prépare les jauges à l'évenement suivant
  
  iterator it=playing_list.begin(), fin=playing_list.end();
  uint status;

  bool en_attente = true; // Toute les jauges sont en attente

  for (; it != fin; ++it) {
    if( (**it).energie.status != EnergieStatusAttend)
    {
      en_attente = false;
      break;
    }
  }

  //Une des jauge éxécute un ordre?
  if(!en_attente)
  {
    status = EnergieStatusOK;

    //Une des jauges change de valeur?
    for (it=playing_list.begin(); it != fin; ++it) {
      if( (**it).energie.status == EnergieStatusValeurChange) {
        status = EnergieStatusValeurChange;
        break;
      }
    }
  
    //Une des jauges change de classement?
    for (it=playing_list.begin(); it != fin; ++it) {
      if( (**it).energie.status == EnergieStatusClassementChange
      && ((**it).energie.EstEnMouvement() || status == EnergieStatusOK)) {
        status = EnergieStatusClassementChange;
        break;
      }
    }
  }
  else {
    //Les jauges sont toutes en attente
    //->on les met OK pour un nouvel ordre
    status = EnergieStatusOK;
  }

  // On recopie l'ordre a donner aux jauges
  if(status != EnergieStatusOK || en_attente)
  {
    it=playing_list.begin();
    for (; it != fin; ++it) {
      (**it).energie.status = status;
    }
  }

  // Actualisation des valeurs (pas d'actualisation de l'affichage)
  for (it=playing_list.begin(); it != fin; ++it)
  {
    (**it).ActualiseBarreEnergie();
    RefreshSort();
  }
}
//-----------------------------------------------------------------------------

void TeamsList::RefreshSort ()
{
  iterator it=playing_list.begin(), fin=playing_list.end();
  uint classement;
  
  //Cherche le classement sans tenir comte des égalités
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    classement = 0;
    iterator it2=playing_list.begin();
    for (; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).LitEnergie() > (**it).LitEnergie() )
        ++classement;
    }
    (**it).energie.classement_tmp = classement;
  }

  //Réglage des égalités
  it=playing_list.begin();
  for (; it != fin; ++it)
  {
    classement = (**it).energie.classement_tmp;
    iterator it2=playing_list.begin();
    for (it2 = it; it2 != fin; ++it2)
    {
      if((it != it2)
      && (**it2).LitEnergie() == (**it).LitEnergie() )
        ++classement;
    }
    (**it).energie.NouveauClassement(classement);
  }
}

//-----------------------------------------------------------------------------

void TeamsList::ChangeSelection (const std::list<uint>& nv_selection)
{
  selection = nv_selection;

  selection_iterator it=selection.begin(), fin=selection.end();
  playing_list.clear();
  for (; it != fin; ++it) playing_list.push_back (FindByIndex(*it));
  m_equipe_active = playing_list.begin();
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
	m_equipe_active = playing_list.begin();
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
			m_equipe_active = it;
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
