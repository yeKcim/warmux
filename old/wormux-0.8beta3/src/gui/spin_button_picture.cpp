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

#include "gui/spin_button_picture.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "gui/button.h"
#include <sstream>
#include "include/app.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "graphic/polygon_generator.h"
#include "tool/affine_transform.h"

SpinButtonWithPicture::SpinButtonWithPicture (const std::string& label,
                                              const std::string& resource_id,
                                              const Point2i& _size,
                                              int value, int step,
                                              int min_value, int max_value) :
  AbstractSpinButton(value, step, min_value, max_value)
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

  ValueHasChanged();
}

SpinButtonWithPicture::~SpinButtonWithPicture ()
{
  delete txt_label;
  delete txt_value_black;
  delete txt_value_white;
}

void SpinButtonWithPicture::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());
}

void SpinButtonWithPicture::Draw(const Point2i &/*mousePosition*/, Surface& surf) const
{
  //  the computed positions are to center on the image part of the widget

  // 1. first draw the annulus background
  uint tmp_back_x = GetPositionX() + (GetSizeX() - m_annulus_background.GetWidth())/4 ;
  uint tmp_back_y = GetPositionY() + (GetSizeY() - m_annulus_background.GetHeight() - txt_label->GetHeight() - 5) /2;
  surf.Blit(m_annulus_background, Point2i(tmp_back_x, tmp_back_y));

  // 2. then draw the progress annulus
  static uint small_r = 25;
  static uint big_r = 35;
  static double open_angle_value = 0.96; // 55 °
  uint center_x = tmp_back_x + m_annulus_background.GetWidth() / 2;
  uint center_y = tmp_back_y + m_annulus_background.GetHeight() / 2;
  double angle = (2 * M_PI - open_angle_value) * (GetValue() - GetMinValue()) / (GetMaxValue() - GetMinValue());
  Polygon *tmp = PolygonGenerator::GeneratePartialTorus(big_r * 2, small_r * 2, 100, angle, open_angle_value / 2.0);
  tmp->SetPlaneColor(m_progress_color);
  tmp->ApplyTransformation(AffineTransform2D::Translate(center_x, center_y));
  tmp->Draw(&surf);
  delete(tmp);

  // 3. then draw the annulus foreground
  uint tmp_fore_x = GetPositionX() + (GetSizeX() - m_annulus_foreground.GetWidth())/4 ;
  uint tmp_fore_y = GetPositionY() + (GetSizeY() - m_annulus_foreground.GetHeight() - txt_label->GetHeight() - 5) /2;
  surf.Blit(m_annulus_foreground, Point2i(tmp_fore_x, tmp_fore_y));

  // 4. then draw the image
  uint tmp_x = center_x - m_image.GetWidth() / 2;
  uint tmp_y = center_y - m_image.GetHeight() / 2;
  surf.Blit(m_image, Point2i(tmp_x, tmp_y));

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

Widget* SpinButtonWithPicture::ClickUp(const Point2i &mousePosition, uint button)
{
  need_redrawing = true;

  if (!Contains(mousePosition)) {
    return NULL;
  }

  if (button == SDL_BUTTON_RIGHT || button == SDL_BUTTON_WHEELDOWN) {
    DecValue();
    return this;

  } else if (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_WHEELUP) {
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
  txt_value_black->Set(s);
  txt_value_white->Set(s);
}
