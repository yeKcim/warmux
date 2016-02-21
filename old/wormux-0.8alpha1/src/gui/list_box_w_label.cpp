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
 * A widget integrating a ListBox with a bottom Label
 *****************************************************************************/

#include "list_box_w_label.h"
#include <algorithm>
#include <SDL_gfxPrimitives.h>
#include "../graphic/font.h"
#include "../include/app.h"
#include "../tool/math_tools.h"
#include "../tool/resource_manager.h"

//#define SCROLLBAR

ListBoxWithLabel::ListBoxWithLabel (const std::string &label, const Rectanglei &rect) : ListBox(rect)
{  
  txt_label = new Text(label, gray_color, Font::GetInstance(Font::FONT_NORMAL));
  SetSizePosition(rect);
}

ListBoxWithLabel::~ListBoxWithLabel()
{
   delete txt_label;
}

void ListBoxWithLabel::Draw(const Point2i &mousePosition, Surface& surf)
{
  int item = MouseIsOnWhichItem(mousePosition);
  Rectanglei rect (GetPositionX(),GetPositionY(),GetSizeX(),
		   GetSizeY()- 2 - Font::GetInstance(Font::FONT_NORMAL)->GetHeight());

  surf.BoxColor(rect, defaultListColor1);
  surf.RectangleColor(rect, white_color);

  for(uint i=0; i < nb_visible_items; i++){
	 Rectanglei rect(GetPositionX() + 1, GetPositionY() + i * height_item + 1, GetSizeX() - 2, height_item - 2);
	 
     if( int(i + first_visible_item) == selected_item)
       surf.BoxColor(rect, defaultListColor2);
     else
       if( i + first_visible_item == uint(item) )
         surf.BoxColor(rect, defaultListColor3);
     
     (*Font::GetInstance(Font::FONT_SMALL)).WriteLeft( 
			  GetPosition() + Point2i(5, i*height_item),
			  m_items[i + first_visible_item].label,
			  white_color);
     if(!m_items[i].enabled)
       surf.BoxColor(rect, defaultDisabledColorBox);
  }  

  // Draw the label
  txt_label->DrawTopLeft( GetPositionX(), GetPositionY() + GetSizeY() - txt_label->GetHeight() );

  // buttons for listbox with more items than visible
  if (m_items.size() > nb_visible_items_max){
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
  m_up->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - m_up->GetSizeX() - 2, 
				    GetPositionY()+2, 
				    m_up->GetSizeX(), m_up->GetSizeY()) );
  m_down->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - m_down->GetSizeX() - 2, 
				      GetPositionY() + GetSizeY() - m_down->GetSizeY() - 2 -
				           Font::GetInstance(Font::FONT_NORMAL)->GetHeight() - 2, 
				      m_down->GetSizeX(), m_down->GetSizeY()) );  

  nb_visible_items_max = GetSizeY()/height_item;
}
