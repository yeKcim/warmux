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
 * Warmux configuration : all variables interesting to tweak should be here.
 * A default value is affected for each variable, the value can be changed by
 * the configuration file.
 *****************************************************************************/

#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <errno.h>
#include <libxml/tree.h>

#ifdef __APPLE__
#  include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef WIN32
#  include <windows.h>
#  include <direct.h>
#endif

#include <WARMUX_file_tools.h>
#include <WARMUX_team_config.h>

#include "game/config.h"
#include "game/game.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/keyboard.h"
#include "network/network.h"
#include "object/object_cfg.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"

#ifdef _WIN32
// Under Windows, binary may be relocated
static std::string GetWarmuxPath()
{
  WCHAR  buffer[4*MAX_PATH];
  DWORD size = GetModuleFileNameW(NULL, buffer, 4*MAX_PATH);

  if (size<1)
    return std::string("");

  // Now get shortname
  size = GetShortPathNameW(buffer, NULL, 0);
  ASSERT(size);
  WCHAR *buf = new WCHAR[size];
  GetShortPathNameW(buffer, buf, size);

  // Retrieve the path and convert it to ANSI
  size = wcsrchr((wchar_t*)buf, L'\\')+1 - buf;
  ASSERT(size < MAX_PATH);
  int ulen = WideCharToMultiByte(CP_UTF8, 0, buf, size, NULL, 0, NULL, NULL);

  std::string ret;
  ret.resize(ulen-1);
  WideCharToMultiByte(CP_UTF8, 0, buf, size, (LPSTR)ret.c_str(), ulen-1, NULL, NULL);
  delete[] buf;

  return ret;
}
#else
#  if defined(ANDROID)
static std::string GetWarmuxPath() { return "."; }
#  elif defined(GEKKO)
static std::string GetWarmuxPath() { return "sd:/apps/Warmux"; }
#  endif
#  include <unistd.h> // not needed by mingw
#endif

static const std::string FILENAME="config.xml";

Config::Config()
  : default_language("")
  , m_game_mode("classic")
  , display_energy_character(true)
  , display_name_character(true)

#ifdef HAVE_HANDHELD
  , wind_particles_percentage(0) // Too CPU intensive
  , display_multi_layer_sky(false) // Memory hungry + CPU intensive
#else
  , wind_particles_percentage(100)
  , display_multi_layer_sky(true)
#endif

  , default_mouse_cursor(false)
  , video_width(0)
  , video_height(0)

#ifdef HAVE_TOUCHSCREEN
  , video_fullscreen(true) // No other mode supported
  , max_fps(25)
#else
  , video_fullscreen(false)
  , max_fps(50)
#endif

  , bling_bling_interface(false)
  , scroll_on_border(false)
  , scroll_border_size(50)
  , sound_music(true)
  , sound_effects(true)
#ifdef HAVE_HANDHELD
  , sound_frequency(22050)
#else
  , sound_frequency(44100)
#endif
  , warn_on_new_player(true)
  , check_updates(false)
  , lefthanded_mouse(false)
  , m_network_client_host("localhost")
  , m_network_client_port(WARMUX_NETWORK_PORT)
  , m_network_server_game_name("Warmux party")
  , m_network_server_port(WARMUX_NETWORK_PORT)
  , m_network_server_public(true)

#ifdef HAVE_HANDHELD
  , quality(QUALITY_16BPP)
#else
  , quality(QUALITY_32BPP)
#endif
#ifdef HAVE_FACEBOOK
  , fb_email("")
  , fb_save_pwd(false)
#endif
#ifdef HAVE_TWITTER
  , twit_user("")
  , twit_save_pwd(false)
