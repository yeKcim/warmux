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
 * Liste de choix.
 *****************************************************************************/

#ifndef LIST_BOX_H
#define LIST_BOX_H

#include <string>
#include <vector>
#include "widget.h"
#include "label.h"
#include "include/base.h"

class Button;

class BaseListBox : public Widget
{
  /* If you need this, implement it (correctly)*/
  BaseListBox(const BaseListBox&);
  BaseListBox operator=(const BaseListBox&);
  /*********************************************/

  bool always_one_selected;

protected:
  bool scrolling;
  // what are the items ?
  uint first_visible_item;
  int selected_item;
  std::vector<Widget*> m_items;

  // Buttons
  Button *m_up, *m_down;

  // Colors
  Color selected_item_color;
  Color default_item_color;

  uint margin; // for BaseListBoxWithLabel

  Rectanglei ScrollBarPos() const;
  virtual void __Update(const Point2i &mousePosition,
			const Point2i &lastMousePosition);
  void AddWidgetItem(bool selected, Widget* w);

public:
  void SetSelectedItemColor(const Color & selected_item) { selected_item_color = selected_item; };
  void SetDefaultItemColor(const Color & default_item) { default_item_color = default_item; };

  BaseListBox (const Point2i &size, bool always_one_selected_b = true);
  ~BaseListBox();

  virtual void Draw(const Point2i &mousePosition) const;

  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
  virtual void Pack();

  void Sort() const;

  int MouseIsOnWhichItem(const Point2i &mousePosition) const;

  void Select(uint index);
  int GetSelectedItem() const { return selected_item; };
  void Deselect();
  void RemoveSelected();
  void ClearItems();

  uint Size() const { return m_items.size(); };
};

class ListBoxItem;

class ListBox : public BaseListBox
{
  static const ListBoxItem* GetItem(const Widget* w) { return (const ListBoxItem*)w; }
public:
  ListBox(const Point2i &size, bool b = true) : BaseListBox(size, b) { }
  void AddItem(bool selected, const std::string &label,
               const std::string &value,
               Font::font_size_t fsize = Font::FONT_SMALL,
               Font::font_style_t fstyle = Font::FONT_NORMAL,
               const Color& color = white_color);
  void Select(const std::string& val);
  void Select(uint index) { BaseListBox::Select(index); }
  const std::string& ReadLabel() const;
  const std::string& ReadValue() const;
  int ReadIntValue() const;
  const std::string& ReadValue(int index) const;
};

#endif
