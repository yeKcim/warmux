/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Clipping : découpe une grosse image en petit morceaux indépendants pour
 * permettre un chargement des donnéees plus rapide. On peut ensuite creuser
 * des trous dans cette image (en touchant à la couche alpha).
 *****************************************************************************/

#ifndef _TILE_H
#define _TILE_H

#include <vector>
#include "../tool/Rectangle.h"

struct SDL_Surface;
class TileItem;

class Tile
{
public:
  Tile ();
  ~Tile ();

  // Dig a hole
  void Dig (int ox, int oy, SDL_Surface *provider);
   
  // Load an image
  void LoadImage (SDL_Surface *ground_surface);

  // Get size
  unsigned int GetWidth () const { return width; }
  unsigned int GetHeight () const { return height; }

  // Get alpha value of a pixel
  unsigned char GetAlpha (const int x, const int y) const;

  // Draw it (on the entire visible part) 
  void DrawTile () const;
   
  // Draw a part that is inside the given clipping rectangle
  // Clipping rectangle is in World corrdinate not screen coordinates
  // usefull to redraw only a part that is under a sprite that has moved,... 
  void DrawTile_Clipped (Rectanglei clip_rectangle) const;
   
protected:
   
  void InitTile (unsigned int width, unsigned int height);

  void FreeMem ();

  // Dimension du terrain
  unsigned int width;
  unsigned int height;

  unsigned int nbr_cell_width, nbr_cell_height;
  unsigned int nbr_cell;

  // Canvas donnant accès aux cellules
  std::vector<TileItem *> item;
};

#endif // _TILE_H
