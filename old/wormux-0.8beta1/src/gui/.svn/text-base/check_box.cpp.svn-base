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
 * Checkbox in GUI.
 *****************************************************************************/

#include "check_box.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "tool/resource_manager.h"

CheckBox::CheckBox(const std::string &label, const Rectanglei &rect, bool value):
  txt_label(new Text(label, white_color, Font::FONT_SMALL, Font::FONT_NORMAL)),
  m_value(value),
  m_checked_image(NULL),
  hidden(false)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  m_checked_image = resource_manager.LoadSprite( res, "menu/check");
  resource_manager.UnLoadXMLProfile( res);

  m_checked_image->cache.EnableLastFrameCache();

  SetPosition( rect.GetPosition() );
  SetSize( rect.GetSize() );

  SetSizeY( (*Font::GetInstance(Font::FONT_SMALL)).GetHeight() );
}

CheckBox::~CheckBox()
{
  delete m_checked_image;
  delete txt_label;
}

void CheckBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  if (!hidden)
    {
      txt_label->DrawTopLeft( GetPosition() );

      if (m_value)
	m_checked_image->SetCurrentFrame(0);
      else
	m_checked_image->SetCurrentFrame(1);

      m_checked_image->Blit(surf, GetPositionX() + GetSizeX() - 16, GetPositionY());
    }
}

Widget* CheckBox::Click(const Point2i &mousePosition, uint button)
{
  // do nothing since user has not released the button
  return this;
}

Widget* CheckBox::ClickUp(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;
  m_value = !m_value;
  return this;
}

void CheckBox::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
}

bool CheckBox::GetValue() const
{
  return m_value;
}

void CheckBox::SetValue(bool value)
{
  m_value = value;
}

void CheckBox::SetVisible(bool visible)
{
  if (hidden == visible) {
    hidden = !visible;
    need_redrawing = true;
  }
}
