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
 *  Constants...
 *****************************************************************************/

#ifndef CONSTANT_H
#define CONSTANT_H
#include "base.h"
#include <string>
#include <vector>
#include "tool/point.h"

class Constants
{
public:
  // Version number of Wormux
  static const std::string VERSION;

  // Env variables name to override previous values
  static const std::string ENV_DATADIR;
  static const std::string ENV_LOCALEDIR;
  static const std::string ENV_FONT_PATH;

  // Maximum number of loops for the test ...
  static const uint NBR_BCL_MAX_EST_VIDE; // Is an object in vacuum ?

  // Authors list
  std::vector<std::string> AUTHORS;

  // Web site address and email
  static const std::string WEB_SITE;
  static const std::string EMAIL;

  // Dimension min/max of playground (in pixel)
  static const Point2i MAP_MIN_SIZE;
  static const int MAP_MAX_SIZE;

  // Minimal free heigth (in pixel) for the playground to be said "open"
  static const uint HAUT_MIN_TERRAIN_OUVERT;

  static Constants * GetInstance();
  ~Constants() { singleton = NULL; AUTHORS.clear(); };

private:
  Constants();

  static Constants * singleton;
};

#endif
