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
 * Liste des terrains.
 *****************************************************************************/

#include "maps_list.h"
//-----------------------------------------------------------------------------
#include "../game/config.h"
#include "map.h"
#include "../tool/i18n.h"
#include "../tool/file_tools.h"
#include <iostream>
#if !defined(WIN32) || defined(__MINGW32__)
#include <dirent.h>
#include <sys/stat.h>
#endif
//-----------------------------------------------------------------------------

#ifdef DEBUG
//#  define VERBOSE_MAP_LOADING
#  define DBG_COUT std::cout << "[Map " << name << "] "
#endif

ListeTerrain lst_terrain;
//-----------------------------------------------------------------------------

InfoTerrain::InfoTerrain ()
{ 
  m_donnees_chargees = false;
  nb_mine = 0;
  wind.nb_sprite = 0;
  wind.need_flip = false;
  infinite_bg = false;
  img_terrain = NULL;
  img_ciel = NULL;
}

//-----------------------------------------------------------------------------

bool InfoTerrain::Init (const std::string &map_name, 
			            const std::string &directory)
{
  std::string nomfich;

  m_directory = directory;

  res_profile = NULL;
  m_donnees_chargees = false;

  try
  {
    nomfich = m_directory+"config.xml";

    // Load resources
    if (!FichierExiste(nomfich)) return false;
    res_profile = resource_manager.LoadXMLProfile( nomfich, true), 
    // Load preview
    preview = resource_manager.LoadImage( res_profile, "preview");
    // Load other informations
    LitDocXml doc;
    if (!doc.Charge (nomfich)) return false;
    if (!TraiteXml (doc.racine())) return false;
  }
  catch (const xmlpp::exception &e)
  {
    std::cout << std::endl
              << Format(_("XML error during loading map '%s' :"), map_name.c_str())
              << std::endl
              << e.what() << std::endl;
    return false;
  }
 
#ifdef VERBOSE_MAP_LOADING
  DBG_COUT << "Map loaded." << std::endl;
#endif

  return true;
}

//-----------------------------------------------------------------------------

bool InfoTerrain::TraiteXml (xmlpp::Element *xml)
{
  // Read author informations
  xmlpp::Element *author = LitDocXml::AccesBalise (xml, "author");
  if (author != NULL) {
    std::string
      a_name,
      a_nickname,
      a_country,
      a_email;

    LitDocXml::LitString (author, "name", a_name);
    LitDocXml::LitString (author, "nickname", a_nickname);
    if (!LitDocXml::LitString (author, "country", a_country))
      a_country = "?";
    if (!LitDocXml::LitString (author, "email", a_email))
      a_email = "?";

    if (!a_nickname.empty()) 
      author_info = Format
	(_("%s <%s> aka %s from %s"),
	 a_name.c_str(),
	 a_email.c_str(),
	 a_nickname.c_str(),
	 a_country.c_str());
    else
      author_info = Format
	(_("%s <%s> from %s"),
	 a_name.c_str(),
	 a_email.c_str(),
	 a_country.c_str());
  }

  LitDocXml::LitString (xml, "name", name);
  LitDocXml::LitBool (xml, "water", use_water);
  LitDocXml::LitUint (xml, "nb_mine", nb_mine);
  LitDocXml::LitBool (xml, "is_open", is_opened);
  LitDocXml::LitBool (xml, "infinite_background", infinite_bg);

  if(!is_opened && infinite_bg)
  {
    std::cout << _("<infinite_background> needs <is_open> to be set to work correctly!!") << std::endl;
    infinite_bg = false;
  }

  xmlpp::Element *xmlwind = LitDocXml::AccesBalise (xml, "wind");
  if (xmlwind != NULL)
  {
    LitDocXml::LitUint (xmlwind, "nbr_sprite", wind.nb_sprite);
    LitDocXml::LitDouble (xmlwind, "mass", wind.particle_mass);
    if(wind.particle_mass<0.1)
    {
      printf("\n%s\n",_("Warning! Winds particles mass is too low! Set by default to 0.3"));
      wind.particle_mass = 0.1;
    }
    LitDocXml::LitDouble (xmlwind, "wind_factor", wind.particle_wind_factor);
    LitDocXml::LitBool (xmlwind, "need_flip", wind.need_flip);

    if (wind.nb_sprite > MAX_WIND_OBJECTS)
      wind.nb_sprite = MAX_WIND_OBJECTS ;
  } else {
    wind.nb_sprite = 0;
  }
  wind.default_nb_sprite = wind.nb_sprite;

  return true;
}

//-----------------------------------------------------------------------------

void InfoTerrain::LoadData()
{
  if (m_donnees_chargees) return;
  m_donnees_chargees = true;

#ifdef VERBOSE_MAP_LOADING
  DBG_COUT << "Map data loaded." << std::endl;
#endif

  img_terrain = resource_manager.LoadImage(res_profile, "map");
  img_ciel = resource_manager.LoadImage(res_profile,"sky");   
}

