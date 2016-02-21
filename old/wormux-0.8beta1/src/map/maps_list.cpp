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
 * Maps list
 *****************************************************************************/

#include "maps_list.h"
#include "map.h"
#include "game/config.h"
#include "tool/debug.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include <iostream>
#if !defined(WIN32) || defined(__MINGW32__)
#include <dirent.h>
#include <sys/stat.h>
#endif

InfoMap::InfoMap ()
{
  is_data_loaded = false;
  nb_mine = 4;
  nb_barrel = 4;
  wind.nb_sprite = 0;
  wind.need_flip = false;
  wind.rotation_speed = 0;
  random = false;
  music_playlist = "ingame";
}

bool InfoMap::Init (const std::string &map_name,
                    const std::string &directory)
{
  std::string nomfich;

  m_directory = directory;

  res_profile = NULL;
  is_data_loaded = false;

  try
  {
    nomfich = m_directory+"config.xml";

    // Load resources
    if (!IsFileExist(nomfich))
      return false;
    res_profile = resource_manager.LoadXMLProfile( nomfich, true),
    // Load preview
    preview = resource_manager.LoadImage( res_profile, "preview");
    // Load other informations
    XmlReader doc;
    if (!doc.Load(nomfich)) return false;
    if (!ProcessXmlData(doc.GetRoot())) return false;
  }
  catch (const xmlpp::exception &e)
  {
    std::cout << std::endl
              << Format(_("XML error during loading map '%s' :"), map_name.c_str())
              << std::endl
              << e.what() << std::endl;
    return false;
  }

  MSG_DEBUG("map.load", "Map loaded: %s", map_name.c_str());

  return true;
}

bool InfoMap::ProcessXmlData(xmlpp::Element *xml)
{
  XmlReader::ReadBool(xml, "random", random);
  // Read author informations
  xmlpp::Element *author = XmlReader::GetMarker(xml, "author");
  if (author != NULL) {
    std::string
      a_name,
      a_nickname,
      a_country,
      a_email;

    XmlReader::ReadString(author, "name", a_name);
    XmlReader::ReadString(author, "nickname", a_nickname);
    if (!XmlReader::ReadString(author, "country", a_country))
      a_country = "?";
    if (!XmlReader::ReadString(author, "email", a_email))
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

  XmlReader::ReadString(xml, "name", name);
  XmlReader::ReadBool(xml, "water", use_water);
  XmlReader::ReadUint(xml, "nb_mine", nb_mine);
  XmlReader::ReadUint(xml, "nb_barrel", nb_barrel);
  XmlReader::ReadBool(xml, "is_open", is_opened);

  xmlpp::Element *xmlwind = XmlReader::GetMarker(xml, "wind");
  if (xmlwind != NULL)
  {
    double rot_speed=0.0;
    XmlReader::ReadUint(xmlwind, "nbr_sprite", wind.nb_sprite);
    XmlReader::ReadDouble(xmlwind, "rotation_speed", rot_speed);
    wind.rotation_speed = rot_speed;
    XmlReader::ReadBool(xmlwind, "need_flip", wind.need_flip);

    if (wind.nb_sprite > MAX_WIND_OBJECTS)
      wind.nb_sprite = MAX_WIND_OBJECTS ;
  } else {
    wind.nb_sprite = 0;
  }
  wind.default_nb_sprite = wind.nb_sprite;

  XmlReader::ReadString(xml, "music_playlist", music_playlist);

  return true;
}

void InfoMap::LoadData()
{
  if (is_data_loaded)
    return;
  is_data_loaded = true;

  MSG_DEBUG("map.load", "Map data loaded: %s", name.c_str());

  img_sky = resource_manager.LoadImage(res_profile,"sky");
  if(!random) {
    img_ground = resource_manager.LoadImage(res_profile, "map");
  } else {
    img_ground = resource_manager.GenerateMap(res_profile, img_sky.GetWidth(), img_sky.GetHeight());
    //img_ground.ImgSave("/tmp/generate_" + name + ".png");
  }
}

void InfoMap::FreeData()
{
  img_sky.Free();
  img_ground.Free();
  is_data_loaded = false;
}

Surface InfoMap::ReadImgGround()
{
  LoadData();
  return img_ground;
}

Surface InfoMap::ReadImgSky()
{
  LoadData();
  return img_sky;
}

MapsList* MapsList::singleton = NULL;

MapsList* MapsList::GetInstance()
{
  if (singleton == NULL) {
    singleton = new MapsList();
  }

  return singleton;
}

MapsList::MapsList()
{
  terrain_actif = 0;
  lst.clear() ;

  std::cout << "o " << _("Load maps:");

  Config * config = Config::GetInstance();
  std::string dirname = config->GetDataDir() + PATH_SEPARATOR + "map" + PATH_SEPARATOR;
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
  dirname = config->GetPersonalDir() + PATH_SEPARATOR + "map";
  dir = opendir(dirname.c_str());
  if (dir != NULL) {
    while ((file = readdir(dir)) != NULL)
      LoadOneMap (dirname, file->d_name);
    closedir (dir);
  }
#endif
  std::cout << std::endl << std::endl;

  // On a au moins une carte ?
  if (lst.size() < 1)
    Error(_("You need at least one valid map !"));

  /* Get the full set of map ordered */
  std::sort(lst.begin(), lst.end(), compareMaps);

  /* Read the personnal player data and try to restore the map that was played
   * the last time. If it is no found the map 0 is used as we know here that
   * there is at least one map */
  SelectMapByName(Config::GetInstance()->GetMapName());
}

void MapsList::LoadOneMap (const std::string &dir, const std::string &file)
{
  std::string fullname = dir+file;

#if !defined(WIN32) || defined(__MINGW32__)
  struct stat stat_file;
  if (file[0] == '.') return;
  if (stat(fullname.c_str(), &stat_file) != 0) return;
  if (!S_ISDIR(stat_file.st_mode)) return;
#endif

  InfoMap nv_terrain;
  bool ok = nv_terrain.Init (file, fullname + PATH_SEPARATOR);
  if (!ok) return;

  std::cout << (lst.empty()?" ":", ") << file;
  std::cout.flush();
  lst.push_back(nv_terrain);
}

int MapsList::FindMapById (const std::string &id)
{
  iterator
    terrain=lst.begin(),
    fin_terrain=lst.end();
  uint i=0;
  for (; i < lst.size(); ++i)
    if (lst[i].ReadName() == id)
      return i;
  return -1;
}

void MapsList::SelectMapByName (const std::string &nom)
{
  int index = FindMapById (nom);

  if (index == -1){
    index = 0;
    std::cout << Format(_("! Map %s not found :-("), nom.c_str()) << std::endl;
  }
  SelectMapByIndex (index);
}

void MapsList::SelectMapByIndex (uint index)
{
  assert (index < lst.size());
  if (terrain_actif == (int)index)
    return;

  terrain_actif = index;
}

int MapsList::GetActiveMapIndex ()
{
  return terrain_actif;
}

InfoMap& MapsList::ActiveMap()
{
  assert (0 <= terrain_actif);
  return lst.at(terrain_actif);
}

InfoMap& ActiveMap()
{
  return MapsList::GetInstance()->ActiveMap();
}

bool compareMaps(const InfoMap& a, const InfoMap& b)
{
  return a.ReadName() < b.ReadName();
}

