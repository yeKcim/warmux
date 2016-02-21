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
 * Mouvement droite/gauche pour un ver.
 *****************************************************************************/

#ifndef MOVE_H
#define MOVE_H

#include "include/base.h"

class Character;

// Pause between two movements
const uint PAUSE_MOVEMENT=30; // ms

// Compute the height to fall or to walk on when moving horizontally
// Return a boolean which says if movement is possible
bool ComputeHeightMovement (Character &character, int &height,
                            bool falling);

void MoveCharacter (Character &character);

// Move the active character to the left/right
void MoveActiveCharacterRight(bool shift);
void MoveActiveCharacterLeft(bool shift);

#endif
