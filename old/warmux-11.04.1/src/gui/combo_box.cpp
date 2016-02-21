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

#include <vector>
#include <sstream>
#include "graphic/polygon_generator.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/combo_box.h"
#include "gui/torus_cache.h"
#include "include/app.h"
#include "tool/affine_transform.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

#define SMALL_R 25
#define BIG_R   35
#define OPEN_ANGLE 0.96f // 55

ComboBox::ComboBox (const std::string &label,
                    const std::string &resource_id,
                    const Point2i &_size,
                    const std::vector<std::pair<std::string, std::string> > &choices,
                    const std::string choice)
  : m_choices(choices)
  , m_index(0)
{
  position = Point2i(-1, -1);
  size = _size;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  torus = new TorusCache(res, resource_id, BIG_R, SMALL_R);

  txt_label = new Text(label, dark_gray_color, Font::FONT_SMALL, Font::FONT_BOLD, false);
  txt_label->SetMaxWidth(GetSizeX());

  txt_value_black = new Text("", black_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
  txt_value_white = new Text("", white_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);

  std::vector<std::string>::size_type index = 0;
  for (std::vector<std::pair<std::string, std::string> >::const_iterator iter
       = choices.begin ();
       iter != choices.end ();
       iter++) {

    if (iter->first == choice)
      m_index = index;
    index++;
  }

  SetChoice(m_index);
}

ComboBox::~ComboBox ()
{
  delete txt_label;
  delete txt_value_black;
  delete txt_value_white;
  delete torus;
}

void ComboBox::Pack()
{
  txt_label->SetMaxWidth(size.x);
}

void ComboBox::Draw(const Point2i &mousePosition)
{
  Surface& window = GetMainWindow();

  //  the computed positions are to center on the image part of the widget

  // 1. first draw the torus
  torus->Draw(*this);


  // 2. then draw buttons
  #define IMG_BUTTONS_W 5
  #define IMG_BUTTONS_H 12

  Point2i center = GetPosition() + torus->GetCenter();
  if (m_index > 0) {

    if (Contains(mousePosition) && mousePosition.x < center.x)
      torus->m_minus->SetCurrentFrame(1);
    else
      torus->m_minus->SetCurrentFrame(0);

    torus->m_minus->Blit(window, GetPositionX() + IMG_BUTTONS_W, GetPositionY() + IMG_BUTTONS_H);
  }

  if (m_index < m_choices.size() - 1) {
    if (Contains(mousePosition) && mousePosition.x > center.x)
      torus->m_plus->SetCurrentFrame(1);
    else
      torus->m_plus->SetCurrentFrame(0);

    torus->m_plus->Blit(window, GetPositionX() + GetSizeX() - torus->m_plus->GetWidth() - IMG_BUTTONS_W,
                        GetPosition().y + IMG_BUTTONS_H);
  }

  // 3. add in the value image
  uint tmp_x = GetPositionX() + GetSizeX()/2;
  uint tmp_y = center.y + SMALL_R - 3;
  uint value_h = Font::GetInstance(Font::FONT_MEDIUM)->GetHeight();

  txt_value_black->DrawCenterTop(Point2i(tmp_x + 1, tmp_y + 1 - value_h/2));
  txt_value_white->DrawCenterTop(Point2i(tmp_x, tmp_y - value_h/2));

  // 7. and finally the label image
  txt_label->DrawCenterTop(Point2i(tmp_x, GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

Widget* ComboBox::ClickUp(const Point2i &mousePosition, uint button)
{
  NeedRedrawing();

  bool is_click = Mouse::IS_CLICK_BUTTON(button);
  if ( (is_click && mousePosition.x > (GetPositionX() + GetSizeX()/2))
       || button == SDL_BUTTON_WHEELUP ) {
    SetChoice(m_index + 1);
    return this;
  } else if ( (is_click && mousePosition.x <= (GetPositionX() + GetSizeX()/2))
              || button == SDL_BUTTON_WHEELDOWN ) {
    SetChoice(m_index - 1);
    return this;
  }

  return NULL;
}

void ComboBox::SetChoice(std::vector<std::string>::size_type index)
{
  std::string text;

  if (index >= m_choices.size ())
    return; /* index = 0; // loop back */

  m_index = index;

  txt_value_black->SetText(m_choices[m_index].second);
  txt_value_white->SetText(m_choices[m_index].second);

  RecreateTorus();
  NeedRedrawing();
}

int ComboBox::GetIntValue() const
{
  int tmp = 0;
  sscanf(GetValue().c_str(),"%d", &tmp);
  return tmp;
}

void ComboBox::RecreateTorus()
{
  float angle;
  if (m_choices.size () > 1)
    angle = m_index*(2.0f*M_PI - OPEN_ANGLE) / (m_choices.size () - 1);
  else
    angle = 0;
  torus->Refresh(angle, OPEN_ANGLE);
}
