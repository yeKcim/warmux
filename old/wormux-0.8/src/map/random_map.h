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
 * Random map generator
 *****************************************************************************/

#ifndef RANDOM_MAP_H
#define RANDOM_MAP_H

#include <vector>
#include "tool/point.h"
#include "graphic/surface.h"
#include "map/maps_list.h"

// Forward declarations
class Profile;
class Polygon;
class Sprite;

class RandomElementList : public std::vector<Surface *> {
 public:
   void AddElement(const Surface *element) { push_back(new Surface(*element)); };
   Surface * GetRandomElement();
   ~RandomElementList();
};

class MapElement {
 protected:
   Surface element;
   Point2i position;
 public:
   MapElement(const Surface & obj, const Point2i & pos) { element = obj; position = pos; };
   Surface & GetElement() { return element; };
   Point2i & GetPosition() { return position; };
};

class RandomMap {
  /* if you need that, implement it (correctly)*/
  RandomMap(const RandomMap&);
  RandomMap operator=(const RandomMap&);
  /*********************************************/

  typedef enum {
    DENTED_CIRCLE,
    DENTED_TRAPEZE,
    ROUNDED_RECTANGLE
  } Shape_type;
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
   uint number_of_element;

   // Internal random element vector
   RandomElementList random_element_list;
   std::vector<MapElement> element_list;

 public:
   RandomMap(Profile *profile, const int width, const int height);
   void SetSize(const int width, const int height);
   const Point2i GetSize() const { return Point2i(width, height); };
   int GetWidth() const { return width; };
   int GetHeight() const { return height; };
   void AddElement(const Surface * object, const Point2i& position);
   void DrawElement();
   void SetBorderSize(const double border) { border_size = border; };
   void SetBorderColor(const Color& color) { border_color = color; };
   bool IsOpen() const { return is_open; };
   void Generate(InfoMap::Island_type generator);
   void GenerateIsland();
   void GeneratePlatforms();
   void SaveMap();
   Surface GetRandomMap() const { return result; };
};

#endif /* RANDOM_MAP_H */
