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
 * Keyboard managment
 *****************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H
//-----------------------------------------------------------------------------
#include <map>
#include <SDL.h>
#include "include/action.h"
#include "include/base.h"
//-----------------------------------------------------------------------------

class Keyboard
{
public:  
  typedef enum
  {
    KEY_QUIT,        KEY_WEAPONS1,                KEY_WEAPONS2,
    KEY_WEAPONS3,    KEY_WEAPONS4,                KEY_WEAPONS5,
    KEY_WEAPONS6,    KEY_WEAPONS7,                KEY_WEAPONS8,
    KEY_PAUSE,       KEY_FULLSCREEN,              KEY_TOGGLE_INTERFACE,
    KEY_CENTER,      KEY_TOGGLE_WEAPONS_MENUS,    KEY_CHAT,
    KEY_MOVE_LEFT,   KEY_MOVE_RIGHT,              KEY_UP,   KEY_DOWN, 
    KEY_JUMP,        KEY_HIGH_JUMP,               KEY_BACK_JUMP, 
    KEY_SHOOT,       KEY_CHANGE_WEAPON,           
    KEY_WEAPON_1,    KEY_WEAPON_2,                KEY_WEAPON_3,
    KEY_WEAPON_4,    KEY_WEAPON_5,                KEY_WEAPON_6,
    KEY_WEAPON_7,    KEY_WEAPON_8,                KEY_WEAPON_9,
    KEY_WEAPON_LESS, KEY_WEAPON_MORE,             
    KEY_NEXT_CHARACTER,
  } Key_t;

private:

  typedef enum
  {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_REFRESH
  } Key_Event_t ; 

  Keyboard();
  static Keyboard * singleton;

  std::map<int, Key_t> layout;
  bool PressedKeys[256]; // stupid default value

  void HandleKeyPressed (const Key_t &action_key);
  void HandleKeyReleased (const Key_t &action_key);

  void SetKeyAction(int key, Key_t at);

public:
  static Keyboard * GetInstance();

  void HandleKeyEvent(const SDL_Event& event) ;
  void Reset();

  // Refresh keys 
  void Refresh();

  // Get the key associated to an action.
  int GetKeyAssociatedToAction(Key_t at);
};

//-----------------------------------------------------------------------------
#endif