#endif
{
  // Set audio volume
  volume_music = JukeBox::GetMaxVolume()/2;
  volume_effects = JukeBox::GetMaxVolume()/2;

  Constants::GetInstance();

  // directories
#if defined(__APPLE__)
  // the following code will enable warmux to find its data when placed in an app bundle on mac OS X.
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
      std::string default_ttf_filename = contents + std::string("/Resources/data/font/Ubuntu-R.ttf");

      // if environment variables exist, they will override default values
      data_dir     = GetEnv(Constants::ENV_DATADIR, default_data_dir);
      ttf_filename = GetEnv(Constants::ENV_FONT_PATH, default_ttf_filename);
      personal_config_dir = GetHome() + "/Library/Application Support/Warmux/";
      personal_data_dir = personal_config_dir;
#  ifdef ENABLE_NLS
      std::string default_locale_dir = contents + std::string("/Resources/locale/");
      locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, default_locale_dir);
#  endif
  }
  else {
      // executable is installed Unix-style, use default paths
      data_dir     = GetEnv(Constants::ENV_DATADIR, INSTALL_DATADIR);
#  ifdef ENABLE_NLS
      locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, INSTALL_LOCALEDIR);
#  endif
      ttf_filename = GetEnv(Constants::ENV_FONT_PATH, FONT_FILE);
  }
#elif defined(_WIN32) || defined(ANDROID) || defined(GEKKO)
  std::string basepath = GetWarmuxPath();
  data_dir     = basepath + PATH_SEPARATOR "data" PATH_SEPARATOR;
#  ifdef ENABLE_NLS
  locale_dir   = basepath + PATH_SEPARATOR "locale" PATH_SEPARATOR;
#  endif
  ttf_filename = basepath + PATH_SEPARATOR FONT_FILE;
  font_dir     = data_dir + PATH_SEPARATOR "font" PATH_SEPARATOR;

  personal_config_dir = GetHome() + PATH_SEPARATOR "Warmux" PATH_SEPARATOR;
  personal_data_dir = personal_config_dir;

#else //Neither WIN32, ANDROID or __APPLE__
  data_dir     = GetEnv(Constants::ENV_DATADIR, INSTALL_DATADIR);
#  ifdef ENABLE_NLS
  locale_dir   = GetEnv(Constants::ENV_LOCALEDIR, INSTALL_LOCALEDIR);
#  endif
  ttf_filename = GetEnv(Constants::ENV_FONT_PATH, FONT_FILE);
  font_dir     = GetEnv(Constants::ENV_FONT_PATH, data_dir + PATH_SEPARATOR "font" PATH_SEPARATOR);

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
    personal_data_dir = GetHome() + "/.local/share";
  }
  else
    personal_data_dir = c_data_dir;

  personal_data_dir += "/wormux/";
#endif

  std::string old_dir = GetOldPersonalDir();
  if (old_dir != "") {
    std::cout << "Moving " << old_dir << " to " << personal_data_dir << std::endl;
    if (old_dir != personal_data_dir)
      Rename(old_dir, personal_data_dir);
  }

  std::string old_config_file_name = personal_data_dir + "config.xml";
  std::string config_file_name = personal_config_dir + "config.xml";
  if (old_config_file_name != config_file_name) {
    std::cout << "Moving " << old_config_file_name
              << " to " << config_file_name << std::endl;
    Rename(old_config_file_name, config_file_name);
  }

  chat_log_dir = personal_data_dir + "logs" PATH_SEPARATOR;

  // Create the directories
  // Delayed after copy/creation of personal_data_dir, because they
  // might cause an access denied (because of indexing under Windows)
  MkdirChatLogDir();
  MkdirPersonalConfigDir();
  MkdirPersonalDataDir();

  LoadDefaultValue();

  // Load personal config
  std::string dir;
  if (!DoLoading())
  {
#ifdef ENABLE_NLS
    // Failed, still try to apply default config then
    SetLanguage("");
#endif
  }

  dir = TranslateDirectory(data_dir);
  GetResourceManager().SetDataPath(dir + PATH_SEPARATOR);
}

bool Config::MkdirChatLogDir() const
{
  return CreateFolder(chat_log_dir);
}

bool Config::MkdirPersonalConfigDir() const
{
  bool r = CreateFolder(personal_config_dir);

  if (r) {
    CreateFolder(personal_config_dir + "custom_team");
  }

  return r;
}

