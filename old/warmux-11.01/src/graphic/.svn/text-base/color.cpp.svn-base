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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Handle a color
 *****************************************************************************/

#include "graphic/color.h"
#include <SDL.h>

SDL_Color Color::GetSDLColor() const
{
  SDL_Color sdlColor = { value.components.red, value.components.green,
                         value.components.blue, value.components.alpha };

  return sdlColor;
}

Uint32 Color::GetColor() const
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  return (value.components.red << 24) | (value.components.green << 16) |
         (value.components.blue << 8) | value.components.alpha;
#else
  return (value.components.alpha << 24) | (value.components.blue << 16) |
         (value.components.green << 8) | value.components.red;
#endif
}
