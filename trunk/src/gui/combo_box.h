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
 * A combo box
 *****************************************************************************/

#ifndef COMBO_BOX_H
#define COMBO_BOX_H

#include <WARMUX_base.h>
#include "graphic/font.h"
#include "graphic/surface.h"
#include "widget.h"
#include <string>
#include <vector>

class Text;
class Button;
class TorusCache;

class ComboBox : public Widget
{
  TorusCache *torus;

protected:
  Text *txt_label, *txt_value_white, *txt_value_black;

  std::vector<std::pair <std::string, std::string> > m_choices;
  std::vector<std::string>::size_type m_index;

  void RecreateTorus();

public:

  ComboBox(const std::string &label,
           const std::string &resource_id,
           const Point2i &size,
           const std::vector<std::pair <std::string, std::string> > &choices,
           const std::string& choice,
           Font::font_size_t legend_fsize = Font::FONT_SMALL,
           Font::font_size_t value_fsize = Font::FONT_MEDIUM);

  virtual ~ComboBox();

  virtual void Pack();
  virtual void Draw(const Point2i &mousePosition);
  virtual Widget* Click(const Point2i&, uint) const { return NULL; };
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  const std::string& GetValue() const { return m_choices[m_index].first; };
  int GetIntValue() const;
  void SetChoice(std::vector<std::string>::size_type index);
};

#endif
