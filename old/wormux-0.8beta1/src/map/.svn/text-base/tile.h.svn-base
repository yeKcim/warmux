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

#ifndef _TILE_H
#define _TILE_H

#include <vector>
#include "tileitem.h"
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "tool/rectangle.h"

const uint EXPLOSION_BORDER_SIZE = 10;

class TileItem;

class Tile : public Rectanglei{
public:
  Tile ();
  ~Tile ();

  // Dig a hole
  void Dig(const Point2i &position, const Surface& provider);
  // Dig a circle hole
  void Dig(const Point2i &center, const uint radius);

  // Insert a sprite into the ground
  void PutSprite(const Point2i pos, Sprite* spr);
  // Merge a sprite into map (using alpha information)
  void MergeSprite(const Point2i &position, Surface & provider);

  // Load an image
  void LoadImage (Surface& ground_surface);

  // Get alpha value of a pixel
  unsigned char GetAlpha(const Point2i &pos) const;

  // Draw it (on the entire visible part) 
  void DrawTile();

  // Draw a part that is inside the given clipping rectangle
  // Clipping rectangle is in World corrdinate not screen coordinates
  // usefull to redraw only a part that is under a sprite that has moved,... 
  void DrawTile_Clipped(Rectanglei clip_rectangle) const;

  // Return a surface of the ground inside the rect
  Surface GetPart(Rectanglei& rec);

  // Check if a title is empty, so we can delete it
  void CheckEmptyTiles();
protected:
  void InitTile(const Point2i &pSize);

  void FreeMem();
  Point2i Clamp(const Point2i &v) const;

  // Dimension du terrain
  Point2i nbCells;
  unsigned int nbr_cell;

  // Canvas donnant acc� aux cellules
  std::vector<TileItem *> item;
};

#endif // _TILE_H
