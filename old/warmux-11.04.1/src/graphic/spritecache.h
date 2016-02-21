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

#ifndef _SPRITE_CACHE_H
#define _SPRITE_CACHE_H

#include <vector>
#include <assert.h>
#include <WARMUX_base.h>
#include "graphic/surface.h"

class Sprite;

#if 0 //def ANDROID
#  define RotoZoomC(a, x, y) RotoZoom(a, x, y).DisplayFormatColorKey(128)
#else
#  define RotoZoomC(a, x, y) RotoZoom(a, x, y)
#endif

class SpriteSubframeCache
{
  std::vector<Surface> rotated;
  Double min, max;

  Double RestrictAngle(Double angle) const
  {
    while (angle < min)
      angle += TWO_PI;
    while (angle >= max)
      angle -= TWO_PI;
    ASSERT(angle>=min && angle<max);
    return angle;
  }

public:
  Surface              surface;

  SpriteSubframeCache() { };
  SpriteSubframeCache(const Surface& surf) : surface(surf) { };
  SpriteSubframeCache(const SpriteSubframeCache& other)
    : rotated(other.rotated)
    , min(other.min)
    , max(other.max)
    , surface(other.surface)
  { }
  ~SpriteSubframeCache() { rotated.clear(); surface.Free(); }

  Surface GetSurfaceForAngle(Double angle);
  void SetCache(uint num, const Double& mini, const Double& maxi);
};

class SpriteFrameCache
{
public:
  uint delay;

  SpriteSubframeCache normal;
  SpriteSubframeCache flipped;

  SpriteFrameCache(uint d = 100) { delay = d; }
  SpriteFrameCache(const Surface& surf, uint d = 100)
    : delay(d)
    , normal(surf)
  { }
  SpriteFrameCache(const SpriteFrameCache& other)
    : delay(other.delay)
    , normal(other.normal)
    , flipped(other.flipped)
  { }

  void SetCaches(bool flipped, uint rotation_num, Double min, Double max);
};

class SpriteCache : public std::vector<SpriteFrameCache>
{
  Sprite &sprite;

  uint rotation_cache_size;
  bool have_flipping_cache;

public:
  explicit SpriteCache(Sprite &spr)
    : sprite(spr)
    , rotation_cache_size(0)
    , have_flipping_cache(false)
  { }

  void SetFrames(const SpriteCache &other)
  {
    clear();
    rotation_cache_size = other.rotation_cache_size;
    have_flipping_cache = other.have_flipping_cache;
    for (uint i=0; i<other.size(); i++)
      push_back(SpriteFrameCache(other[i]));
  }
  void AddFrame(const Surface& surf, uint delay=100) { push_back(SpriteFrameCache(surf, delay)); }
  void EnableCaches(bool flipped, uint rotation_num, const Double& min, const Double& max);

  //operator SpriteFrameCache& [](uint index) { return frames.at(index); }
  void SetDelay(uint delay)
  {
    for (uint i=0; i<size(); i++)
      operator[](i).delay = delay;
  }

  void FixParameters(const Double& rotation_rad,
                     const Double& scale_x, const Double& scale_y,
                     bool force_color_key);
  bool HasRotationCache() const { return rotation_cache_size; }
  bool HasFlippedCache() const { return have_flipping_cache; }
};

#endif /* _SPRITE_CACHE_H */
