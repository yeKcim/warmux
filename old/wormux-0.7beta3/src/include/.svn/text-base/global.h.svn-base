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
 * Class with instanciate everything.
 *****************************************************************************/

#ifndef GLOBAL_H
#define GLOBAL_H
//-----------------------------------------------------------------------------
#include "base.h"
#include "../graphic/font.h"
//-----------------------------------------------------------------------------

class Global
{
public:
  Global();
  ~Global();
  Font& large_font();
  Font& huge_font();
  Font& big_font();
  Font& normal_font();
  Font& small_font();
  Font& tiny_font();

private:
  Font* AccessFont (Font **ptr, int size);

  // Fonts
  Font *ptr_large_font;
  Font *ptr_huge_font;
  Font *ptr_big_font;
  Font *ptr_normal_font;
  Font *ptr_small_font;
  Font *ptr_tiny_font;
};

Global& global();
void createGlobal();
void destroyGlobal();

//-----------------------------------------------------------------------------
#endif
