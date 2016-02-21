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
 * Wormux enums ... 
 *****************************************************************************/

#ifndef WORMUX_ENUM_H
#define WORMUX_ENUM_H
//-----------------------------------------------------------------------------

typedef enum
{
  WEAPON_BAZOOKA,
  WEAPON_AUTOMATIC_BAZOOKA,
  WEAPON_RIOT_BOMB,
  WEAPON_GRENADE,
  WEAPON_HOLLY_GRENADE,
  WEAPON_CLUSTER_BOMB,

  WEAPON_GUN,
  WEAPON_SHOTGUN,
  WEAPON_SUBMACHINE_GUN,
  WEAPON_BASEBALL,

  WEAPON_DYNAMITE,
  WEAPON_MINE,

  WEAPON_SUPERTUX,
  WEAPON_AIR_ATTACK,
  WEAPON_ANVIL,
  WEAPON_GNU,
  WEAPON_POLECAT,
  WEAPON_BOUNCE_BALL,

  WEAPON_TELEPORTATION,
  WEAPON_NINJA_ROPE,
  WEAPON_LOWGRAV,
  WEAPON_SUICIDE,
  WEAPON_SKIP_TURN,
  WEAPON_JETPACK,
  WEAPON_PARACHUTE,
  WEAPON_AIR_HAMMER,
  WEAPON_CONSTRUCT,
  WEAPON_SNIPE_RIFLE,
  WEAPON_BLOWTORCH,
  WEAPON_SYRINGE
} Weapon_type;

//-----------------------------------------------------------------------------

typedef enum
{
  // Game action.
  ACTION_MOVE_LEFT,
  ACTION_MOVE_RIGHT,
  ACTION_UP,
  ACTION_DOWN,
  ACTION_JUMP,
  ACTION_HIGH_JUMP,
  ACTION_BACK_JUMP,
  ACTION_SHOOT,
  ACTION_CHANGE_WEAPON,
  ACTION_WIND,
  ACTION_NEXT_CHARACTER,
  ACTION_CHANGE_CHARACTER,
  ACTION_CHANGE_TEAM,
  ACTION_SET_CLOTHE,
  ACTION_SET_MOVEMENT,
  ACTION_SET_FRAME,
  ACTION_SYNC_BEGIN,
  ACTION_SYNC_END,
  ACTION_EXPLOSION,
  ACTION_SUPERTUX_STATE,
  ACTION_WEAPON_1,
  ACTION_WEAPON_2,
  ACTION_WEAPON_3,
  ACTION_WEAPON_4,
  ACTION_WEAPON_5,
  ACTION_WEAPON_6,
  ACTION_WEAPON_7,
  ACTION_WEAPON_8,
  ACTION_WEAPON_9,
  ACTION_WEAPON_MORE,
  ACTION_WEAPON_LESS,
  ACTION_SET_TARGET,
  ACTION_SET_TIMEOUT,
  ACTION_CONSTRUCTION_UP,
  ACTION_CONSTRUCTION_DOWN,
  ACTION_SET_CHARACTER_ENERGY,
  ACTION_WEAPON_STOP_USE,

  // Game initialisation
  ACTION_SET_GAME_MODE,
  ACTION_SET_MAP,
  ACTION_CLEAR_TEAMS,
  ACTION_NEW_TEAM,
  ACTION_DEL_TEAM,
  ACTION_SET_CHARACTER_PHYSICS,
  ACTION_SET_CHARACTER_DIRECTION,
  ACTION_CHANGE_STATE,
  ACTION_ASK_VERSION,
  ACTION_ASK_TEAM,
  ACTION_SEND_VERSION,
  ACTION_SEND_TEAM,
  ACTION_SEND_RANDOM,
  ACTION_CHAT_MESSAGE,
  ACTION_NICKNAME,

  // Out of game actions (local only).
  ACTION_QUIT,
  ACTION_WEAPONS1,
  ACTION_WEAPONS2,
  ACTION_WEAPONS3,
  ACTION_WEAPONS4,
  ACTION_WEAPONS5,
  ACTION_WEAPONS6,
  ACTION_WEAPONS7,
  ACTION_WEAPONS8,
  ACTION_PAUSE,
  ACTION_FULLSCREEN,
  ACTION_TOGGLE_INTERFACE,
  ACTION_CENTER,
  ACTION_TOGGLE_WEAPONS_MENUS,
  ACTION_MAX,
  ACTION_CHAT,
} Action_t;

//-----------------------------------------------------------------------------

typedef enum
{
  KEY_PRESSED,
  KEY_RELEASED,
  KEY_REFRESH
} Key_Event_t ;

//-----------------------------------------------------------------------------
#endif
