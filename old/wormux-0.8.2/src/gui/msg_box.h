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
 * Message box widget
 *****************************************************************************/

#ifndef MSG_BOX_H
#define MSG_BOX_H

#include "include/base.h"
#include "graphic/font.h"
#include "tool/rectangle.h"
#include "widget.h"
#include <list>

class Text;

class MsgBox : public Widget
{
  Font::font_size_t font_size;
  Font::font_style_t font_style;
  std::list<Text*> messages;
  void Flush();

 public:

  MsgBox(const Point2i& size, Font::font_size_t font_size, Font::font_style_t font_style);
  virtual ~MsgBox();

  void NewMessage(const std::string& msg, const Color& color = white_color);

  void Clear();

  virtual void Draw (const Point2i &mousePosition) const;
  virtual void Pack();
};

#endif

