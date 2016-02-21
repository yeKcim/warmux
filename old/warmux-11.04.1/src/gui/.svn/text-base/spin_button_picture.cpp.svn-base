/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include "include/app.h"
#include "gui/spin_button_picture.h"
#include "gui/torus_cache.h"
#include "graphic/polygon_generator.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/math_tools.h"
#include "tool/affine_transform.h"
#include "tool/resource_manager.h"

#define SMALL_R 25
#define BIG_R   35
#define OPEN_ANGLE 0.96f // 55

SpinButtonWithPicture::SpinButtonWithPicture(const std::string& label,
                                             const std::string& resource_id,
                                             const Point2i& _size,
                                             int value, int step,
                                             int min_value, int max_value)
  : AbstractSpinButton(value, step, min_value, max_value)
{
  position = Point2i(-1, -1);
  size = _size;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  torus = new TorusCache(res, resource_id, BIG_R, SMALL_R);

  txt_label = new Text(label, dark_gray_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth(GetSizeX());

  txt_value_black = new Text("", black_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_value_white = new Text("", white_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);

  ValueHasChanged();
}

SpinButtonWithPicture::~SpinButtonWithPicture ()
{
  delete txt_label;
  delete txt_value_black;
  delete txt_value_white;
  delete torus;
}

void SpinButtonWithPicture::Pack()
{
  txt_label->SetMaxWidth(size.x);
}

void SpinButtonWithPicture::Draw(const Point2i &mousePosition)
{
  Surface& surf = GetMainWindow();

  //  the computed positions are to center on the image part of the widget

  // 1. draw torus
  torus->Draw(*this);

  // 2. then draw buttons
  #define IMG_BUTTONS_W 5
  #define IMG_BUTTONS_H 12

  Point2i center = GetPosition() + torus->GetCenter();
  if (GetValue() > GetMinValue()) {

    if (Contains(mousePosition) && mousePosition.x < center.x)
      torus->m_minus->SetCurrentFrame(1);
    else
      torus->m_minus->SetCurrentFrame(0);

    torus->m_minus->Blit(surf, GetPosition().x + IMG_BUTTONS_W, GetPosition().y + IMG_BUTTONS_H);
  }

  if (GetValue() < GetMaxValue()) {
    if (Contains(mousePosition) && mousePosition.x > center.x)
      torus->m_plus->SetCurrentFrame(1);
    else
      torus->m_plus->SetCurrentFrame(0);

    torus->m_plus->Blit(surf, GetPosition().x + GetSize().x - torus->m_plus->GetWidth() - IMG_BUTTONS_W,
                        GetPosition().y + IMG_BUTTONS_H);
  }

  // 6. add in the value image
  int tmp_x = center.x;
  int tmp_y = center.y + SMALL_R - 3;
  uint value_h = Font::GetInstance(Font::FONT_MEDIUM)->GetHeight();

  txt_value_black->DrawCenterTop(Point2i(tmp_x + 1, tmp_y + 1 - value_h/2));
  txt_value_white->DrawCenterTop(Point2i(tmp_x, tmp_y - value_h/2));

  // 7. and finally the label image
  txt_label->DrawCenterTop(Point2i(GetPositionX() + GetSizeX()/2,
                                   GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

void SpinButtonWithPicture::RecreateTorus()
{
  float angle = (2.0f*M_PI - OPEN_ANGLE) * (GetValue() - GetMinValue())
              / (GetMaxValue() - GetMinValue());
  torus->Refresh(angle, OPEN_ANGLE);
}

Widget* SpinButtonWithPicture::ClickUp(const Point2i &mousePosition, uint button)
{
  NeedRedrawing();

  if (!Contains(mousePosition)) {
    return NULL;
  }

  bool is_click = Mouse::IS_CLICK_BUTTON(button);
  //if (button == Mouse::BUTTON_RIGHT() || button == SDL_BUTTON_WHEELDOWN) {
  if ( (is_click && mousePosition.x <= (GetPositionX() + GetSizeX()/2))
       || button == SDL_BUTTON_WHEELDOWN ) {
    DecValue();
    return this;
  } else if ( (is_click && mousePosition.x > (GetPositionX() + GetSizeX()/2))
              || button == SDL_BUTTON_WHEELUP ) {
  //} else if (button == Mouse::BUTTON_LEFT() || button == SDL_BUTTON_WHEELUP) {
    IncValue();
    return this;
  }

  return NULL;
}

void SpinButtonWithPicture::ValueHasChanged()
{
  std::ostringstream value_s;
  value_s << GetValue();

  std::string s(value_s.str());
  txt_value_black->SetText(s);
  txt_value_white->SetText(s);
  RecreateTorus();
}
