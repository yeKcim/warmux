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
#include "graphic/text.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

PictureTextCBox::PictureTextCBox(const std::string &label,
                                 const std::string &resource_id,
                                 const Rectanglei &rect, bool value):
  CheckBox(new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false), rect, value)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  m_image = resource_manager.LoadImage(res, resource_id);
  m_enabled = resource_manager.LoadImage(res, "menu/enabled");
  m_disabled_front = resource_manager.LoadImage(res, "menu/disabled_front");
  m_disabled_back = resource_manager.LoadImage(res, "menu/disabled_back");
  resource_manager.UnLoadXMLProfile( res);

  //SetPosition( rect.GetPosition() );
  //SetSize( rect.GetSize() );

  SetSizeY( m_image.GetHeight() + (*Font::GetInstance(Font::FONT_MEDIUM, Font::FONT_BOLD)).GetHeight() );
  m_value = value;

  txt_label->SetMaxWidth (GetSizeX());
}

void PictureTextCBox::Draw(const Point2i &/*mousePosition*/, Surface& /*surf*/) const
{
  Surface video_window = AppWormux::GetInstance()->video->window;

  if (!hidden)
    {
      if (m_value)
        {
          uint enabled_x = GetPositionX() + (GetSizeX() - m_enabled.GetWidth() - 20)/2 ;
          uint enabled_y = GetPositionY() + (GetSizeY() - m_enabled.GetHeight() - txt_label->GetHeight() - 5) /2;
          uint outside_x = max(uint(0), GetPositionX() - enabled_x);
          uint outside_y = max(uint(0), GetPositionY() - enabled_y);

          enabled_x+= outside_x;
          enabled_y+= outside_y;
          Rectanglei srcRect(outside_x, outside_y, m_enabled.GetWidth() - outside_x,
                             m_enabled.GetHeight() - outside_y);
          video_window.Blit(m_enabled, srcRect, Point2i(enabled_x, enabled_y));
        }
      else
        {
          uint disabled_x = GetPositionX() + (GetSizeX() - m_disabled_back.GetWidth() - 20)/2 ;
          uint disabled_y = GetPositionY() + (GetSizeY() - m_disabled_back.GetHeight() - txt_label->GetHeight() - 5) /2;
          uint outside_x = max(uint(0), GetPositionX() - disabled_x);
          uint outside_y = max(uint(0), GetPositionY() - disabled_y);

          disabled_x+= outside_x;
          disabled_y+= outside_y;
          Rectanglei srcRect(outside_x, outside_y, m_disabled_back.GetWidth() - outside_x,
                             m_disabled_back.GetHeight() - outside_y);
          video_window.Blit(m_disabled_back, srcRect, Point2i(disabled_x, disabled_y));
        }
      // center the image
      uint tmp_x = GetPositionX() + (GetSizeX() - m_image.GetWidth() - 20)/2 ;
      uint tmp_y = GetPositionY() + (GetSizeY() - m_image.GetHeight() - txt_label->GetHeight() - 5) /2;

      video_window.Blit(m_image, Point2i(tmp_x, tmp_y));

      txt_label->DrawCenterTop(GetPosition()
                               + Point2i(GetSizeX()/2,
                                         GetSizeY() - txt_label->GetHeight()));

      if (!m_value)
        {
          uint disabled_x = GetPositionX() + (GetSizeX() - m_disabled_front.GetWidth() - 20)/2 ;
          uint disabled_y = GetPositionY() + (GetSizeY() - m_disabled_front.GetHeight() - txt_label->GetHeight() - 5) /2;
          video_window.Blit(m_disabled_front, Point2i(disabled_x, disabled_y));
        }
    }
}

void PictureTextCBox::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth (GetSizeX());
}
