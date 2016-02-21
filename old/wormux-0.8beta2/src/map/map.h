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
 ******************************************************************************
 * Monde ou plateau de jeu.
 *****************************************************************************/

#ifndef MAP_H
#define MAP_H

#include "include/base.h"
#include "ground.h"
#include "sky.h"
#include "water.h"

// Forward declarations
class Text;
class Surface;
class PhysicalObj;

extern const uint MAX_WIND_OBJECTS;

class Map{
  Map(const Map&);
  const Map& operator=(const Map&);

 private:
  Text * author_info1;
  Text * author_info2;

public:
  Map();
  ~Map();

  Ground ground;
  Sky sky;
  double min_distance_between_characters;
  Water water;

  std::list<Rectanglei> *to_redraw;
  std::list<Rectanglei> *to_redraw_now;
  std::list<Rectanglei> *to_redraw_particles;
  std::list<Rectanglei> *to_redraw_particles_now;

public:
  void Reset();
  void Refresh();
  void FreeMem();
  void Draw(bool redraw_all = false);
  void DrawWater();
  void DrawSky(bool redraw_all = false);
  void DrawAuthorName();

  // To manage the cache mechanism
  void ToRedrawOnMap(const Rectanglei& r) { to_redraw->push_back(r); };
  void ToRedrawOnScreen(Rectanglei r);

  // Are we in the world or in vacuum ?
  bool IsInVacuum(const Point2i &pos) const { return ground.IsEmpty(pos); };
  bool IsInVacuum (int x, int y) const { return ground.IsEmpty(Point2i(x, y)); };
  bool RectIsInVacuum (const Rectanglei &rect) const;
  bool ParanoiacRectIsInVacuum (const Rectanglei &rect) const;

  // Test only the border lines
  // Test occurs on test rectangle with dx, dy delta
  bool IsInVacuum_top (const PhysicalObj &obj, int dx, int dy) const;
  bool IsInVacuum_bottom (const PhysicalObj &obj, int dx, int dy) const;
  bool IsInVacuum_left (const PhysicalObj &obj, int dx, int dy) const;
  bool IsInVacuum_right (const PhysicalObj &obj, int dx, int dy) const;

  // Is outside of the world ?
  bool IsOutsideWorldX (int x) const { return (x < 0) || ((int)GetWidth() <= x); };
  bool IsOutsideWorldY (int y) const { return (y < 0) || ((int)GetHeight() <= y); };
  bool IsOutsideWorldXwidth (int x, uint larg) const { return (x + (int)larg - 1 < 0) || ((int)GetWidth() <= x); };
  bool IsOutsideWorldYheight (int y, uint haut) const { return ((y + (int)haut - 1 < 0) || ((int)GetHeight() <= y)); };
  bool IsOutsideWorldXY (int x, int y) const { return IsOutsideWorldX(x) || IsOutsideWorldY(y); };
  bool IsOutsideWorld (const Point2i &pos) const { return IsOutsideWorldXY(pos.x, pos.y); };

  // Is it an open or closed world ?
  bool IsOpen() const { return ground.IsOpen(); }

  // Dig the map using a picture
  void Dig(const Point2i& position, const Surface& alpha_sur);
  // Dig a circle hole in the map
  void Dig(const Point2i& center, const uint radius);

  // Insert a sprite into the ground
  void PutSprite(const Point2i& pos, const Sprite* spr);
  // Merge a sprite into the ground
  void MergeSprite(const Point2i& pos, const Sprite* spr);

  int GetWidth() const { return ground.GetSizeX(); }
  int GetHeight() const { return ground.GetSizeY(); }
  Point2i GetSize() const{ return ground.GetSize(); }
  double GetDistanceBetweenCharacters() const { return min_distance_between_characters; }

 private:
  bool HorizontalLine_IsInVacuum (int left, int y,  int right) const;
  bool VerticalLine_IsInVacuum (int x,  int top, int bottom) const;

  void SwitchDrawingCache();
  void SwitchDrawingCacheParticles();
  void OptimizeCache(std::list<Rectanglei>& rectangleCache) const;
};

extern Map world;
#endif
