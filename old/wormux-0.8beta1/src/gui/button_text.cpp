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
 * Button with text.
 *****************************************************************************/

#include "button_text.h"
#include "include/app.h"

ButtonText::~ButtonText(){
  delete text;
}

ButtonText::ButtonText(Point2i position,
		       const Profile *res_profile,
		       const std::string& resource_id,
		       const std::string &new_text,
		       Font::font_size_t font_size,
		       Font::font_style_t font_style):
  Button(position, res_profile, resource_id),
  text(new Text(new_text, white_color, font_size, font_style))
{ }

void ButtonText::Draw(const Point2i &mousePosition, Surface& surf) const
{
  Point2i textPosition = position + size/2;

  Button::Draw(mousePosition, surf);
  text->DrawCenter(textPosition);
}


