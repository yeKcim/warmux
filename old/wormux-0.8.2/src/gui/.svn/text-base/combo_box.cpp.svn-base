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

#include <vector>
#include <sstream>

#include "gui/combo_box.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "include/app.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "graphic/polygon_generator.h"
#include "tool/affine_transform.h"

ComboBox::ComboBox (const std::string &label,
		    const std::string &resource_id,
		    const Point2i &_size,
		    const std::vector<std::pair<std::string, std::string> > &choices,
		    const std::string choice):
  m_choices(choices), m_index(0)
{
  position = Point2i(-1, -1);
  size = _size;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  m_image = resource_manager.LoadImage(res, resource_id);
  m_annulus_background = resource_manager.LoadImage(res, "menu/annulus_background");
  m_annulus_foreground = resource_manager.LoadImage(res, "menu/annulus_foreground");
  m_progress_color = resource_manager.LoadColor(res, "menu/annulus_progress_color");
  resource_manager.UnLoadXMLProfile( res);

  txt_label = new Text(label, dark_gray_color, Font::FONT_MEDIUM, Font::FONT_BOLD, false);
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
}

void ComboBox::Pack()
{
  txt_label->SetMaxWidth(size.x);
}

void ComboBox::Draw(const Point2i &/*mousePosition*/) const
{
  Surface& video_window = GetMainWindow();

  //  the computed positions are to center on the image part of the widget

  // 1. first draw the annulus background
  uint tmp_back_x = GetPositionX() + (GetSizeX() - m_annulus_background.GetWidth())/4 ;
  uint tmp_back_y = GetPositionY();
  video_window.Blit(m_annulus_background, Point2i(tmp_back_x, tmp_back_y));

  // 2. then draw the progress annulus
  static uint small_r = 25;
  static uint big_r = 35;
  static double open_angle_value = 0.96; // 55
  uint center_x = tmp_back_x + m_annulus_background.GetWidth() / 2;
  uint center_y = tmp_back_y + m_annulus_background.GetHeight() / 2;
  double angle;
  if (m_choices.size () > 1)
    angle = (2 * M_PI - open_angle_value) * m_index / (m_choices.size () - 1);
  else
    angle = 0;
  Polygon *tmp = PolygonGenerator::GeneratePartialTorus(big_r * 2, small_r * 2, 100, angle, open_angle_value / 2.0);
  tmp->SetPlaneColor(m_progress_color);
  tmp->ApplyTransformation(AffineTransform2D::Translate(center_x, center_y));
  tmp->Draw(&video_window);
  delete(tmp);

  // 3. then draw the annulus foreground
  uint tmp_fore_x = tmp_back_x;
  uint tmp_fore_y = tmp_back_y;
  video_window.Blit(m_annulus_foreground, Point2i(tmp_fore_x, tmp_fore_y));

  // 4. then draw the image
  uint tmp_x = center_x - m_image.GetWidth() / 2;
  uint tmp_y = center_y - m_image.GetHeight() / 2;

  video_window.Blit(m_image, Point2i(tmp_x, tmp_y));

  // 5. add in the value image
  tmp_x = center_x;
  tmp_y = center_y + small_r - 3;
  uint value_h = Font::GetInstance(Font::FONT_MEDIUM)->GetHeight();

  txt_value_black->DrawCenterTop(Point2i(tmp_x + 1, tmp_y + 1 - value_h/2));
  txt_value_white->DrawCenterTop(Point2i(tmp_x, tmp_y - value_h/2));

  // 6. and finally the label image
  txt_label->DrawCenterTop(Point2i(GetPositionX() + GetSizeX()/2,
                            GetPositionY() + GetSizeY() - txt_label->GetHeight()));
}

Widget* ComboBox::ClickUp(const Point2i &mousePosition, uint button)
{
  NeedRedrawing();

  if (button == SDL_BUTTON_LEFT && Contains(mousePosition)) {

    SetChoice(m_index + 1);
    return this;

  } else if (button == SDL_BUTTON_RIGHT && Contains(mousePosition)) {

    SetChoice(m_index - 1);
    return this;

  } else if( button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition) ) {

    SetChoice (m_index - 1);
    return this;

  } else if( button == SDL_BUTTON_WHEELUP && Contains(mousePosition) ) {

    SetChoice(m_index + 1);
    return this;
  }
  return NULL;
}

void ComboBox::SetChoice (std::vector<std::string>::size_type index)
{
  std::string text;

  if (index >= m_choices.size ())
    return; /* index = 0; // loop back */

  m_index = index;

  txt_value_black->Set(m_choices[m_index].second);
  txt_value_white->Set(m_choices[m_index].second);

  NeedRedrawing();
}

int ComboBox::GetIntValue() const
{
  int tmp = 0;
  sscanf(GetValue().c_str(),"%d", &tmp);
  return tmp;
}
