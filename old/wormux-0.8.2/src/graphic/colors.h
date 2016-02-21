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
 * Colors.
 ******************************************************************************/

#ifndef COLORS_H
#define COLORS_H

#include "color.h"

const Color transparent_color (255, 255, 255, SDL_ALPHA_TRANSPARENT);

// Greyscale
const Color white_color (255, 255, 255, SDL_ALPHA_OPAQUE);
const Color light_gray_color (169, 169, 169, SDL_ALPHA_OPAQUE);
const Color gray_color (128, 128, 128, SDL_ALPHA_OPAQUE);
const Color dark_gray_color (64, 64, 64, SDL_ALPHA_OPAQUE);
const Color black_color (0, 0, 0, SDL_ALPHA_OPAQUE);

const Color c_white  (0xFF, 0xFF, 0xFF, 0x70);
const Color c_black  (0x00, 0x00, 0x00, 0x70);
const Color c_red    (0xFF, 0x00, 0x00, 0x70);
const Color c_yellow (0x00, 0xFF, 0xFF, 0x70);
const Color c_grey   (0xF0, 0xF0, 0xF0, 0x70);

// Green
const Color green_color (68, 120, 51, SDL_ALPHA_OPAQUE);

const Color primary_red_color(255, 0, 0, SDL_ALPHA_OPAQUE);
const Color primary_green_color(0, 255, 0, SDL_ALPHA_OPAQUE);
const Color primary_blue_color(0, 0, 255, SDL_ALPHA_OPAQUE);

// Default colors:
const Color defaultColorBox(80, 80, 159, 206);
const Color defaultColorRect(49, 32, 122, 255);

const Color defaultOptionColorBox(255, 255, 255, 160);
const Color defaultOptionColorRect(59, 88, 129, SDL_ALPHA_OPAQUE);

const Color highlightOptionColorBox(200, 100, 100, 206);

//const Color defaultListColor1(255, 255, 255, 255*3/10);
const Color defaultListColor1(75, 163, 200, 255*3/10);
const Color defaultListColor2(0, 0, 255*6/10, 255*8/10);
const Color defaultListColor3(0, 0, 255*6/10, 255*4/10);

#endif /* COLORS_H */
