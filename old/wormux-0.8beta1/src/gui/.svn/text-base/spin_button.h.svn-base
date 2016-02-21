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
 * Spinbutton in GUI.
 *****************************************************************************/

#ifndef SPIN_BUTTON_H
#define SPIN_BUTTON_H

#include "include/base.h"
#include "graphic/text.h"
#include "widget.h"
#include "button.h"
#include <string>

class SpinButton : public Widget
{
 private:
  /* If you need this, implement it (correctly)*/
  SpinButton(const SpinButton&);
  SpinButton operator=(const SpinButton&);
  /*********************************************/

  bool shadowed;

  Text *txt_label, *txt_value;

  int m_value;
  int m_min_value, m_max_value, m_step;
  Button *m_plus, *m_minus;

 public:
  SpinButton(const std::string &label, const Rectanglei &rect,
	     int value=0, int step=1, int min_value=-1, int max_value=-1,
	     const Color& color = white_color, bool shadowed = true);
  virtual ~SpinButton();

  void SetSizePosition(const Rectanglei &rect);

  void Draw(const Point2i &mousePosition, Surface& surf) const;
  Widget* Click(const Point2i &mousePosition, uint button);
  Widget* ClickUp(const Point2i &mousePosition, uint button);
  int GetValue() const;
  void SetValue(int value);
};

#endif