bool Config::MkdirPersonalDataDir() const
{
  bool r = CreateFolder(personal_data_dir);

  if (r) {
    CreateFolder(personal_data_dir + "map");
    CreateFolder(personal_data_dir + "team");
    CreateFolder(personal_data_dir + "game_mode");
  }

  return r;
}

bool Config::RemovePersonalConfigFile() const
{
  std::string personal_config_file = personal_config_dir + FILENAME;

  int r = unlink(personal_config_file.c_str());
  if (r) {
    if (errno == -ENOENT) {
      r = 0;
    } else {
      perror((Format("Fail to remove personal config file %s", personal_config_file.c_str())).c_str());
    }
  }

  if (r)
    return false;

  return true;
}

#ifdef ENABLE_NLS
void Config::SetLanguage(const std::string& language)
{
  default_language = language;
  InitI18N(TranslateDirectory(locale_dir), language);

  Font::ReleaseInstances();
  if (GameIsRunning()) {
    Game::GetInstance()->UpdateTranslation();
  }
}
#endif

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
#ifdef ANDROID
  m_default_config = GetDataDir() + "warmux_default_android_config.xml";
#elif MAEMO
  m_default_config = GetDataDir() + "warmux_default_maemo_config.xml";
#elif __SYMBIAN32__
  m_default_config = GetDataDir() + "warmux_default_symbian_config.xml";
#else
  m_default_config = GetDataDir() + "warmux_default_config.xml";
#endif
  Profile *res = GetResourceManager().LoadXMLProfile(m_default_config, true);

  std::cout << "o " << _("Reading the default config file") << std::endl;
  std::ostringstream section;
  Point2i tmp;

  //=== Default video value ===
  int number_of_resolution_available = GetResourceManager().LoadInt(res, "default_video_mode/number_of_resolution_available");
  for(int i = 1; i <= number_of_resolution_available; i++) {
    tmp = Point2i(0, 0);
    std::ostringstream section; section << "default_video_mode/" << i;
    tmp = GetResourceManager().LoadPoint2i(res, section.str());
    if(tmp.GetX() > 0 && tmp.GetY() > 0)
      resolution_available.push_back(tmp);
  }

  //== Default keyboard key
  const xmlNode *node = GetResourceManager().GetElement(res, "section", "default_keyboard_layout");
  if (node) {
    Keyboard::GetInstance()->SetConfig(node);
  }

#ifdef ENABLE_NLS
  //=== Default fonts value ===
  node = GetResourceManager().GetElement(res, "section", "default_language_fonts");
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
#endif

#if 0 //== Team Color
  int number_of_team_color = GetResourceManager().LoadInt(res, "team_colors/number_of_team_color");
  for(int i = 1; i <= number_of_team_color; i++) {
    tmp = Point2i(0, 0);
    std::ostringstream section; section << "team_colors/" << i;
    tmp = GetResourceManager().LoadPoint2i(res, section.str());
    if(tmp.GetX() > 0 && tmp.GetY() > 0)
      resolution_available.push_back(tmp);
  }
#endif

  GetResourceManager().UnLoadXMLProfile(res);
}

void Config::ReadTeams(std::list<ConfigTeam>& teams, const xmlNode* elem)
{
  uint i = 0;
  const xmlNode *team;

  while ((team = XmlReader::GetMarker(elem, "team_" + int2str(i)))) {
    ConfigTeam one_team;
    XmlReader::ReadString(team, "id", one_team.id);
    XmlReader::ReadString(team, "player_name", one_team.player_name);
    XmlReader::ReadUint(team, "nb_characters", one_team.nb_characters);
    // The ai element needs a defaut as it has been added afterwards:
    if (!XmlReader::ReadString(team, "ai", one_team.ai)) {
      one_team.ai = (i == 1) ? DEFAULT_AI_NAME : NO_AI_NAME;
    }
    if (!XmlReader::ReadUint(team, "group", one_team.group))
      one_team.group = i;

    teams.push_back(one_team);

    // get next team
    i++;
  }
}

