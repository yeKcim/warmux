/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Wormux configuration : all main configuration variables have there default
 * value here. They should all be modifiable using the xml config file
 *****************************************************************************/

#include "game_mode.h"
#include <iostream>
#include "config.h"
#include "game_loop.h"
#include "../tool/file_tools.h"
#include "../tool/i18n.h"
#include "../weapon/all.h"
#include "../weapon/weapons_list.h"

GameMode * GameMode::singleton = NULL;

GameMode * GameMode::GetInstance() {
  if (singleton == NULL) {
    singleton = new GameMode();
  }
  return singleton;
}

GameMode::GameMode()
{
  max_characters = 6;
  max_teams = 4;
  duration_turn = 60;
  duration_exchange_player = 2;
  duration_before_death_mode = 20 * 60;
  damage_per_turn_during_death_mode = 5;
  gravity = 9.81;
  safe_fall = 10;
  damage_per_fall_unit = 7;
  duration_move_player = 3;
  allow_character_selection = BEFORE_FIRST_ACTION_AND_END_TURN;
  character.init_energy = 100;
  character.max_energy = 200;
  character.mass = 100;
  character.air_resist_factor = 1.0;
  character.jump_strength = 8;
  character.jump_angle = -60;
  character.super_jump_strength = 11;
  character.super_jump_angle = -80;
  character.back_jump_strength = 9;
  character.back_jump_angle = -100;
}

// Load data options from the selected game_mode
bool GameMode::LoadXml(xmlpp::Element *xml)
{
  std::string txt;
  if (LitDocXml::LitString (xml, "allow_character_selection", txt)) 
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

  LitDocXml::LitUint (xml, "duration_turn", duration_turn);
  LitDocXml::LitUint (xml, "duration_move_player", duration_move_player);
  LitDocXml::LitUint (xml, "duration_exchange_player", duration_exchange_player);
  LitDocXml::LitUint (xml, "duration_before_death_mode", duration_before_death_mode);
  LitDocXml::LitUint (xml, "damage_per_turn_during_death_mode", damage_per_turn_during_death_mode);
  LitDocXml::LitUint (xml, "max_teams", max_teams);
  LitDocXml::LitUint (xml, "max_characters", max_characters);
  LitDocXml::LitDouble (xml, "gravity", gravity);
  LitDocXml::LitDouble (xml, "safe_fall", safe_fall);
  LitDocXml::LitDouble (xml, "damage_per_fall_unit", damage_per_fall_unit);

  // Character options
  xmlpp::Element *xmlver = LitDocXml::AccesBalise (xml, "character");
  if (xmlver != NULL)
  {
    xmlpp::Element *item = LitDocXml::AccesBalise (xmlver, "energy");
    if (item != NULL) {
      LitDocXml::LitAttrUint (item, "initial", character.init_energy);
      LitDocXml::LitAttrUint (item, "maximum", character.max_energy);
      if (character.init_energy==0) character.init_energy = 1;
      if (character.max_energy==0) character.max_energy = 1;
    }
    LitDocXml::LitUint (xmlver, "mass", character.mass);
    LitDocXml::LitDouble (xmlver, "air_resist_factor", character.air_resist_factor);
    item = LitDocXml::AccesBalise (xmlver, "jump");
    if (item != NULL) {
      LitDocXml::LitAttrUint (item, "strength", character.jump_strength);
      LitDocXml::LitAttrInt  (item, "angle", character.jump_angle);
    }

    item = LitDocXml::AccesBalise (xmlver, "super_jump");
    if (item != NULL) {
      LitDocXml::LitAttrUint (item, "strength", character.super_jump_strength);
      LitDocXml::LitAttrInt  (item, "angle", character.super_jump_angle);
    }
    item = LitDocXml::AccesBalise (xmlver, "back_jump");
    if (item != NULL) {
      LitDocXml::LitAttrUint (item, "strength", character.back_jump_strength);
      LitDocXml::LitAttrInt  (item, "angle", character.back_jump_angle);
    }
  }

  //=== Weapons ===
  xmlpp::Element *armes = LitDocXml::AccesBalise (xml, "weapons");
  if (armes != NULL)
  {
    std::list<Weapon*> l_weapons_list = weapons_list.GetList() ;
    std::list<Weapon*>::iterator 
      itw = l_weapons_list.begin(), 
      end = l_weapons_list.end();

    for (; itw != end ; ++itw) (*itw)->LoadXml(armes);
  }

  return true;
}

bool GameMode::Load(const std::string &mode)
{
  if (mode == m_current) return true;
  m_current = mode;

  std::string fullname;
  try
  {
    LitDocXml doc;
    std::string filename = 
      PATH_SEPARATOR
      + std::string("game_mode") 
      + std::string(PATH_SEPARATOR)
      + mode 
      + std::string(".xml");

    Config * config = Config::GetInstance();
    fullname = config->GetPersonalDir() + filename;

    if( !IsFileExist(fullname) )
      fullname = config->GetDataDir() + filename;
    if( !doc.Charge(fullname) )
      return false;
    if( !LoadXml (doc.racine()) )
      return false;
  }
  catch (const xmlpp::exception &e)
  {
    std::cerr << Format(_("Error while loading game mode %s (file %s):"), 
                        mode.c_str(), fullname.c_str())
			  << std::endl << e.what() << std::endl;
    return false;
  }
  return true;
}

bool GameMode::AllowCharacterSelection() const
{
  switch (allow_character_selection)
  {
  case GameMode::ALWAYS: break;

  case GameMode::BEFORE_FIRST_ACTION:
  case GameMode::BEFORE_FIRST_ACTION_AND_END_TURN:
	  return (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING) && !GameLoop::GetInstance()->character_already_chosen;

  case GameMode::CHANGE_ON_END_TURN:
  case GameMode::NEVER:
	  return false;
  }

  return true;
}

