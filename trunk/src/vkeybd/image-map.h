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

#ifndef COMMON_IMAGEMAP_H
#define COMMON_IMAGEMAP_H

#include <WARMUX_config.h>

#ifdef ENABLE_VKEYBD
#include <string>
#include <map>
#include "vkeybd/vkpolygon.h"

namespace Common {

class ImageMap
{

public:

  ~ImageMap();

  Polygon *createArea(const std::string& id);
  void removeArea(const std::string& id);
  void removeAllAreas();
  std::string findMapArea(int x, int y);

protected:
  std::map<std::string, Polygon *> _areas;
};

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
#endif // #ifndef COMMON_IMAGEMAP_H
