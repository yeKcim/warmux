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
 * Label in GUI.
 *****************************************************************************/

#ifndef LABEL_H
#define LABEL_H

#include <string>
#include <WARMUX_base.h>
#include "gui/widget.h"
#include "graphic/text.h"

class Label : public Text, public Widget
{
public:
  Label(const std::string & label,
        uint max_width,
        Font::font_size_t font_size = Font::FONT_SMALL,
        Font::font_style_t font_style = Font::FONT_BOLD,
        const Color & color = dark_gray_color,
        Text::Alignment align = Text::ALIGN_LEFT_TOP,
        bool shadowed = false,
        const Color & shadowColor = black_color);
  Label(const Point2i & size);
  Label(Profile * profile,
        const xmlNode * pictureNode);
  Label(void) {}
  virtual ~Label();

  virtual bool LoadXMLConfiguration(void);
  virtual void Draw(const Point2i & mousePosition);
  virtual void Pack();

  void SetText(const std::string & new_txt);
};

#endif
