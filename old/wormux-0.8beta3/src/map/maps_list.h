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
class Action;
class Profile;
namespace xmlpp
{
  class Element;
}

class InfoMap {
 public:
  typedef enum {
    RANDOM_GENERATED,
    SINGLE_ISLAND,
    PLATEFORMS,
    DEFAULT
  } Island_type;

  struct s_wind
  {
    uint nb_sprite;
    uint default_nb_sprite;
    bool need_flip; //do we need to flip the sprite when it changes direction?
    float rotation_speed;
  };

private:

  std::string name;
  std::string author_info;
  std::string music_playlist;
  std::string m_directory;

  std::string m_map_name;

  Surface img_ground, img_sky;
  Surface preview;

  uint nb_mine;
  uint nb_barrel;

  bool is_opened;
  bool use_water;
  bool is_basic_info_loaded;
  bool is_data_loaded;
  bool random_generated;
  Point2i upper_left_pad;
  Point2i lower_right_pad;
  Island_type island_type;

  struct s_wind wind;

  Profile *res_profile;

  bool ProcessXmlData(const xmlpp::Element *xml);
  void LoadData();
  bool LoadBasicInfo();

public:
  InfoMap(const std::string&, const std::string&);
  void FreeData();

  const std::string& GetRawName() const { return m_map_name; };
  const std::string& ReadFullMapName() { LoadBasicInfo(); return name; };
  const std::string& ReadAuthorInfo() { LoadBasicInfo(); return author_info; };
  const std::string& ReadMusicPlaylist() { LoadBasicInfo(); return music_playlist; };
  std::string GetConfigFilepath() const;

  Surface ReadImgGround();
  Surface ReadImgSky();
  const Surface& ReadPreview() { LoadBasicInfo(); return preview; };

  const struct s_wind& GetWind() const { return wind; }; 

  uint GetNbBarrel() { LoadBasicInfo(); return nb_barrel; };
  uint GetNbMine() { LoadBasicInfo(); return nb_mine; };
  const Profile * const ResProfile() const { return res_profile; };

  bool IsOpened() { LoadBasicInfo(); return is_opened; };
  bool UseWater() { LoadBasicInfo(); return use_water; };
  bool IsRandomGenerated() { LoadBasicInfo(); return random_generated; };

  Point2i GetUpperLeftPad() { return upper_left_pad; };
  Point2i GetLowerRightPad() { return lower_right_pad; };
  void SetUpperLeftPad(const Point2i & value) { upper_left_pad = value; };
  void SetLowerRightPad(const Point2i & value) { lower_right_pad = value; };

};


class MapsList
{
public:
  std::vector<InfoMap> lst;
  typedef std::vector<InfoMap>::iterator iterator;

private:
  int active_map_index;
  bool m_init;
  bool random_map;
  static MapsList * singleton;

  void LoadOneMap (const std::string &dir, const std::string &file);
  MapsList();

public:
  static MapsList * GetInstance();

  // Return -1 if fails
  int FindMapById (const std::string &id) const;
  void SelectMapByName(const std::string &nom);
  void SelectRandomMapByName(const std::string &nom);
  void SelectMapByIndex (uint index);
  int GetActiveMapIndex () const;
  InfoMap& ActiveMap();

  void FillActionMenuSetMap(Action& a) const;
};

InfoMap& ActiveMap();

bool compareMaps(const InfoMap& a, const InfoMap& b);

#endif
