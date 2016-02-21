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
 * Configuration of Wormux : store game config of every tunable variable of Wormux.
 * Vars have a default value and can be change with the file configuration.
 * This object aims to become the only place where the XML is read and/or
 * written. It is handled as a singleton pattern whereas one doesn't really
 * care when the XML config file is loaded, but when it was actually read, this
 * is useless to load it again.
 * On the other hand, this objet is not designed to handle any other
 * information whereas it is nothing but a shell around the XML config file
 * created to be able to handle config data easily.
 *****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H
//-----------------------------------------------------------------------------
#include <list>
#include <string>
#include "include/base.h"
#include "team/team_config.h"
#include "tool/xml_document.h"
#include "object/object_cfg.h"

//-----------------------------------------------------------------------------
#if defined(WIN32) || defined(__MINGW32__)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#ifdef __MINGW32__
#undef LoadImage
#endif
//-----------------------------------------------------------------------------

class Config
{
public:
  static const int ALPHA = 0;
  static const int COLORKEY = 1;

  const ObjectConfig &GetOjectConfig(const std::string &name,
                                     const std::string &xml_config);

  bool GetDisplayEnergyCharacter() const;
  void SetDisplayEnergyCharacter(const bool dec);

  bool GetDisplayNameCharacter() const;
  void SetDisplayNameCharacter(const bool dnc);

  bool GetDisplayWindParticles() const;
  void SetDisplayWindParticles(bool dwp);

  bool GetDefaultMouseCursor() const;
  void SetDefaultMouseCursor(const bool dmc);

  bool GetScrollOnBorder() const;
  void SetScrollOnBorder(const bool sob);

  bool IsNetworkActivated() const;
  void SetNetworkActivated(const bool set_net);

  bool IsVideoFullScreen() const;
  void SetVideoFullScreen(const bool set_fullscreen);

  uint GetVideoWidth() const;
  void SetVideoWidth(const uint width);

  uint GetVideoHeight() const;
  void SetVideoHeight(const uint height);

  inline uint GetMaxFps() const { return max_fps; };

  bool GetSoundMusic() const;
  void SetSoundMusic(const bool music);

  bool GetSoundEffects() const;
  void SetSoundEffects(const bool effects);

  uint GetSoundFrequency() const;
  void SetSoundFrequency(const uint freq);

  std::list<struct ConfigTeam> & AccessTeamList();
  const std::string & GetMapName() const;
  inline void SetMapName(const std::string& new_name)
  { map_name = new_name; }

  int GetTransparency() const;

  std::string GetTtfFilename() const;

  std::string GetDataDir() const;
  std::string GetLocaleDir() const;
  std::string GetPersonalDir() const;

  static Config * GetInstance();

  bool Save();
  inline const std::string &GetGameMode() const { return m_game_mode; };

protected:
  bool LoadXml(xmlpp::Element *xml);
  bool SaveXml();
  std::string GetEnv(const std::string & name, const std::string &default_value);

  std::string m_game_mode;
  bool m_xml_loaded;
  std::string m_filename;

  std::string data_dir, locale_dir, personal_dir;

  std::list<struct ConfigTeam> teams;
  std::string map_name;
  // Game settings
  bool display_energy_character;
  bool display_name_character;
  bool display_wind_particles;
  bool default_mouse_cursor;
  bool scroll_on_border;
  // Video settings
  uint video_width;
  uint video_height;
  bool video_fullscreen;
  uint max_fps;
  // Sound settings
  bool sound_music;
  bool sound_effects;
  uint sound_frequency;
  // network
  bool enable_network;

  std::string ttf_filename;

  int transparency;
  std::map<std::string, ObjectConfig *> config_set;

private:
  Config();
  static Config * singleton;
  bool DoLoading(void);
};
#endif
