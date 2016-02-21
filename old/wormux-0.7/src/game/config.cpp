/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Configuration de Wormux : toutes les variables qui sont intéressantes à
 * modifier se retrouvent ici. Les variables ont une valeur par défaut qui
 * peut être modifiée avec le fichier de configuration.
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
#include "../graphic/video.h"
#include "../include/action.h"
#include "../include/app.h"
#include "../interface/keyboard.h"
#include "../include/constant.h"
#include "../map/maps_list.h"
#include "../sound/jukebox.h"
#include "../team/teams_list.h"
#include "../team/skin.h"
#include "../tool/file_tools.h"
#include "../tool/string_tools.h"
#include "../tool/i18n.h"
#include "../weapon/weapons_list.h"

const std::string NOMFICH="config.xml";
Config * Config::singleton = NULL;

Config * Config::GetInstance() {
  if (singleton == NULL) {
    singleton = new Config();
  }
  return singleton;
}

Config::Config()
{
  // Default values
  exterieur_monde_vide = true;
  m_game_mode = "classic";
  display_energy_character = true;
  display_name_character = true;
  display_wind_particles = true;
  transparency = ALPHA;
   
  // video
  tmp.video.width = 800;
  tmp.video.height = 600;
  tmp.video.fullscreen = false;

  tmp.sound.music = true;
  tmp.sound.effects = true;
  tmp.sound.frequency = 44100;
}

void Config::Init()
{
  Constants::GetInstance();

  // directories
#ifndef WIN32
  data_dir = *GetEnv(Constants::ENV_DATADIR, Constants::DEFAULT_DATADIR);
  locale_dir = *GetEnv(Constants::ENV_LOCALEDIR, Constants::DEFAULT_LOCALEDIR);
#else
  data_dir = "data\\";
  locale_dir = "locale\\";
#endif
  std::string dft_font_path = std::string(data_dir + PATH_SEPARATOR + "font" + PATH_SEPARATOR + "DejaVuSans.ttf");
  ttf_filename = *GetEnv(Constants::ENV_FONT_PATH, dft_font_path);

#ifndef WIN32
  personal_dir = GetHome()+"/.wormux/";
#else
  personal_dir = GetHome()+"\\Wormux\\";
#endif
}

bool Config::Load()
{
  bool result = ChargeVraiment();
  std::string dir;
  dir = TranslateDirectory(locale_dir);
  I18N_SetDir (dir + PATH_SEPARATOR);

  dir = TranslateDirectory(data_dir);
  resource_manager.AddDataPath(dir + PATH_SEPARATOR);
  return result;
}

