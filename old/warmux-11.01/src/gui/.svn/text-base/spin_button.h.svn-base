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
 * Spinbutton in GUI.
 *****************************************************************************/

#ifndef SPIN_BUTTON_H
#define SPIN_BUTTON_H

#include <WARMUX_base.h>
#include "graphic/colors.h"
#include "widget.h"
#include "abstract_spin_button.h"
#include <string>
#include "gui/label.h"

class Button;
//class Text;

class SpinButton : public AbstractSpinButton
{
  bool     shadowed;
  Label *  txtLabel;
  Label *  txtValue;
  Button * m_plus;
  Button * m_minus;

public:
  SpinButton(const std::string & label,
             int width,
             int value,
             int step,
             int min_value,
             int max_value,
             const Color & color = white_color,
             bool shadowed = true);
  SpinButton(Profile * profile,
             const xmlNode * spinButtonNode);
  virtual ~SpinButton(void);

  // From Widget
  virtual void Pack();
  virtual void Draw(const Point2i & mousePosition);
  virtual Widget * Click(const Point2i &/*mousePosition*/, uint /*button*/) const { return NULL; };
  virtual Widget * ClickUp(const Point2i & mousePosition, uint button);
  virtual bool LoadXMLConfiguration(void);

  // From AbstractSpinButton
  virtual void ValueHasChanged();
};

#endif

