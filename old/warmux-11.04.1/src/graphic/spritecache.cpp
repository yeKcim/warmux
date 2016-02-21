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
 * Sprite cache.
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#include "graphic/spritecache.h"

void SpriteSubframeCache::SetCache(uint rotation_num, const Double& mini, const Double& maxi)
{
  min = mini;
  max = maxi;
  rotated.clear();
  if (rotation_num) {
    rotated.resize(rotation_num);
    rotated[0] = surface;
  }
}

Surface SpriteSubframeCache::GetSurfaceForAngle(Double angle)
{
  ASSERT(max - min > ZERO);
  //Double fmin = PI-max;
  angle = RestrictAngle(angle);
  uint index = ((uint)rotated.size()*angle - min) / Double(max-min);
  ASSERT(rotated.size()>index);

  // On demand-cache
  if (rotated[index].IsNull()) {
    // Some compilers (old gccs?) need the explicit size_t->uint cast here
    angle = min + (max-min)*(1-index/(Double)uint(rotated.size()));
    rotated[index] = surface.RotoZoomC(angle, ONE, ONE);
  }
  return rotated[index];
}

void SpriteFrameCache::SetCaches(bool flip, uint rotation_num, Double mini, Double maxi)
{
  ASSERT(!normal.surface.IsNull());
  normal.SetCache(rotation_num, mini, maxi);
  if (flip) {
    flipped.surface = normal.surface.Mirror();
    flipped.SetCache(rotation_num, mini, maxi);
  }
}

void SpriteCache::EnableCaches(bool flipped, uint rotation_num, const Double& min, const Double& max)
{
  //For each frame, we pre-render 'rotation_num' rotated surface
  //At runtime the prerender Surface with the nearest angle to what is asked is displayed
  have_flipping_cache = flipped;
  rotation_cache_size = rotation_num;

  assert(!empty());

  for (uint f=0; f<size(); f++) {
    operator[](f).SetCaches(flipped, rotation_num, min, max);
  }
}

void SpriteCache::FixParameters(const Double& rotation_rad,
                                const Double& scale_x, const Double& scale_y,
                                bool force_color_key)
{
  bool rotozoom = rotation_rad.IsNotZero() || scale_x!=ONE || scale_y!=ONE;
  for (uint i=0; i<size(); i++) {
    SpriteFrameCache& frame = operator[](i);
    if (rotozoom)
      frame.normal.surface = frame.normal.surface.RotoZoom(rotation_rad, scale_x, scale_y);
    if (force_color_key)
      frame.normal.surface = frame.normal.surface.DisplayFormatColorKey(128, true);
  }
}
