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
#include <map>
#include "include/base.h"
#include "include/singleton.h"
#include "tool/point.h"
#include "team/team_config.h"

// Forward declarations
class ObjectConfig;
typedef struct _xmlNode xmlNode;

//-----------------------------------------------------------------------------
#ifdef WIN32
#  define PATH_SEPARATOR "\\"
#else
#  define PATH_SEPARATOR "/"
#endif

//-----------------------------------------------------------------------------

class Config : public Singleton<Config>
{
public:
  static const int ALPHA = 0;
  static const int COLORKEY = 1;

  const ObjectConfig &GetObjectConfig(const std::string &name,
                                     const std::string &xml_config) const;
  void RemoveAllObjectConfigs();

  void SetLanguage(const std::string language);
  std::string GetLanguage() const { return default_language; };

  bool GetDisplayEnergyCharacter() const { return display_energy_character; };
  void SetDisplayEnergyCharacter(const bool dec) { display_energy_character = dec; };

  bool GetDisplayNameCharacter() const { return display_name_character; };
  void SetDisplayNameCharacter(const bool dnc) { display_name_character = dnc; };

  bool GetDisplayWindParticles() const { return display_wind_particles; };
  void SetDisplayWindParticles(bool dwp) { display_wind_particles = dwp; };

  bool GetDefaultMouseCursor() const { return default_mouse_cursor; };
  void SetDefaultMouseCursor(const bool dmc) { default_mouse_cursor = dmc; };

  bool IsMouseDisable() const { return disable_mouse; };
  bool IsJoystickDisable() const { return disable_joystick; };

  bool GetScrollOnBorder() const { return scroll_on_border; };
  void SetScrollOnBorder(const bool sob) { scroll_on_border = sob; };

  uint GetScrollBorderSize() const { return scroll_border_size; };
  void SetScrollBorderSize(const uint size) { scroll_border_size = size; };

  bool IsVideoFullScreen() const { return video_fullscreen; };
  void SetVideoFullScreen(const bool set_fullscreen) { video_fullscreen = set_fullscreen; };

  uint GetVideoWidth() const { return video_width; };
  void SetVideoWidth(const uint width) { video_width = width; };

  uint GetVideoHeight() const { return video_height; };
  void SetVideoHeight(const uint height) { video_height = height; };

  std::list<Point2i> & GetResolutionAvailable() { return resolution_available; };
  uint GetMaxFps() const { return max_fps; };

  bool IsBlingBlingInterface() const { return bling_bling_interface; };
  void SetBlingBlingInterface(bool bling_bling) { bling_bling_interface = bling_bling; };

  bool GetSoundMusic() const { return sound_music; };
  void SetSoundMusic(const bool music) { sound_music = music; };

  bool GetSoundEffects() const { return sound_effects; };
  void SetSoundEffects(const bool effects) { sound_effects = effects; };

  uint GetSoundFrequency() const { return sound_frequency; };
  void SetSoundFrequency(const uint freq) { sound_frequency = freq; };

  uint GetVolumeMusic() const { return volume_music; }
  void SetVolumeMusic(uint vol);
  uint GetVolumeEffects() const { return volume_effects; }
  void SetVolumeEffects(uint vol) { volume_effects = vol; }
  static uint GetMaxVolume();

  bool GetCheckUpdates() const { return check_updates; }
  void SetCheckUpdates(const bool check) { check_updates = check; }

  std::list<ConfigTeam> & AccessTeamList() { return teams; };
  const std::string & GetMapName() const { return map_name; };
  void SetMapName(const std::string& new_name) { map_name = new_name; }

  int GetTransparency() const { return transparency; };

  const std::string& GetTtfFilename();

  std::string GetDataDir() const { return data_dir; };
  std::string GetLocaleDir() const { return locale_dir; };
  std::string GetPersonalDataDir() const { return personal_data_dir; };
  std::string GetChatLogDir() const { return chat_log_dir; };

  bool Save(bool save_current_teams = false);
  const std::string &GetGameMode() const { return m_game_mode; }
  void SetGameMode(std::string s) { m_game_mode = s; }

  const std::string &GetNetworkHost() const { return m_network_host; }
  void SetNetworkHost(std::string s) { m_network_host = s; }
  const std::string &GetNetworkPort() const { return m_network_port; }
  void SetNetworkPort(std::string s) { m_network_port = s; }

protected:
  bool SaveXml(bool save_current_teams);
  std::string GetEnv(const std::string & name, const std::string &default_value) const;

  std::string default_language;
  std::string m_game_mode;
  std::string m_network_host;
  std::string m_network_port;
  std::string m_filename;

  // Code setting it must make sure it ends with the path separator
  std::string data_dir, locale_dir, personal_data_dir, personal_config_dir, chat_log_dir;

  std::list<ConfigTeam> teams;
  std::string map_name;

  std::string m_default_config;

  // Game settings
  bool display_energy_character;
  bool display_name_character;
  bool display_wind_particles;
  bool default_mouse_cursor;
  bool disable_joystick;
  bool disable_mouse;

  // Video settings
  uint video_width;
  uint video_height;
  bool video_fullscreen;
  uint max_fps;
  bool bling_bling_interface;
  std::list<Point2i> resolution_available;
  bool scroll_on_border;
  uint scroll_border_size;

  // Sound settings
  bool sound_music;
  bool sound_effects;
  uint sound_frequency;
  uint volume_music;
  uint volume_effects;

  // network
  bool enable_network;
  bool check_updates;

  // Font setting
  std::map<std::string, std::string>  fonts;
  std::string font_dir;
  std::string ttf_filename;

  int transparency;

  friend class Singleton<Config>;
  Config();
  ~Config() { RemoveAllObjectConfigs(); singleton = NULL; }

private:
  bool DoLoading(void);
  void LoadDefaultValue();
  void LoadXml(xmlNode* xml);

  // return true if the directory is created
  bool MkdirPersonalConfigDir();
  bool MkdirPersonalDataDir();
  bool MkdirChatLogDir();

  /* this is mutable in order to be able to load config on fly when calling
   * GetObjectConfig() witch is not supposed to modify the object itself */
  mutable std::map<std::string, ObjectConfig *> config_set;
};

#endif
