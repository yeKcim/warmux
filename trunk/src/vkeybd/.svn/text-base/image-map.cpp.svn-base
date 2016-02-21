/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
 *
 ******************************************************************************
 *
 * This file is derived from "ScummVM - Graphic Adventure Engine"
 * Original licence below:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */


#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD

#include <WARMUX_point.h>
#include <map>
#include "vkeybd/vkpolygon.h"
#include "vkeybd/image-map.h"

namespace Common {

ImageMap::~ImageMap()
{
  removeAllAreas();
}

Polygon *ImageMap::createArea(const std::string& id)
{
  if (_areas.count(id)) {
    printf("Image map already contains an area with target of '%s'\n", id.c_str());
    return 0;
  }
  Polygon *p = new Polygon();
  _areas[id] = p;
  return p;
}

void ImageMap::removeArea(const std::string& id)
{
  if (!_areas.count(id))
    return;
  delete _areas[id];
  _areas.erase(id);
}

void ImageMap::removeAllAreas()
{
  std::map<std::string, Polygon*>::iterator it;
  for (it = _areas.begin(); it != _areas.end(); ++it) {
    delete it->second;
  }
  _areas.clear();
}

std::string ImageMap::findMapArea(int x, int y)
{
  std::map<std::string, Polygon*>::iterator it;
  for (it = _areas.begin(); it != _areas.end(); ++it) {
    if (it->second->contains(x, y)) {
      printf("Virtual Keyboard pressed: %s\n", it->first.c_str());
      return it->first;
    }
  }
  return std::string();
}

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