// Read personal config file
void Config::LoadXml(const xmlNode *xml)
{
  const xmlNode *elem;

  std::cout << "o " << _("Reading the personal config file") << std::endl;

  //=== Map ===
  XmlReader::ReadString(xml, "map", map_name);

  //=== Language ===
  XmlReader::ReadString(xml, "default_language", default_language);
#ifdef ENABLE_NLS
  SetLanguage(default_language);
#endif

  //=== Teams ===
  if ((elem = XmlReader::GetMarker(xml, "teams")))
    ReadTeams(teams, elem);

  //=== Video ===
  if ((elem = XmlReader::GetMarker(xml, "video"))) {
    XmlReader::ReadBool(elem, "bling_bling_interface", bling_bling_interface);
    XmlReader::ReadUint(elem, "max_fps", max_fps);
    XmlReader::ReadUint(elem, "wind_particles_percentage", wind_particles_percentage);
    if (wind_particles_percentage > 100)
      wind_particles_percentage = 100;
    XmlReader::ReadBool(elem, "display_multi_layer_sky", display_multi_layer_sky);
    XmlReader::ReadBool(elem, "display_energy_character", display_energy_character);
    XmlReader::ReadBool(elem, "display_name_character", display_name_character);
    XmlReader::ReadBool(elem, "default_mouse_cursor", default_mouse_cursor);
#ifndef HAVE_TOUCHSCREEN // Those should never be set
    XmlReader::ReadBool(elem, "scroll_on_border", scroll_on_border);
    XmlReader::ReadUint(elem, "scroll_border_size", scroll_border_size);
#endif
    XmlReader::ReadUint(elem, "width", video_width);
    XmlReader::ReadUint(elem, "height", video_height);
    XmlReader::ReadBool(elem, "full_screen", video_fullscreen);

    uint qual;
    if (XmlReader::ReadUint(elem, "quality", qual)) {
	    if (qual>QUALITY_MAX-1) qual=QUALITY_MAX-1;
	    quality = (Quality)qual;
    }
  }

  //=== Sound ===
  if ((elem = XmlReader::GetMarker(xml, "sound"))) {
    XmlReader::ReadBool(elem, "music", sound_music);
    XmlReader::ReadBool(elem, "effects", sound_effects);
#ifndef HAVE_HANDHELD
    XmlReader::ReadUint(elem, "frequency", sound_frequency);
#endif
    XmlReader::ReadUint(elem, "volume_music", volume_music);
    XmlReader::ReadUint(elem, "volume_effects", volume_effects);
  }

  //=== network ===
  if ((elem = XmlReader::GetMarker(xml, "network"))) {
    const xmlNode *sub_elem;
    if ((sub_elem = XmlReader::GetMarker(elem, "as_client"))) {
      XmlReader::ReadString(sub_elem, "host", m_network_client_host);
      XmlReader::ReadString(sub_elem, "port", m_network_client_port);
    }

    if ((sub_elem = XmlReader::GetMarker(elem, "as_server"))) {
      XmlReader::ReadString(sub_elem, "game_name", m_network_server_game_name);
      XmlReader::ReadString(sub_elem, "port", m_network_server_port);
      XmlReader::ReadBool(sub_elem, "public", m_network_server_public);
    }

    //=== personal teams used in last network game ===
    if ((sub_elem = XmlReader::GetMarker(elem, "local_teams")))
      ReadTeams(network_local_teams, sub_elem);
  }

  //=== misc ===
  if ((elem = XmlReader::GetMarker(xml, "misc"))) {
    XmlReader::ReadBool(elem, "check_updates", check_updates);
    XmlReader::ReadBool(elem, "left-handed_mouse", lefthanded_mouse);
  }

  //=== Social ===
  if ((elem = XmlReader::GetMarker(xml, "social"))) {
#ifdef HAVE_FACEBOOK
    XmlReader::ReadString(elem, "facebook_email", fb_email);
    XmlReader::ReadBool(elem, "facebook_savepwd", fb_save_pwd);
    if (fb_save_pwd)
      XmlReader::ReadString(elem, "facebook_password", fb_pwd);
#endif
#ifdef HAVE_TWITTER
    XmlReader::ReadString(elem, "twitter_user", twit_user);
    XmlReader::ReadBool(elem, "twitter_savepwd", twit_save_pwd);
    if (twit_save_pwd)
      XmlReader::ReadString(elem, "twitter_password", twit_pwd);
#endif
  }

  //=== game mode ===
  XmlReader::ReadString(xml, "game_mode", m_game_mode);

  //=== controls ===
  if ((elem = XmlReader::GetMarker(xml, "controls"))) {
    const xmlNode *node = XmlReader::GetMarker(elem, "keyboard");
    if (node) {
      Keyboard::GetInstance()->SetConfig(node);
    }
  }
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

void Config::WriteTeams(const std::list<ConfigTeam>& teams, XmlWriter& doc, xmlNode* xml)
{
  std::list<ConfigTeam>::const_iterator
    it = teams.begin(),
    end = teams.end();

  for (uint i=0; it!=end; ++it, i++) {
    std::string name = "team_"+uint2str(i);
    xmlNode* a_team = xmlAddChild(xml,
                                  xmlNewNode(NULL /* empty prefix */, (const xmlChar*)name.c_str()));
    doc.WriteElement(a_team, "id", (*it).id);
    doc.WriteElement(a_team, "player_name", (*it).player_name);
    doc.WriteElement(a_team, "nb_characters", uint2str((*it).nb_characters));
    doc.WriteElement(a_team, "ai", (*it).ai);
    doc.WriteElement(a_team, "group", uint2str((*it).group));
  }
}

bool Config::SaveXml(bool save_current_teams)
{
  XmlWriter doc;

  doc.Create(m_filename, "config", "1.0", "utf-8");
  xmlNode *root = doc.GetRoot();
  doc.WriteElement(root, "version", Constants::WARMUX_VERSION);

  //=== Map ===
  //The map name is modified when the player validate its choice in the
  //map selection box.
  doc.WriteElement(root, "map", map_name);

  //=== Language ==
  doc.WriteElement(root, "default_language", default_language);

  //=== Teams ===
  xmlNode *team_elements = xmlAddChild(root,
                                       xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"teams"));

  if (TeamsList::IsLoaded()) {
    if (save_current_teams) {
      teams.clear();

      TeamsList::iterator
        it = GetTeamsList().playing_list.begin(),
        end = GetTeamsList().playing_list.end();

      for (int i=0; it!=end; ++it, i++) {
        ConfigTeam config;
        config.id = (**it).GetId();
        config.player_name = (**it).GetPlayerName();
        config.nb_characters = (**it).GetNbCharacters();
        config.ai = (**it).GetAIName();
        config.group = (**it).GetGroup();

        teams.push_back(config);
      }
    }

    WriteTeams(teams, doc, team_elements);
  }

  //=== Video ===
  Video * video = AppWarmux::GetInstance()->video;
  xmlNode* video_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"video"));
  doc.WriteElement(video_node, "wind_particles_percentage", uint2str(wind_particles_percentage));
  doc.WriteElement(video_node, "display_multi_layer_sky", bool2str(display_multi_layer_sky));
  doc.WriteElement(video_node, "display_energy_character", bool2str(display_energy_character));
  doc.WriteElement(video_node, "display_name_character", bool2str(display_name_character));
  doc.WriteElement(video_node, "bling_bling_interface", bool2str(bling_bling_interface));
  doc.WriteElement(video_node, "default_mouse_cursor", bool2str(default_mouse_cursor));
  doc.WriteElement(video_node, "scroll_on_border", bool2str(scroll_on_border));
  doc.WriteElement(video_node, "scroll_border_size", uint2str(scroll_border_size));
  doc.WriteElement(video_node, "width", uint2str(video->window.GetWidth()));
  doc.WriteElement(video_node, "height", uint2str(video->window.GetHeight()));
  doc.WriteElement(video_node, "full_screen", bool2str(video->IsFullScreen()));
  doc.WriteElement(video_node, "quality", uint2str(quality));
  doc.WriteElement(video_node, "max_fps", uint2str(video->GetMaxFps()));

  //=== Sound ===
  xmlNode *sound_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"sound"));
  doc.WriteElement(sound_node, "music",  bool2str(sound_music));
  doc.WriteElement(sound_node, "effects", bool2str(sound_effects));
