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

#include "list_box_w_label.h"
#include <algorithm>
#include "graphic/text.h"
#include "button.h"

//#define SCROLLBAR

ListBoxWithLabel::ListBoxWithLabel (const std::string &label, const Rectanglei &rect) : ListBox(rect)
{
  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  SetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());
}

ListBoxWithLabel::~ListBoxWithLabel()
{
   delete txt_label;
}

void ListBoxWithLabel::Draw(const Point2i &mousePosition, Surface& surf) const
{
  int item = MouseIsOnWhichItem(mousePosition);

  Rectanglei rect (GetPositionX(),
                   GetPositionY(),
                   GetSizeX(),
                   GetSizeY()- 2 - txt_label->GetHeight());

  surf.BoxColor(rect, defaultListColor1);
  surf.RectangleColor(rect, white_color);

  // Draw items
  Point2i pos = GetPosition() + Point2i(5, 0);
  uint local_max_visible_items = m_items.size();
  bool draw_it = true;

  for(uint i=first_visible_item; i < m_items.size(); i++){

    Rectanglei rect2(GetPositionX() + 1,
                     pos.GetY() + 1,
                     GetSizeX() - 2,
                     m_items[i]->GetSizeY() - 2);

    // no more place to add item
    if (draw_it && rect2.GetPositionY()+rect2.GetSizeY() > GetPositionY()+ rect.GetSizeY()) {
      local_max_visible_items = i - first_visible_item;
      draw_it = false;
    }

    // item is selected or mouse-overed
    if (draw_it) {
      if( int(i) == selected_item) {
        surf.BoxColor(rect2, defaultListColor2);
      } else if( i == uint(item) ) {
        surf.BoxColor(rect2, defaultListColor3);
      }
    }

    // Really draw items
    Rectanglei rect3(pos.x, pos.y,
                     GetSizeX()-2, m_items[i]->GetSizeY() - 2);

    m_items[i]->SetSizePosition(rect3);
    if (draw_it) {
      m_items[i]->Draw(mousePosition, surf);
    }

    pos += Point2i(0, m_items[i]->GetSizeY());
  }

  // Draw the label
  txt_label->DrawTopLeft( Point2i(GetPositionX(),
        GetPositionY() + GetSizeY() - txt_label->GetHeight() ));

  // buttons for listbox with more items than visible
  if (m_items.size() > local_max_visible_items){
    m_up->Draw(mousePosition, surf);
    m_down->Draw(mousePosition, surf);
#ifdef SCROLLBAR
    uint tmp_y, tmp_h;
    tmp_y = y+10+ first_visible_item* (h-20) / m_items.size();
    tmp_h = nb_visible_items_max * (h-20) / m_items.size();
    if (tmp_h < 5) tmp_h =5;

    boxRGBA(surf,
            x+w-10, tmp_y,
            x+w-1,  tmp_y+tmp_h,
            white_color);
#endif
  }
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
