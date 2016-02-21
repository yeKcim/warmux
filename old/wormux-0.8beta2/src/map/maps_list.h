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
 * Maps List
 *****************************************************************************/

#ifndef LST_TERRAINS_H
#define LST_TERRAINS_H

#include <vector>
#include "include/base.h"
#include "graphic/surface.h"

// Forward declarations
class Profile;
namespace xmlpp
{
  class Element;
}

class InfoMap{
 public:
  typedef enum {
    RANDOM,
    SINGLE_ISLAND,
    PLATEFORMS,
    DEFAULT
  } Island_type;

private:

  std::string name;
  std::string author_info;
  std::string music_playlist;
  /* FIXME make m_directory private */
public:
  std::string m_directory;

private:
  std::string m_map_name;

  Surface img_ground, img_sky;
  Surface preview;

  uint nb_mine;
  uint nb_barrel;

  bool is_opened;
  bool use_water;
  bool is_basic_info_loaded;
  bool is_data_loaded;
  bool random;
  Island_type island_type;

  Profile *res_profile;

  bool ProcessXmlData(const xmlpp::Element *xml);
  void LoadData();

public:
  struct s_wind
  {
    uint nb_sprite;
    uint default_nb_sprite;
    bool need_flip; //do we need to flip the sprite when it changes direction?
    float rotation_speed;
  } wind;

public:
  InfoMap(const std::string&, const std::string&);
  bool LoadBasicInfo();
  void FreeData();

  const std::string& GetRawName() const { return m_map_name; };
  const std::string& ReadFullMapName() { LoadBasicInfo(); return name; };
  const std::string& ReadAuthorInfo() { LoadBasicInfo(); return author_info; };
  const std::string& ReadMusicPlaylist() { LoadBasicInfo(); return music_playlist; };

  Surface ReadImgGround();
  Surface ReadImgSky();
  const Surface& ReadPreview() { LoadBasicInfo(); return preview; };

  uint GetNbBarrel() { LoadBasicInfo(); return nb_barrel; };
  uint GetNbMine() { LoadBasicInfo(); return nb_mine; };
  const Profile * const ResProfile() const { return res_profile; };

  bool IsOpened() { LoadBasicInfo(); return is_opened; };
  bool UseWater() { LoadBasicInfo(); return use_water; };
  bool IsRandom() { LoadBasicInfo(); return random; };

};


class MapsList
{
public:
  std::vector<InfoMap> lst;
  typedef std::vector<InfoMap>::iterator iterator;

private:
  int terrain_actif;
  bool m_init;
  static MapsList * singleton;

  void LoadOneMap (const std::string &dir, const std::string &file);
  MapsList();

public:
  static MapsList * GetInstance();

  // Return -1 if fails
  int FindMapById (const std::string &id);
  void SelectMapByName (const std::string &nom);
  void SelectMapByIndex (uint index);
  int GetActiveMapIndex () const;
  InfoMap& ActiveMap();
};

InfoMap& ActiveMap();

bool compareMaps(const InfoMap& a, const InfoMap& b);

#endif
