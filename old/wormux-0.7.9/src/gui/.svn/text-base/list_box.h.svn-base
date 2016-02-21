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
 * Liste de choix.
 *****************************************************************************/

#ifndef LIST_BOX_H
#define LIST_BOX_H

#include <string>
#include <vector>
#include <SDL.h>
#include "widget.h"
#include "button.h"
#include "label.h"
#include "../include/base.h"

class ListBoxItem : public Label
{
private:
  std::string value;

public:
  ListBoxItem(const std::string& _label, Font& _font, const std::string& value,
	      const Color& color = white_color);

  const std::string& GetLabel() const;
  const std::string& GetValue() const;
};

class ListBox : public Widget
{
private:
  bool always_one_selected;

protected:
  // what are the items ?
  uint first_visible_item;
  int selected_item;
  std::vector<ListBoxItem*> m_items;

  // Buttons
  Button *m_up, *m_down;

public:
  ListBox (const Rectanglei &rect, bool always_one_selected_b = true);
  ~ListBox();

  void Draw(const Point2i &mousePosition, Surface& surf) const;
  Widget* Clic(const Point2i &mousePosition, uint button);
  void SetSizePosition(const Rectanglei &rect);

  void AddItem(bool selected, const std::string &label,
	       const std::string &value,
	       Font& font = *Font::GetInstance(Font::FONT_SMALL),
	       const Color& color = white_color);
  void Sort();

  int MouseIsOnWhichItem(const Point2i &mousePosition) const;

  void Select(uint index);
  void Select(const std::string& val);
  int GetSelectedItem() const;
  void Deselect();
  void RemoveSelected();
  const std::string& ReadLabel() const;
  const std::string& ReadValue() const;
  const std::string& ReadValue(int index) const;

  uint Size() const;
};

#endif
