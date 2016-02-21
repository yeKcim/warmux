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
 * A really big Spinbutton in GUI.
 *****************************************************************************/

#ifndef SPIN_BUTTON_BIG_H
#define SPIN_BUTTON_BIG_H

#include "include/base.h"
#include "widget.h"
#include "abstract_spin_button.h"
#include <string>

class Text;
class Button;

class SpinButtonBig : public AbstractSpinButton
{
  /* If you need this, implement it (correctly)*/
  SpinButtonBig(const SpinButtonBig&);
  SpinButtonBig operator=(const SpinButtonBig&);
  /*********************************************/

 protected:
  Text *txt_label, *txt_value;
  Button *m_plus, *m_minus;

 public:
  SpinButtonBig(const std::string &label, const Point2i &size,
                int value, int step, int min_value, int max_value);
  virtual ~SpinButtonBig();

  // From Widget
  virtual void Pack();
  virtual void Draw(const Point2i &mousePosition) const;
  virtual Widget* Click(const Point2i &/*mousePosition*/, uint /*button*/) const { return NULL; };
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // From AbstractSpinButton
  virtual void ValueHasChanged();
};

#endif

