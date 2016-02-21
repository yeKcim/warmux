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
 * Checkbox in GUI.
 *****************************************************************************/

#ifndef CHECK_BOX_H
#define CHECK_BOX_H

#include "include/base.h"
#include "gui/widget.h"
#include <string>

class Sprite;
class Text;

class CheckBox : public Widget
{
  /* If you need this, implement it (correctly)*/
 CheckBox(const CheckBox&);
 CheckBox operator=(const CheckBox&);
 /**********************************************/
 void Init(uint width);

 protected:
  Text *txt_label;
  bool m_value;
  Sprite *m_checked_image;

 public:
  CheckBox(const std::string &label, uint width, bool value = true);
  CheckBox(Text* text, uint width, bool value = true);
  ~CheckBox();

  virtual void Draw(const Point2i &mousePosition) const;
  virtual Widget* Click(const Point2i&, uint) { return this; };
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
  virtual void Pack();

  bool GetValue() const { return m_value; };
  void SetValue(bool value) { m_value = value; };
};

#endif

