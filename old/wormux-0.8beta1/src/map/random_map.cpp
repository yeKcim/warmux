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

#include "random_map.h"
#include "tool/random.h"
#include "map/maps_list.h"

MapElement::MapElement(Surface & object, Point2i & pos)
{
  element = object;
  position = pos;
}

Surface & MapElement::GetElement()
{
  return element;
}

Point2i & MapElement::GetPosition()
{
  return position;
}

RandomMap::RandomMap(Profile *profile, const int width, const int height)
{
  this->profile = profile;
  SetSize(width, height);
  border_size = 8;
  result = Surface(Point2i(width,height), SDL_SWSURFACE|SDL_SRCALPHA, true);
  random_shape = NULL;
  bezier_shape = NULL;
  expanded_bezier_shape = NULL;
  // Loading resources
  border_color = resource_manager.LoadColor(profile, "border_color");
  texture = resource_manager.LoadImage(profile, "texture");
  element = resource_manager.LoadImage(profile, "element");
  element_list.clear();
}

void RandomMap::SetSize(const int width, const int height)
{
  this->width = width;
  this->height = height;
}

const Point2i RandomMap::GetSize()
{
  return Point2i(width, height);
}

const int RandomMap::GetWidth()
{
  return width;
}

const int RandomMap::GetHeight()
{
  return height;
}

void RandomMap::AddElement(Surface & object, Point2i position)
{
  element_list.push_back(MapElement(object, position));
}

void RandomMap::DrawElement()
{
  std::vector<MapElement>::iterator elt;
  for(elt = element_list.begin(); elt != element_list.end(); elt++) {
    Surface & tmp = elt->GetElement();
    result.MergeSurface(tmp, elt->GetPosition() - Point2i((int)(tmp.GetWidth() / 2.0), tmp.GetHeight()));
  }
}

void RandomMap::SetBorderSize(const double border)
{
  border_size = border;
}

void RandomMap::SetBorderColor(const Color color)
{
  border_color = color;
}

const bool RandomMap::IsOpen()
{
  return is_open;
}

void RandomMap::Generate()
{
  srand(time(NULL));
  // Computing number of island
  number_of_island = Random::GetInt(2, 4);
  // is_open = (Random::GetInt(0, 10) > 8); // Open ?
  is_open = true;
  // Initializing ground generator
  double h = height / number_of_island;
  double w = width / number_of_island;
  double current_h_position = (width / 2.0) + Random::GetDouble(-w / 2, w / 2);
  double current_v_position = (height / 2.0) + Random::GetDouble(-h / 2, h / 2);
  double x_direction = 1.0;
  double y_direction = 1.0;
  AffineTransform2D translate;
  result.Fill(0);
  for(int i = 0; i < number_of_island; i++) {
    translate = AffineTransform2D::Translate(current_h_position, current_v_position);
    // Generate Island
    while(!GenerateIsland(Random::GetDouble(w * 0.55, w * 1.25),
                          Random::GetDouble(h * 0.55, h * 1.25)));
    bezier_shape->ApplyTransformation(translate);
    expanded_bezier_shape->ApplyTransformation(translate);
    // Then draw it
    expanded_bezier_shape->Draw(&result);
    bezier_shape->Draw(&result);
    // compute a new position
    current_h_position += x_direction * Random::GetDouble(w * 0.75, w * 1.25);
    current_v_position += y_direction * Random::GetDouble(h * 0.75, h * 1.25);
    if(current_h_position > width) {
      x_direction = -x_direction;
      current_h_position = width - (current_h_position - width);
    } else if(current_h_position < 0) {
      x_direction = -x_direction;
      current_h_position = -current_h_position;
    }
    if(current_v_position > height) {
      y_direction = -y_direction;
      current_v_position = height - (current_v_position - height);
    } else if(current_v_position < 0) {
      y_direction = -y_direction;
      current_v_position = -current_v_position;
    }
    Point2d pos = bezier_shape->GetRandomUpperPoint();
    AddElement(element, translate * Point2i((int)pos.x, (int)pos.y));
  }
  AddElement(element, Point2i(width / 2, height / 2));
  AddElement(element, Point2i(-20, 0));
  DrawElement();
}

bool RandomMap::GenerateIsland(double width, double height)
{
  int nb_of_point;
  double x_rand_offset, y_rand_offset, coef;
  if(random_shape) {
    random_shape = NULL;
    delete random_shape;
  }
  if(bezier_shape) {
    bezier_shape = NULL;
    delete bezier_shape;
  }
  if(expanded_bezier_shape) {
    expanded_bezier_shape = NULL;
    delete expanded_bezier_shape;
  }
  // Generate a random shape
  switch(Random::GetInt(DENTED_CIRCLE, ROUNDED_RECTANGLE)) {
    case DENTED_CIRCLE:
      nb_of_point = Random::GetInt(5, 20);
      x_rand_offset = width / Random::GetDouble(2.0, 15.0);
      random_shape = PolygonGenerator::GenerateDentedCircle(width, nb_of_point, x_rand_offset);
      break;
    case ROUNDED_RECTANGLE:
      random_shape = PolygonGenerator::GenerateRectangle(width, height);
      break;
    default: case DENTED_TRAPEZE:
      x_rand_offset = Random::GetDouble(10.0, 15.0);
      y_rand_offset = Random::GetDouble(10.0, 15.0);
      coef = Random::GetSign() * Random::GetDouble(0.5, 1.0);
      random_shape = PolygonGenerator::GenerateRandomTrapeze(width, height, x_rand_offset, y_rand_offset, coef);
      break;
  }
  if(random_shape->GetNbOfPoint() < 4)
    return false;
  bezier_shape = random_shape->GetBezierInterpolation(1.0, 20, 1.5);
  expanded_bezier_shape = new Polygon(*bezier_shape);
  // Expand the random, bezier shape !
  expanded_bezier_shape->Expand(border_size);
  // Setting texture and border color
  bezier_shape->SetTexture(&texture);
  expanded_bezier_shape->SetPlaneColor(border_color);
  return true;
}

void RandomMap::SaveMap()
{
  result.ImgSave(Config::GetInstance()->GetPersonalDir() + ActiveMap().ReadName() + " - last random generation.png");
}

Surface RandomMap::GetRandomMap()
{
  return result;
}
