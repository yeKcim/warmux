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
 * Wormux configuration : all variables interesting to tweak should be here.
 * A default value is affected for each variable, the value can be changed by
 * the configuration file.
 *****************************************************************************/

#include "game/config.h"

#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/tree.h>
#ifdef WIN32
#  include <direct.h>
#endif
#ifdef __APPLE__
#  include <CoreFoundation/CoreFoundation.h>
#endif
#include "graphic/font.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "network/network.h"
#include "object/object_cfg.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "team/team_config.h"
#include "tool/resource_manager.h"
#include "tool/file_tools.h"
#include "tool/string_tools.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"
#ifdef USE_AUTOPACKAGE
#  include "include/binreloc.h"
#endif

#ifndef WIN32
#define MKDIR_P(dir) (mkdir(dir, 0750))
#else
#define MKDIR_P(dir) (_mkdir(dir))
#endif

#ifdef _WIN32
#include <windows.h>

// Under windows, binary may be relocated
static std::string GetWormuxPath()
{
  char  buffer[MAX_PATH];
  DWORD size = MAX_PATH;
#if 0
  HKEY  hK;
  DWORD type;

  buffer[0] = 0;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Games\\Wormux", 0, KEY_READ, &hK) != ERROR_SUCCESS ||
      RegQueryValueEx(hK, "Path", NULL, &type, buffer, &size) != ERROR_SUCCESS && type != REG_SZ)
#endif
  {
    size = GetModuleFileName(NULL, buffer, MAX_PATH);
    if (size<1) return std::string("");
    char *ptr = strrchr(buffer, '\\');
    if (ptr) ptr[0] = 0; // Mask name
    else     return std::string("");
  }
  return std::string(buffer);
}
#endif

const std::string FILENAME="config.xml";

Config::Config():
  default_language(""),
  m_game_mode("classic"),
  m_network_host("localhost"),
  m_network_port(WORMUX_NETWORK_PORT),
  m_filename(),
  data_dir(),
  locale_dir(),
  personal_data_dir(),
  personal_config_dir(),
  chat_log_dir(),
  teams(),
  map_name(),
  display_energy_character(true),
  display_name_character(true),
  display_wind_particles(true),
  default_mouse_cursor(false),
  disable_joystick(true),
  disable_mouse(false),
  video_width(800),
  video_height(600),
  video_fullscreen(false),
  max_fps(0),
  bling_bling_interface(false),
  scroll_on_border(true),
  scroll_border_size(50),
  sound_music(true),
  sound_effects(true),
  sound_frequency(44100),
  enable_network(true),
  check_updates(false),
  ttf_filename(),
  transparency(ALPHA),
  config_set()
{
  // Set audio volume
  volume_music = JukeBox::GetMaxVolume();
  volume_effects = JukeBox::GetMaxVolume();

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
  font_dir     = data_dir + PATH_SEPARATOR "font" PATH_SEPARATOR;
  filename     = font_dir + PATH_SEPARATOR "DejaVuSans.ttf";
  ttf_filename = GetEnv(Constants::ENV_FONT_PATH, br_find_locale_dir(filename.c_str()));
#elif defined(__APPLE__)
  // the following code will enable wormux to find its data when placed in an app bundle on mac OS X.
  // configure with './configure ... CPPFLAGS=-DOSX_BUNDLE' to enable
  char path[1024];
  CFBundleRef mainBundle = CFBundleGetMainBundle(); assert(mainBundle);
  CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle); assert(mainBundleURL);
  CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle); assert(cfStringRef);
  CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
  CFRelease(mainBundleURL);
  CFRelease(cfStringRef);

  std::string contents = std::string(path) + std::string("/Contents");
  if(contents.find(".app") != std::string::npos){
      // executable is inside an app bundle, use app bundle-relative paths
      std::string default_data_dir = contents + std::string("/Resources/data/");
      std::string default_ttf_filename = contents + std::string("/Resources/data/font/DejaVuSans.ttf");
      std::string default_locale_dir = contents + std::string("/Resources/locale/");

      // if environment variables exist, they will override default values
      data_dir     = GetEnv(Constants::ENV_DATADIR, default_data_dir);
      locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, default_locale_dir);
      ttf_filename = GetEnv(Constants::ENV_FONT_PATH, default_ttf_filename);
  }
  else {
      // executable is installed Unix-style, use default paths
      data_dir     = GetEnv(Constants::ENV_DATADIR, INSTALL_DATADIR);
      locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, INSTALL_LOCALEDIR);
      ttf_filename = GetEnv(Constants::ENV_FONT_PATH, FONT_FILE);
  }
