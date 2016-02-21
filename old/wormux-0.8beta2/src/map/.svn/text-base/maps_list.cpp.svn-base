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
#include "game/config.h"
#include "graphic/surface.h"
#include "tool/resource_manager.h"
#include "tool/debug.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"
#include <iostream>
#ifdef _MSC_VER
#  include <algorithm>
#endif

extern const uint MAX_WIND_OBJECTS;

InfoMap::InfoMap(const std::string &map_name,
                 const std::string &directory):
  name("not initialized"),
  author_info("not initialized"),
  music_playlist("ingame"),
  m_directory(directory),
  m_map_name(map_name),
  img_ground(),
  img_sky(),
  preview(),
  nb_mine(4),
  nb_barrel(4),
  is_opened(false),
  use_water(false),
  is_basic_info_loaded(false),
  is_data_loaded(false),
  random(false),
  island_type(RANDOM),
  res_profile(NULL)
{
  wind.nb_sprite = 0;
  wind.need_flip = false;
  wind.rotation_speed = 0;
}

bool InfoMap::LoadBasicInfo()
{
  if(is_basic_info_loaded)
    return true;
  std::string nomfich;
  try
    {
      nomfich = m_directory + "config.xml";

      // Load resources
      if (!IsFileExist(nomfich))
        return false;
      // FIXME: not freed
      res_profile = resource_manager.LoadXMLProfile(nomfich, true),
      // Load preview
      preview = resource_manager.LoadImage( res_profile, "preview");
      // Load other informations
      XmlReader doc;
      is_basic_info_loaded = true;
      if (!doc.Load(nomfich)) return false;
      if (!ProcessXmlData(doc.GetRoot())) return false;
    }

  catch (const xmlpp::exception &e)
    {
      std::cout << std::endl
                << Format(_("XML error during loading map '%s' :"), m_map_name.c_str())
                << std::endl
                << e.what() << std::endl;
      return false;
    }

  MSG_DEBUG("map.load", "Map loaded: %s", m_map_name.c_str());

  return true;
}

bool InfoMap::ProcessXmlData(const xmlpp::Element *xml)
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
    img_ground = resource_manager.GenerateMap(res_profile, island_type, img_sky.GetWidth(), img_sky.GetHeight());
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
  LoadBasicInfo();
  LoadData();
  return img_ground;
}

Surface InfoMap::ReadImgSky()
{
  LoadBasicInfo();
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

  const Config * config = Config::GetInstance();
  std::string dirname = config->GetDataDir() + PATH_SEPARATOR + "map" + PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneMap(dirname, name);
    CloseFolder(f);
  } else {
    Error (Format(_("Unable to open maps directory (%s)!"), dirname.c_str()));
  }

  // Load personal maps
  dirname = config->GetPersonalDir() + "map" + PATH_SEPARATOR;
  f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneMap(dirname, name);
    CloseFolder(f);
  } else {
        std::cerr << std::endl
          << Format(_("Unable to open maps directory (%s)!"), dirname.c_str())
          << std::endl;
  }

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

void MapsList::LoadOneMap (const std::string &dir, const std::string &map_name)
{
  if (map_name[0] == '.') return;

  std::string fullname = dir + map_name;
  if (!IsFolderExist(fullname))
          return;

  InfoMap nv_terrain(map_name, fullname + PATH_SEPARATOR);

  std::cout << (lst.empty()?" ":", ") << map_name;
  std::cout.flush();
  lst.push_back(nv_terrain);
}

int MapsList::FindMapById (const std::string &id)
{
  // XXX Not used !?
  //iterator terrain=lst.begin(), fin_terrain=lst.end();

  uint i=0;
  for (; i < lst.size(); ++i)
    if (lst[i].GetRawName() == id)
      return i;
  return -1;
}

void MapsList::SelectMapByName (const std::string &nom)
{
  int index = FindMapById (nom);

  if (index == -1){
    index = 0;
    if(nom != "")
      std::cout << Format(_("! Map %s not found :-("), nom.c_str()) << std::endl;
  }
  SelectMapByIndex (index);
}

void MapsList::SelectMapByIndex (uint index)
{
  ASSERT (index < lst.size());
  if (terrain_actif == (int)index)
    return;

  terrain_actif = index;
}

int MapsList::GetActiveMapIndex () const
{
  return terrain_actif;
}

InfoMap& MapsList::ActiveMap()
{
  ASSERT (0 <= terrain_actif);
  return lst.at(terrain_actif);
}

InfoMap& ActiveMap()
{
  return MapsList::GetInstance()->ActiveMap();
}

bool compareMaps(const InfoMap& a, const InfoMap& b)
{
  return a.GetRawName() < b.GetRawName();
}

