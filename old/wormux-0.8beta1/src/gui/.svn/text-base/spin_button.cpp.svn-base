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

#include "spin_button.h"
#include <sstream>
#include <iostream>
#include "include/app.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "graphic/font.h"

SpinButton::SpinButton (const std::string &label, const Rectanglei &rect,
			int value, int step, int min_value, int max_value,
			const Color& color, bool _shadowed)
{
  position =  rect.GetPosition();
  size = rect.GetSize();
  size.y = (*Font::GetInstance(Font::FONT_SMALL)).GetHeight();
  shadowed = _shadowed;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false); 

  txt_label = new Text(label, color, Font::FONT_SMALL, Font::FONT_NORMAL, shadowed);
  txt_label->SetMaxWidth(size.x - 30);

  if ( min_value != -1 && min_value <= value)
    m_min_value = min_value;
  else m_min_value = value/2;

  if ( max_value != -1 && max_value >= value)
    m_max_value = max_value;
  else m_max_value = value*2;

  txt_value = new Text("", color, Font::FONT_SMALL, Font::FONT_NORMAL, shadowed);
  SetValue(value);

  std::ostringstream max_value_s;
  max_value_s << m_max_value ;
  uint max_value_w = (*Font::GetInstance(Font::FONT_SMALL)).GetWidth(max_value_s.str());
  
  uint margin = 5;

  m_plus = new Button( Point2i(position.x + size.x - 5, position.y), res, "menu/plus");
  m_minus = new Button( Point2i(position.x + size.x - max_value_w - 5 - 2 * margin, position.y), res, "menu/minus");   
  resource_manager.UnLoadXMLProfile( res);
  m_step = step;
}

SpinButton::~SpinButton ()
{
  delete txt_label;
  delete txt_value;
  delete m_plus;
  delete m_minus;
}

void SpinButton::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);

  std::ostringstream max_value_s;
  max_value_s << m_max_value ;
  uint max_value_w = (*Font::GetInstance(Font::FONT_SMALL)).GetWidth(max_value_s.str());
  
  uint margin = 5;
  
  m_plus->SetSizePosition( Rectanglei(position.x + size.x - 5, position.y, 5, 10) );
  m_minus->SetSizePosition( Rectanglei(position.x + size.x - max_value_w - 5 - 2 * margin, position.y, 5, 10) );

  txt_label->SetMaxWidth(size.x - 30);
}

void SpinButton::Draw(const Point2i &mousePosition, Surface& surf) const
{
  txt_label->DrawTopLeft(position);
   
  m_minus->Draw(mousePosition, surf);
  m_plus->Draw(mousePosition, surf);

  uint center = (m_plus->GetPositionX() + 5 + m_minus->GetPositionX() )/2;
  txt_value->DrawCenterTop(center, position.y);
}

Widget* SpinButton::ClickUp(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  if( (button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
      (button == SDL_BUTTON_LEFT && m_minus->Contains(mousePosition)) ){
    SetValue(m_value - m_step);
    return this;
  } else if( (button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
	     (button == SDL_BUTTON_LEFT && m_plus->Contains(mousePosition)) ){
    SetValue(m_value + m_step);
    return this;
  }
  return NULL;
}

Widget* SpinButton::Click(const Point2i &mousePosition, uint button)
{
  return NULL;
}

int SpinButton::GetValue() const
{
  return m_value;
}

void SpinButton::SetValue(int value)  
{
  m_value = BorneLong(value, m_min_value, m_max_value);  

  std::ostringstream value_s;
  value_s << m_value ;

  std::string s(value_s.str());
  txt_value->Set(s);
}