#else
#  ifdef _WIN32
  std::string basepath = GetWormuxPath();
  data_dir     = basepath + "\\data\\";
  locale_dir   = basepath + "\\locale\\";
  ttf_filename = basepath + "\\" FONT_FILE;
#  else
  data_dir     = GetEnv(Constants::ENV_DATADIR, INSTALL_DATADIR);
  locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, INSTALL_LOCALEDIR);
  ttf_filename = GetEnv(Constants::ENV_FONT_PATH, FONT_FILE);
#  endif
  font_dir     = GetEnv(Constants::ENV_FONT_PATH, data_dir + PATH_SEPARATOR "font" PATH_SEPARATOR);
#endif

#ifndef WIN32

  // To respect XDG Base Directory Specification from FreeDesktop
  // http://standards.freedesktop.org/basedir-spec/basedir-spec-0.6.html

  const char * c_config_dir = std::getenv("XDG_CONFIG_HOME");
  const char * c_data_dir = std::getenv("XDG_DATA_HOME");

  if (c_config_dir == NULL)
    personal_config_dir = GetHome() + "/.config";
  else
    personal_config_dir = c_config_dir;

  personal_config_dir += "/wormux/";

  if (c_data_dir == NULL) {
    personal_data_dir = GetHome() + "/.local";
    MKDIR_P(personal_data_dir.c_str());
    personal_data_dir = GetHome() + "/.local/share";
    MKDIR_P(personal_data_dir.c_str());
  }
  else
    personal_data_dir = c_data_dir;

  personal_data_dir += "/wormux/";

  // create the directories
  MkdirPersonalConfigDir();
  MkdirPersonalDataDir();

  // move from old directory ($HOME/.wormux/)
  std::string old_dir = GetHome() + "/.wormux/";
  rename(old_dir.c_str(), personal_data_dir.c_str());

  std::string old_config_file_name = personal_data_dir + "config.xml";
  std::string config_file_name = personal_config_dir + "config.xml";
  rename(old_config_file_name.c_str(), config_file_name.c_str());

#else
  personal_config_dir = GetHome() + "\\Wormux\\";
  personal_data_dir = personal_config_dir;
#endif

  chat_log_dir = personal_data_dir + "logs" PATH_SEPARATOR;
  MkdirChatLogDir();

  LoadDefaultValue();

  // Load personal config
  std::string dir;
  if (!DoLoading())
  {
    // Failed, still try to apply default config then
    SetLanguage("");
  }

  dir = TranslateDirectory(data_dir);
  resource_manager.AddDataPath(dir + PATH_SEPARATOR);
}

bool Config::MkdirChatLogDir()
{
  // Create the directory if it doesn't exist
  if (MKDIR_P(chat_log_dir.c_str()) == 0 || errno == EEXIST)
    return true;

  return false;
}

bool Config::MkdirPersonalConfigDir()
{
  // Create the directory if it doesn't exist
  if (MKDIR_P(personal_config_dir.c_str()) == 0 || errno == EEXIST)
    return true;

  return false;
}

bool Config::MkdirPersonalDataDir()
{
  // Create the directory if it doesn't exist
  if ( MKDIR_P(personal_data_dir.c_str()) == 0 || errno == EEXIST)
      return true;

  return false;
}

