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

#include "map/random_map.h"
#include "game/config.h"
#include "graphic/polygon_generator.h"
#include "graphic/sprite.h"
#include "map/maps_list.h"
#include "network/randomsync.h"
#include "tool/affine_transform.h"
#include "tool/resource_manager.h"
#include "tool/debug.h"
#include "tool/xml_document.h"
#include <sstream>

Surface * RandomElementList::GetRandomElement()
{
  if(size() == 0)
    return NULL;
  return (*this)[RandomSync().GetInt(0, size() - 1)];
}

RandomElementList::~RandomElementList()
{
  for(iterator elt = begin(); elt != end(); elt++) {
    delete((*elt));
  }
}

RandomMap::RandomMap(Profile *profile, const int width, const int height)
{
  this->profile = profile;
  SetSize(width, height);
  result = Surface(Point2i(width, height), SDL_SWSURFACE|SDL_SRCALPHA, true);
  random_shape = NULL;
  bezier_shape = NULL;
  expanded_bezier_shape = NULL;

  number_of_element = 0;
  XmlReader::ReadUint(profile->doc->GetRoot(), "nb_element", number_of_element);
  border_size = 8.0;
  XmlReader::ReadDouble(profile->doc->GetRoot(), "border_size", border_size);

  // Loading resources
  border_color = GetResourceManager().LoadColor(profile, "border_color");
  texture = GetResourceManager().LoadImage(profile, "texture");
  for(uint i = 0; i < number_of_element; i++) {
    std::stringstream ss;
    ss << "element_" << (i + 1);
    element = GetResourceManager().LoadImage(profile, ss.str());
    random_element_list.AddElement(&element);
  }
  element_list.clear();
}

void RandomMap::SetSize(const int width, const int height)
{
  this->width = width;
  this->height = height;
}

void RandomMap::AddElement(const Surface * object, const Point2i& position)
{
  element_list.push_back(MapElement(*object, position));
}

void RandomMap::DrawElement()
{
  for(std::vector<MapElement>::iterator elt = element_list.begin(); elt != element_list.end(); elt++) {
    Surface & tmp = elt->GetElement();
    result.MergeSurface(tmp, elt->GetPosition() - Point2i((int)(tmp.GetWidth() / 2.0), tmp.GetHeight()));
  }
}

void RandomMap::GeneratePlatforms()
{
  uint minplats = 15, maxplats = 35; /* number of platforms */
  uint minwidth = 50, maxwidth = 500; /* platform widths */
  uint minhei = 10, maxhei = 100; /* height */
  uint vertchance = 10; /* % chance of the platform being vertical */
  uint elemchance = 10; /* % chance of putting down a random element on the platform */

  XmlReader::ReadUint(profile->doc->GetRoot(), "nb_platforms_min", minplats);
  XmlReader::ReadUint(profile->doc->GetRoot(), "nb_platforms_max", maxplats);
  if (minplats < 1)
    minplats = 1;
  if (maxplats < minplats)
    maxplats = minplats;

  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_width_min", minwidth);
  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_width_max", maxwidth);
  if (minwidth < 2)
    minwidth = 2;
  if (maxwidth < minwidth)
    maxwidth = minwidth;

  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_height_min", minhei);
  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_height_max", maxhei);
  if (minhei < 2)
    minhei = 2;
  if (maxhei < minhei)
    maxhei = minhei;

  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_vert_chance", vertchance);

  XmlReader::ReadUint(profile->doc->GetRoot(), "platform_element_chance", elemchance);

  uint nplats = RandomSync().GetInt(minplats, maxplats);

  result.Fill(0);

  for (uint i = 0; i < nplats; i++) {
    double wid = RandomSync().GetDouble(minwidth, maxwidth);
    double hei = RandomSync().GetDouble(minhei, maxhei);
    if (RandomSync().GetInt(0,99) < (int) vertchance) {
      double tmp = wid;
      wid = hei;
      hei = tmp;
    }
    double x = RandomSync().GetDouble(0, (width - wid));
    double y = RandomSync().GetDouble(0, (height - hei));

    Polygon *tmp = new Polygon();

    tmp->AddPoint(Point2d(x, y));
    tmp->AddPoint(Point2d(x+wid, y));
    tmp->AddPoint(Point2d(x+wid, y+hei));
    tmp->AddPoint(Point2d(x, y+hei));

    if (RandomSync().GetInt(0,99) < (int)elemchance) {
      Surface * random_element = random_element_list.GetRandomElement();
      int dx = RandomSync().GetInt((int)(x+10), (int)(x+wid-10));
      int dy = (int) (y+(minhei / 2));
      if (random_element != NULL) {
        Surface * tmp_surf = new Surface(random_element->GetSurface());
        AddElement(tmp_surf, Point2i(dx, dy));
      }
    }

    bezier_shape = tmp->GetBezierInterpolation(1.0, 30, RandomSync().GetDouble(0.0, 0.5));

    // Expand
    expanded_bezier_shape = new Polygon(*bezier_shape);
    expanded_bezier_shape->Expand(border_size);

    // Set color, texture etc.
    bezier_shape->SetTexture(&texture);
    bezier_shape->SetPlaneColor(border_color);
    expanded_bezier_shape->SetPlaneColor(border_color);

    // expanded_bezier_shape->ClearItem();
    // bezier_shape->ClearItem();
    // Then draw it
    expanded_bezier_shape->Draw(&result);
    bezier_shape->Draw(&result);

    tmp->SetTexture(&texture);
    tmp->SetPlaneColor(border_color);

    //  tmp->Draw(&result);
  }
  DrawElement();
}

