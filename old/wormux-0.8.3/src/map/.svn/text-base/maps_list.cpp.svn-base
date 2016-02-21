/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#include "include/action.h"
#include "map/maps_list.h"
#include "map/water.h"
#include "game/config.h"
#include "graphic/surface.h"
#include "tool/resource_manager.h"
#include "tool/debug.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include "tool/random.h"
#include "tool/string_tools.h"
#include "tool/xml_document.h"
#include <iostream>
#include <algorithm>

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
  is_basic_info_loaded(false),
  is_data_loaded(false),
  random_generated(false),
  island_type(RANDOM_GENERATED),
  water_type(Water::NO_WATER),
  res_profile(NULL)
{
  wind.nb_sprite = 0;
  wind.need_flip = false;
  wind.rotation_speed = 0;
}

void InfoMap::LoadBasicInfo()
{
  if(is_basic_info_loaded)
    return;

  std::string nomfich = m_directory + "config.xml";

  // Load resources
  if (!DoesFileExist(nomfich))
    throw _("no configuration file!");
  // FIXME: not freed
  res_profile = GetResourceManager().LoadXMLProfile(nomfich, true);
  if (!res_profile)
    throw _("couldn't load config");
  // Load preview
  preview = GetResourceManager().LoadImage(res_profile, "preview");
  is_basic_info_loaded = true;
  // Load other informations
  XmlReader doc;
  if (!doc.Load(nomfich) || !ProcessXmlData(doc.GetRoot()))
    throw _("error parsing the config file");

  MSG_DEBUG("map.load", "Map loaded: %s", m_map_name.c_str());
}

bool InfoMap::ProcessXmlData(const xmlNode *xml)
{
    uint tmpisle = (uint) island_type;

  XmlReader::ReadBool(xml, "random", random_generated);

  XmlReader::ReadUint(xml, "generator", tmpisle);
  island_type = (Island_type) tmpisle;

  // Read author informations
  const xmlNode *author = XmlReader::GetMarker(xml, "author");
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
  XmlReader::ReadUint(xml, "nb_mine", nb_mine);
  XmlReader::ReadUint(xml, "nb_barrel", nb_barrel);
  XmlReader::ReadBool(xml, "is_open", is_opened);

  // reading water type
  std::string water_name;
  XmlReader::ReadString(xml, "water", water_name);
  water_type = (Water::Water_type)Water::GetWaterType(water_name);

  // Load padding value
  bool add_pad = false;
  XmlReader::ReadBool(xml, "add_pad", add_pad);
  if(is_opened && add_pad) {
    upper_left_pad = GetResourceManager().LoadPoint2i(res_profile, "upper_left_pad");
    lower_right_pad = GetResourceManager().LoadPoint2i(res_profile, "lower_right_pad");
  }

  const xmlNode* xmlwind = XmlReader::GetMarker(xml, "wind");
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

InfoMap::~InfoMap()
{
  if (res_profile)
    delete res_profile;
}

void InfoMap::LoadData()
{
  if (is_data_loaded)
    return;
  is_data_loaded = true;

  MSG_DEBUG("map.load", "Map data loaded: %s", name.c_str());

  img_sky = GetResourceManager().LoadImage(res_profile,"sky");
  if(!random_generated) {
    img_ground = GetResourceManager().LoadImage(res_profile, "map");
  } else {
    img_ground = GetResourceManager().GenerateMap(res_profile, island_type, img_sky.GetWidth(), img_sky.GetHeight());
  }
}

void InfoMap::FreeData()
{
  img_sky.Free();
  img_ground.Free();
  is_data_loaded = false;
}

Surface& InfoMap::ReadImgGround()
{
  LoadBasicInfo();
  LoadData();
  return img_ground;
}

Surface& InfoMap::ReadImgSky()
{
  LoadBasicInfo();
  LoadData();
  return img_sky;
}

std::string InfoMap::GetConfigFilepath() const
{
  return m_directory + PATH_SEPARATOR + "config.xml";
}

/* ========================================================================== */
static bool compareMaps(const InfoMap* a, const InfoMap* b)
{
  return a->GetRawName() < b->GetRawName();
}

MapsList::MapsList()
{
  active_map_index = 0;
  random_map = false;
  lst.clear() ;

  std::cout << "o " << _("Load maps:");

  const Config * config = Config::GetInstance();
  std::string dirname = config->GetDataDir() + "map" PATH_SEPARATOR;
  FolderSearch *f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneMap(dirname, name);
    CloseFolder(f);
  } else {
    Error (Format(_("Unable to open maps directory (%s)!"), dirname.c_str()));
  }

  // Load personal maps
  dirname = config->GetPersonalDataDir() + "map" PATH_SEPARATOR;
  f = OpenFolder(dirname);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) LoadOneMap(dirname, name);
    CloseFolder(f);
  } else {
        std::cerr << std::endl
          << Format(_("Unable to open personal maps directory (%s)!"),
                      dirname.c_str())
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

MapsList::~MapsList()
{
  for (uint i=0; i < lst.size(); ++i)
    delete lst[i];
}

void MapsList::LoadOneMap (const std::string &dir, const std::string &map_name)
{
  if (map_name[0] == '.') return;

  std::string fullname = dir + map_name;
  if (!IsFolderExist(fullname))
    return;

  InfoMap *nv_terrain = new InfoMap(map_name, fullname + PATH_SEPARATOR);

  std::cout << (lst.empty()?" ":", ") << map_name;
  std::cout.flush();
  lst.push_back(nv_terrain);
}

int MapsList::FindMapById (const std::string &id) const
{
  uint i=0;
  for (; i < lst.size(); ++i)
    if (lst[i]->GetRawName() == id)
      return i;
  return -1;
}

void MapsList::SelectRandomMapByName(const std::string &name)
{
  MSG_DEBUG("map.random", "select %s", name.c_str());

  ASSERT(name != "random");
  SelectMapByName(name);
  random_map = true;
}

void MapsList::SelectMapByName (const std::string &name)
{
  // Random map!!
  if (name == "random") {
    active_map_index = RandomLocal().GetLong(0, lst.size()-1);

    MSG_DEBUG("map.random", "select %u", active_map_index);
    random_map = true;
    return;
  }

  // standard case!
  int index = FindMapById (name);

  if (index == -1){
    index = 0;
    if(name != "")
      std::cout << Format(_("! Map %s not found :-("), name.c_str()) << std::endl;
  }
  SelectMapByIndex (index);
}

void MapsList::SelectMapByIndex (uint index)
{
  MSG_DEBUG("map", "select %u", index);

  ASSERT (index < lst.size());
  if (active_map_index == (int)index)
    return;

  active_map_index = index;
  random_map = false;
}

int MapsList::GetActiveMapIndex () const
{
  if (!random_map)
    return active_map_index;
  else
    return lst.size();
}

void MapsList::FillActionMenuSetMap(Action& a) const
{
  if (!random_map) {
    a.Push(lst.at(active_map_index)->GetRawName());
  } else {
    a.Push("random");
    a.Push(lst.at(active_map_index)->GetRawName());
  }
}

InfoMap* MapsList::ActiveMap()
{
  ASSERT (0 <= active_map_index);
  return lst.at(active_map_index);
}

InfoMap* ActiveMap()
{
  return MapsList::GetInstance()->ActiveMap();
}

