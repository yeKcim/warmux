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
 ******************************************************************************
 * Progress bar for GUI.
 *****************************************************************************/

#include "gui/progress_bar.h"
#include <SDL.h>
#include "graphic/video.h"
#include "include/app.h"
#include "map/map.h"
#include "tool/math_tools.h"

ProgressBar::ProgressBar() :
  border_color(0, SDL_ALPHA_OPAQUE),
  value_color(255, SDL_ALPHA_OPAQUE),
  background_color(100, SDL_ALPHA_OPAQUE),
  gradientMode(false),
  x(0),
  y(0),
  width(0),
  height(0),
  val(0),
  min(0),
  max(0),
  m_use_ref_val(false),
  m_ref_val(0),
  bar_value(0),
  orientation(PROG_BAR_HORIZONTAL)
{
}

ProgressBar::ProgressBar(uint _x,
                         uint _y,
                         uint _width,
                         uint _height,
                         int _value,
                         int minValue,
                         int maxValue,
                         enum orientation _orientation) :
  border_color(0, SDL_ALPHA_OPAQUE),
  value_color(255, SDL_ALPHA_OPAQUE),
  background_color(100, SDL_ALPHA_OPAQUE),
  gradientMode(false),
  x(_x),
  y(_y),
  width(_width),
  height(_height),
  val(_value),
  min(minValue),
  max(maxValue),
  m_use_ref_val(false),
  m_ref_val(0),
  bar_value(0),
  orientation(_orientation)
{
  image.NewSurface(Point2i(width, height), SDL_SWSURFACE, false);
  image.SetColorKey(SDL_SRCCOLORKEY, 0);
}

void ProgressBar::SetMinMaxValueColor(const Color & min,
                                      const Color & max)
{
  gradientMode = true;
  colorMin     = min;
  colorMax     = max;
}

void ProgressBar::InitPos(uint px, uint py,
                          uint pwidth, uint pheight)
{
  assert(3 <= pwidth);
  assert(3 <= pheight);
  x    = px;
  y    = py;
  width = pwidth;
  height = pheight;
  image.NewSurface(Point2i(width, height), SDL_SWSURFACE, false);
  image.SetColorKey(SDL_SRCCOLORKEY, 0);
}

/*
 * intitialize the progress bar
 * orientation is set with ProgressBar::PROG_BAR_VERTICAL or
 *                         ProgressBar::PROG_BAR_HORIZONTAL
 * default orientation is ProgressBar::PROG_BAR_HORIZONTAL
 */
void ProgressBar::InitVal(int pval, int pmin, int pmax,
                          enum orientation porientation)
{
  ASSERT (pmin < pmax);
  val         = pval;
  min         = pmin;
  max         = pmax;
  orientation = porientation;
  bar_value   = ComputeBarValue(val);

  if (gradientMode) {
    float inv_fmax = 1.0f / max;
    coefRed   = (colorMax.GetRed()   - colorMin.GetRed())   * inv_fmax;
    coefGreen = (colorMax.GetGreen() - colorMin.GetGreen()) * inv_fmax;
    coefBlue  = (colorMax.GetBlue()  - colorMin.GetBlue())  * inv_fmax;
  }
}

void ProgressBar::UpdateValue(int pval)
{
  val       = ComputeValue(pval);
  bar_value = ComputeBarValue(val);

  if (gradientMode) {
    float absVal = abs(val);
    value_color.SetColor((Uint8) (colorMin.GetRed()   + (int)(coefRed   * absVal)),
                         (Uint8) (colorMin.GetGreen() + (int)(coefGreen * absVal)),
                         (Uint8) (colorMin.GetBlue()  + (int)(coefBlue  * absVal)),
                         SDL_ALPHA_OPAQUE);
  }

}

int ProgressBar::ComputeValue(int pval) const
{
  return BorneTpl<int>(pval, min, max);
}

uint ProgressBar::ComputeBarValue(int val) const
{
  if (PROG_BAR_HORIZONTAL == orientation) {
    return (ComputeValue(val) -min)*(width - 2)/(max-min);
  } else {
    return (ComputeValue(val) -min)*(height -2)/(max-min);
  }
}

// TODO pass a Surface as parameter
void ProgressBar::DrawXY(const Point2i & pos) const
{
  int begin, end;

  // Bordure
  image.Fill(border_color);

  // Fond
  Rectanglei r_back(1, 1, width - 2, height - 2);
  image.FillRect(r_back, background_color);

  // Valeur
  if (m_use_ref_val) {
    int ref = ComputeBarValue (m_ref_val);
    if (val < m_ref_val) { // FIXME hum, this seems buggy
      begin = 1+bar_value;
      end = 1+ref;
    } else {
      begin = 1+ref;
      end = 1+bar_value;
    }
  } else {
    begin = 1;
    end = 1+bar_value;
  }

  Rectanglei r_value;
  if (PROG_BAR_HORIZONTAL == orientation) {
    r_value = Rectanglei(begin, 1, end - begin, height - 2);
  } else {
    r_value = Rectanglei(1, height - end + begin - 1, width - 2, end -1 );
  }

  image.FillRect(r_value, value_color);

  if (m_use_ref_val) {
    int ref = ComputeBarValue (m_ref_val);
    Rectanglei r_ref;
    if (PROG_BAR_HORIZONTAL == orientation) {
       r_ref = Rectanglei(1 + ref, 1, 1, height - 2);
    } else {
       r_ref = Rectanglei(1, 1 + ref, width - 2, 1);
    }
    image.FillRect(r_ref, border_color);
  }

  // marks
  mark_it_const it = mark.begin(), it_end = mark.end();

  for (; it != it_end; ++it) {
    Rectanglei r_marq;
    if (PROG_BAR_HORIZONTAL == orientation) {
      r_marq = Rectanglei(1 + it->val, 1, 1, height - 2);
    } else {
      r_marq = Rectanglei(1, 1 + it->val, width -2, 1);
    }
    image.FillRect( r_marq, it->color);
  }
  Rectanglei dst(pos.x, pos.y, width, height);
  GetMainWindow().Blit(image, pos);

  GetWorld().ToRedrawOnScreen(dst);
}

// Ajoute/supprime un mark
ProgressBar::mark_it ProgressBar::AddTag(int val, const Color & color)
{
  mark_t m;
  m.val   = ComputeBarValue(val);
  m.color = color;
  mark.push_back(m);

  return --mark.end();
}

void ProgressBar::SetReferenceValue(bool use, int value)
{
  m_use_ref_val = use;
  m_ref_val     = ComputeValue(value);
}
