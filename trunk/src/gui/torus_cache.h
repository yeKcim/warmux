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
 * Cache rendering of a partial torus
 *****************************************************************************/

#ifndef TORUS_CACHE_H
#define TORUS_CACHE_H

#include <WARMUX_rectangle.h>
#include "graphic/surface.h"

class Profile;
class Sprite;

class TorusCache
{
  Surface  m_image;

  float    m_last_angle;
  Surface *m_torus;

  int      m_big_r;
  int      m_small_r;

public:
  TorusCache(Profile *res, const std::string& resource_id, int bigr, int smallr);
  ~TorusCache();
  void Draw(const Rectanglei& box);
  void Refresh(float angle, float open);
  Point2i GetSize() const;

  Sprite  *m_plus;
  Sprite  *m_minus;
};

#endif //TORUS_CACHE_H