bool Config::ChargeVraiment()
{
  m_xml_charge=false;
  try
  {
    // Charge la configuration XML
    LitDocXml doc;
    m_nomfich = personal_dir+NOMFICH;
    if (!doc.Charge (m_nomfich)) return false;
    if (!ChargeXml (doc.racine())) return false;
    m_xml_charge = true;
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

// Lit les données sur une équipe
bool Config::ChargeXml(xmlpp::Element *xml)
{
  xmlpp::Element *elem;
  
  //=== Map ===
  LitDocXml::LitString  (xml, "map", tmp.map_name);

  //=== Equipes ===
  elem = LitDocXml::AccesBalise (xml, "teams");
  if (elem != NULL)
  {
    LitDocXml::LitListeString (elem, "team", tmp.teams);
  }

  //=== Vidéo ===
  elem = LitDocXml::AccesBalise (xml, "video");
  if (elem != NULL)
  {
    uint max_fps;
    if (LitDocXml::LitUint (elem, "max_fps", max_fps)) 
      AppWormux::GetInstance()->video.SetMaxFps(max_fps);

    LitDocXml::LitBool (elem, "display_wind_particles", display_wind_particles);  
    LitDocXml::LitBool (elem, "display_energy_character", display_energy_character);
    LitDocXml::LitBool (elem, "display_name_character", display_name_character);
    LitDocXml::LitInt (elem, "width", tmp.video.width);
    LitDocXml::LitInt (elem, "height", tmp.video.height);
    LitDocXml::LitBool (elem, "full_screen", tmp.video.fullscreen);
  }

  //=== Son ===
  elem = LitDocXml::AccesBalise (xml, "sound");
  if (elem != NULL)
  {
    LitDocXml::LitBool (elem, "music", tmp.sound.music);
    LitDocXml::LitBool (elem, "effects", tmp.sound.effects);
    LitDocXml::LitUint (elem, "frequency", tmp.sound.frequency);
  }

  //=== Mode de jeu ===
  LitDocXml::LitString (xml, "game_mode", m_game_mode);
  return true;
}

void Config::SetKeyboardConfig()
{
  Clavier * clavier = Clavier::GetInstance();
  
  clavier->SetKeyAction(SDLK_LEFT,		ACTION_MOVE_LEFT);		
  clavier->SetKeyAction(SDLK_RIGHT,	ACTION_MOVE_RIGHT);
  clavier->SetKeyAction(SDLK_UP,			ACTION_UP);
  clavier->SetKeyAction(SDLK_DOWN,	ACTION_DOWN);
  clavier->SetKeyAction(SDLK_RETURN,	ACTION_JUMP);
  clavier->SetKeyAction(SDLK_BACKSPACE, ACTION_HIGH_JUMP);
  clavier->SetKeyAction(SDLK_SPACE, ACTION_SHOOT);
  clavier->SetKeyAction(SDLK_TAB, ACTION_CHANGE_CHARACTER);
  clavier->SetKeyAction(SDLK_ESCAPE, ACTION_QUIT);
  clavier->SetKeyAction(SDLK_p, ACTION_PAUSE);
  clavier->SetKeyAction(SDLK_F10, ACTION_FULLSCREEN);
  clavier->SetKeyAction(SDLK_F9, ACTION_TOGGLE_INTERFACE);
  clavier->SetKeyAction(SDLK_F1, ACTION_WEAPONS1);
  clavier->SetKeyAction(SDLK_F2, ACTION_WEAPONS2);
  clavier->SetKeyAction(SDLK_F3, ACTION_WEAPONS3);
  clavier->SetKeyAction(SDLK_F4, ACTION_WEAPONS4);
  clavier->SetKeyAction(SDLK_F5, ACTION_WEAPONS5);
  clavier->SetKeyAction(SDLK_F6, ACTION_WEAPONS6);
  clavier->SetKeyAction(SDLK_F7, ACTION_WEAPONS7);
  clavier->SetKeyAction(SDLK_F8, ACTION_WEAPONS8);
  clavier->SetKeyAction(SDLK_c, ACTION_CENTER);
}

void Config::Apply()
{
  SetKeyboardConfig();

  // Charge le mode jeu
  weapons_list.Init();
  
  GameMode::GetInstance()->Load(m_game_mode);

  // Son
  jukebox.ActiveMusic (tmp.sound.music);
  jukebox.ActiveEffects (tmp.sound.effects);
  jukebox.SetFrequency (tmp.sound.frequency);

  // Charge les équipes 
  InitSkins();
  teams_list.LoadList();
  if (m_xml_charge)
    teams_list.InitList (tmp.teams);
   
  // Charge les terrains
  lst_terrain.Init();
  if (m_xml_charge && !tmp.map_name.empty()) 
    lst_terrain.ChangeTerrainNom (tmp.map_name);
  else
    lst_terrain.ChangeTerrain (0);
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

  if (!SauveXml())
  {
    return false;
  }
  return true;
}

bool Config::SauveXml()
{
  EcritDocXml doc;

  doc.Cree (m_nomfich, "config", "1.0", "iso-8859-1");
  xmlpp::Element *racine = doc.racine();
  doc.EcritBalise (racine, "version", Constants::VERSION);

  //=== Terrain ===
  doc.EcritBalise (racine, "map", lst_terrain.TerrainActif().name);

  //=== Equipes ===
  xmlpp::Element *balise_equipes = racine -> add_child("teams");
  TeamsList::iterator
    it=teams_list.playing_list.begin(),
    fin=teams_list.playing_list.end();
  for (; it != fin; ++it)
  {
    doc.EcritBalise (balise_equipes, "team", (**it).GetId());
  }

  //=== Video ===
  AppWormux * app = AppWormux::GetInstance();

  xmlpp::Element *noeud_video = racine -> add_child("video");
  doc.EcritBalise (noeud_video, "display_wind_particles", ulong2str(display_wind_particles));  
  doc.EcritBalise (noeud_video, "display_energy_character", ulong2str(display_energy_character));
  doc.EcritBalise (noeud_video, "display_name_character", ulong2str(display_name_character));
  doc.EcritBalise (noeud_video, "width", ulong2str(app->video.window.GetWidth()));
  doc.EcritBalise (noeud_video, "height", ulong2str(app->video.window.GetHeight()));
  doc.EcritBalise (noeud_video, "full_screen", 
		   ulong2str(static_cast<uint>(app->video.IsFullScreen())) );	  
  doc.EcritBalise (noeud_video, "max_fps", 
          long2str(static_cast<int>(app->video.GetMaxFps())));

  if ( transparency == ALPHA )
    doc.EcritBalise (noeud_video, "transparency", "alpha");
  else if ( transparency == COLORKEY )
    doc.EcritBalise (noeud_video, "transparency", "colorkey");
 
  //=== Son ===
  xmlpp::Element *noeud_son = racine -> add_child("sound");
  doc.EcritBalise (noeud_son, "music", ulong2str(jukebox.UseMusic()));
  doc.EcritBalise (noeud_son, "effects", 
		   ulong2str(jukebox.UseEffects()));
  doc.EcritBalise (noeud_son, "frequency",
		   ulong2str(jukebox.GetFrequency()));
   
  //=== Mode de jeu ===
  doc.EcritBalise (racine, "game_mode", m_game_mode);
  return doc.Sauve();
}

/*
 * Return the value of the environment variable 'name' or
 * 'dft' if not set
 */
std::string * Config::GetEnv(const std::string & name, const std::string & dft) {
  std::string * value;
  char * c_value = std::getenv(name.c_str());
  
  if (c_value != NULL) {
    value = new std::string(c_value);
  } else {
    value = new std::string(dft);
  }
  return value;
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

bool Config::GetExterieurMondeVide() const
{
  return exterieur_monde_vide;
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

std::string Config::GetTtfFilename() const
{
  return ttf_filename;
}

void Config::SetDisplayEnergyCharacter(bool dec)
{
  display_energy_character = dec;
}

void Config::SetDisplayNameCharacter(bool dnc)
{
  display_name_character = dnc;
}

void Config::SetDisplayWindParticles(bool dwp)
{
  display_wind_particles = dwp;
}

void Config::SetExterieurMondeVide(bool emv)
{
  exterieur_monde_vide = emv;
}

int Config::GetTransparency() const
{
  return transparency;
}