#ifndef HAVE_HANDHELD
  doc.WriteElement(sound_node, "frequency", uint2str(sound_frequency));
#endif
  doc.WriteElement(sound_node, "volume_music",  uint2str(volume_music));
  doc.WriteElement(sound_node, "volume_effects", uint2str(volume_effects));

  //=== Network ===
  xmlNode *net_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"network"));

  // Network as client parameters
  xmlNode *net_as_client_node = xmlAddChild(net_node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"as_client"));
  doc.WriteElement(net_as_client_node, "host", m_network_client_host);
  doc.WriteElement(net_as_client_node, "port", m_network_client_port);

  // Network as server parameters
  xmlNode *net_as_server_node = xmlAddChild(net_node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"as_server"));
  doc.WriteElement(net_as_server_node, "game_name", m_network_server_game_name);
  doc.WriteElement(net_as_server_node, "port", m_network_server_port);
  doc.WriteElement(net_as_server_node, "public", bool2str(m_network_server_public));

  // personal teams used durint last network game
  xmlNode *net_teams = xmlAddChild(net_node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"local_teams"));
  WriteTeams(network_local_teams, doc, net_teams);

  //=== Misc ===
  xmlNode *misc_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"misc"));
  doc.WriteElement(misc_node, "check_updates", bool2str(check_updates));
  doc.WriteElement(misc_node, "left-handed_mouse", bool2str(lefthanded_mouse));

  //=== Social ===
