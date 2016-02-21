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

#ifndef SELECT_BOX
#define SELECT_BOX

#include "gui/label.h"
#include "gui/scroll_box.h"

class SelectBox : public ScrollBox
{
protected:
  // Colors
  Color selected_item_color;
  Color default_item_color;

  // Selection
  std::vector<Widget*> m_items;
  bool always_one_selected;
  int  selected_item;
  Widget *last;

  virtual void __Update(const Point2i & mousePosition,
                        const Point2i & lastMousePosition);

public:
  SelectBox(const Point2i& size,
            bool always_one_selected = true,
            bool force_widget_size = true,
            bool vertical = true);

  // No need for a Draw method: the additional stuff drawn is made by Update
  virtual bool Update(const Point2i& mousePosition,
                      const Point2i& lastMousePosition);
  virtual Widget* ClickUp(const Point2i & mousePosition, uint button);

  // to add a widget
  virtual void RemoveSelected();
  virtual void AddWidget(Widget* w);
  virtual void AddWidgetItem(bool select, Widget* w);
  virtual void Empty();
  virtual void Clear();

  // Specific to selection
  int MouseIsOnWhichItem(const Point2i & mousePosition) const;
  Widget* GetSelectedWidget() const { return (selected_item==-1) ? NULL : m_items[selected_item]; };
  int GetSelectedItem() const { return selected_item; };
  bool IsItemSelected() const { return selected_item != -1; }
  int Size() { return m_items.size(); }

  void SetSelectedItemColor(const Color& color) { selected_item_color = color; };
  void SetDefaultItemColor(const Color& color) { default_item_color = color; };
  virtual void Select(uint index);
  void Deselect();
  void ScrollToItem(uint index);
  Widget* MouseIsOnWhichWidget(const Point2i & mousePosition) const
  {
    int index = MouseIsOnWhichItem(mousePosition);
    return (index==-1) ? NULL : m_items[selected_item];
  }
  const std::vector<Widget*>& GetWidgets() { return m_items; }
};

class ItemBox : public SelectBox
{
protected:
  std::vector<const void*> m_values;

public:
  ItemBox(const Point2i& size, bool always = false,
          bool force = true, bool vertical = true)
    : SelectBox(size, always, force, vertical) { };
  // Should not be used and thus prevents its use!
  void AddItem(bool select, Widget* w);
  void AddItem(bool select, Widget* w, const void* value);
  void AddLabelItem(bool selected,
                    const std::string & label,
                    const void* value,
                    Font::font_size_t fsize = Font::FONT_SMALL,
                    Font::font_style_t fstyle = Font::FONT_BOLD,
                    const Color & color = white_color);
  virtual void RemoveSelected();
  virtual void Empty() { m_values.clear(); SelectBox::Empty(); }
  virtual void Clear() { m_values.clear(); SelectBox::Clear(); }

  const void* GetSelectedValue() const { return (selected_item==-1) ? NULL : m_values[selected_item]; }
  // Our accessors somewhat ensures that this is a label, but beware
  const char* GetSelectedName() const;
};

#endif //SELECT_BOX
