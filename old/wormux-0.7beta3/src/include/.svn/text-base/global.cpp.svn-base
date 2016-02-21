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

#include "global.h"
//-----------------------------------------------------------------------------
Global *global_ptr = NULL;

Global::Global()
{
 
  ptr_tiny_font = ptr_small_font = \
  ptr_normal_font = ptr_big_font = \
  ptr_large_font = ptr_huge_font = NULL;
}

Global::~Global()
{
  delete ptr_large_font;
  delete ptr_huge_font;
}

Global& global()
{
  assert (global_ptr != NULL);
  return *global_ptr;
}

Font& Global::huge_font() { return *AccessFont(&ptr_huge_font, 40); }
Font& Global::large_font() { return *AccessFont(&ptr_large_font, 32); }
Font& Global::big_font() { return *AccessFont(&ptr_big_font, 24); }
Font& Global::normal_font() { return *AccessFont(&ptr_normal_font, 16); }
Font& Global::small_font() { return *AccessFont(&ptr_small_font, 12); }
Font& Global::tiny_font() { return *AccessFont(&ptr_tiny_font, 8); }

Font* Global::AccessFont(Font **ptr, int size)
{
  if (*ptr == NULL) *ptr = new Font(size);
  return *ptr;
}

void createGlobal()
{
  assert (global_ptr == NULL);
  global_ptr = new Global();
}

void destroyGlobal()
{
  assert (global_ptr != NULL);
  delete global_ptr;
  global_ptr = NULL; 
}    

//-----------------------------------------------------------------------------
