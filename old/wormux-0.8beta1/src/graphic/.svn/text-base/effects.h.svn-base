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
 *  Graphic effects on sprite
 *****************************************************************************/
#ifndef EFFECTS_H
#define EFFECTS_H

#include <SDL.h>
#include "sprite.h"

Sprite* WaveSurface(Surface &a, unsigned int nbr_frames, unsigned int duration, float wave_amp, float wave_per);
void Rebound(Sprite* spr, int &y, uint t0, uint per, int dy_max);
void Gelatine(int &y, int &stretch_y, uint t0, uint amp, uint dur, uint per);

#endif //EFFECTS_H
