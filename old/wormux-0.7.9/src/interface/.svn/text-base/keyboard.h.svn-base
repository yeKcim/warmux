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
 * Keyboard managment. Use ClanLIB event.
 *****************************************************************************/

#ifndef KEYBOARD_H
#define KEYBOARD_H
//-----------------------------------------------------------------------------
#include <map>
#include <SDL.h>
#include "../include/action.h"
#include "../include/base.h"
//-----------------------------------------------------------------------------

class Keyboard
{
private:
  std::map<int, Action::Action_t> layout;
  bool PressedKeys[Action::ACTION_LAST];

  static Keyboard * singleton;

private:
  // Traite une touche relach�e
  void HandleKeyPressed (const Action::Action_t &action);
  void HandleKeyReleased (const Action::Action_t &action);


public:
  Keyboard();
  typedef enum
  {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_REFRESH
  } Key_Event_t ;

  void HandleKeyEvent( const SDL_Event *event) ;
  void Reset();

  // On veut bouger la cam�ra au clavier ?
  void TestCamera();

  // Refresh des touches du clavier
  void Refresh();

  // Associe une touche � une action.
  void SetKeyAction(int key, Action::Action_t at);

  // Get the key associated to an action.
  int GetKeyAssociatedToAction(Action::Action_t at);

};

//-----------------------------------------------------------------------------
#endif
