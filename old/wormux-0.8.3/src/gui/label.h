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
 * Label in GUI.
 *****************************************************************************/

#ifndef LABEL_H
#define LABEL_H

#include "include/base.h"
#include "gui/widget.h"
#include "graphic/font.h"
#include <string>

class Text;

class Label : public Widget{
  /* If you need this, implement it (correctly)*/
  Label(const Label&);
  Label operator=(const Label&);
  /*********************************************/

 protected:
  Text *txt_label;

 private:
  bool center;

 public:
  Label(const std::string &label,
        uint max_width,
        Font::font_size_t font_size = Font::FONT_SMALL,
        Font::font_style_t font_style = Font::FONT_NORMAL,
        const Color& color = white_color,
        bool center = false,
        bool shadowed = true);
  ~Label();

  virtual void Draw(const Point2i &mousePosition) const;
  void SetText(const std::string &new_txt);
  const std::string& GetText() const;

  virtual void OnFontChange();
  virtual void Pack();
};

#endif