void Config::SetLanguage(const std::string language)
{
  default_language = language;
  InitI18N(TranslateDirectory(locale_dir), language);

  Font::ReleaseInstances();
  WeaponsList::UpdateTranslation();
}

/*
 * Load physics constants from the xml file and cache it.
 * This tries to find already loaded data in the map<> config_set and actually
 * load it if it cannot be found.
 */
const ObjectConfig &Config::GetObjectConfig(const std::string &name, const std::string &xml_config) const
{
  ObjectConfig * objcfg;

  std::map<std::string, ObjectConfig*>::const_iterator  it = config_set.find(name);
  if (it == config_set.end()) {
    objcfg = new ObjectConfig();
    objcfg->LoadXml(name,xml_config);
    config_set[name] = objcfg;
  } else {
    objcfg = it->second;
  }
  return *objcfg;
}

void Config::RemoveAllObjectConfigs()
{
  std::map<std::string, ObjectConfig*>::iterator it = config_set.begin(),
    end = config_set.end();

  while (it != end) {
    delete (it->second);
    config_set.erase(it);
    it = config_set.begin();
  }
}

bool Config::DoLoading(void)
{
  // create the directory if it does not exist (we should do it before exiting the game)
  // the user can ask to start an internet game and download a file in the personnal dir
  // so it should exist
  MkdirPersonalDataDir();
  MkdirPersonalConfigDir();

  // Load XML conf
  XmlReader doc;

  m_filename = personal_config_dir + FILENAME;

  if (!doc.Load(m_filename))
    return false;

  LoadXml(doc.GetRoot());

  return true;
}

void Config::LoadDefaultValue()
{
  // Load default XML conf
  m_default_config = GetDataDir() + "wormux_default_config.xml";
  Profile *res = resource_manager.LoadXMLProfile(m_default_config, true);

  std::cout << "o " << _("Reading default config file") << std::endl;
  std::ostringstream section;
  Point2i tmp;

  //=== Default video value ===
  int number_of_resolution_available = resource_manager.LoadInt(res, "default_video_mode/number_of_resolution_available");
  for(int i = 1; i <= number_of_resolution_available; i++) {
    tmp = Point2i(0, 0);
    std::ostringstream section; section << "default_video_mode/" << i;
    tmp = resource_manager.LoadPoint2i(res, section.str());
    if(tmp.GetX() > 0 && tmp.GetY() > 0)
      resolution_available.push_back(tmp);
  }

  //=== Default fonts value ===
  xmlNode *node = resource_manager.GetElement(res, "section", "default_language_fonts");
  if (node) {
    xmlNodeArray list = XmlReader::GetNamedChildren(node, "language");
    for (xmlNodeArray::iterator it = list.begin(); it != list.end(); ++it) {
      std::string lang, font;
      if (res->doc->ReadStringAttr(*it, "name", lang) &&
         res->doc->ReadStringAttr(*it, "file", font)) {
        bool rel = false;
        res->doc->ReadBoolAttr(*it, "relative", rel);
        fonts[lang] = (rel) ? font_dir + font : font;
        //std::cout << "Language " << lang << ": " << fonts[lang] << std::endl;
      }
    }
  }
  else printf("Bleh...\n");

#if 0 //== Team Color
  int number_of_team_color = resource_manager.LoadInt(res, "team_colors/number_of_team_color");
  for(int i = 1; i <= number_of_team_color; i++) {
    tmp = Point2i(0, 0);
    std::ostringstream section; section << "team_colors/" << i;
    tmp = resource_manager.LoadPoint2i(res, section.str());
    if(tmp.GetX() > 0 && tmp.GetY() > 0)
      resolution_available.push_back(tmp);
  }
#endif

  resource_manager.UnLoadXMLProfile(res);
}

