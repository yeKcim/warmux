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

#include "gui/spin_button_big.h"
#include "gui/button.h"
#include <sstream>
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "graphic/text.h"

SpinButtonBig::SpinButtonBig (const std::string &label, const Point2i &_size,
                              int value, int step, int min_value, int max_value) :
  AbstractSpinButton(value, step, min_value, max_value)
{
  position = Point2i(-1, -1);
  size = _size;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth(GetSizeX());

  txt_value = new Text("", dark_gray_color, Font::FONT_HUGE, Font::FONT_NORMAL, false);

  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(Font::FONT_HUGE)).GetWidth(max_value_s.str());

  uint margin = 5;
  m_plus = new Button(res, "menu/big_plus");
  m_plus->SetXY(position.x + size.x - margin, position.y);
  m_minus = new Button(res, "menu/big_minus");
  m_minus->SetXY(position.x + size.x - max_value_w - margin - 2 * margin, position.y);
  resource_manager.UnLoadXMLProfile( res);

  ValueHasChanged();
}

SpinButtonBig::~SpinButtonBig ()
{
  delete txt_label;
  delete txt_value;
  delete m_plus;
  delete m_minus;
}

void SpinButtonBig::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);

  // label can be multiline
  txt_label->SetMaxWidth(GetSizeX());

  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = Font::GetInstance(Font::FONT_HUGE)->GetWidth(max_value_s.str());

  // center the value
  uint center_x = GetPositionX() + GetSizeX()/2 ;
  uint center_y = GetPositionY() + GetSizeY()/2 - txt_label->GetHeight()/2;

  m_minus->SetSizePosition( Rectanglei(center_x - max_value_w/2 - m_minus->GetSizeX() - 5,
                            center_y - m_minus->GetSizeY()/2,
                            m_minus->GetSizeX(), m_minus->GetSizeY()) );
  m_plus->SetSizePosition( Rectanglei(center_x + max_value_w/2 + 5,
                           center_y - m_plus->GetSizeY()/2,
                           m_plus->GetSizeX(), m_plus->GetSizeY()) );
}

void SpinButtonBig::Draw(const Point2i &mousePosition, Surface& surf) const
{
  if (GetValue() != GetMinValue()) {
    m_minus->Draw(mousePosition, surf);
  }
  if (GetValue() != GetMaxValue()) {
    m_plus->Draw(mousePosition, surf);
  }

  uint center_x = GetPositionX() + (GetSizeX()/2);
  uint center_y = GetPositionY() + (GetSizeY()/2) - txt_label->GetHeight()/2;
  uint value_h = Font::GetInstance(Font::FONT_HUGE)->GetHeight();

  txt_value->DrawCenterTop(Point2i(center_x, center_y - value_h/2));

  txt_label->DrawCenterTop(Point2i(GetPositionX() + GetSizeX()/2,
                            GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

Widget* SpinButtonBig::ClickUp(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  if( (button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
      (button == SDL_BUTTON_LEFT && m_minus->Contains(mousePosition)) ){
    DecValue();
    return this;
  } else if( (button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
             (button == SDL_BUTTON_LEFT && m_plus->Contains(mousePosition)) ){
    IncValue();
    return this;
  }
  return NULL;
}

void SpinButtonBig::ValueHasChanged()
{
  std::ostringstream value_s;
  value_s << GetValue();

  std::string s(value_s.str());
  txt_value->Set(s);
}
