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
 * A beautiful button with picture and text
 *****************************************************************************/

#include "gui/big/button_pic.h"
#include "include/app.h"
#include "graphic/text.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

ButtonPic::ButtonPic(const std::string &label,
		     const std::string &resource_id,
		     const Point2i &_size)
{
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  m_img_normal = resource_manager.LoadImage(res, resource_id);
  resource_manager.UnLoadXMLProfile(res);

  SetSize(_size);

  SetSizeY( m_img_normal.GetHeight() + (*Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)).GetHeight() );

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth (GetSizeX());
}

void ButtonPic::Draw(const Point2i &mousePosition, Surface& surf) const
{
  // center the image horizontally
  uint tmp_x = GetPositionX() + (GetSizeX() - m_img_normal.GetWidth())/2 ;
  uint tmp_y = GetPositionY();

  surf.Blit(m_img_normal, Point2i(tmp_x, tmp_y));

  if (Contains(mousePosition)) {
    surf.RectangleColor(*this, c_yellow, 1);
    txt_label->SetColor(black_color);
  } else {
    txt_label->SetColor(dark_gray_color);
  }

  txt_label->DrawCenterTop(GetPosition()
			   + Point2i(GetSizeX()/2,
				     GetSizeY() - txt_label->GetHeight()));
}

void ButtonPic::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth (GetSizeX());
}
