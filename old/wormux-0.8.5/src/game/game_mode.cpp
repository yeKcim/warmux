/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Wormux configuration : all main configuration variables have there default
 * value here. They should all be modifiable using the xml config file
 *****************************************************************************/

#include <iostream>
#include <cstdio>
#include <WORMUX_file_tools.h>
#include "game/config.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "object/medkit.h"
#include "object/bonus_box.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"

GameMode::GameMode():
  doc_objects(NULL)
{
  m_current = "classic";

  LoadDefaultValues();
}

void GameMode::LoadDefaultValues()
{
  rules = "none";
  nb_characters = 6;
  max_teams = 4;
  duration_turn = 60;
  duration_move_player = 3;
  duration_exchange_player = 2;
  duration_before_death_mode = 20 * 60;
  damage_per_turn_during_death_mode = 5;
  gravity = 9.81;
  safe_fall = 10;
  damage_per_fall_unit = 7;

  character.init_energy = 100;
  character.max_energy = 100;
  character.mass = 100;
  character.air_resist_factor = 1.0;
  character.jump_strength = 8;
  character.jump_angle = -60;
  character.super_jump_strength = 11;
  character.super_jump_angle = -80;
  character.back_jump_strength = 9;
  character.back_jump_angle = -100;
  character.walking_pause = 50;

  auto_change_character = true;

  allow_character_selection = BEFORE_FIRST_ACTION;

  if (doc_objects)
    delete doc_objects;

  doc_objects = new XmlReader();
}

GameMode::~GameMode()
{
  delete doc_objects;
}

const std::string& GameMode::GetName() const
{
  return m_current;
}

