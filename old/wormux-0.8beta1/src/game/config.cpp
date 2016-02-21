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
 * Wormux configuration : all variables interesting to tweak should be here.
 * A default value is affected for each variable, the value can be changed by
 * the configuration file.
 *****************************************************************************/

#include "config.h"

#include <sstream>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <errno.h>
#ifdef WIN32
#  include <direct.h>
#endif
#include "game_mode.h"
#include "errno.h"
#include "graphic/video.h"
#include "include/action.h"
#include "include/app.h"
#include "interface/keyboard.h"
#include "include/constant.h"
#include "map/maps_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/file_tools.h"
#include "tool/string_tools.h"
#include "tool/i18n.h"
#include "weapon/weapons_list.h"
#ifdef USE_AUTOPACKAGE
#  include "include/binreloc.h"
#endif


const std::string FILENAME="config.xml";
Config * Config::singleton = NULL;

Config * Config::GetInstance() {
  if (singleton == NULL) {
    singleton = new Config();
  }
  return singleton;
}

Config::Config():
  m_game_mode("classic"),
  m_xml_loaded(),
  m_filename(),
  data_dir(),
  locale_dir(),
  personal_dir(),
  teams(),
  map_name(),
  display_energy_character(true),
  display_name_character(true),
  display_wind_particles(true),
  default_mouse_cursor(false),
  scroll_on_border(true),
  video_width(800),
  video_height(600),
  video_fullscreen(false),
  max_fps(0),
  sound_music(true),
  sound_effects(true),
  sound_frequency(44100),
  enable_network(true),
  ttf_filename(),
  transparency(ALPHA),
  config_set()
{

#ifdef USE_AUTOPACKAGE
  BrInitError error;
  std::string filename;

  if (br_init (&error) == 0 && error != BR_INIT_ERROR_DISABLED) {
    std::cout << "Warning: BinReloc failed to initialize (error code "
              << error << ")" << std::endl;
    std::cout << "Will fallback to hardcoded default path." << std::endl;
  }
#endif
  Constants::GetInstance();

  // directories
#ifdef USE_AUTOPACKAGE
  data_dir     = GetEnv(Constants::ENV_DATADIR, br_find_data_dir(INSTALL_DATADIR));
  locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, br_find_locale_dir(INSTALL_LOCALEDIR));
  filename     = data_dir + PATH_SEPARATOR + "font" + PATH_SEPARATOR + "DejaVuSans.ttf";
  ttf_filename = GetEnv(Constants::ENV_FONT_PATH, br_find_locale_dir(filename.c_str()));
#else
  data_dir     = GetEnv(Constants::ENV_DATADIR, INSTALL_DATADIR);
  locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, INSTALL_LOCALEDIR);
  ttf_filename = GetEnv(Constants::ENV_FONT_PATH, FONT_FILE);
#endif

#ifndef WIN32
  personal_dir = GetHome() + "/.wormux/";
#else
  personal_dir = GetHome() + "\\Wormux\\";
#endif
  InitI18N(locale_dir.c_str());

  DoLoading();
  std::string dir = TranslateDirectory(locale_dir);
  I18N_SetDir (dir + PATH_SEPARATOR);

  dir = TranslateDirectory(data_dir);
  resource_manager.AddDataPath(dir + PATH_SEPARATOR);
}

/*
 * Load physics constants from the xml file and cache it.
 * This tries to find already loaded data in the map<> config_set and actually
 * load it if it cannot be found.
 */
const ObjectConfig &Config::GetOjectConfig(const std::string &name, const std::string &xml_config)
{
  ObjectConfig * objcfg;

  std::map<std::string, ObjectConfig*>::iterator  it = config_set.find(name);
  if (it == config_set.end())
    {
      objcfg = new ObjectConfig();
      objcfg->LoadXml(name,xml_config);
      config_set[name] = objcfg;
    }
  else
    objcfg = it->second;

  return *objcfg;
}


bool Config::DoLoading(void)
{
  m_xml_loaded = false;
  try
  {
    // Load XML conf
    XmlReader doc;
    m_filename = personal_dir + FILENAME;
    if (!doc.Load(m_filename))
      return false;
    if (!LoadXml(doc.GetRoot()))
      return false;
    m_xml_loaded = true;
  }
  catch (const xmlpp::exception &e)
  {
    std::cout << "o "
        << _("Error while loading configuration file: %s") << std::endl
        << e.what() << std::endl;
    return false;
  }
  return true;
}

