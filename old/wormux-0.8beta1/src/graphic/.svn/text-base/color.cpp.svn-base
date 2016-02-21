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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Handle a color
 *****************************************************************************/

#include "color.h"
#include <SDL.h>

Color::Color(){
	SetColor(200, 50, 50, 130);
}

Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
	SetColor(r, g, b, a);
}

bool Color::operator==(const Color &color) const{
	return red == color.red 
		&& green == color.green 
		&& blue == color.blue 
		&& alpha == color.alpha;
}

void Color::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
	red = r;
	green = g;
	blue = b;
	alpha = a;
}

Uint8 Color::GetRed() const{
	return red;
}

Uint8 Color::GetGreen() const{
	return green;
}

Uint8 Color::GetBlue() const{
	return blue;
}

Uint8 Color::GetAlpha() const{
	return alpha;
}

SDL_Color Color::GetSDLColor() const{
	SDL_Color sdlColor;

	sdlColor.r = red;
	sdlColor.g = green;
	sdlColor.b = blue;
	sdlColor.unused = alpha;

	return sdlColor;
}
