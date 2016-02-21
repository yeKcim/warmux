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
 * Vertical Scroll Box with selection
 *****************************************************************************/

#include "graphic/video.h"
#include "gui/select_box.h"
#include "include/app.h"

SelectBox::SelectBox(const Point2i& size, bool always,
                     bool force, bool vertical)
  : ScrollBox(size, force, vertical, vertical)
  , selected_item_color(defaultListColor2)
  , default_item_color(defaultListColor3)
  , always_one_selected(always)
  , selected_item(-1)
  , last(NULL)
{
  box->SetMargin(0);
}

bool SelectBox::Update(const Point2i& mousePosition,
                       const Point2i& lastMousePosition)
{
  bool updated = false;
  if (last && (selected_item==-1 || m_items[selected_item]!=last)) {
    last->SetHighlighted(false);
    last->SetHighlightBgColor(selected_item_color);
    updated = true;
  }
  int item = MouseIsOnWhichItem(mousePosition);
  if (item!=-1 && item!=selected_item) {
    last = m_items[item];
    //last->SetHighlighted(true);
    //last->SetHighlightBgColor(default_item_color);
    updated = true;
  }

  updated |= ScrollBox::Update(mousePosition, lastMousePosition);
  return updated;
}

void SelectBox::__Update(const Point2i & mousePosition,
                         const Point2i & lastMousePosition)
{
  if (scroll_mode == SCROLL_MODE_KINETIC_DONE) {
    int item = MouseIsOnWhichItem(GetPosition()+GetSize()/2);
    if (item != -1 && selected_item != item && selected_item != MouseIsOnWhichItem(mousePosition))
      Select(item);
    else if (item == -1 && offset > GetMaxOffset())
      Select(m_items.size()-1);
    else if (item == -1 && offset < 0)
      Select(0);
  }
  ScrollBox::__Update(mousePosition, lastMousePosition);
}

Widget * SelectBox::ClickUp(const Point2i & mousePosition, uint button)
{
  if (Mouse::IS_CLICK_BUTTON(button)) {
    int item = MouseIsOnWhichItem(mousePosition);

    if (item!=-1) {
      if (item==selected_item) {
        if (!always_one_selected)
          Deselect();
      } else if (SCROLL_MODE_NONE==scroll_mode || LargeDrag(mousePosition)) {
        // We don't want to select a widget if we were scrolling
        // If we click up close to where we clicked down, it will think it
        // it was not scrolling, though
        Select(item);
      }
    }
  }

  return ScrollBox::ClickUp(mousePosition, button);
}

void SelectBox::AddWidget(Widget* w)
{
  m_items.push_back(w);
  w->SetHighlightBgColor(selected_item_color);
  ScrollBox::AddWidget(w);
}

void SelectBox::AddWidgetItem(bool select, Widget* w)
{
  // Let's make sure we call SelectBox method and not a child method,
  // as we are not sure of the consequences
  AddWidget(w);
  if (select)
    Select(m_items.size()-1);
}

void SelectBox::Empty()
{
  last = NULL;
  SetFocusOn(NULL);
  m_items.clear();
  selected_item = -1;
  ScrollBox::Empty();
}

void SelectBox::Clear()
{
  last = NULL;
  SetFocusOn(NULL);
  m_items.clear();
  selected_item = -1;
  ScrollBox::Clear();
}

void SelectBox::Select(uint index)
{
  ASSERT(index < m_items.size());
  if (selected_item != -1)
    m_items[selected_item]->SetHighlighted(false);
  selected_item = index;
  m_items[index]->SetHighlightBgColor(selected_item_color);
  m_items[index]->SetHighlighted(true);
  SetFocusOn(m_items[index]);
  ScrollToItem(index);
  //m_items[index]->NeedRedrawing();
  NeedRedrawing();
}

void SelectBox::Deselect()
{
  ASSERT(always_one_selected == false);
  if (selected_item != -1) {
    m_items[selected_item]->SetHighlighted(false);
    //m_items[selected_item]->NeedRedrawing();
  }
  selected_item = -1;
  SetFocusOn(NULL);
  NeedRedrawing();
}

void SelectBox::ScrollToItem(uint index)
{
  if (m_items.empty())
    return;

  uint i=0;
  int offset = 0;
  for (i=0; i<m_items.size() && i<index ; i++) {
    offset += m_items[i]->GetSizeX() + GetMargin();
  }

  ScrollToPos(offset - GetSizeX()/2 + m_items[i]->GetSizeX()/2);
}

void SelectBox::RemoveSelected()
{
  ASSERT (always_one_selected == false);

  if (selected_item != -1) {
    if (last == m_items[selected_item])
      last = NULL;
    SetFocusOnPreviousWidget();
    RemoveWidget(m_items[selected_item]);
    m_items.erase(m_items.begin() + selected_item);
    selected_item =- 1;
  }
  NeedRedrawing();
}

int SelectBox::MouseIsOnWhichItem(const Point2i & mousePosition) const
{
  if (!Contains(mousePosition)) {
    return -1;
  }

  for (uint i=0; i<m_items.size(); i++) {
    if (m_items[i]->Contains(mousePosition))
      return i;
  }
  return -1;
}

//--------------------------------------------------------------------------
void ItemBox::AddItem(bool, Widget*)
{
  fprintf(stderr, "You must not use that accessor directly!\n");
  exit(1);
}

void ItemBox::AddItem(bool select, Widget* w, const void* value)
{
  // First put the value, because it is accessed by Select
  m_values.push_back(value);
  SelectBox::AddWidgetItem(select, w);
}

void ItemBox::RemoveSelected()
{
  if (selected_item != -1) {
    SelectBox::RemoveSelected();
    m_values.erase(m_values.begin() + selected_item);
  }
}

void ItemBox::AddLabelItem(bool selected,
                           const std::string & label,
                           const void* value,
                           Font::font_size_t fsize,
                           Font::font_style_t fstyle,
                           const Color & color)
{
  AddItem(selected,
          new Label(label, 200, fsize, fstyle,
                    color, Text::ALIGN_LEFT_CENTER, true),
          value);
}

const char* ItemBox::GetSelectedName() const
{
  if (selected_item==-1)
    return NULL;
  // Our accessors somewhat ensures that this is a label, but beware
  return static_cast<Label*>(m_items[selected_item])->GetText().c_str();
}