// Load data options from the selected game_mode
bool GameMode::LoadXml(const xmlNode* xml)
{
  bool r;

  r = XmlReader::ReadString(xml, "rules", rules);
  if (!r) {
    fprintf(stderr, "Game mode: missing <rules>\n");
    return false;
  }

  XmlReader::ReadBool(xml, "auto_change_character", auto_change_character);

  std::string txt;
  if (XmlReader::ReadString(xml, "allow_character_selection", txt))
  {
    if (txt == "always")
      allow_character_selection = ALWAYS;
    else if (txt == "never")
      allow_character_selection = NEVER;
    else if (txt == "before_first_action")
      allow_character_selection = BEFORE_FIRST_ACTION;
    else
      fprintf(stderr, "%s is not a valid option for \"allow_character_selection\"\n", txt.c_str());
  }

  XmlReader::ReadUint(xml, "duration_turn", duration_turn);
  XmlReader::ReadUint(xml, "duration_move_player", duration_move_player);
  XmlReader::ReadUint(xml, "duration_exchange_player", duration_exchange_player);
  XmlReader::ReadUint(xml, "duration_before_death_mode", duration_before_death_mode);
  XmlReader::ReadUint(xml, "damage_per_turn_during_death_mode", damage_per_turn_during_death_mode);
  XmlReader::ReadUint(xml, "max_teams", max_teams);
  XmlReader::ReadUint(xml, "nb_characters", nb_characters);
  XmlReader::ReadDouble(xml, "gravity", gravity);
  XmlReader::ReadDouble(xml, "safe_fall", safe_fall);
  XmlReader::ReadDouble(xml, "damage_per_fall_unit", damage_per_fall_unit);

  // Character options
  const xmlNode* character_xml = XmlReader::GetMarker(xml, "character");
  if (character_xml != NULL)
  {
    const xmlNode* item = XmlReader::GetMarker(character_xml, "energy");
    if (item != NULL) {
      XmlReader::ReadUintAttr(item, "initial", character.init_energy);
      XmlReader::ReadUintAttr(item, "maximum", character.max_energy);
      if (character.init_energy==0) character.init_energy = 1;
      if (character.max_energy==0) character.max_energy = 1;
    }
    XmlReader::ReadUint(character_xml, "mass", character.mass);
    XmlReader::ReadDouble(character_xml, "air_resist_factor", character.air_resist_factor);
    item = XmlReader::GetMarker(character_xml, "jump");
    if (item != NULL) {
      int angle_deg;
      XmlReader::ReadUintAttr(item, "strength", character.jump_strength);
      XmlReader::ReadIntAttr(item, "angle", angle_deg);
      character.jump_angle = static_cast<double>(angle_deg) * M_PI / 180;
    }

    item = XmlReader::GetMarker(character_xml, "super_jump");
    if (item != NULL) {
      int angle_deg;
      XmlReader::ReadUintAttr(item, "strength", character.super_jump_strength);
      XmlReader::ReadIntAttr(item, "angle", angle_deg);
      character.super_jump_angle = static_cast<double>(angle_deg) * M_PI / 180;
    }
    item = XmlReader::GetMarker(character_xml, "back_jump");
    if (item != NULL) {
      int angle_deg;
      XmlReader::ReadUintAttr(item, "strength", character.back_jump_strength);
      XmlReader::ReadIntAttr(item, "angle", angle_deg);
      character.back_jump_angle = static_cast<double>(angle_deg) * M_PI / 180;
    }
    XmlReader::ReadUint(character_xml, "walking_pause", character.walking_pause);
    const xmlNode* explosion = XmlReader::GetMarker(character_xml, "death_explosion");
    if (explosion != NULL)
      death_explosion_cfg.LoadXml(explosion);
  }

  // Barrel explosion
  const xmlNode* barrel_xml = XmlReader::GetMarker(xml, "barrel");
  if (barrel_xml != NULL) {
    const xmlNode* barrel_explosion = XmlReader::GetMarker(barrel_xml, "explosion");
    if (barrel_explosion != NULL)
      barrel_explosion_cfg.LoadXml(barrel_explosion);
  }

  //=== Weapons ===
  const xmlNode* weapons_xml = XmlReader::GetMarker(xml, "weapons");
  if (weapons_xml != NULL) {
    WeaponsList::LoadXml(weapons_xml);
  }

  // Bonus box explosion - must be loaded after the weapons.
  const xmlNode* bonus_box_xml = XmlReader::GetMarker(xml, "bonus_box");
  if (bonus_box_xml != NULL) {
    BonusBox::LoadXml(bonus_box_xml);

    const xmlNode* bonus_box_explosion = XmlReader::GetMarker(bonus_box_xml, "explosion");
    if (bonus_box_explosion != NULL)
      bonus_box_explosion_cfg.LoadXml(bonus_box_explosion);
  }

  // Medkit - reuses the bonus_box explosion.
  const xmlNode* medkit_xml = XmlReader::GetMarker(xml, "medkit");
  if (medkit_xml != NULL) {
    Medkit::LoadXml(medkit_xml);
  }

  return true;
}

bool GameMode::Load(void)
{
  Config * config = Config::GetInstance();
  m_current = config->GetGameMode();

  LoadDefaultValues();

  // Game mode objects configuration file
  if(!doc_objects->Load(GetObjectsFilename()))
    return false;

  // Game mode file
  XmlReader doc;

  if(!doc.Load(GetFilename()))
    return false;
  if(!LoadXml(doc.GetRoot()))
    return false;

  return true;
}

// Load the game mode from strings (probably from network)
bool GameMode::LoadFromString(const std::string& game_mode_name,
                              const std::string& game_mode_contents,
                              const std::string& game_mode_objects_contents)
{
  m_current = game_mode_name;
  MSG_DEBUG("game_mode", "Loading %s from network: ", m_current.c_str());

  if(!doc_objects->LoadFromString(game_mode_objects_contents))
    return false;

  XmlReader doc;
  if(!doc.LoadFromString(game_mode_contents))
    return false;
  if(!LoadXml(doc.GetRoot()))
    return false;

  MSG_DEBUG("game_mode", "OK\n");
  return true;
}

