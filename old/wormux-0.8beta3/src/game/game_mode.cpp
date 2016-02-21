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
 * Wormux configuration : all main configuration variables have there default
 * value here. They should all be modifiable using the xml config file
 *****************************************************************************/

#include "game/game_mode.h"
#include <iostream>
#include "game/config.h"
#include "game/game.h"
#include "object/medkit.h"
#include "object/bonus_box.h"
#include "tool/file_tools.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"
#include "weapon/weapons_list.h"

GameMode * GameMode::singleton = NULL;

GameMode * GameMode::GetInstance() {
  if (singleton == NULL) {
    singleton = new GameMode();
  }
  return singleton;
}

GameMode::GameMode():
  nb_characters(6),
  max_teams(4),
  duration_turn(60),
  duration_move_player(3),
  duration_exchange_player(2),
  duration_before_death_mode(20 * 60),
  damage_per_turn_during_death_mode(5),
  gravity(9.81),
  safe_fall(10),
  damage_per_fall_unit(7),
  death_explosion_cfg(),
  barrel_explosion_cfg(),
  bonus_box_explosion_cfg(),
  character(),
  allow_character_selection(BEFORE_FIRST_ACTION_AND_END_TURN),
  m_current("classic"),
  doc_objects(new XmlReader)
{
  character.init_energy = 100; /* overwritten when reading XML */
  character.max_energy = 100; /* overwritten when reading XML */
  character.mass = 100;
  character.air_resist_factor = 1.0;
  character.jump_strength = 8;
  character.jump_angle = -60;
  character.super_jump_strength = 11;
  character.super_jump_angle = -80;
  character.back_jump_strength = 9;
  character.back_jump_angle = -100;
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
bool GameMode::LoadXml(const xmlpp::Element *xml)
{
  std::string txt;
  if (XmlReader::ReadString(xml, "allow_character_selection", txt))
  {
    if (txt == "always")
      allow_character_selection = ALWAYS;
    else if (txt == "never")
      allow_character_selection = NEVER;
    else if (txt == "change_on_end_turn")
      allow_character_selection = CHANGE_ON_END_TURN;
    else if (txt == "before_first_action_and_end_turn")
      allow_character_selection = BEFORE_FIRST_ACTION_AND_END_TURN;
    else if (txt == "before_first_action")
      allow_character_selection = BEFORE_FIRST_ACTION;
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
  xmlpp::Element *character_xml = XmlReader::GetMarker(xml, "character");
  if (character_xml != NULL)
  {
    xmlpp::Element *item = XmlReader::GetMarker(character_xml, "energy");
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
    xmlpp::Element *explosion = XmlReader::GetMarker(character_xml, "death_explosion");
    if (explosion != NULL)
      death_explosion_cfg.LoadXml(explosion);
  }

  // Barrel explosion
  xmlpp::Element *barrel_xml = XmlReader::GetMarker(xml, "barrel");
  if(barrel_xml != NULL) {
    xmlpp::Element *barrel_explosion = XmlReader::GetMarker(barrel_xml, "explosion");
    if (barrel_explosion != NULL)
      barrel_explosion_cfg.LoadXml(barrel_explosion);
  }

  //=== Weapons ===
  xmlpp::Element *weapons_xml = XmlReader::GetMarker(xml, "weapons");
  if (weapons_xml != NULL)
  {
    std::list<Weapon*> l_weapons_list = WeaponsList::GetInstance()->GetList() ;
    std::list<Weapon*>::iterator
      itw = l_weapons_list.begin(),
      end = l_weapons_list.end();

    for (; itw != end ; ++itw) (*itw)->LoadXml(weapons_xml);
  }

  // Bonus box explosion - must be loaded after the weapons.
  xmlpp::Element *bonus_box_xml = XmlReader::GetMarker(xml, "bonus_box");
  if(bonus_box_xml != NULL) {
    BonusBox::LoadXml(bonus_box_xml);

    xmlpp::Element *bonus_box_explosion = XmlReader::GetMarker(bonus_box_xml, "explosion");
    if (bonus_box_explosion != NULL)
      bonus_box_explosion_cfg.LoadXml(bonus_box_explosion);
  }

  // Medkit - reuses the bonus_box explosion.
  xmlpp::Element *medkit_xml = XmlReader::GetMarker(xml, "medkit");
  if(medkit_xml != NULL) {
    Medkit::LoadXml(medkit_xml);
  }

  return true;
}

bool GameMode::Load(void)
{
  std::string fullname;
  Config * config = Config::GetInstance();
  m_current = config->GetGameMode();
  try
  {
    // Game mode objects configuration file
    fullname = config->GetPersonalDir() + GetObjectsFilename();

    if(!IsFileExist(fullname))
      fullname = config->GetDataDir() + GetObjectsFilename();

    if(!IsFileExist(fullname)) {
      Error(Format("Can not find file %s\n", fullname.c_str()));
      return false;
    }

    if(!doc_objects->Load(fullname))
      return false;
    MSG_DEBUG("game_mode", "successful loading of %s\n", fullname.c_str());

    // Game mode file
    XmlReader doc;
    fullname = config->GetPersonalDir() + GetFilename();

    if(!IsFileExist(fullname))
      fullname = config->GetDataDir() + GetFilename();

    if(!IsFileExist(fullname)) {
      Error(Format("Can not find file %s\n", fullname.c_str()));
      return false;
    }

    if(!doc.Load(fullname))
      return false;
    if(!LoadXml(doc.GetRoot()))
      return false;

    MSG_DEBUG("game_mode", "successful loading of %s\n", fullname.c_str());
  }
  catch (const xmlpp::exception &e)
  {
    std::cerr << Format(_("Error while loading game mode %s (file %s):"),
                        m_current.c_str(), fullname.c_str())
              << std::endl << e.what() << std::endl;
    return false;
  }

  return true;
}

// Load the game mode from strings (probably from network)
bool GameMode::LoadFromString(const std::string& game_mode_name,
                              const std::string& game_mode_contents,
                              const std::string& game_mode_objects_contents)
{
  m_current = game_mode_name;
  MSG_DEBUG("game_mode", "Loading %s from network: ", m_current.c_str());

  try
  {
    if(!doc_objects->LoadFromString(game_mode_objects_contents))
      return false;

    XmlReader doc;
    if(!doc.LoadFromString(game_mode_contents))
      return false;
    if(!LoadXml(doc.GetRoot()))
      return false;
  }
  catch (const xmlpp::exception &e)
  {
    Error(Format("Error while loading game mode %s from memory:\n\t%s",
                 m_current.c_str(), e.what()));
    return false;
  }

  MSG_DEBUG("game_mode", "OK\n");
  return true;
}

bool GameMode::ExportFileToString(const std::string& filename, std::string& contents) const
{
  std::string fullname;
  contents = "";

  try
  {
    XmlReader doc;
    fullname = Config::GetInstance()->GetPersonalDir() + filename;

    if (!IsFileExist(fullname))
      fullname = Config::GetInstance()->GetDataDir() + filename;
    if (!doc.Load(fullname))
      return false;

    contents = doc.ExportToString();
  }
  catch (const xmlpp::exception &e)
  {
    Error(Format("Error while exporting file %s:\n\t%s",
                 fullname.c_str(), e.what()));
    return false;
  }
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
  case GameMode::ALWAYS: break;

  case GameMode::BEFORE_FIRST_ACTION:
  case GameMode::BEFORE_FIRST_ACTION_AND_END_TURN:
          return (Game::GetInstance()->ReadState() == Game::PLAYING) && !Game::GetInstance()->character_already_chosen;

  case GameMode::CHANGE_ON_END_TURN:
  case GameMode::NEVER:
          return false;
  }

  return true;
}

std::string GameMode::GetFilename() const
{
  std::string filename =
    PATH_SEPARATOR
    + std::string("game_mode")
    + std::string(PATH_SEPARATOR)
    + m_current
    + std::string(".xml");

  return filename;
}

std::string GameMode::GetObjectsFilename() const
{
  std::string filename =
    PATH_SEPARATOR
    + std::string("game_mode")
    + std::string(PATH_SEPARATOR)
    + m_current
    + std::string("_objects.xml");

  return filename;
}


