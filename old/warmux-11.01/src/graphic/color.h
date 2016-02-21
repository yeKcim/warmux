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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Color
 *****************************************************************************/

#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>

class Color
{
  union
  {
    struct
    {
      Uint8 red   : 8;
      Uint8 green : 8;
      Uint8 blue  : 8;
      Uint8 alpha : 8;
    } components;
    Uint32 color;
  } value;

public:
  Color()
    { SetColor(200, 50, 50, 130); }
  Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 130)
    { SetColor(r, g, b, a); }
  Color(Uint8 grey, Uint8 a = SDL_ALPHA_OPAQUE)
    { SetColor(grey, grey, grey, a); }
  Color(const Color& other) { value.color = other.value.color; }

  bool operator==(const Color &c) const { return value.color == c.value.color; }
  bool operator!=(const Color &c) const { return value.color != c.value.color; }

  void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
  {
    value.components.red   = r;
    value.components.green = g;
    value.components.blue  = b;
    value.components.alpha = a;
  }

  void SetRed(Uint8 r)   { value.components.red   = r; }
  void SetGreen(Uint8 g) { value.components.green = g; }
  void SetBlue(Uint8 b)  { value.components.blue  = b; }
  void SetAlpha(Uint8 a) { value.components.alpha = a; }

  Uint8 GetRed() const   { return value.components.red;   }
  Uint8 GetGreen() const { return value.components.green; }
  Uint8 GetBlue() const  { return value.components.blue;  }
  Uint8 GetAlpha() const { return value.components.alpha; }
  Uint32 GetColor() const;

  SDL_Color GetSDLColor() const;
};

#endif
