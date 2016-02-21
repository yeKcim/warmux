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
 * A widget integrating a ListBox with a bottom Label
 *****************************************************************************/

#include "gui/list_box_w_label.h"
#include <algorithm>
#include "graphic/text.h"
#include "gui/button.h"

ListBoxWithLabel::ListBoxWithLabel (const std::string &label, const Point2i &_size) : ListBox(_size)
{
  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  margin =  2 + txt_label->GetHeight();
  txt_label->SetMaxWidth(size.x);
}

ListBoxWithLabel::~ListBoxWithLabel()
{
   delete txt_label;
}

void ListBoxWithLabel::Draw(const Point2i &mousePosition) const
{
  ListBox::Draw(mousePosition);

  // Draw the label
  txt_label->DrawTopLeft(Point2i(GetPositionX(),
				 GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

void ListBoxWithLabel::Pack()
{
  txt_label->SetMaxWidth(size.x);

  m_up->SetPosition(position.x + size.x - m_up->GetSizeX() - 2,
		    position.y + 2);

  m_down->SetPosition(position.x + size.x - m_down->GetSizeX() - 2,
		      position.y + size.y - m_down->GetSizeY() - 2 - txt_label->GetHeight() - 2);

  for(std::vector<Widget*>::iterator it=m_items.begin(); it != m_items.end(); it++)
  {
    (*it)->Pack();
  }
}
