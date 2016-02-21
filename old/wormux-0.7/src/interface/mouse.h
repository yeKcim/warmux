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

#include <SDL.h>
#include "../include/base.h"
#include "../tool/point.h"

class Mouse
{
private:
  bool scroll_actif;
  Point2i savedPos;

  static Mouse * singleton;

 private:
  Mouse();
  
public:
  static Mouse * GetInstance();

  void TraiteClic (const SDL_Event *event); 

  void Reset();

  // Refresh de la souris à chaque boucle du jeu
  void Refresh();
  void TestCamera();
  bool ActionClicG();
  bool ActionClicD();
  void ChoixVerPointe();

  // Lit la position dans le monde
  Point2i GetPosition() const;
  Point2i GetWorldPosition() const;
  bool ClicG() const;
  bool ClicD() const;
  bool ClicM() const;
  void ScrollCamera() const;
};

#endif