void RandomMap::GenerateIsland()
{
  double minhei = height / RandomSync().GetDouble(7, 5);
  double maxhei = height / RandomSync().GetDouble(1.5, 4);

  double current_y_pos = height - RandomSync().GetDouble(minhei, maxhei);
  int num_of_points = RandomSync().GetInt(5, 20);

  result.Fill(0);

  Polygon *tmp = new Polygon();

  // +10 so it's outside the screen
  tmp->AddPoint(Point2d(-100, height + 100));

  for (int i = 1; i < num_of_points - 1; i++) {
    current_y_pos = height - RandomSync().GetDouble(minhei, maxhei);
    double current_x_pos = (((double)i / (double) num_of_points) * (double)width);
    tmp->AddPoint(Point2d(current_x_pos, current_y_pos));
    if (RandomSync().GetInt(0, 5) < 1) {
      Surface * random_element = random_element_list.GetRandomElement();
      if(random_element != NULL) {
        Point2i position((int)current_x_pos, (int)(current_y_pos + 20.0));
        Surface * tmp_surf = new Surface(random_element->GetSurface());
        AddElement(tmp_surf, Point2i((int)current_x_pos, (int)(current_y_pos + 20.0)));
        MSG_DEBUG("ground_generator.element", "Add an element in (x = %f, y = %f)", position.GetX(), position.GetY());
      }
    }
  }

  tmp->AddPoint(Point2d(width + 100, height + 100));
  tmp->AddPoint(Point2d(width / 2, height + 100));

  // Get bezier interpolation
  double nb = RandomSync().GetDouble(0.0, 0.5);
  MSG_DEBUG("ground_generator.island", "bezier interpolation: 1.0, 30, %f", nb);
  bezier_shape = tmp->GetBezierInterpolation(1.0, 30, nb);
  delete tmp;

  // Expand
  expanded_bezier_shape = new Polygon(*bezier_shape);
  expanded_bezier_shape->Expand(border_size);

  // Set color, texture etc.
  bezier_shape->SetTexture(&texture);
  bezier_shape->SetPlaneColor(border_color);
  expanded_bezier_shape->SetPlaneColor(border_color);

  // expanded_bezier_shape->ClearItem();
  // bezier_shape->ClearItem();
  // Then draw it
  expanded_bezier_shape->Draw(&result);
  bezier_shape->Draw(&result);
  DrawElement();
}

void RandomMap::GenerateGridElements()
{
    uint grid_wid = 300, grid_hei = 300;
    uint elemchance = 75;
    uint elem_adj_x_min = 0, elem_adj_x_max = 0;
    uint elem_adj_y_min = 0, elem_adj_y_max = 0;

    uint dx, dy;

    if (number_of_element < 1) return;

    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_grid_wid", grid_wid);
    if (grid_wid < 1) grid_wid = 1;

    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_grid_hei", grid_hei);
    if (grid_hei < 1) grid_hei = 1;

    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_element_chance", elemchance);

    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_element_adj_x_min", elem_adj_x_min);
    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_element_adj_x_max", elem_adj_x_max);
    if (elem_adj_x_min > elem_adj_x_max) elem_adj_x_min = elem_adj_x_max;

    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_element_adj_y_min", elem_adj_y_min);
    XmlReader::ReadUint(profile->doc->GetRoot(), "generator_element_adj_y_max", elem_adj_y_max);
    if (elem_adj_y_min > elem_adj_y_max) elem_adj_y_min = elem_adj_y_max;

    result.Fill(0);

    for (dx = grid_wid; dx < (uint)width; dx += grid_wid)
	for (dy = grid_hei; dy < (uint)height; dy += grid_hei)
	    if (RandomSync().GetInt(0,99) < (int)elemchance) {
		Surface * random_element = random_element_list.GetRandomElement();
		if (random_element != NULL) {
		    uint ex = dx + RandomSync().GetInt(elem_adj_x_min, elem_adj_x_max);
		    uint ey = dy + RandomSync().GetInt(elem_adj_y_min, elem_adj_y_max);
		    Surface * tmp_surf = new Surface(random_element->GetSurface());
		    AddElement(tmp_surf, Point2i(ex, ey));
		}
	    }

    DrawElement();
}

void RandomMap::Generate(InfoMap::Island_type generator)
{
  MSG_DEBUG("map.generation", "> Begin creation of random generated map");

  if(generator == InfoMap::RANDOM_GENERATED) {
    generator = (InfoMap::Island_type) RandomSync().GetInt(InfoMap::SINGLE_ISLAND, InfoMap::DEFAULT);
    generator = InfoMap::PLATEFORMS;
  }
  switch (generator) {
    case InfoMap::PLATEFORMS: GeneratePlatforms(); break;
    case InfoMap::SINGLE_ISLAND: GenerateIsland(); break;
    case InfoMap::GRID_ELEMENTS: GenerateGridElements(); break;
    default: GenerateIsland(); break;
  }

  MSG_DEBUG("map.generation", "< End creation of random generated map");
}

void RandomMap::SaveMap()
{
  result.ImgSave(Config::GetInstance()->GetPersonalDataDir() + ActiveMap()->ReadFullMapName() + " - last random generation.png");
}
