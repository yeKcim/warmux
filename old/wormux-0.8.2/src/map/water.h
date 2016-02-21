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
 ******************************************************************************
 * Refresh de l'eau pouvant apparaitre en bas du terrain.
 *****************************************************************************/

#ifndef WATER_H
#define WATER_H

#include <vector>
#include "include/base.h"
#include "graphic/surface.h"

const uint WATER_INITIAL_HEIGHT = 100;
#define pattern_width 180

// Forward declaration
class Color;

class Water
{
public:
  typedef enum {
    NO_WATER,
    WATER,
    LAVA,
    RADIOACTIVE,
    MAX_WATER_TYPE
  } Water_type;

private:
  static int pattern_height;
  Color* type_color;
  int height_mvt;
  double shift1;
  uint water_height;
  uint temps_montee;
  int height[pattern_width];
  Surface surface;
  Surface pattern;
  Surface bottom;
  Surface wpattern;
  Water_type water_type;
  std::string water_name;

public:
  Water() : type_color(NULL) { }
  ~Water();
  void Init();
  void Reset();
  void Free();
  void Refresh();
  void Draw();
  bool IsActive() const { return water_type != NO_WATER; }
  int GetHeight(int x) const;
  uint GetSelfHeight() const { return water_height+(pattern_height/2); }
  static Water_type GetWaterType(std::string & water);
  const Color* GetColor() { return type_color; }

  void Splash(const Point2i& pos) const;
  void Smoke(const Point2i& pos) const;
};
#endif
