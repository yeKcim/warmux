/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
#include <sstream>

#include "gui/spin_button_big.h"
#include "gui/button.h"
#include "graphic/text.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

SpinButtonBig::SpinButtonBig (const std::string &label, const Point2i &_size,
                              int value, int step, int min_value, int max_value) :
  AbstractSpinButton(value, step, min_value, max_value)
{
  position = Point2i(-1, -1);
  size = _size;

  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth(GetSizeX());

  txt_value = new Text("", dark_gray_color, Font::FONT_HUGE, Font::FONT_NORMAL, false);

  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(Font::FONT_HUGE)).GetWidth(max_value_s.str());

  uint margin = 5;
  m_plus = new Button(res, "menu/big_plus");
  m_plus->SetPosition(position.x + size.x - margin, position.y);
  m_minus = new Button(res, "menu/big_minus");
  m_minus->SetPosition(position.x + size.x - max_value_w - margin - 2 * margin, position.y);
  GetResourceManager().UnLoadXMLProfile( res);

  ValueHasChanged();
}

SpinButtonBig::~SpinButtonBig ()
{
  delete txt_label;
  delete txt_value;
  delete m_plus;
  delete m_minus;
}

void SpinButtonBig::Pack()
{
  // label can be multiline
  txt_label->SetMaxWidth(size.x);

  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = Font::GetInstance(Font::FONT_HUGE)->GetWidth(max_value_s.str());

  // center the value
  uint center_x = position.x + size.x/2 ;
  uint center_y = position.y + size.y/2 - txt_label->GetHeight()/2;

  m_minus->SetPosition(center_x - max_value_w/2 - m_minus->GetSizeX() - 5,
		       center_y - m_minus->GetSizeY()/2);

  m_plus->SetPosition(center_x + max_value_w/2 + 5,
		      center_y - m_plus->GetSizeY()/2);
}

void SpinButtonBig::Draw(const Point2i &mousePosition) const
{
  if (GetValue() != GetMinValue()) {
    m_minus->Draw(mousePosition);
  }
  if (GetValue() != GetMaxValue()) {
    m_plus->Draw(mousePosition);
  }

  uint center_x = position.x + (size.x/2);
  uint center_y = position.y + (size.y/2) - txt_label->GetHeight()/2;
  uint value_h = Font::GetInstance(Font::FONT_HUGE)->GetHeight();

  txt_value->DrawCenterTop(Point2i(center_x, center_y - value_h/2));

  txt_label->DrawCenterTop(Point2i(position.x + size.x/2,
                            position.y + size.y - txt_label->GetHeight()));
}

Widget* SpinButtonBig::ClickUp(const Point2i &mousePosition, uint button)
{
  NeedRedrawing();

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
