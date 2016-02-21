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
 * Talk box: box handling chat in network menus
 *****************************************************************************/

#ifndef TALK_BOX_H
#define TALK_BOX_H

#include "graphic/surface.h"
#include "gui/vertical_box.h"
#include <WARMUX_base.h>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>

class Button;
class MsgBox;
class TextBox;

class TalkBox : public VBox
{
  MsgBox* msg_box;
  TextBox* line_to_send_tbox;
  Button* send_txt_bt;

public:
  TalkBox(const Point2i& size, Font::font_size_t font_size, Font::font_style_t font_style);
  virtual ~TalkBox(void) {}

  void NewMessage(const std::string &msg, const Color& color = white_color);
  void SendChatMsg();
  void Clear();

  bool TextHasFocus() const;
  TextBox* GetTextBox() const {return line_to_send_tbox; };

  bool SendKey(SDL_keysym key);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
};


#endif
