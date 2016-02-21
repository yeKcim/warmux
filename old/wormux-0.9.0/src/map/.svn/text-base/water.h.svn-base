/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Refresh de l'eau pouvant apparaitre en bas du terrain.
 *****************************************************************************/

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "include/base.h"
#include "graphic/surface.h"

const uint WATER_INITIAL_HEIGHT = 100;

// Forward declaration
class Color;

class Water
{
private:
  static int pattern_height;
  Color* type_color;
  int height_mvt;
  double shift1;
  uint water_height;
  uint time_raise;
  std::vector<int> height;
  std::vector< std::vector<int> > wave_height;
  Surface surface;
  Surface pattern;
  Surface bottom;
  Surface wpattern;
  std::string water_type;
  uint       m_last_preview_redraw;
  uint next_wave_shift;

  void Init();
  void CalculateWaveHeights();
  void CalculateWavePattern();

public:
  Water();
  ~Water();
  void Reset();
  void Free();
  void Refresh();
  void Draw();

  bool IsActive() const;
  int GetHeight(int x) const;
  uint GetSelfHeight() const;
  const Color* GetColor() const;

  void Splash(const Point2i& pos) const;
  void Smoke(const Point2i& pos) const;
  uint GetLastPreviewRedrawTime() const { return m_last_preview_redraw; };
};
#endif
