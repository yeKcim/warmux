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
 * TextBox widget
 *****************************************************************************/

#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <SDL_keyboard.h>
#include "label.h"

class TextBox : public Label
{
protected:
  std::string::size_type cursor_pos;
public:
  TextBox(const std::string &label, 
	  const Rectanglei &rect, 
	  Font::font_size_t fsize,
	  Font::font_style_t fstyle);
  ~TextBox();

  void SendKey(SDL_keysym key);
  void Draw(const Point2i &mousePosition, Surface& surf) const;
  void SetText(std::string const &new_txt);
  void SetCursor(std::string::size_type pos);
};

#endif

