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

#include "gui/spin_button.h"
#include "graphic/text.h"
#include "gui/button.h"
#include <sstream>
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "gui/button.h"

SpinButton::SpinButton (const std::string &label, int width,
                        int value, int step, int min_value, int max_value,
                        const Color& color, bool _shadowed) :
  AbstractSpinButton(value, step, min_value, max_value)
{
  position = Point2i(-1, -1);
  size.x = width;
  size.y = (*Font::GetInstance(Font::FONT_SMALL)).GetHeight();
  shadowed = _shadowed;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  txt_label = new Text(label, color, Font::FONT_SMALL, Font::FONT_NORMAL, shadowed);
  txt_label->SetMaxWidth(size.x - 30);

  txt_value = new Text("", color, Font::FONT_SMALL, Font::FONT_NORMAL, shadowed);
  std::ostringstream max_value_s;
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(Font::FONT_SMALL)).GetWidth(max_value_s.str());

  uint margin = 5;

  m_plus = new Button(res, "menu/plus");
  m_plus->SetXY(position.x + size.x - 5, position.y);
  m_minus = new Button(res, "menu/minus");
  m_minus->SetXY(position.x + size.x - max_value_w - 5 - 2 * margin, position.y);
  resource_manager.UnLoadXMLProfile( res);

  ValueHasChanged();
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
  max_value_s << GetMaxValue();
  uint max_value_w = (*Font::GetInstance(Font::FONT_SMALL)).GetWidth(max_value_s.str());

  uint margin = 5;

  m_plus->SetSizePosition( Rectanglei(position.x + size.x - 5, position.y, 5, 10) );
  m_minus->SetSizePosition( Rectanglei(position.x + size.x - max_value_w - 5 - 2 * margin, position.y, 5, 10) );

  txt_label->SetMaxWidth(size.x - 30);
}

void SpinButton::Draw(const Point2i &mousePosition, Surface& surf) const
{
  txt_label->DrawTopLeft(position);

  if (GetValue() != GetMinValue()) {
    m_minus->Draw(mousePosition, surf);
  }
  if (GetValue() != GetMaxValue()) {
    m_plus->Draw(mousePosition, surf);
  }

  uint center = (m_plus->GetPositionX() + 5 + m_minus->GetPositionX() )/2;
  txt_value->DrawCenterTop(Point2i(center, position.y));
}

Widget* SpinButton::ClickUp(const Point2i &mousePosition, uint button)
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

void SpinButton::ValueHasChanged()
{
  std::ostringstream value_s;
  value_s << GetValue() ;

  std::string s(value_s.str());
  txt_value->Set(s);
}
