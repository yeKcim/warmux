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
 * Arrow on top of the active character
 *****************************************************************************/

#ifndef CURSOR_H
#define CURSOR_H
//-----------------------------------------------------------------------------
#include "graphic/sprite.h"
#include "object/physical_obj.h"
#include "include/base.h"
//-----------------------------------------------------------------------------

class CharacterCursor
{
public:
   Sprite *image;

private:
  /* If you need this, implement it (correctly)*/
  CharacterCursor(const CharacterCursor&);
  const CharacterCursor& operator=(const CharacterCursor&);
  /*********************************************/

  uint time_begin_anim;
  uint last_update;
  PhysicalObj *obj_designe;
  int dy;
  bool actif;
  bool want_hide;

  static CharacterCursor * singleton;

public:
  static CharacterCursor * GetInstance();

  void Reset();
  void Draw();
  void Refresh();

  // Pointe un objet
  void PointeObj (PhysicalObj *obj);

  // Pointe un objet
  void FollowActiveCharacter ();

  // Hide the cursor
  void Hide();



private:
  CharacterCursor();
  ~CharacterCursor();

  bool IsDisplayed() const;
};

//-----------------------------------------------------------------------------
#endif