// Read personal config file
bool Config::LoadXml(xmlpp::Element *xml)
{
  std::cout << "o " << _("Reading personal config file") << std::endl;

  xmlpp::Element *elem;

  //=== Map ===
  XmlReader::ReadString(xml, "map", map_name);

  //=== Teams ===
  elem = XmlReader::GetMarker(xml, "teams");
  int i = 0;

  xmlpp::Element *team;

  while ((team = XmlReader::GetMarker(elem, "team_" + ulong2str(i))) != NULL)
  {
    ConfigTeam one_team;
    XmlReader::ReadString(team, "id", one_team.id);
    XmlReader::ReadString(team, "player_name", one_team.player_name);

    int tmp_nb_characters;
    XmlReader::ReadInt(team, "nb_characters", tmp_nb_characters);
    one_team.nb_characters = (uint)tmp_nb_characters;

    teams.push_back(one_team);

    // get next team
    i++;
  }

  //=== Video ===
  if ((elem = XmlReader::GetMarker(xml, "video")) != NULL)
  {
    XmlReader::ReadUint(elem, "max_fps", max_fps);
    XmlReader::ReadBool(elem, "display_wind_particles", display_wind_particles);
    XmlReader::ReadBool(elem, "display_energy_character", display_energy_character);
    XmlReader::ReadBool(elem, "display_name_character", display_name_character);
    XmlReader::ReadBool(elem, "default_mouse_cursor", default_mouse_cursor);
    XmlReader::ReadBool(elem, "scroll_on_border", scroll_on_border);
    XmlReader::ReadUint(elem, "width", video_width);
    XmlReader::ReadUint(elem, "height", video_height);
    XmlReader::ReadBool(elem, "full_screen", video_fullscreen);
  }

  //=== Sound ===
  if ((elem = XmlReader::GetMarker(xml, "sound")) != NULL)
  {
    XmlReader::ReadBool(elem, "music", sound_music);
    XmlReader::ReadBool(elem, "effects", sound_effects);
    XmlReader::ReadUint(elem, "frequency", sound_frequency);
  }

  //=== network ===
  //if ((elem = XmlReader::GetMarker(xml, "network")) != NULL)
  //{
  //  XmlReader::ReadBool(elem, "enable_network", enable_network);
  //}

  //=== game mode ===
  XmlReader::ReadString(xml, "game_mode", m_game_mode);
  return true;
}

bool Config::Save()
{
  std::string rep = personal_dir;

  // Create the directory if it doesn't exist
#ifndef WIN32
   if (mkdir (personal_dir.c_str(), 0750) != 0 && errno != EEXIST)
#else
  if (_mkdir (personal_dir.c_str()) != 0 && errno != EEXIST)
#endif
  {
    std::cerr << "o "
      << Format(_("Error while creating directory \"%s\": unable to store configuration file."),
          rep.c_str())
      << std::endl;
    return false;
  }

  return SaveXml();
}

