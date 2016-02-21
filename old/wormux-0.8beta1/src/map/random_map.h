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
 * Random map generator
 *****************************************************************************/

#ifndef RANDOM_MAP_H
#define RANDOM_MAP_H

#include <vector>
#include "graphic/polygon_generator.h"
#include "graphic/surface.h"
#include "game/config.h"
#include "tool/resource_manager.h"

class Profile;

class MapElement {
 protected:
   Surface element;
   Point2i position;
 public:
   MapElement(Surface & object, Point2i & position);
   Surface & GetElement();
   Point2i & GetPosition();
};

class RandomMap {
  /* if you need that, implement it (correctly)*/
  RandomMap(const RandomMap&);
  RandomMap operator=(const RandomMap&);
  /*********************************************/

  enum {
    DENTED_CIRCLE,
    DENTED_TRAPEZE,
    ROUNDED_RECTANGLE,
  } island_type;
 protected:
   double border_size;
   bool is_open;
   int number_of_island;
   Surface result;
   int width;
   int height;

   // Shape used to generate island
   Polygon * random_shape;
   Polygon * bezier_shape;
   Polygon * expanded_bezier_shape;

   // Internal parameters
   Profile *profile;
   Color border_color;
   Surface texture;
   Surface element;

   std::vector<MapElement> element_list;

 public:
   RandomMap(Profile *profile, const int width, const int height);
   void SetSize(const int width, const int height);
   const Point2i GetSize();
   const int GetWidth();
   const int GetHeight();
   void AddElement(Surface & object, Point2i position);
   void DrawElement();
   void SetBorderSize(const double border);
   void SetBorderColor(const Color color);
   const bool IsOpen();
   void Generate();
   bool GenerateIsland(double width, double height);
   void SaveMap();
   Surface GetRandomMap();
};

#endif /* RANDOM_MAP_H */
