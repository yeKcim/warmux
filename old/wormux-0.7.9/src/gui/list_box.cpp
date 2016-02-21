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
 * Listbox
 *****************************************************************************/

#include "list_box.h"
#include <algorithm>
#include <SDL_gfxPrimitives.h>
#include "../graphic/font.h"
#include "../include/app.h"
#include "../tool/math_tools.h"
#include "../tool/resource_manager.h"

//#define SCROLLBAR

ListBoxItem::ListBoxItem(const std::string& _label, 
			 Font& _font,
			 const std::string& _value,
			 const Color& color) :
  Label(_label, Rectanglei(0,0,0,0), _font, color)
{
  value = _value;
}

const std::string& ListBoxItem::GetLabel() const
{
  return txt_label->GetText();
}

const std::string& ListBoxItem::GetValue() const
{
  return value;
}

// struct CompareItems
// {
//      bool operator()(const ListBoxItem& a, const ListBoxItem& b)
//      {
//        return a.GetLabel() < b.GetLabel();
//      }
// };

ListBox::ListBox (const Rectanglei &rect, bool always_one_selected_b) : Widget(rect)
{
  Rectanglei buttonRect;
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  buttonRect.SetPosition(GetPositionX() + GetSizeX() - 12, GetPositionY() + 2);
  buttonRect.SetSize(10, 5);
  m_up = new Button(buttonRect, res, "menu/up");
  buttonRect.SetPosition(GetPositionX() + GetSizeX() - 12, GetPositionY() + GetSizeY() - 7);
  m_down = new Button(buttonRect, res, "menu/down");

  resource_manager.UnLoadXMLProfile( res);

  first_visible_item = 0;
  selected_item = -1;
  always_one_selected = always_one_selected_b;
}

ListBox::~ListBox()
{
   delete m_up;
   delete m_down;

   m_items.clear();
}

int ListBox::MouseIsOnWhichItem(const Point2i &mousePosition) const
{
  if( !Contains(mousePosition) )
    return -1;

  for (uint i=first_visible_item; i < m_items.size(); i++) {
    if ( m_items[i]->GetPositionY() <= mousePosition.y
	 && m_items[i]->GetPositionY() + m_items[i]->GetSizeY() >= mousePosition.y)
      return i;
  }
  return -1;
}

Widget* ListBox::Clic(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  if (m_items.empty())
    return NULL;

  // buttons for listbox with more items than visible (first or last item not visible)
  if(first_visible_item > 0  
     || m_items.back()->GetPositionY() + m_items.back()->GetSizeY() > GetPositionY() + GetSizeY()) 
    {
      if( (button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
	  (button == SDL_BUTTON_LEFT && m_down->Contains(mousePosition)) ){
	
	// bottom button
	if( m_items.back()->GetPositionY() + m_items.back()->GetSizeY() > GetPositionY() + GetSizeY() )
	  first_visible_item++ ;
	
	return this;
      }
      else if( (button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
	       (button == SDL_BUTTON_LEFT && m_up->Contains(mousePosition)) ){
	
	// top button
	if( first_visible_item > 0 )
	  first_visible_item-- ;
	
	return this;
      }
    }

  if( button == SDL_BUTTON_LEFT ){
    int item = MouseIsOnWhichItem(mousePosition);

    if( item == -1 )
      return NULL;

    if( item == selected_item ){
        //Deselect ();
    } else
      Select (item);
    return this;
  }
  else{
    return NULL;
  }
}

void ListBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  int item = MouseIsOnWhichItem(mousePosition);
  Rectanglei rect (*this);

  // Draw border and bg color
  surf.BoxColor(rect, defaultListColor1);
  surf.RectangleColor(rect, white_color);

  // Draw items
  Point2i pos = GetPosition() + Point2i(5, 0);
  uint local_max_visible_items = m_items.size();
  bool draw_it = true;

  for(uint i=first_visible_item; i < m_items.size(); i++){

    Rectanglei rect(GetPositionX() + 1, 
		    pos.GetY() + 1, 
		    GetSizeX() - 2, 
		    m_items.at(i)->GetSizeY() - 2);

    // no more place to add item
    if (draw_it && rect.GetPositionY() + rect.GetSizeY() > GetPositionY() + GetSizeY()) {
      local_max_visible_items = i - first_visible_item;
      draw_it = false;
    }
    
    // item is selected or mouse-overed
    if (draw_it) {
      if( int(i) == selected_item) {
	surf.BoxColor(rect, defaultListColor2);
      } else if( i == uint(item) ) {
	surf.BoxColor(rect, defaultListColor3);
      }
    }

    // Really draw items
    Rectanglei rect2(pos.x, pos.y, 
		     GetSizeX()-2, m_items.at(i)->GetSizeY() - 2);

    m_items.at(i)->SetSizePosition(rect2);
    if (draw_it) {
      m_items.at(i)->Draw(mousePosition, surf);
    }

    pos += Point2i(0, m_items.at(i)->GetSizeY());
  }

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

void ListBox::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  m_up->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - 12, GetPositionY()+2, 10, 5) );
  m_down->SetSizePosition( Rectanglei(GetPositionX() + GetSizeX() - 12, GetPositionY() + GetSizeY() - 7, 10, 5) );
}

void ListBox::AddItem (bool selected,
		       const std::string &label,
		       const std::string &value,
		       Font& font,
		       const Color& color)
{
  uint pos = m_items.size();

  // Push item
  ListBoxItem * item = new ListBoxItem(label, font, value, color);
  m_items.push_back (item);

  // Select it if selected
  if( selected )
    Select (pos);
}

void ListBox::Sort()
{
  //std::sort( m_items.begin(), m_items.end(), CompareItems() );
}

void ListBox::RemoveSelected()
{
  assert (always_one_selected == false);

  if( selected_item != -1 ){
    m_items.erase( m_items.begin() + selected_item );
    selected_item =- 1;
  }
}

void ListBox::Select (uint index)
{
  assert(index < m_items.size());
  selected_item = index;
}

void ListBox::Select(const std::string& val)
{
  uint index = 0;
  for(std::vector<ListBoxItem*>::iterator it=m_items.begin();
      it != m_items.end();
      it++,index++)
  {
    if((*it)->GetLabel() == val)
    {
      Select(index);
      return;
    }
  }
}

void ListBox::Deselect ()
{
  assert (always_one_selected == false);
  selected_item = -1;
}

int ListBox::GetSelectedItem () const
{
  return selected_item;
}

const std::string& ListBox::ReadLabel () const
{
  assert (selected_item != -1);
  return m_items.at(selected_item)->GetLabel();
}

const std::string& ListBox::ReadValue () const
{
  assert (selected_item != -1);
  return m_items.at(selected_item)->GetValue();
}

const std::string& ListBox::ReadValue (int index) const
{
  assert (index != -1 && index < (int)m_items.size());
  return m_items.at(index)->GetValue();
}

uint ListBox::Size() const
{
  return m_items.size();
}
