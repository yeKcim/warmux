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
 * Listbox
 *****************************************************************************/

#include <algorithm>

#include "gui/list_box.h"
#include "gui/button.h"
#include "gui/container.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "tool/resource_manager.h"

BaseListBox::BaseListBox (const Point2i &_size, bool always_one_selected_b):
  Widget(Point2i(_size.x, _size.y)),
  always_one_selected(always_one_selected_b),
  scrolling(false),
  first_visible_item(0),
  selected_item(-1),
  m_items(),
  m_up(NULL),
  m_down(NULL),
  selected_item_color(defaultListColor2),
  default_item_color(defaultListColor3),
  margin(0)
{
  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  m_up = new Button(res, "menu/up");
  m_down = new Button(res, "menu/down");
  GetResourceManager().UnLoadXMLProfile(res);

  Widget::SetBorder(white_color, 1);
  Widget::SetBackgroundColor(defaultListColor1);
}

BaseListBox::~BaseListBox()
{
  delete m_up;
  delete m_down;

  ClearItems();
}

void BaseListBox::ClearItems()
{
  for (std::vector<Widget*>::iterator lbi=m_items.begin(); lbi!=m_items.end(); lbi++)
     delete *lbi;

  m_items.clear();
}

int BaseListBox::MouseIsOnWhichItem(const Point2i &mousePosition) const
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

static uint last_visible_item = 0; // to not break Draw constness

Widget* BaseListBox::ClickUp(const Point2i &mousePosition, uint button)
{
  scrolling = false;

  if (m_items.empty())
    return NULL;

  // buttons for listbox with more items than visible (first or last item not visible)
  if ((button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
      (button == SDL_BUTTON_LEFT && m_down->Contains(mousePosition))) {

    // bottom button
    if (last_visible_item < m_items.size() - 1)
      first_visible_item++ ;

    return this;
  }
  else if ((button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
	   (button == SDL_BUTTON_LEFT && m_up->Contains(mousePosition))) {

    // top button
    if (first_visible_item > 0)
      first_visible_item-- ;

    return this;
  }

  if (button == SDL_BUTTON_LEFT) {
    int item = MouseIsOnWhichItem(mousePosition);

    if (item == -1)
      return NULL;

    if (item == selected_item) {
        //Deselect ();
    } else
      Select (item);
    return this;
  }

  return NULL;
}

Widget* BaseListBox::Click(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition)) return NULL;

  if (ScrollBarPos().Contains(mousePosition) && button == SDL_BUTTON_LEFT) {
    scrolling = true;
  }
  return this;
}

void BaseListBox::__Update(const Point2i &mousePosition,
		       const Point2i &/*lastMousePosition*/)
{
  if (!Contains(mousePosition)) {
    scrolling = false;
  }
  // update position of items because of scrolling with scroll bar
  if (scrolling &&
      uint(mousePosition.y) < GetPositionY() + GetSizeY() - 12 -margin&&
      mousePosition.y > GetPositionY() + 12)
    {
      first_visible_item = (mousePosition.y - GetPositionY() - 10) * m_items.size() / (GetSizeY()-20-margin);
    }
}

void BaseListBox::Draw(const Point2i &mousePosition) const
{
  Surface& surf = GetMainWindow();
  int item = MouseIsOnWhichItem(mousePosition);

  // Draw items
  Point2i pos = GetPosition() + Point2i(5, 0);
  bool draw_it = true;

  for (uint i=first_visible_item; i < m_items.size(); i++) {

    Rectanglei rect(GetPositionX() + 1,
                    pos.GetY() + 1,
                    GetSizeX() - 2,
                    m_items.at(i)->GetSizeY() - 2);

    // no more place to add item
    if (draw_it && uint(rect.GetPositionY() + rect.GetSizeY()) >= GetPositionY() + GetSizeY() -2 - margin) {
      last_visible_item = i-1;
      draw_it = false;
    }

    // item is selected or mouse-overed
    if (draw_it) {
      if (int(i) == selected_item) {
        surf.BoxColor(rect, selected_item_color);
      } else if (i == uint(item)) {
        surf.BoxColor(rect, default_item_color);
      }
    }

    // Really draw items
    m_items.at(i)->SetPosition(pos);
    m_items.at(i)->SetSize(size.x - 12, m_items.at(i)->GetSizeY() - 2);
    m_items.at(i)->Pack();
    if (draw_it) {
      m_items.at(i)->Draw(mousePosition);
    }

    pos += Point2i(0, m_items.at(i)->GetSizeY());
  }

  if (draw_it) {
    last_visible_item = m_items.size()-1;
  }

  // buttons for listbox with more items than visible
  if (first_visible_item != 0 || last_visible_item != m_items.size() - 1) {
    m_up->Draw(mousePosition);
    m_down->Draw(mousePosition);

    Rectanglei scrollbar = ScrollBarPos();
    surf.BoxColor(scrollbar, (scrolling || scrollbar.Contains(mousePosition)) ? white_color : gray_color);
  }
}

