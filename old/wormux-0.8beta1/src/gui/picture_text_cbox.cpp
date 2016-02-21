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
 * A beautiful checkbox with picture and text
 *****************************************************************************/

#include "picture_text_cbox.h"
#include "include/app.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "tool/resource_manager.h"

PictureTextCBox::PictureTextCBox(const std::string &label, const std::string &resource_id, 
				 const Rectanglei &rect, bool value):
  CheckBox(label, rect, value)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);   
  m_image = resource_manager.LoadImage(res, resource_id);
  resource_manager.UnLoadXMLProfile( res);
 
  SetPosition( rect.GetPosition() );
  SetSize( rect.GetSize() );

  SetSizeY( m_image.GetHeight() + (*Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)).GetHeight() );
  m_value = value;

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth (GetSizeX());
}

void PictureTextCBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  if (!hidden)
    {
      // center the image
      uint tmp_x = GetPositionX() + (GetSizeX() - m_image.GetWidth() - 20)/2 ;
      uint tmp_y = GetPositionY() + (GetSizeY() - m_image.GetHeight() - txt_label->GetHeight() - 5) /2;
      
      AppWormux::GetInstance()->video.window.Blit(m_image, Point2i(tmp_x, tmp_y));
      
      txt_label->DrawCenterTop( GetPositionX() + GetSizeX()/2, 
				GetPositionY() + GetSizeY() - txt_label->GetHeight() );
      
      if (m_value)
	m_checked_image->SetCurrentFrame(0);
      else 
	m_checked_image->SetCurrentFrame(1);
      
      m_checked_image->Blit(surf, 
			    GetPositionX() + GetSizeX() - 16, 
			    GetPositionY() + (GetSizeY()-16)/2 );
    }
}

void PictureTextCBox::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth (GetSizeX());
}