void InfoTerrain::FreeData()
{
  if (!m_donnees_chargees) return;
  assert(img_ciel!=NULL && img_terrain!=NULL);
  m_donnees_chargees = false;

#ifdef VERBOSE_MAP_LOADING
  DBG_COUT << "Map data deleted." << std::endl;
#endif

  SDL_FreeSurface(img_terrain);
  SDL_FreeSurface(img_ciel);
  img_terrain=NULL;
  img_ciel=NULL;
}

//-----------------------------------------------------------------------------

SDL_Surface *InfoTerrain::LitImgTerrain() 
{ 
  LoadData(); 
  return img_terrain;
}

SDL_Surface *InfoTerrain::LitImgCiel() 
{ 
  LoadData(); 
  return img_ciel;
}

bool InfoTerrain::DonneesChargees() const 
{ 
   return m_donnees_chargees; 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ListeTerrain::ListeTerrain()
{
  m_init = false;
  terrain_actif = -1;
}

//-----------------------------------------------------------------------------

void ListeTerrain::LoadOneMap (const std::string &dir, const std::string &file)
{
  std::string fullname = dir+file;

#if !defined(WIN32) || defined(__MINGW32__)
  struct stat stat_file;
  if (file[0] == '.') return;
  if (stat(fullname.c_str(), &stat_file) != 0) return;
  if (!S_ISDIR(stat_file.st_mode)) return;
#endif
	
  InfoTerrain nv_terrain;  
  bool ok = nv_terrain.Init (file, fullname+'/');
  if (!ok) return;

  std::cout << (liste.empty()?" ":", ") << file;
  std::cout.flush();
  liste.push_back(nv_terrain);
}

//-----------------------------------------------------------------------------

void ListeTerrain::Init()
{
  if (m_init) return;
  
  m_init = true;
  liste.clear() ;

  std::cout << "o " << _("Load maps:");

  std::string dirname = Wormux::config.data_dir+"map"+PATH_SEPARATOR;
#if !defined(WIN32) || defined(__MINGW32__)
  DIR *dir = opendir(dirname.c_str());
  struct dirent *file;
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL)
	  LoadOneMap (dirname, file->d_name);
    closedir (dir);
  } else {
    Error (Format(_("Unable to open maps directory (%s)!"),
		   dirname.c_str()));
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
	    LoadOneMap(dirname,file.cFileName);
	}
  } else {
    Error (Format(_("Unable to open maps directory (%s)!"),
		   dirname.c_str()));
  }
  FindClose(file_search);
#endif

  
#if !defined(WIN32) || defined(__MINGW32__)
  // Load personal maps
  dirname = Wormux::config.GetWormuxPersonalDir()+"map/";
  dir = opendir(dirname.c_str());
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL) 
      LoadOneMap (dirname, file->d_name);
    closedir (dir);
  }
#endif
  std::cout << std::endl << std::endl;

  // On a au moins une carte ?
  if (liste.size() < 1)
    Error(_("You need at least one valid map !"));
  

  std::sort(lst_terrain.liste.begin(), lst_terrain.liste.end(), compareMaps);
}

//-----------------------------------------------------------------------------

int ListeTerrain::FindMapById (const std::string &id)
{
  iterator
    terrain=liste.begin(),
    fin_terrain=liste.end();
  uint i=0;
  for (; i < liste.size(); ++i)
  {
    if (liste[i].name == id) return i;
  }
  return -1;
}

//-----------------------------------------------------------------------------

void ListeTerrain::ChangeTerrainNom (const std::string &nom)
{
  int index = FindMapById (nom);
  if (index == -1) {
    index = 0;
    std::cout << Format(_("! Map %s not found :-("), nom.c_str()) << std::endl;
  }
  ChangeTerrain (index);
}

//-----------------------------------------------------------------------------

void ListeTerrain::ChangeTerrain (uint index)
{
  assert (index < liste.size());
  if (terrain_actif == (int)index) return;

  terrain_actif = index;
  //monde.terrain.terrain_charge = false;
  //monde.terrain.Init();
  //monde.ciel.charge = false;
  //monde.ciel.Init();
}

//-----------------------------------------------------------------------------

InfoTerrain& ListeTerrain::TerrainActif()
{
  assert (0 <= terrain_actif);
  return liste.at(terrain_actif);
}

//-----------------------------------------------------------------------------

InfoTerrain& TerrainActif()
{
  return lst_terrain.TerrainActif();
}

//-----------------------------------------------------------------------------

bool compareMaps(const InfoTerrain& a, const InfoTerrain& b) 
{  
  return a.name < b.name;
}

//-----------------------------------------------------------------------------
