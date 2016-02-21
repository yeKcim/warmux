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
 * A combo box
 *****************************************************************************/

#ifndef COMBO_BOX_H
#define COMBO_BOX_H

#include "include/base.h"
#include "graphic/surface.h"
#include "widget.h"
#include <string>
#include <vector>

class Text;
class Button;

class ComboBox : public Widget
{
 private:
  /* If you need this, implement it (correctly)*/
  ComboBox(const ComboBox&);
  ComboBox operator=(const ComboBox&);
  /*********************************************/

  Surface m_image;
  Surface m_annulus_background;
  Surface m_annulus_foreground;
  Color m_progress_color;

 protected:
  Text *txt_label, *txt_value_white, *txt_value_black;

  std::vector<std::pair <std::string, std::string> > m_choices;
  std::vector<std::string>::size_type m_index;

 public:
  
  ComboBox(const std::string &label,
	   const std::string &resource_id,
	   const Point2i &size,
	   const std::vector<std::pair <std::string, std::string> > &choices,
	   const std::string choice);

  virtual ~ComboBox();

  void SetSizePosition(const Rectanglei &rect);

  void Draw(const Point2i &mousePosition, Surface& surf) const;
  Widget* Click(const Point2i&, uint) const { return NULL; };
  Widget* ClickUp(const Point2i &mousePosition, uint button);
  const std::string GetValue() const { return m_choices[m_index].first; };
  const int GetIntValue() const;
  void SetChoice(std::vector<std::string>::size_type index);
};

#endif
