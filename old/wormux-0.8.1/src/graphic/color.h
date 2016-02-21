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
  private:

    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint8 alpha;

  public:
    Color(Uint8 r = 200, Uint8 g = 50, Uint8 b = 50, Uint8 a = 130)
      { SetColor(r, g, b, a); }

    bool operator==(const Color &c) const
      { return red==c.red && green==c.green && blue==c.blue && alpha==c.alpha; }
    bool operator!=(const Color &c) const
      { return red!=c.red || green!=c.green || blue!=c.blue || alpha!=c.alpha; }
    Color operator*(const Color &fact) const;

    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
      red = r;
      green = g;
      blue = b;
      alpha = a;
    }

    Uint8 GetRed() const { return red; }
    Uint8 GetGreen() const { return green; }
    Uint8 GetBlue() const { return blue; }
    Uint8 GetAlpha() const { return alpha; }
    Uint32 GetColor() const;

    SDL_Color GetSDLColor() const;
};

#endif
