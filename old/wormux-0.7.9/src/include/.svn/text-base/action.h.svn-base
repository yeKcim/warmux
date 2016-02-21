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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef ACTION_H
#define ACTION_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <string>
#include <iostream>
#include <list>
#include "base.h"
#include "../tool/point.h"

//-----------------------------------------------------------------------------

class Action
{
private:
  std::list<Uint32> var;
  Action ( const Action& an_action);
public:
  typedef enum
  {
    // Game action.
    ACTION_MOVE_LEFT,   ACTION_MOVE_RIGHT, ACTION_UP,   ACTION_DOWN, ACTION_JUMP,
    ACTION_HIGH_JUMP,   ACTION_BACK_JUMP, ACTION_SHOOT, ACTION_CHANGE_WEAPON,
    ACTION_WIND,        ACTION_NEXT_CHARACTER,          ACTION_CHANGE_CHARACTER,
    ACTION_CHANGE_TEAM, ACTION_SET_SKIN,                ACTION_SYNC_BEGIN,
    ACTION_SYNC_END,    ACTION_EXPLOSION,               ACTION_SUPERTUX_STATE,
    ACTION_WEAPON_1,    ACTION_WEAPON_2,                ACTION_WEAPON_3,
    ACTION_WEAPON_4,    ACTION_WEAPON_5,                ACTION_WEAPON_6,
    ACTION_WEAPON_7,    ACTION_WEAPON_8,                ACTION_WEAPON_9,
    ACTION_WEAPON_MORE, ACTION_WEAPON_LESS,             ACTION_SET_TARGET,
    ACTION_SET_TIMEOUT, ACTION_CONSTRUCTION_UP,         ACTION_CONSTRUCTION_DOWN,
    ACTION_SET_CHARACTER_ENERGY, ACTION_WEAPON_STOP_USE,

    // Game initialisation
    ACTION_PING,        ACTION_SET_GAME_MODE,           ACTION_SET_MAP,
    ACTION_UPDATE_TEAM, ACTION_NEW_TEAM,                ACTION_DEL_TEAM,
    ACTION_ASK_VERSION, ACTION_ASK_TEAM,                ACTION_SEND_VERSION,
    ACTION_SEND_TEAM,   ACTION_SEND_RANDOM,             ACTION_CHAT_MESSAGE,
    ACTION_NICKNAME,    ACTION_SET_CHARACTER_PHYSICS,   ACTION_SET_CHARACTER_DIRECTION,
    ACTION_CHANGE_STATE,

    // Out of game actions (local only).
    ACTION_QUIT,        ACTION_WEAPONS1,                ACTION_WEAPONS2,
    ACTION_WEAPONS3,    ACTION_WEAPONS4,                ACTION_WEAPONS5,
    ACTION_WEAPONS6,    ACTION_WEAPONS7,                ACTION_WEAPONS8,
    ACTION_PAUSE,       ACTION_FULLSCREEN,              ACTION_TOGGLE_INTERFACE,
    ACTION_CENTER,      ACTION_TOGGLE_WEAPONS_MENUS,    ACTION_CHAT,
  } Action_t;

  static const Action_t ACTION_FIRST = ACTION_MOVE_LEFT; /* keep this as the first name in enum */
  static const Action_t ACTION_LAST  = ACTION_CHAT; /* keep this as the last name in enum */

  //inline Action_t &operator++() { ;}

  // Action without parameter
  Action (Action_t type);
  // Action with various parameter
  Action (Action_t type, int value);
  Action (Action_t type, double value);
  Action (Action_t type, double value1, int value2);
  Action (Action_t type, double value1, double value2);
  Action (Action_t type, const std::string& value);

  // Build an action from a network packet
  Action (const char* is);

  ~Action();

  std::ostream& out(std::ostream &os) const;
  // Push / Back functions to add / retreive datas
  // Work as a FIFO container, inspiteof the name of methods !
  void Push(int val);
  void Push(double val);
  void Push(std::string val);
  void Push(const Point2i& val);
  void Push(const Point2d& val);
  int PopInt();
  double PopDouble();
  std::string PopString();
  Point2i PopPoint2i();
  Point2d PopPoint2d();
  bool IsEmpty() const;

  // Store character's information
  void StoreCharacter(uint team_no, uint char_no);
  void StoreActiveCharacter();
  void RetrieveCharacter();

  // Timestamp handling
  void SetTimestamp(uint timestamp);
  uint GetTimestamp();

  void WritePacket(char* & packet, int & size);
  Action_t GetType() const;
protected:
  Action_t m_type;
  uint m_timestamp;
};

//-----------------------------------------------------------------------------
// Output action in a ostream (for debug)
std::ostream& operator<<(std::ostream& os, const Action &a);

//-----------------------------------------------------------------------------
#endif
