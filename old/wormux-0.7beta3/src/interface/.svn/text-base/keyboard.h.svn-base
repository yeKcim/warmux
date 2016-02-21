/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include "../include/base.h"
#include <SDL.h>
#include <map>
#include "../include/action.h"
//-----------------------------------------------------------------------------

class Clavier
{
private:
  std::map<int, Action_t> layout;
  bool PressedKeys[ACTION_MAX];

private:
  // Traite une touche relachée
  void HandleKeyPressed (const Action_t &action);
  void HandleKeyReleased (const Action_t &action);
public:
  void HandleKeyEvent( const SDL_Event *event) ;
  Clavier();
  void Reset();

  // On veut bouger la caméra au clavier ?
  void TestCamera();

  // Refresh des touches du clavier
  void Refresh();
  
  // Associe une touche à une action.
  void SetKeyAction(int key, Action_t at);

};

extern Clavier clavier;

//-----------------------------------------------------------------------------
#endif