// Read personal config file
void Config::LoadXml(xmlNode *xml)
{
  std::cout << "o " << _("Reading personal config file") << std::endl;

  //=== Map ===
  XmlReader::ReadString(xml, "map", map_name);

  //=== Language ===
  XmlReader::ReadString(xml, "default_language", default_language);
  SetLanguage(default_language);

  //=== Teams ===
  xmlNode *elem = XmlReader::GetMarker(xml, "teams");
  int i = 0;

  xmlNode *team;

  while ((team = XmlReader::GetMarker(elem, "team_" + ulong2str(i))) != NULL)
  {
    ConfigTeam one_team;
    XmlReader::ReadString(team, "id", one_team.id);
    XmlReader::ReadString(team, "player_name", one_team.player_name);
    XmlReader::ReadUint(team, "nb_characters", one_team.nb_characters);

    teams.push_back(one_team);

    // get next team
    i++;
  }

  //=== Video ===
  if ((elem = XmlReader::GetMarker(xml, "video")) != NULL)
  {
    XmlReader::ReadBool(elem, "bling_bling_interface", bling_bling_interface);
    XmlReader::ReadUint(elem, "max_fps", max_fps);
    XmlReader::ReadBool(elem, "display_wind_particles", display_wind_particles);
    XmlReader::ReadBool(elem, "display_energy_character", display_energy_character);
    XmlReader::ReadBool(elem, "display_name_character", display_name_character);
    XmlReader::ReadBool(elem, "default_mouse_cursor", default_mouse_cursor);
    XmlReader::ReadBool(elem, "scroll_on_border", scroll_on_border);
    XmlReader::ReadUint(elem, "scroll_border_size", scroll_border_size);
    XmlReader::ReadBool(elem, "disable_mouse", disable_mouse);
    XmlReader::ReadBool(elem, "disable_joystick", disable_joystick);
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
    XmlReader::ReadUint(elem, "volume_music", volume_music);
    XmlReader::ReadUint(elem, "volume_effects", volume_effects);
  }

  //=== network ===
  if ((elem = XmlReader::GetMarker(xml, "network")) != NULL)
  {
    //XmlReader::ReadBool(elem, "enable_network", enable_network);
    XmlReader::ReadString(elem, "host", m_network_host);
    XmlReader::ReadString(elem, "port", m_network_port);
  }

  //=== misc ===
  if ((elem = XmlReader::GetMarker(xml, "misc")) != NULL)
  {
    XmlReader::ReadBool(elem, "check_updates", check_updates);
  }

  //=== game mode ===
  XmlReader::ReadString(xml, "game_mode", m_game_mode);
}

bool Config::Save(bool save_current_teams)
{
  std::string rep = personal_config_dir;

  // Create the directory if it doesn't exist
  if (!MkdirPersonalConfigDir())
  {
    std::cerr << "o "
	      << Format(_("Error while creating directory \"%s\": unable to store configuration file."),
			rep.c_str())
	      << " " << strerror(errno)
	      << std::endl;
    return false;
  }

  return SaveXml(save_current_teams);
}

