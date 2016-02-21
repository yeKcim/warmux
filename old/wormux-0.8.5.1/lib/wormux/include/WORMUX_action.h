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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef WORMUX_ACTION_H
#define WORMUX_ACTION_H
//-----------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <list>
#include <WORMUX_distant_cpu.h>
#include <WORMUX_euler_vector.h>
#include <WORMUX_point.h>
#include <WORMUX_types.h>

//-----------------------------------------------------------------------------

class Action
{
public:
  typedef enum
  {
    ACTION_NETWORK_CLIENT_CHANGE_STATE,
    ACTION_NETWORK_MASTER_CHANGE_STATE,
    ACTION_NETWORK_CHECK_PHASE1,
    ACTION_NETWORK_CHECK_PHASE2,
    ACTION_NETWORK_DISCONNECT_ON_ERROR,
    ACTION_NETWORK_SET_GAME_MASTER,

    // ########################################################
    ACTION_PLAYER_CHANGE_WEAPON,
    ACTION_PLAYER_CHANGE_CHARACTER,
    ACTION_GAMELOOP_NEXT_TEAM,
    ACTION_GAMELOOP_SET_STATE,

    // ########################################################
    // To be sure that rules will be the same on each computer
    ACTION_RULES_SET_GAME_MODE,

    // ########################################################
    // Chat message
    ACTION_CHAT_MESSAGE,

    // Initial information about the game: map, teams already selected, ...
    ACTION_GAME_INFO,

    // Map selection in network menu
    ACTION_GAME_SET_MAP,

    // Teams selection in network menu
    ACTION_GAME_ADD_TEAM,
    ACTION_GAME_DEL_TEAM,
    ACTION_GAME_UPDATE_TEAM,

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
    ACTION_WEAPON_CONSTRUCTION,
    ACTION_WEAPON_GNU,
    ACTION_WEAPON_GRAPPLE,
    ACTION_WEAPON_POLECAT,
    ACTION_WEAPON_SUPERTUX,

    // Bonus Box
    ACTION_NEW_BONUS_BOX,
    ACTION_DROP_BONUS_BOX,

    // ########################################################
    ACTION_NETWORK_SYNC_BEGIN,
    ACTION_NETWORK_SYNC_END,
    ACTION_EXPLOSION,
    ACTION_WIND,
    ACTION_NETWORK_PING,
    ACTION_NETWORK_RANDOM_INIT,
    ACTION_INFO_CLIENT_CONNECT,
    ACTION_INFO_CLIENT_DISCONNECT,

    // ########################################################
  } Action_t;

private:
  /* if you need that, implement it (correctly)*/
  Action(const Action& an_action);
  const Action& operator=(const Action&);
  /*********************************************/

  std::list<uint32_t> var;
  Action_t m_type;
  uint m_timestamp;

  DistantComputer* creator;

  void Init (Action_t type);
  void WriteTo(char *packet) const;

public:

  // Action without parameter
  Action (Action_t type);

  // Action with various parameter
  Action (Action_t type, int value);
  Action (Action_t type, double value);
  Action (Action_t type, const std::string& value);
  Action (Action_t type, double value1, int value2);
  Action (Action_t type, double value1, double value2);

  // Build an action from a network packet
  Action (const char* buffer, DistantComputer* _creator);

  ~Action() { };

  // Push / Back functions to add / retreive datas
  // Work as a FIFO container, inspiteof the name of methods !
  void Push(int val);
  void Push(double val);
  void Push(const std::string& val);
  void Push(const Point2i& val);
  void Push(const Point2d& val);
  void Push(const EulerVector& val);

  int PopInt();
  double PopDouble();
  std::string PopString();
  Point2i PopPoint2i();
  Point2d PopPoint2d();
  EulerVector PopEulerVector();

  void WriteToPacket(char* & packet, int & size) const;

  bool IsEmpty() const;

  DistantComputer* GetCreator() const;
  int  GetSize() const;
  uint GetTimestamp() const;
  Action_t GetType() const;
};

// to be defined outside of the library
extern uint Action_TimeStamp();

#endif
