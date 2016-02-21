/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Label in GUI.
 *****************************************************************************/

#ifndef LABEL_H
#define LABEL_H

#include "../include/base.h"
#include "../graphic/text.h"
#include "../gui/widget.h"
#include <string>

class Label : public Widget{
 protected:
  Text *txt_label;
 private:
  Font *font;
  const Color& font_color;
  bool center;

 public:
  Label(const std::string &label, const Rectanglei &rect, Font& font,
	const Color& color = white_color, bool center = false);
  ~Label();
  void Draw (const Point2i &mousePosition, Surface& surf);
  void SetSizePosition(const Rectanglei &rect);
  void SetText(const std::string &new_txt);
  std::string &GetText();

  void SetFontColor(const Color& color);
};

#endif