bool Config::SaveXml(bool save_current_teams)
{
  XmlWriter doc;

  doc.Create(m_filename, "config", "1.0", "utf-8");
  xmlNode *root = doc.GetRoot();
  doc.WriteElement(root, "version", Constants::WORMUX_VERSION);

  //=== Map ===
  //The map name is modified when the player validate its choice in the
  //map selection box.
  doc.WriteElement(root, "map", map_name);

  //=== Language ==
  doc.WriteElement(root, "default_language", default_language);

  //=== Teams ===
  xmlNode *team_elements = xmlAddChild(root,
                                       xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"teams"));

  if (TeamsList::IsLoaded())
  {
    if (save_current_teams)
    {
      teams.clear();

      TeamsList::iterator
        it = GetTeamsList().playing_list.begin(),
        end = GetTeamsList().playing_list.end();

      for (int i=0; it != end; ++it, i++)
      {
        ConfigTeam config;
        config.id = (**it).GetId();
        config.player_name = (**it).GetPlayerName();
        config.nb_characters = (**it).GetNbCharacters();

        teams.push_back(config);
      }
    }

    std::list<ConfigTeam>::iterator
      it = teams.begin(),
      end = teams.end();

    for (int i=0; it != end; ++it, i++)
    {
       std::string name = "team_"+ulong2str(i);
       xmlNode* a_team = xmlAddChild(team_elements,
                                     xmlNewNode(NULL /* empty prefix */, (const xmlChar*)name.c_str()));
       doc.WriteElement(a_team, "id", (*it).id);
       doc.WriteElement(a_team, "player_name", (*it).player_name);
       doc.WriteElement(a_team, "nb_characters", ulong2str((*it).nb_characters));
    }
  }

  //=== Video ===
  Video * video = AppWormux::GetInstance()->video;
  xmlNode* video_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"video"));
  doc.WriteElement(video_node, "display_wind_particles", ulong2str(display_wind_particles));
  doc.WriteElement(video_node, "display_energy_character", ulong2str(display_energy_character));
  doc.WriteElement(video_node, "display_name_character", ulong2str(display_name_character));
  doc.WriteElement(video_node, "bling_bling_interface", ulong2str(bling_bling_interface));
  doc.WriteElement(video_node, "default_mouse_cursor", ulong2str(default_mouse_cursor));
  doc.WriteElement(video_node, "scroll_on_border", ulong2str(scroll_on_border));
  doc.WriteElement(video_node, "scroll_border_size", ulong2str(scroll_border_size));
  doc.WriteElement(video_node, "disable_mouse", ulong2str(disable_mouse));
  doc.WriteElement(video_node, "disable_joystick", ulong2str(disable_joystick));
  doc.WriteElement(video_node, "width", ulong2str(video->window.GetWidth()));
  doc.WriteElement(video_node, "height", ulong2str(video->window.GetHeight()));
  doc.WriteElement(video_node, "full_screen",
                   ulong2str(static_cast<uint>(video->IsFullScreen())) );
  doc.WriteElement(video_node, "max_fps",
                   long2str(static_cast<int>(video->GetMaxFps())));

  if (transparency == ALPHA)
    doc.WriteElement(video_node, "transparency", "alpha");
  else if (transparency == COLORKEY)
    doc.WriteElement(video_node, "transparency", "colorkey");

  //=== Sound ===
  xmlNode *sound_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"sound"));
  doc.WriteElement(sound_node, "music",  ulong2str(JukeBox::GetConstInstance()->UseMusic()));
  doc.WriteElement(sound_node, "effects", ulong2str(JukeBox::GetConstInstance()->UseEffects()));
  doc.WriteElement(sound_node, "frequency", ulong2str(JukeBox::GetConstInstance()->GetFrequency()));
  doc.WriteElement(sound_node, "volume_music",  ulong2str(volume_music));
  doc.WriteElement(sound_node, "volume_effects", ulong2str(volume_effects));

  //=== Network ===
  xmlNode *net_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"network"));
  doc.WriteElement(net_node, "host", m_network_host);
  doc.WriteElement(net_node, "port", m_network_port);

  //=== Misc ===
  xmlNode *misc_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"misc"));
  doc.WriteElement(misc_node, "check_updates", ulong2str(check_updates));

  //=== game mode ===
  doc.WriteElement(root, "game_mode", m_game_mode);
  return doc.Save();
}

/*
 * Return the value of the environment variable 'name' or
 * 'default' if not set
 */
std::string Config::GetEnv(const std::string & name, const std::string &default_value) const
{
  const char *env = std::getenv(name.c_str());
  return (env) ? env : default_value;
}

void Config::SetVolumeMusic(uint vol)
{
  volume_music = vol;
  JukeBox::SetMusicVolume(vol);
}

uint Config::GetMaxVolume()
{
  return JukeBox::GetMaxVolume();
}

const std::string& Config::GetTtfFilename()
{
  if (fonts.find(default_language) == fonts.end()) return ttf_filename;
  else                                             return fonts[default_language];
}
