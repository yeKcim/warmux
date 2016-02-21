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
 *****************************************************************************/

#include "spin_button_picture.h"
#include <sstream>
#include <iostream>
#include "include/app.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "graphic/font.h"

SpinButtonWithPicture::SpinButtonWithPicture (const std::string &label, const std::string &resource_id,
					      const Rectanglei &rect,
					      int value, int step, int min_value, int max_value)
{
  position =  rect.GetPosition();
  size = rect.GetSize();

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false); 
  m_image = resource_manager.LoadImage(res, resource_id);
  resource_manager.UnLoadXMLProfile( res); 

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth(GetSizeX());

  if ( min_value != -1 && min_value <= value)
    m_min_value = min_value;
  else m_min_value = value/2;

  if ( max_value != -1 && max_value >= value)
    m_max_value = max_value;
  else m_max_value = value*2;

  txt_value = new Text("", dark_gray_color, Font::FONT_LARGE, Font::FONT_NORMAL, false);
  SetValue(value);

  m_step = step;
}

SpinButtonWithPicture::~SpinButtonWithPicture ()
{
  delete txt_label;
  delete txt_value;
}

void SpinButtonWithPicture::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());
}

void SpinButtonWithPicture::Draw(const Point2i &mousePosition, Surface& surf) const
{
  // center the image on the first half
  uint tmp_x = GetPositionX() + (GetSizeX() - m_image.GetWidth())/4 ;
  uint tmp_y = GetPositionY() + (GetSizeY() - m_image.GetHeight() - txt_label->GetHeight() - 5) /2;

  AppWormux::GetInstance()->video.window.Blit(m_image, Point2i(tmp_x, tmp_y));

  tmp_x = GetPositionX() + (3*GetSizeX()/4);
  tmp_y = GetPositionY() + (GetSizeY()/2) - txt_label->GetHeight()/2;

  uint value_h = Font::GetInstance(Font::FONT_HUGE)->GetHeight();

  txt_value->DrawCenterTop(tmp_x, tmp_y - value_h/2);

  txt_label->DrawCenterTop( GetPositionX() + GetSizeX()/2, 
			    GetPositionY() + GetSizeY() - txt_label->GetHeight() );
}

Widget* SpinButtonWithPicture::ClickUp(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  if (button == SDL_BUTTON_LEFT && Contains(mousePosition)) {  
    
    m_value += m_step;
    if (m_value > m_max_value) SetValue(m_min_value);
    else SetValue(m_value);

  } else if (button == SDL_BUTTON_RIGHT && Contains(mousePosition)) {  
    
    m_value -= m_step;
    if (m_value < m_min_value) SetValue(m_max_value);
    else SetValue(m_value);

  } else if( button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition) ) {
    
    SetValue(m_value - m_step);
    return this;
    
  } else if( button == SDL_BUTTON_WHEELUP && Contains(mousePosition) ) {
    
    SetValue(m_value + m_step);
    return this;
  }
  return NULL;
}

Widget* SpinButtonWithPicture::Click(const Point2i &mousePosition, uint button)
{
  return NULL;
}

int SpinButtonWithPicture::GetValue() const
{
  return m_value;
}

void SpinButtonWithPicture::SetValue(int value)  
{
  m_value = BorneLong(value, m_min_value, m_max_value);  

  std::ostringstream value_s;
  value_s << m_value ;

  std::string s(value_s.str());
  txt_value->Set(s);

  ForceRedraw();
}
