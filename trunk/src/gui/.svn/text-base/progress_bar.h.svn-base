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

#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <list>
#include <WARMUX_base.h>
#include "graphic/color.h"
#include "graphic/surface.h"

class ProgressBar
{
public:
  Color border_color, value_color, background_color;
  Surface image; // in order to pemit alpha blended progressbar
  enum orientation
  {
    PROG_BAR_VERTICAL,
    PROG_BAR_HORIZONTAL
  };

private:
  float coefRed;
  float coefGreen;
  float coefBlue;
  int divisor;
  bool gradientMode;

protected:
  uint x;
  uint y;
  uint width;
  uint height;
  int val, min, max; // current, min and max values
  bool m_use_ref_val; // use reference value ?
  int m_ref_val; // reference value
  uint bar_value; // current value in the progress bar
  enum orientation orientation;

  Color colorMin; // Color used for start value
  Color colorMax; // Color used for end value

  int ComputeValue(int val) const;
  uint ComputeBarValue(int val) const;

  typedef struct s_mark_t
  {
    Color color;
    uint val;
  } mark_t;

public:
  void SetBorderColor(const Color & color) { border_color = color; }
  void SetBackgroundColor(const Color & color) { background_color = color; }
  void SetValueColor(const Color & color) { value_color = color; }
  void SetMinMaxValueColor(const Color & min, const Color & max);

protected:
  typedef std::list<mark_t>::iterator mark_it;
  typedef std::list<mark_t>::const_iterator mark_it_const;
  std::list<mark_t> mark;

public:
  ProgressBar();
  ProgressBar(uint _x,
              uint _y,
              uint _width,
              uint _height,
              int _value,
              int minValue,
              int maxValue,
              enum orientation _orientation);
  virtual ~ProgressBar() {};

  int GetCurrentValue() const { return val; };
  int GetMinValue() const { return min; };
  int GetMaxValue() const { return max; };

  // Update current value
  void UpdateValue(int val);

  // Initialise la position
  virtual void InitPos(uint x,
                       uint y,
                       uint width,
                       uint height);
  virtual void SetHeight(uint height) { InitPos(x, y, image.GetWidth(), height); }

  // Initialise les valeurs
  void InitVal(int val,
               int min,
               int max,
               enum orientation orientation = PROG_BAR_HORIZONTAL);

  // Set reference value
  // Use it after InitVal !
  void SetReferenceValue(bool use, int value = 0);

  // Draw la barre de progresssion
  void Draw() const { DrawXY(Point2i(x, y)); };

  // Draw the progress bar
  virtual void DrawXY(const Point2i & pos) const;

  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  Point2i GetSize() const { return Point2i(width, height); }
  uint GetX() const { return x; }
  uint GetY() const { return y; }

  // add/remove value tag
  mark_it AddTag(int val, const Color & coul);
  void ResetTag() { mark.clear(); };
};

#endif