bool Config::SaveXml()
{
  XmlWriter doc;

  doc.Create(m_filename, "config", "1.0", "utf-8");
  xmlpp::Element *root = doc.GetRoot();
  doc.WriteElement(root, "version", Constants::VERSION);

  //=== Map ===
  //The map name is modified when the player validate its choice in the
  //map selection box.
  doc.WriteElement(root, "map", map_name);

  //=== Teams ===
  xmlpp::Element *team_elements = root->add_child("teams");

  TeamsList::iterator
    it=teams_list.playing_list.begin(),
    fin=teams_list.playing_list.end();
  for (int i=0; it != fin; ++it, i++)
  {
    xmlpp::Element *a_team = team_elements->add_child("team_"+ulong2str(i));
    doc.WriteElement(a_team, "id", (**it).GetId());
    doc.WriteElement(a_team, "player_name", (**it).GetPlayerName());
    doc.WriteElement(a_team, "nb_characters", ulong2str((**it).GetNbCharacters()));
  }

  //=== Video ===
  AppWormux * app = AppWormux::GetInstance();
  xmlpp::Element *video_node = root->add_child("video");
  doc.WriteElement(video_node, "display_wind_particles", ulong2str(display_wind_particles));
  doc.WriteElement(video_node, "display_energy_character", ulong2str(display_energy_character));
  doc.WriteElement(video_node, "display_name_character", ulong2str(display_name_character));
  doc.WriteElement(video_node, "default_mouse_cursor", ulong2str(default_mouse_cursor));
  doc.WriteElement(video_node, "scroll_on_border", ulong2str(scroll_on_border));
  doc.WriteElement(video_node, "width", ulong2str(app->video.window.GetWidth()));
  doc.WriteElement(video_node, "height", ulong2str(app->video.window.GetHeight()));
  doc.WriteElement(video_node, "full_screen",
                   ulong2str(static_cast<uint>(app->video.IsFullScreen())) );
  doc.WriteElement(video_node, "max_fps",
                   long2str(static_cast<int>(app->video.GetMaxFps())));

  if (transparency == ALPHA)
    doc.WriteElement(video_node, "transparency", "alpha");
  else if (transparency == COLORKEY)
    doc.WriteElement(video_node, "transparency", "colorkey");

  //=== Sound ===
  xmlpp::Element *sound_node = root->add_child("sound");
  doc.WriteElement(sound_node, "music",  ulong2str(jukebox.UseMusic()));
  doc.WriteElement(sound_node, "effects", ulong2str(jukebox.UseEffects()));
  doc.WriteElement(sound_node, "frequency", ulong2str(jukebox.GetFrequency()));

  //=== Network ===
  //xmlpp::Element *net_node = root->add_child("network");
  //doc.WriteElement(net_node, "enable_network",  ulong2str(IsNetworkActivated()));

  //=== game mode ===
  doc.WriteElement(root, "game_mode", m_game_mode);
  return doc.Save();
}

/*
 * Return the value of the environment variable 'name' or
 * 'default' if not set
 */
std::string Config::GetEnv(const std::string & name, const std::string &default_value)
{
  const char *env = std::getenv(name.c_str());
  if (env != NULL) {
    return env;
  } else {
    return default_value;
  }
}

std::string Config::GetDataDir() const
{
  return data_dir;
}

std::string Config::GetLocaleDir() const
{
  return locale_dir;
}

std::string Config::GetPersonalDir() const
{
  return personal_dir;
}

std::list<struct ConfigTeam> & Config::AccessTeamList()
{
  return teams;
}

const std::string & Config::GetMapName() const
{
  return map_name;
}

bool Config::GetDisplayEnergyCharacter() const
{
  return display_energy_character;
}

bool Config::GetDisplayNameCharacter() const
{
  return display_name_character;
}

bool Config::GetDisplayWindParticles() const
{
  return display_wind_particles;
}

bool Config::GetDefaultMouseCursor() const
{
  return default_mouse_cursor;
}

bool Config::GetScrollOnBorder() const
{
  return scroll_on_border;
}

std::string Config::GetTtfFilename() const
{
  return ttf_filename;
}

bool Config::IsNetworkActivated() const
{
  return enable_network;
}

bool Config::IsVideoFullScreen() const
{
  return video_fullscreen;
}

void Config::SetNetworkActivated(const bool set_net)
{
  enable_network = set_net;
}

void Config::SetVideoFullScreen(const bool set_fullscreen)
{
  video_fullscreen = set_fullscreen;
}

uint Config::GetVideoWidth() const
{
  return video_width;
}

void Config::SetVideoWidth(const uint width)
{
  video_width = width;
}

uint Config::GetVideoHeight() const
{
  return video_height;
}

void Config::SetVideoHeight(const uint height)
{
  video_height = height;
}

bool Config::GetSoundMusic() const
{
  return sound_music;
}

void Config::SetSoundMusic(const bool music)
{
  sound_music = music;
}

bool Config::GetSoundEffects() const
{
  return sound_effects;
}

void Config::SetSoundEffects(const bool effects)
{
  sound_effects = effects;
}

uint Config::GetSoundFrequency() const
{
  return sound_frequency;
}

void Config::SetSoundFrequency(const uint freq)
{
  sound_frequency = freq;
}

void Config::SetDisplayEnergyCharacter(const bool dec)
{
  display_energy_character = dec;
}

void Config::SetDisplayNameCharacter(const bool dnc)
{
  display_name_character = dnc;
}

void Config::SetDisplayWindParticles(const bool dwp)
{
  display_wind_particles = dwp;
}

void Config::SetDefaultMouseCursor(const bool dmc)
{
  default_mouse_cursor = dmc;
}

void Config::SetScrollOnBorder(const bool sob)
{
  scroll_on_border = sob;
}

int Config::GetTransparency() const
{
  return transparency;
}
