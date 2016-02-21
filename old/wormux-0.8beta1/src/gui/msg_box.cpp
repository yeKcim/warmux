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
 * Message Box
 *****************************************************************************/

#include "tool/rectangle.h"
#include "widget.h"
#include "msg_box.h"

const uint vmargin = 5;
const uint hmargin = 5;

MsgBox::MsgBox(const Rectanglei& rect, Font::font_size_t fsize, Font::font_style_t fstyle) :
  Widget(rect), font_size(fsize), font_style(fstyle)
{
}

void MsgBox::Flush()
{
  std::list<Text *>::iterator it ;

  uint y = vmargin;
  for (it = messages.begin(); it != messages.end(); it++) 
    {
      y += (*it)->GetHeight() + vmargin;

      while (int(y) > GetSizeY() && !messages.empty()) {
	Text* tmp = messages.front();
	y -= tmp->GetHeight() - vmargin; 
	delete tmp;
	messages.pop_front();
      }
    }
}

void MsgBox::NewMessage(const std::string &msg, const Color& color)
{
  messages.push_back(new Text(msg, color, font_size, font_style));
  messages.back()->SetMaxWidth(GetSizeX() - (2*hmargin));

  // Remove old messages if needed
  Flush();

  ForceRedraw();
}

void MsgBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  // Draw the border
  surf.BoxColor(*this, defaultOptionColorBox);
  surf.RectangleColor(*this, defaultOptionColorRect,2);

  // Draw the messages
  std::list<Text*>::const_iterator it;
  int x = GetPositionX()+hmargin;
  int y = GetPositionY()+vmargin;
  for (it = messages.begin(); it != messages.end(); it++) {
    (*it)->DrawTopLeft(Point2i(x,y));
    y += (*it)->GetHeight() + vmargin;
  }
}

void MsgBox::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);

  // render the messages with the correct width
  std::list<Text*>::iterator it;
  for (it = messages.begin(); it != messages.end(); it++) {
    (*it)->SetMaxWidth(GetSizeX() - (2*hmargin));
  }
  
  // Remove old messages if needed
  Flush();
}