#if defined(HAVE_FACEBOOK) || defined(HAVE_TWITTER)
  xmlNode *social_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"social"));
#endif
#ifdef HAVE_FACEBOOK
  doc.WriteElement(social_node, "facebook_email", fb_email);
  doc.WriteElement(social_node, "facebook_savepwd", bool2str(fb_save_pwd));
  if (fb_save_pwd)
    doc.WriteElement(social_node, "facebook_password", fb_pwd);
#endif
#ifdef HAVE_TWITTER
  doc.WriteElement(social_node, "twitter_user", twit_user);
  doc.WriteElement(social_node, "twitter_savepwd", bool2str(twit_save_pwd));
  if (twit_save_pwd)
    doc.WriteElement(social_node, "twitter_password", twit_pwd);
#endif

  //=== game mode ===
  doc.WriteElement(root, "game_mode", m_game_mode);

  //=== controls ===
  xmlNode *controls_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"controls"));
  Keyboard::GetInstance()->SaveConfig(controls_node);

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

const std::string& Config::GetTtfFilename() const
{
#ifdef ENABLE_NLS
  std::map<std::string, std::string>::const_iterator it = fonts.begin();
  while (it != fonts.end()) {
    const std::string& lang = it->first;
    if (!lang.compare(0, lang.size(), default_language)) {
      printf("Matched %s to %s\n", default_language.c_str(), lang.c_str());
      return it->second;
    }
    ++it;
  }
  return ttf_filename;
#else
  return ttf_filename;
#endif
}

void Config::SetNetworkLocalTeams()
{
  // personal teams used durint last network game
  network_local_teams.clear();

  TeamsList::iterator
    it = GetTeamsList().playing_list.begin(),
    end = GetTeamsList().playing_list.end();

  for (int i=0; it != end; ++it, i++) {
    if ((**it).IsLocal()) {
      ConfigTeam config;
      config.id = (**it).GetId();
      config.player_name = (**it).GetPlayerName();
      config.nb_characters = (**it).GetNbCharacters();
      config.ai = (**it).GetAIName();
      config.group = (**it).GetGroup();
      network_local_teams.push_back(config);
    }
  }
}
