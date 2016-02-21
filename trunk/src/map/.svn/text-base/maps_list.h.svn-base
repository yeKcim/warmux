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
 * Maps List
 *****************************************************************************/

#ifndef LST_TERRAINS_H
#define LST_TERRAINS_H

#include <vector>
#include <assert.h>
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "graphic/surface.h"
#include "tool/xml_document.h"

// Forward declarations
class Action;
class Profile;
class Water;
class InfoMapBasicAccessor;
class InfoMapAccessor;

class InfoMap {
 public:
  typedef enum {
    RANDOM_GENERATED,
    SINGLE_ISLAND,
    PLATEFORMS,
    GRID_ELEMENTS,
    DEFAULT
  } Island_type;

  struct s_wind
  {
    uint nb_sprite;
    uint default_nb_sprite;
    bool need_flip; //do we need to flip the sprite when it changes direction?
    Double rotation_speed;
  };

protected:
  friend class InfoMapBasicAccessor;
  friend class InfoMapAccessor;

  std::string name;
  std::string author_info;
  std::string music_playlist;
  std::string m_directory;

  std::string m_map_name;

  Surface     img_sky;
  std::string ground_filename;
  Surface preview;
  std::vector<Surface> sky_layer;
  XmlReader doc;

  uint nb_mine;
  uint nb_barrel;

  uint alpha_threshold;

  bool is_opened;
  bool random_generated;
  Point2i upper_left_pad;
  Point2i lower_right_pad;
  Island_type island_type;
  std::string water_type;

  struct s_wind wind;
  InfoMapBasicAccessor *basic;
  InfoMapAccessor      *normal;

  Profile *res_profile;

  bool ProcessXmlData(const xmlNode* xml);

public:
  InfoMap(const std::string&, const std::string&);
  ~InfoMap();
  void FreeData();

  const std::string& GetRawName() const { return m_map_name; }
  const std::string& GetDirectory() const { return m_directory; }
  std::string GetConfigFilepath() const { return m_directory + PATH_SEPARATOR + "config.xml"; }

  Profile * ResProfile() const { return res_profile; }

  // Shouldn't those be protected?
  const struct s_wind& GetWind() const { return wind; }
  const std::string& GetWaterType() { return water_type; }
  Point2i GetUpperLeftPad() { return upper_left_pad; }
  Point2i GetLowerRightPad() { return lower_right_pad; }
  void SetUpperLeftPad(const Point2i & value) { upper_left_pad = value; }
  void SetLowerRightPad(const Point2i & value) { lower_right_pad = value; }

  // The 2 below must be checked for NULL, otherwise...
  InfoMapAccessor      *LoadData();
  InfoMapBasicAccessor *LoadBasicInfo();

  InfoMapBasicAccessor *LoadedInfo() { assert(basic); return basic; }
  InfoMapAccessor      *LoadedData() { assert(normal); return normal; }
};

class InfoMapBasicAccessor {
protected:
  InfoMap* info;
public:
  InfoMapBasicAccessor(InfoMap* info_) : info(info_) { }
  const std::string& ReadFullMapName() const { return info->name; }
  const std::string& ReadAuthorInfo() const { return info->author_info; }
  const std::string& ReadMusicPlaylist() const { return info->music_playlist; }
  const Surface& ReadPreview() const { return info->preview; }
  uint GetNbBarrel() const { return info->nb_barrel; }
  uint GetNbMine() const { return info->nb_mine; }
  uint GetAlphaThreshold() const { return info->alpha_threshold; }
  bool IsOpened() const { return info->is_opened; }
  bool IsRandomGenerated() const { return info->random_generated; }
};

class InfoMapAccessor : public InfoMapBasicAccessor {
public:
  InfoMapAccessor(InfoMap* info_) : InfoMapBasicAccessor(info_) { }
  const std::string& GetGroundFileName() const { return info->ground_filename; }
  const Surface& ReadImgSky() const { return info->img_sky; }
  const std::vector<Surface>& ReadSkyLayer() const { return info->sky_layer; }
};


class MapsList : public Singleton<MapsList>
{
public:
  std::vector<InfoMap*> lst;
  typedef std::vector<InfoMap*>::iterator iterator;

private:
  int active_map_index;
  bool m_init;
  bool random_map;

  void LoadOneMap (const std::string &dir, const std::string &file);

protected:
  friend class Singleton<MapsList>;
  MapsList();
  ~MapsList();

public:
  // Return -1 if fails
  int FindMapById(const std::string &id) const;
  void SelectMapByName(const std::string &nom);
  void SelectRandomMapByName(const std::string &nom);
  void SelectMapByIndex(uint index);
  int GetActiveMapIndex() const { return random_map ? lst.size() : active_map_index; }
  InfoMap* ActiveMap() { return lst.at(active_map_index); }

  bool IsRandom() const { return random_map; }
  void FillActionMenuSetMap(Action& a) const;
  std::vector<std::string> GetAvailableMaps() const;
};

inline InfoMap* ActiveMap() { return MapsList::GetInstance()->ActiveMap(); }

#endif
