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
 * Define all Warmux actions.
 *****************************************************************************/

#ifndef WARMUX_ACTION_H
#define WARMUX_ACTION_H
//-----------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <cstddef>

#include <WARMUX_types.h>
#include <WARMUX_point.h>

//-----------------------------------------------------------------------------
// Forward declaration
class DistantComputer;
class EulerVector;

//-----------------------------------------------------------------------------

class Action
{
public:
  typedef enum
  {
    ACTION_NETWORK_CLIENT_CHANGE_STATE = 0,
    ACTION_NETWORK_MASTER_CHANGE_STATE,
    ACTION_NETWORK_CHECK_PHASE1,
    ACTION_NETWORK_CHECK_PHASE2,
    ACTION_NETWORK_DISCONNECT_ON_ERROR,
    ACTION_NETWORK_SET_GAME_MASTER,

    // ########################################################
    // To be sure that rules will be the same on each computer
    ACTION_RULES_SET_GAME_MODE,

    // ########################################################
    ACTION_ANNOUNCE_PAUSE,

    // Initial information about the game: map, teams already selected, ...
    ACTION_CHAT_MENU_MESSAGE,
    // Chat message
    ACTION_CHAT_INGAME_MESSAGE,

    ACTION_GAME_INFO,

    // Map selection in network menu
    ACTION_GAME_SET_MAP,
    ACTION_GAME_SET_MAP_LIST,
    ACTION_GAME_FORCE_MAP_LIST,

    // Teams selection in network menu
    ACTION_GAME_ADD_TEAM,
    ACTION_GAME_DEL_TEAM,
    ACTION_GAME_UPDATE_TEAM,

    ACTION_GAME_REQUEST_TEAM,
    ACTION_GAME_REQUEST_TEAM_REMOVAL,

    ACTION_GAME_SET_TEAM_LIST,
    ACTION_GAME_FORCE_TEAM_LIST,

    // Other interactions
    ACTION_INFO_CLIENT_CONNECT,
    ACTION_INFO_CLIENT_DISCONNECT,
 
    ACTION_NETWORK_RANDOM_INIT,
    LAST_FRAME_LESS_ACTION = ACTION_NETWORK_RANDOM_INIT,

    // All action below must be executed within their physics frame.
    // ########################################################
    // ########################################################
    ACTION_GAME_CALCULATE_FRAME,
    ACTION_GAME_PACK_CALCULATED_FRAMES,
    ACTION_PLAYER_CHANGE_WEAPON,
    ACTION_PLAYER_CHANGE_CHARACTER,

    // ########################################################
    // Character's move
    ACTION_CHARACTER_JUMP,
    ACTION_CHARACTER_HIGH_JUMP,
    ACTION_CHARACTER_BACK_JUMP,
    ACTION_CHARACTER_START_MOVING_LEFT,
    ACTION_CHARACTER_STOP_MOVING_LEFT,
    ACTION_CHARACTER_START_MOVING_RIGHT,
    ACTION_CHARACTER_STOP_MOVING_RIGHT,
    ACTION_CHARACTER_START_MOVING_UP,
    ACTION_CHARACTER_STOP_MOVING_UP,
    ACTION_CHARACTER_START_MOVING_DOWN,
    ACTION_CHARACTER_STOP_MOVING_DOWN,

    // ########################################################
    // Quite standard weapon options
    ACTION_WEAPON_START_SHOOTING,
    ACTION_WEAPON_STOP_SHOOTING,
    ACTION_WEAPON_SET_TIMEOUT,
    ACTION_WEAPON_SET_TARGET,
    ACTION_WEAPON_START_MOVING_LEFT,
    ACTION_WEAPON_STOP_MOVING_LEFT,
    ACTION_WEAPON_START_MOVING_RIGHT,
    ACTION_WEAPON_STOP_MOVING_RIGHT,
    ACTION_WEAPON_START_MOVING_UP,
    ACTION_WEAPON_STOP_MOVING_UP,
    ACTION_WEAPON_START_MOVING_DOWN,
    ACTION_WEAPON_STOP_MOVING_DOWN,


    // Special weapon options
    ACTION_WEAPON_CONSTRUCTION,

    // Bonus Box
    ACTION_DROP_BONUS_BOX,
    ACTION_REQUEST_BONUS_BOX_DROP,

    // ########################################################
    ACTION_NETWORK_PING,
    ACTION_NETWORK_VERIFY_RANDOM_SYNC,
    ACTION_TIME_VERIFY_SYNC,
    // ########################################################
    ACTION_END_ENUM
  } Action_t;
  static const uint NUM_ACTIONS = ACTION_END_ENUM;
  static const uint MAX_FRAMES  = 10;

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
  typedef struct
  {
    // Write: length of the actual data
    uint32_t len;
    uint8_t  type;
  } Header;
#pragma pack(pop)   /* restore original alignment from stack */

private:
  uint8_t *m_var, *m_write, *m_read;
  uint     m_bufsize;
  Header   m_header;

  DistantComputer* m_creator;

  void Resize(uint n);
  void Increase(uint n = 20) { Resize(m_bufsize+n); }
  uint MemWriteLeft() const { return m_bufsize - (m_write - m_var); }
  uint MemReadLeft() const { return m_header.len - (m_read - m_var); }
  void Init(Action_t type);

public:

  // Action without parameter
  Action(Action_t type);

  // Action with various parameter
  Action(Action_t type, int32_t value);
  Action(Action_t type, Double value);
  Action(Action_t type, const std::string& value);
  Action(Action_t type, Double value1, int32_t value2);
  Action(Action_t type, Double value1, Double value2);

  // Build an action from a network packet
  Action(const char* buffer, DistantComputer* _creator);

  ~Action();

  // Push / Back functions to add / retreive datas
  // Work as a FIFO container, inspiteof the name of methods !
  void Push(int32_t val);
  void Push(Double val);
  void Push(const std::string& val);
  void Push(const Point2i& val);
  void Push(const Point2d& val);
  void Push(const EulerVector& val);

  int PopInt();
  Double PopDouble();
  std::string PopString();
  Point2i PopPoint2i();
  Point2d PopPoint2d();
  EulerVector PopEulerVector();

  int  GetSize() const
  {
    return sizeof(Header) + m_header.len;
  }
  void Write(char *packet) const;
  void WriteToPacket(char* & packet, int & size) const;

  DistantComputer* GetCreator() const { return m_creator; }
  void SetCreator(DistantComputer* creator) { m_creator = creator; }
  Action_t GetType() const { return (Action_t)m_header.type; }
  bool IsFrameLess() const { return m_header.type <= LAST_FRAME_LESS_ACTION; }

  static Action_t GetType(const char *buffer) { return (Action_t)buffer[offsetof(Header, type)]; }
};

#endif
