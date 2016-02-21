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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef ACTION_H
#define ACTION_H
//-----------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <list>
#include "base.h"
#include "tool/point.h"

class DistantComputer;
//-----------------------------------------------------------------------------

class Action
{
private:
  std::list<uint32_t> var;
  Action(const Action& an_action);
  const Action& operator=(const Action&);
public:
  typedef enum
  {
    ACTION_NICKNAME,
    ACTION_NETWORK_CHANGE_STATE,
    ACTION_NETWORK_CHECK_PHASE1,
    ACTION_NETWORK_CHECK_PHASE2,

    // ########################################################
    ACTION_PLAYER_CHANGE_WEAPON,
    ACTION_PLAYER_NEXT_CHARACTER,
    ACTION_PLAYER_PREVIOUS_CHARACTER,
    ACTION_GAMELOOP_CHANGE_CHARACTER,
    ACTION_GAMELOOP_NEXT_TEAM,
    ACTION_GAMELOOP_SET_STATE,

    // ########################################################
    // To be sure that rules will be the same on each computer
    ACTION_RULES_ASK_VERSION,
    ACTION_RULES_SEND_VERSION,
    ACTION_RULES_SET_GAME_MODE,

    // ########################################################
    // Chat message
    ACTION_CHAT_MESSAGE,

    // Map selection in network menu
    ACTION_MENU_SET_MAP,

    // Teams selection in network menu
    ACTION_MENU_ADD_TEAM,
    ACTION_MENU_DEL_TEAM,
    ACTION_MENU_UPDATE_TEAM,

    // ########################################################
    // Character's move
    ACTION_CHARACTER_JUMP,
    ACTION_CHARACTER_HIGH_JUMP,
    ACTION_CHARACTER_BACK_JUMP,

    ACTION_CHARACTER_SET_PHYSICS,

    // ########################################################
    // Using Weapon
    ACTION_WEAPON_SHOOT,
    ACTION_WEAPON_STOP_USE,

    // Quite standard weapon options
    ACTION_WEAPON_SET_TIMEOUT,
    ACTION_WEAPON_SET_TARGET,

    // Special weapon options
    ACTION_WEAPON_SUPERTUX,
    ACTION_WEAPON_CONSTRUCTION,
    ACTION_WEAPON_GRAPPLE,

    // Bonus Box
    ACTION_NEW_BONUS_BOX,

    // ########################################################
    ACTION_NETWORK_SYNC_BEGIN,
    ACTION_NETWORK_SYNC_END,
    ACTION_EXPLOSION,
    ACTION_WIND,
    ACTION_NETWORK_PING,
    ACTION_NETWORK_RANDOM_INIT,
    ACTION_NETWORK_RANDOM_ADD,
    ACTION_NETWORK_CONNECT,
    ACTION_NETWORK_DISCONNECT,

    // ########################################################
  } Action_t;

  DistantComputer* creator;

  //inline Action_t &operator++() { ;}

  // Action without parameter
  Action (Action_t type) { Init(type); };

  // Action with various parameter
  Action (Action_t type, int value) { Init(type); Push(value); };
  Action (Action_t type, double value) { Init(type); Push(value); };
  Action (Action_t type, const std::string& value) { Init(type); Push(value); };

  Action (Action_t type, double value1, int value2);
  Action (Action_t type, double value1, double value2);

  // Build an action from a network packet
  Action (const char* is, DistantComputer* _creator);

  ~Action() { };

  void Init (Action_t type);
  std::ostream& out(std::ostream &os) const;

  // Push / Back functions to add / retreive datas
  // Work as a FIFO container, inspiteof the name of methods !
  void Push(int val);
  void Push(double val);
  void Push(const std::string& val);
  void Push(const Point2i& val);
  void Push(const Point2d& val);

  int PopInt();
  double PopDouble();
  std::string PopString();
  Point2i PopPoint2i();
  Point2d PopPoint2d();

  bool IsEmpty() const { return var.empty(); };

  // Store character's information
  void StoreCharacter(uint team_no, uint char_no);
  void StoreActiveCharacter();
  void RetrieveCharacter();

  // Timestamp handling
  uint GetTimestamp() const { return m_timestamp; };

  int  GetSize() const
  {
    return 4  //Size of the type;
           + 4 //Size of the timestamp
           + 4 //Size of the number of variable
           + int(var.size()) * 4;
  }

  void Write(char *packet) const;
  void WritePacket(char* & packet, int & size) const;
  Action_t GetType() const { return m_type; };
protected:
  Action_t m_type;
  uint m_timestamp;
};

//-----------------------------------------------------------------------------
// Output action in a ostream (for debug)
std::ostream& operator<<(std::ostream& os, const Action &a);

//-----------------------------------------------------------------------------
#endif
