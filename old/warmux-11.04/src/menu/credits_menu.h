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
 * Credits menu
 *****************************************************************************/

#ifndef CREDITS_MENU_H
#define CREDITS_MENU_H

#include "menu/menu.h"
#include "graphic/font.h"

// Forward declarations
class ScrollBox;

class CreditsMenu : public Menu
{
  ScrollBox *lbox_authors;

  void PrepareAuthorsList();

  void AddItem(const std::string & label,
               Font::font_size_t fsize = Font::FONT_SMALL,
               Font::font_style_t fstyle = Font::FONT_BOLD,
               const Color & color = white_color);
public:
  CreditsMenu();
};

#endif
