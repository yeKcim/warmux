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
  SetSizePosition(Rectanglei(-1, -1, _size.x, _size.y));
  txt_label->SetMaxWidth(GetSizeX());
}

ListBoxWithLabel::~ListBoxWithLabel()
{
   delete txt_label;
}

void ListBoxWithLabel::Draw(const Point2i &mousePosition, Surface& surf) const
{
  ListBox::Draw(mousePosition, surf);

  // Draw the label
  txt_label->DrawTopLeft(Point2i(GetPositionX(),
				 GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

void ListBoxWithLabel::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());

  m_up->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - m_up->GetSizeX() - 2,
                                    GetPositionY()+2,
                                    m_up->GetSizeX(), m_up->GetSizeY()) );

  m_down->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - m_down->GetSizeX() - 2,
                                      GetPositionY() + GetSizeY() - m_down->GetSizeY() - 2 -
                                      txt_label->GetHeight() - 2,
                                      m_down->GetSizeX(),
                                      m_down->GetSizeY()) );
}