Rectanglei BaseListBox::ScrollBarPos() const
{
  uint tmp_y, tmp_h;
  if(m_items.size() != 0)
  {
    tmp_y = GetPositionY()+ 10 + first_visible_item* (GetSizeY()-20-margin) / m_items.size();
    tmp_h = /*nb_visible_items_max * */(GetSizeY()-20-margin) / m_items.size();
  }
  else
  {
    tmp_y = GetPositionY()+ 10;
    tmp_h = /*nb_visible_items_max * */GetSizeY()-20-margin;
  }

  if (tmp_h < 5) tmp_h =5;

  return Rectanglei(GetPositionX()+GetSizeX()-11, tmp_y, 9,  /*tmp_y+*/tmp_h);
}

void BaseListBox::Pack()
{
  m_up->SetPosition(position.x + size.x - 12, position.y +2);
  m_down->SetPosition(position.x + size.x - 12, position.y + size.y - 7 -margin);

  for(std::vector<Widget*>::iterator it=m_items.begin();
      it != m_items.end(); it++)
  {
    (*it)->Pack();
  }
}

void BaseListBox::AddWidgetItem(bool selected, Widget* item)
{
  uint pos = m_items.size();
  m_items.push_back (item);

  // Select it if selected
  if( selected )
    Select (pos);
}

bool BaseListBox::IsSelectedItem()
{
  return (selected_item != -1);
}

void BaseListBox::Sort() const
{
  //std::sort( m_items.begin(), m_items.end(), CompareItems() );
}

void BaseListBox::RemoveSelected()
{
  ASSERT (always_one_selected == false);

  if( selected_item != -1 ){
    m_items.erase( m_items.begin() + selected_item );
    selected_item =- 1;
  }
}

void BaseListBox::Select (uint index)
{
  ASSERT(index < m_items.size());
  selected_item = index;
}

void BaseListBox::Deselect ()
{
  ASSERT (always_one_selected == false);
  selected_item = -1;
}

//-----------------------------------------------------------------------------

class ListBoxItem : public Label
{
private:
  std::string value;

public:
  ListBoxItem(const std::string& _label,
              Font::font_size_t font_size,
              Font::font_style_t font_style,
              const std::string& value,
              const Color& color = white_color);

  const std::string& GetLabel() const;
  const std::string& GetValue() const { return value; };
};

ListBoxItem::ListBoxItem(const std::string& _label,
                         Font::font_size_t fsize,
                         Font::font_style_t fstyle,
                         const std::string& _value,
                         const Color& color) :
  Label(_label, 200, fsize, fstyle, color),
  value(_value)
{
}



const std::string& ListBoxItem::GetLabel() const
{
  return txt_label->GetText();
}

//-----------------------------------------------------------------------------

void ListBox::AddItem (bool selected,
                       const std::string &label,
                       const std::string &value,
                       Font::font_size_t fsize,
                       Font::font_style_t fstyle,
                       const Color& color)
{
  // Push item
  AddWidgetItem(selected, new ListBoxItem(label, fsize, fstyle, value, color));
}

void ListBox::Select(const std::string& val)
{
  uint index = 0;
  for(std::vector<Widget*>::iterator it=m_items.begin();
      it != m_items.end();
      it++,index++)
  {
    if(GetItem(*it)->GetLabel() == val)
    {
      BaseListBox::Select(index);
      return;
    }
  }
}

const std::string& ListBox::ReadLabel () const
{
  ASSERT (selected_item != -1);
  return GetItem(m_items[selected_item])->GetLabel();
}

const std::string& ListBox::ReadValue () const
{
  ASSERT (selected_item != -1);
  return GetItem(m_items[selected_item])->GetValue();
}

int ListBox::ReadIntValue() const
{
  int tmp = 0;
  sscanf(ReadValue().c_str(),"%d", &tmp);
  return tmp;
}

const std::string& ListBox::ReadValue (int index) const
{
  ASSERT (index != -1 && index < (int)m_items.size());
  return GetItem(m_items[index])->GetValue();
}
