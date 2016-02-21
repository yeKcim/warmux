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
 * Sprite cache.
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#ifndef _SPRITE_CACHE_H
#define _SPRITE_CACHE_H

#include <vector>
#include "include/base.h"
#include "graphic/spriteframe.h"
#include "graphic/surface.h"

class Sprite;

class SpriteFrameCache
{
  bool use_rotation;
  std::vector<Surface> rotated_surface;
  std::vector<Surface> rotated_flipped_surface;

public:
  Surface flipped_surface;
  Surface GetFlippedSurfaceForAngle(double angle) const;
  Surface GetSurfaceForAngle(double angle) const;

  SpriteFrameCache();
  void CreateRotationCache(Surface &surface, unsigned int cache_size, bool smooth);
  void CreateFlippingCache(Surface &surface, bool smooth);
};

class SpriteCache
{
  Sprite &sprite;

// TODO: Remove "public:" :-)
public:
  bool have_rotation_cache;
  unsigned int rotation_cache_size;
  bool have_flipping_cache;
  bool have_lastframe_cache;
  Surface last_frame;
  std::vector<SpriteFrameCache> frames;

public:
  explicit SpriteCache(Sprite &sprite);

  void EnableRotationCache(std::vector<SpriteFrame> &frames, unsigned int cache_size);
  void EnableFlippingCache(std::vector<SpriteFrame> &frames);
  void EnableLastFrameCache();
  void DisableLastFrameCache();
  void InvalidLastFrame();
};

#endif /* _SPRITE_CACHE_H */
