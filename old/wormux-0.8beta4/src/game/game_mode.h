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
 * Game mode : duration of a turn, weapons configurations, max number of character
 * per team, etc.
 *****************************************************************************/

#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <string>
#include "include/base.h"
#include "weapon/weapon_cfg.h"

namespace xmlpp
{
  class Element;
}
class XmlReader;

class GameMode
{
public:
  uint nb_characters;
  uint max_teams;
  uint duration_turn;
  uint duration_move_player;
  uint duration_exchange_player;
  uint duration_before_death_mode;
  uint damage_per_turn_during_death_mode;
  double gravity;
  double safe_fall ;
  double damage_per_fall_unit ;
  ExplosiveWeaponConfig death_explosion_cfg;
  ExplosiveWeaponConfig barrel_explosion_cfg;
  ExplosiveWeaponConfig bonus_box_explosion_cfg;

  struct s_character
  {
    uint init_energy;
    uint max_energy;
    uint mass;
    double air_resist_factor;
    uint jump_strength;
    double jump_angle;
    uint super_jump_strength;
    double super_jump_angle;
    uint back_jump_strength;
    double back_jump_angle;
    uint walking_pause;
  } character;

  int allow_character_selection;

  enum {
    ALWAYS = 0,
    BEFORE_FIRST_ACTION,
    BEFORE_FIRST_ACTION_AND_END_TURN,
    CHANGE_ON_END_TURN,
    NEVER
  };
private:
  std::string m_current;

  XmlReader* doc_objects;

  bool LoadXml (const xmlpp::Element *xml);
  bool ExportFileToString(const std::string& filename, std::string& contents) const;

  std::string GetFilename() const;
  std::string GetObjectsFilename() const;

public:
  static void CleanUp() { if (singleton) delete singleton; singleton = NULL; };
  static GameMode * GetInstance();

  const std::string& GetName() const;

  bool Load(void);

  // mode: xml text of data/game_mode/<mode>.xml
  // mode_objects: xml text of data/game_mode/<mode>_objects.xml
  bool LoadFromString(const std::string& game_mode_name,
                      const std::string& mode,
                      const std::string& mode_objects);

  bool ExportToString(std::string& mode,
                      std::string& mode_objects) const;

  const XmlReader* GetXmlObjects() const; // for object_cfg

  bool AllowCharacterSelection() const;

private:
  static GameMode * singleton;
  GameMode();
  ~GameMode();

};

#endif /* GAME_MODE_H */
