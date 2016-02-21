/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Liste de choix.
 *****************************************************************************/

#include "list_box.h"
//-----------------------------------------------------------------------------
#include "../tool/math_tools.h"
#include "../tool/resource_manager.h"
#include "../include/app.h"
#include "../graphic/font.h"
#include <algorithm>
#include <SDL_gfxPrimitives.h>
#include "../include/global.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct CompareItems
{
     bool operator()(const list_box_item_t& a, const list_box_item_t& b)
     {
       return a.label < b.label;
     }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ListBox::ListBox (uint _x, uint _y, uint _w, uint _h)
  : Widget(_x,_y,_w,_h) 
{  
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false); 
  m_up = new Button(x+w-10, y, 10, 5, res, "menu/up");
  m_down = new Button(x+w-10, y+h-5, 10, 5, res, "menu/down");

  height_item = global().small_font().GetHeight();
  first_visible_item = 0;
  nb_visible_items_max = h/height_item;
  nb_visible_items = 0;

  selected_item = -1;  
  always_one_selected = true;
}

//-----------------------------------------------------------------------------

ListBox::~ListBox()
{
   delete m_up;
   delete m_down;

   m_items.clear();
}

//-----------------------------------------------------------------------------

int ListBox::MouseIsOnWhichItem (uint mouse_x, uint mouse_y)
{
  if ((mouse_x < x+1) 
      || (mouse_y < y+1)
      || ((y+1 + h) < mouse_y)
      || ((x + w) < mouse_x)) 
    return -1;

  int index = (mouse_y - y)/height_item;
  return BorneLong(index+first_visible_item, 0, m_items.size()-1);
}

//-----------------------------------------------------------------------------

bool ListBox::Clic (uint mouse_x, uint mouse_y, uint button)
{

  // buttons for listbox with more items than visible
  if (m_items.size() > nb_visible_items_max)
  {
    if ( (button == SDL_BUTTON_WHEELDOWN && MouseIsOver(mouse_x, mouse_y)) ||
          (button == SDL_BUTTON_LEFT && m_down->MouseIsOver(mouse_x, mouse_y)) )
    {
      // bottom button
      if ( m_items.size() - first_visible_item > nb_visible_items_max )
        first_visible_item++ ;

      return true;
    }
    else if ( (button == SDL_BUTTON_WHEELUP && MouseIsOver(mouse_x, mouse_y)) || 
                (button == SDL_BUTTON_LEFT && m_up->MouseIsOver(mouse_x,mouse_y)) )
    {     
      // top button
      if (first_visible_item > 0)
        first_visible_item-- ;

      return true;
    }
  }

  if(button == SDL_BUTTON_LEFT)
  {
    int item = MouseIsOnWhichItem(mouse_x,mouse_y);
    if (item == -1) return false;
    
    if (item == selected_item) {
        //Deselect ();
    } else
      Select (item);
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------

void ListBox::Draw (uint mouse_x, uint mouse_y)
{
  int item = MouseIsOnWhichItem(mouse_x, mouse_y);
  
  boxRGBA(app.sdlwindow, x, y, x+w, y+h,
	  255, 255, 255, 255*3/10);

  rectangleRGBA(app.sdlwindow, x, y, x+w, y+h,
		255, 255, 255, 255);

  for (uint i=0; i < nb_visible_items; i++) 
  {
     if ( int(i+first_visible_item) == selected_item ) {
       boxRGBA(app.sdlwindow, 
	       x+1, y+i*height_item+1, 
	       x+1+w-2, y+i*height_item+1+height_item-2,
	       0,0,255*6/10,255*8/10);
     } else if ( i+first_visible_item == uint(item) ) {
       boxRGBA(app.sdlwindow, 
	       x+1, y+i*height_item+1, 
	       x+1+w-2, y+i*height_item+1+height_item-2,
	       0,0,255*6/10,255*4/10);
       
     }
     
     global().small_font().WriteLeft(x+5,
			  y+i*height_item,
			  m_items[i+first_visible_item].label,
			  white_color);
     
  }  


  // buttons for listbox with more items than visible
  if (m_items.size() > nb_visible_items_max)
  {
    m_up->Draw (mouse_x, mouse_y);
    m_down->Draw (mouse_x, mouse_y);
  }
  
  
}

//-----------------------------------------------------------------------------

void ListBox::SetSizePosition(uint _x, uint _y, uint _w, uint _h)
{
  StdSetSizePosition(_x, _y, _w, _h);
  m_up->SetSizePosition(x+w-10, y, 10, 5);
  m_down->SetSizePosition(x+w-10, y+h-5, 10, 5);  

  nb_visible_items_max = h/height_item;
}

//-----------------------------------------------------------------------------

void ListBox::AddItem (bool selected, 
		       const std::string &label,
		       const std::string &value) 
{ 
  uint pos = m_items.size();

  // Push item
  list_box_item_t item;
  item.label = label;
  item.value = value;
  m_items.push_back (item);

  // Select it if selected
  if (selected) Select (pos);

  nb_visible_items = m_items.size();
  if (nb_visible_items_max < nb_visible_items) 
    nb_visible_items = nb_visible_items_max;
}

//-----------------------------------------------------------------------------

void ListBox::Sort()
{
  std::sort(m_items.begin(), m_items.end(), 
               CompareItems());
}

//-----------------------------------------------------------------------------

void ListBox::RemoveSelected()
{
  assert (always_one_selected == false);

  if (selected_item!=-1) {
    m_items.erase(m_items.begin()+selected_item);
    selected_item=-1;
  }  
  
  nb_visible_items = m_items.size();
  if (nb_visible_items_max < nb_visible_items) 
    nb_visible_items = nb_visible_items_max;
}

//-----------------------------------------------------------------------------

void ListBox::Select (uint index)
{
  assert(index < m_items.size());

  selected_item = index;
}

//-----------------------------------------------------------------------------

void ListBox::Deselect ()
{
  assert (always_one_selected == false);
  selected_item = -1;
}

//-----------------------------------------------------------------------------

int ListBox::GetSelectedItem ()
{
  return selected_item;
}

//-----------------------------------------------------------------------------

const std::string& ListBox::ReadLabel () const
{
  assert (selected_item != -1);
  return m_items.at(selected_item).label;
}

//-----------------------------------------------------------------------------

const std::string& ListBox::ReadValue () const
{
  assert (selected_item != -1);
  return m_items.at(selected_item).value;
}
//-----------------------------------------------------------------------------

const std::string& ListBox::ReadValue (int index) const
{
  assert (index != -1 && index < (int)m_items.size());
  return m_items.at(index).value;
}
//-----------------------------------------------------------------------------
  
std::vector<list_box_item_t> * ListBox::GetItemsList()
{
  return &m_items;
}
