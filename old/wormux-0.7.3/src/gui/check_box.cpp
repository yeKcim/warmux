/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
#include "../include/app.h"
#include "../graphic/font.h"
#include "../graphic/sprite.h"
#include "../tool/resource_manager.h"

CheckBox::CheckBox(const std::string &label, const Rectanglei &rect, bool value){
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);   
  m_image = resource_manager.LoadSprite( res, "menu/check");
  m_image->cache.EnableLastFrameCache();
 
  SetPosition( rect.GetPosition() );
  SetSize( rect.GetSize() );

  SetSizeY( (*Font::GetInstance(Font::FONT_SMALL)).GetHeight() );
  m_value = value;

  txt_label = new Text(label, white_color, Font::GetInstance(Font::FONT_SMALL));
}

CheckBox::~CheckBox(){
  delete m_image;
  delete txt_label;
}

void CheckBox::Draw(const Point2i &mousePosition){
  txt_label->DrawTopLeft( GetPosition() );
 
  if (m_value)
    m_image->SetCurrentFrame(0);
  else 
    m_image->SetCurrentFrame(1);

  m_image->Blit(AppWormux::GetInstance()->video.window, GetPositionX() + GetSizeX() - 16, GetPositionY());
}

bool CheckBox::Clic(const Point2i &mousePosition, uint button){
  if( !Contains(mousePosition) )
    return false;

  m_value = !m_value;
  return true ;
}

void CheckBox::SetSizePosition(const Rectanglei &rect){
  StdSetSizePosition(rect);
}

bool CheckBox::GetValue() const{
  return m_value;
}

void CheckBox::SetValue(bool value){
  m_value = value;
}

