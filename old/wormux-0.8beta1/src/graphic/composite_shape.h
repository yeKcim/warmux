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
 * Composite Shape. Use Polygon and Sprite to draw a shape.
 *****************************************************************************/

#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include <vector>
#include "../tool/affine_transform.h"
#include "surface.h"
#include "polygon.h"

class CompositeShape {
 protected:
  std::vector<Polygon *> layers;
 public:
  CompositeShape();
  void AddLayer(Polygon * poly);
  void ApplyTransformation(const AffineTransform2D & trans);
  std::vector<Polygon *> GetLayer() const;
  // Drawing
  void Draw(Surface * dest);
  void DrawOnScreen();
};

#endif /* POLYGON_H */
