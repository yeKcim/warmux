/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

Color Color::operator*(const Color &fact) const{
  return Color((Uint8) ((Uint32)red * fact.red / 255),
               (Uint8) ((Uint32)green * fact.green / 255),
               (Uint8) ((Uint32)blue * fact.blue / 255),
               (Uint8) ((Uint32)alpha * fact.alpha / 255));
}

SDL_Color Color::GetSDLColor() const{
        SDL_Color sdlColor;

        sdlColor.r = red;
        sdlColor.g = green;
        sdlColor.b = blue;
        sdlColor.unused = alpha;

        return sdlColor;
}

Uint32 Color::GetColor() const
{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  return (red << 24) | green << 16 | blue << 8 | alpha;
#else
  return (alpha << 24) | blue << 16 | green << 8 | red;
#endif
}
