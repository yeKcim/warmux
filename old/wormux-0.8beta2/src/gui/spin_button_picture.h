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
 * A big Spinbutton in
 *****************************************************************************/

#ifndef SPIN_BUTTON_PICTURE_H
#define SPIN_BUTTON_PICTURE_H

#include "include/base.h"
#include "graphic/surface.h"
#include "widget.h"
#include <string>

class Text;
class Button;

class SpinButtonWithPicture : public Widget
{
 private:
  /* If you need this, implement it (correctly)*/
  SpinButtonWithPicture(const SpinButtonWithPicture&);
  SpinButtonWithPicture operator=(const SpinButtonWithPicture&);
  /*********************************************/

  Surface m_image;
  Surface m_annulus_background;
  Surface m_annulus_foreground;
  Color m_progress_color;

 protected:
  Text *txt_label, *txt_value_white, *txt_value_black;

  int m_value;
  int m_min_value, m_max_value, m_step;

 public:
  SpinButtonWithPicture(const std::string &label,
                        const std::string &resource_id,
                        const Rectanglei &rect,
                        int value=0, int step=1,
                        int min_value=-1, int max_value=-1);
  virtual ~SpinButtonWithPicture();

  void SetSizePosition(const Rectanglei &rect);

  void Draw(const Point2i &mousePosition, Surface& surf) const;
  Widget* Click(const Point2i&, uint) const { return NULL; };
  Widget* ClickUp(const Point2i &mousePosition, uint button);
  int GetValue() const { return m_value; };
  void SetValue(int value);
};

#endif

