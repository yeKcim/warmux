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
 * Mouse management
 *****************************************************************************/

#ifndef MOUSE_H
#define MOUSE_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../include/base.h"
#include "../tool/Point.h"
//-----------------------------------------------------------------------------

class Mouse
{
private:
  bool scroll_actif;
  int sauve_x, sauve_y;

public:
  void TraiteClic (const SDL_Event *event); 
  Mouse();

  void Reset();

  // Refresh de la souris à chaque boucle du jeu
  void Refresh();
  void TestCamera();
  bool ActionClicG();
  bool ActionClicD();
  void ChoixVerPointe();

  // Lit la position dans le monde
  int GetX() const;
  int GetY() const;
  int GetXmonde() const;
  int GetYmonde() const;
  bool ClicG() const;
  bool ClicD() const;
  bool ClicM() const;
  void ScrollCamera() const;
  Point2i GetPosMonde () const;
};

extern Mouse mouse;
//-----------------------------------------------------------------------------
#endif
