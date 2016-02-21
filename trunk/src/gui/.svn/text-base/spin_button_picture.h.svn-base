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
 * A big Spinbutton in
 *****************************************************************************/

#ifndef SPIN_BUTTON_PICTURE_H
#define SPIN_BUTTON_PICTURE_H

#include <WARMUX_base.h>
#include "graphic/font.h"
#include "graphic/surface.h"
#include "widget.h"
#include "abstract_spin_button.h"
#include <string>

class Text;
class Button;
class TorusCache;


class SpinButtonWithPicture : public AbstractSpinButton
{
  TorusCache *torus;

 protected:
  Text *txt_label, *txt_value_white, *txt_value_black;

  void RecreateTorus();

 public:
  SpinButtonWithPicture(const std::string &label,
                        const std::string &resource_id,
                        const Point2i &size,
                        int value, int step,
                        int min_value, int max_value,
                        Font::font_size_t legend_fsize = Font::FONT_SMALL,
                        Font::font_size_t value_fsize = Font::FONT_MEDIUM);
  virtual ~SpinButtonWithPicture();

  // From Widget
  virtual void Pack();
  virtual void Draw(const Point2i &mousePosition);
  virtual Widget* Click(const Point2i &/*mousePosition*/, uint /*button*/) const { return NULL; };
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // From AbstractSpinButton
  virtual void ValueHasChanged();
};

#endif