bool GameMode::ExportFileToString(const std::string& filename, std::string& contents) const
{
  contents = "";

  XmlReader doc;
  if (!doc.Load(filename))
    return false;

  contents = doc.ExportToString();

  return true;
}

bool GameMode::ExportToString(std::string& mode,
                              std::string& mode_objects) const
{
  bool r;

  r = ExportFileToString(GetFilename(), mode);
  if (r) {
    r = ExportFileToString(GetObjectsFilename(), mode_objects);
  }

  return r;
}

const XmlReader* GameMode::GetXmlObjects() const
{
  return doc_objects;
}


bool GameMode::AllowCharacterSelection() const
{
  switch (allow_character_selection)
  {
  case GameMode::ALWAYS:
    break;

  case GameMode::BEFORE_FIRST_ACTION:
    return (Game::GetInstance()->ReadState() == Game::PLAYING) && !Game::GetInstance()->IsCharacterAlreadyChosen();

  case GameMode::NEVER:
    return false;
  }

  return true;
}

std::string GameMode::GetFilename() const
{
  Config * config = Config::GetInstance();
  std::string filename = std::string("game_mode" PATH_SEPARATOR)
    + m_current
    + std::string(".xml");

  std::string fullname = config->GetPersonalDataDir() + filename;

  if(!DoesFileExist(fullname))
    fullname = config->GetDataDir() + filename;

  if(!DoesFileExist(fullname)) {
    Error(Format("Can not find file %s", fullname.c_str()));
  }

  return fullname;
}

std::string GameMode::GetDefaultObjectsFilename() const
{
  std::string filename =
    std::string("game_mode" PATH_SEPARATOR "default_objects.xml");

  return filename;
}

std::string GameMode::GetObjectsFilename() const
{
  Config * config = Config::GetInstance();
  std::string filename = std::string("game_mode" PATH_SEPARATOR)
    + m_current
    + std::string("_objects.xml");

  std::string fullname = config->GetPersonalDataDir() + filename;

  if(!DoesFileExist(fullname))
    fullname = config->GetDataDir() + filename;

  if(!DoesFileExist(fullname)) {
    fprintf(stderr, "Game mode: File %s does not exist, use the default one instead.\n",
	    fullname.c_str());
  }

  fullname = config->GetDataDir() + GetDefaultObjectsFilename();
  if (!DoesFileExist(fullname)) {
    Error(Format("Can not find file %s", fullname.c_str()));
  }

  return fullname;
}

// Static method
std::vector<std::pair<std::string, std::string> > GameMode::ListGameModes()
{
  std::vector<std::pair<std::string, std::string> > game_modes;
  game_modes.push_back(std::pair<std::string, std::string>("classic", _("Classic")));
  game_modes.push_back(std::pair<std::string, std::string>("unlimited", _("Unlimited")));
  game_modes.push_back(std::pair<std::string, std::string>("blitz", _("Blitz")));
#ifdef DEBUG
  game_modes.push_back(std::pair<std::string, std::string>("skin_viewer", "Skin Viewer"));
#endif

  std::string personal_dir = Config::GetInstance()->GetPersonalDataDir() +
    std::string("game_mode" PATH_SEPARATOR);

  FolderSearch *f = OpenFolder(personal_dir);
  if (f) {
    const char *name;
    while ((name = FolderSearchNext(f)) != NULL) {
      std::string filename(name);

      if (filename.size() >= 5
	  && filename.compare(filename.size()-4, 4, ".xml") == 0
	  && (filename.size() < 12
	      || filename.compare(filename.size()-12, 12, "_objects.xml") != 0)) {

	std::string game_mode_name = filename.substr(0, filename.size()-4);
	game_modes.push_back(std::pair<std::string, std::string>(game_mode_name, game_mode_name));
      }
    }
    CloseFolder(f);
  }

  return game_modes;
}
