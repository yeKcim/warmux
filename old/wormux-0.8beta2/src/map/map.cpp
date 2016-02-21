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
 * Monde ou plateau de jeu.
 *****************************************************************************/

#include <iostream>
#include "map.h"
#include "object/physical_obj.h"
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "camera.h"
#include "maps_list.h"
#include "wind.h"
#include "game/time.h"
#include "object/objbox.h"
#include "tool/i18n.h"

const double MINIMUM_DISTANCE_BETWEEN_CHARACTERS = 50.0;

const uint AUTHOR_INFO_TIME = 5000; // ms
const uint AUTHOR_INFO_X = 100;
const uint AUTHOR_INFO_Y = 50;

Map world;

Map::Map()
{
  min_distance_between_characters = MINIMUM_DISTANCE_BETWEEN_CHARACTERS;

  to_redraw = new std::list<Rectanglei>;
  to_redraw_now = new std::list<Rectanglei>;
  to_redraw_particles = new std::list<Rectanglei>;
  to_redraw_particles_now = new std::list<Rectanglei>;
}

Map::~Map()
{
  delete to_redraw;
  delete to_redraw_now;
  delete to_redraw_particles;
  delete to_redraw_particles_now;
}

void Map::Reset()
{
  sky.Reset();
  ground.Reset();
  water.Reset();
  wind.Reset();

  // Configure game about open or closed world
  bool open = ground.IsOpen();
  ObjBox::Enable(open);

  delete author_info1; author_info1 = NULL;
  delete author_info2; author_info2 = NULL;

  to_redraw->clear();
  to_redraw_now->clear();
  to_redraw_particles->clear();
  to_redraw_particles_now->clear();
}

void Map::Refresh()
{
  water.Refresh();
  wind.Refresh();
}

void Map::FreeMem()
{
  ground.Free();
  sky.Free();
  water.Free();

  to_redraw->clear();
  to_redraw_now->clear();
  to_redraw_particles->clear();
  to_redraw_particles_now->clear();
}

void Map::ToRedrawOnScreen(Rectanglei r)
{
  r.SetPosition( r.GetPosition() + Camera::GetInstance()->GetPosition() );
  to_redraw->push_back(r);
}

void Map::SwitchDrawingCache()
{
  std::list<Rectanglei> *tmp = to_redraw_now;
  to_redraw_now = to_redraw;
  to_redraw = tmp;
  to_redraw->clear();
}

void Map::SwitchDrawingCacheParticles()
{
  std::list<Rectanglei> *tmp = to_redraw_particles_now;
  to_redraw_particles_now = to_redraw_particles;
  to_redraw_particles = tmp;
  to_redraw_particles->clear();
}

void Map::Dig(const Point2i& position, const Surface& surface)
{
   ground.Dig (position, surface);
   to_redraw->push_back(Rectanglei(position, surface.GetSize()));
}

void Map::Dig(const Point2i& center, const uint radius)
{
   ground.Dig (center, radius);
   to_redraw->push_back(Rectanglei(center - Point2i(radius+EXPLOSION_BORDER_SIZE,radius+EXPLOSION_BORDER_SIZE),
                                   Point2i(2*(radius+EXPLOSION_BORDER_SIZE),2*(radius+EXPLOSION_BORDER_SIZE))));
}

void Map::PutSprite(const Point2i& pos, const Sprite* spr)
{
   ground.PutSprite (pos, spr);
   to_redraw->push_back(Rectanglei(pos, spr->GetSizeMax()));
}

void Map::MergeSprite(const Point2i& pos, const Sprite * spr)
{
  Surface tmp = spr->GetSurface();
  ground.MergeSprite (pos, tmp);
  to_redraw->push_back(Rectanglei(pos, spr->GetSizeMax()));
}

void Map::DrawSky(bool redraw_all)
{
  SwitchDrawingCache();
  SwitchDrawingCacheParticles();

  OptimizeCache(*to_redraw_now);

  sky.Draw(redraw_all);
}

void Map::DrawWater()
{ water.Draw(); }

void Map::Draw(bool redraw_all)
{
  std::list<Rectanglei> *tmp = to_redraw;
  to_redraw_particles->clear();
  to_redraw = to_redraw_particles;

  wind.DrawParticles();
  to_redraw = tmp;

//  Done from DrawSky
//  OptimizeCache(*to_redraw_now);

  ground.Draw(redraw_all);
}

bool Map::HorizontalLine_IsInVacuum(int ox, int y, int width) const
{
  // Traite une ligne

  for (int i=0; i<width; ++i)
    if (!IsInVacuum(ox+i, (uint)y))
      return false;
  return true;
}

// TODO : for consistency, VerticalLine_IsInVacuum should use a 'height' as LigneH does it ...
bool Map::VerticalLine_IsInVacuum(int x, int top, int bottom) const
{
  ASSERT (top <= bottom);

  // Check we are still inside the world
  if (IsOutsideWorldX(x) || IsOutsideWorldYheight(top, bottom-top+1))
    return IsOpen();

  if (top < 0) top = 0;
  if ((int)GetHeight() <= bottom) bottom = GetHeight()-1;

  // Traite une ligne
  for (uint iy=(uint)top; iy<=(uint)bottom; iy++)
  {
    if (!IsInVacuum((uint)x, iy)) return false;
  }
  return true;
}

bool Map::RectIsInVacuum(const Rectanglei &prect) const
{
   // only check whether the border touch the ground

   Rectanglei rect(prect);

   // Clip rectangle in the the world area
   rect.Clip( Rectanglei(0, 0, GetWidth(), GetHeight()) );

   // Only check the borders of the rectangle
   if(rect.GetSizeX()==0 || rect.GetSizeY()==0)
     return true;

   if(!HorizontalLine_IsInVacuum (rect.GetPositionX(), rect.GetPositionY(), rect.GetSizeX()))
     return false;

   if(rect.GetSizeY() > 1)
   {
     if(!HorizontalLine_IsInVacuum (rect.GetPositionX(), rect.GetPositionY() + rect.GetSizeY() - 1, rect.GetSizeX()))
       return false;
     if(!VerticalLine_IsInVacuum (rect.GetPositionX(), rect.GetPositionY(), rect.GetPositionY() + rect.GetSizeY() -1))
       return false;

     if(rect.GetSizeX() > 1)
     if(!VerticalLine_IsInVacuum (rect.GetPositionX()+rect.GetSizeX()-1, rect.GetPositionY(), rect.GetPositionY() + rect.GetSizeY() -1))
       return false;
   }

   return true;
}

bool Map::ParanoiacRectIsInVacuum(const Rectanglei &prect) const
{
   // only check whether the rectangle touch the ground pixel by pixel
   // Prefere using the method above, as performing a pixel by pixel test is quite slow!

   Rectanglei rect(prect);

   // Clip rectangle in the the world area
   rect.Clip( Rectanglei(0, 0, GetWidth(), GetHeight()) );

   // Check line by line
   for( int i = rect.GetPositionY(); i < rect.GetPositionY() + rect.GetSizeY(); i++ )
     if( !HorizontalLine_IsInVacuum (rect.GetPositionX(), i, rect.GetSizeX()) )
       return false;

   return true;
}

bool Map::IsInVacuum_top(const PhysicalObj &obj, int dx, int dy) const
{
  return HorizontalLine_IsInVacuum (obj.GetTestRect().GetPositionX() + dx,
                                    obj.GetTestRect().GetPositionY() + obj.GetTestRect().GetSizeY() + dy,
                                    obj.GetTestRect().GetSizeX());
}

bool Map::IsInVacuum_bottom(const PhysicalObj &obj, int dx, int dy) const
{
  return HorizontalLine_IsInVacuum (obj.GetTestRect().GetPositionX() + dx,
                                    obj.GetTestRect().GetPositionY() + dy,
                                    obj.GetTestRect().GetSizeX());
}

bool Map::IsInVacuum_left(const PhysicalObj &obj, int dx, int dy) const
{
  return VerticalLine_IsInVacuum (obj.GetTestRect().GetPositionX() + dx,
                                  obj.GetTestRect().GetPositionY() + dy,
                                  obj.GetTestRect().GetPositionY() + obj.GetTestRect().GetSizeY() + dy);
}

bool Map::IsInVacuum_right(const PhysicalObj &obj, int dx, int dy) const
{
  return VerticalLine_IsInVacuum (obj.GetTestRect().GetPositionX() + obj.GetTestRect().GetSizeX() + dx,
                                  obj.GetTestRect().GetPositionY() + dy,
                                  obj.GetTestRect().GetPositionY() + obj.GetTestRect().GetSizeY() + dy);
}

void Map::DrawAuthorName()
{
  if (AUTHOR_INFO_TIME < Time::GetInstance()->Read()) {
    if (author_info1 != NULL) {
      delete author_info1;
      delete author_info2;
      author_info1 = author_info2 = NULL;
    }
    return;
  }

  if (author_info1 == NULL) {
    std::string txt;
    txt  = Format(_("Map %s, a creation of: "),
                  ActiveMap().ReadFullMapName().c_str());
    author_info1 = new Text(txt, white_color, Font::FONT_SMALL, Font::FONT_NORMAL);
    txt = ActiveMap().ReadAuthorInfo();
    author_info2 = new Text(txt, white_color, Font::FONT_SMALL, Font::FONT_NORMAL);
  }

  /* FIXME use a real layout here... not calculated positions */
  author_info1->DrawTopLeft(Point2i(AUTHOR_INFO_X,AUTHOR_INFO_Y));
  author_info2->DrawTopLeft(Point2i(AUTHOR_INFO_X,AUTHOR_INFO_Y+(*Font::GetInstance(Font::FONT_SMALL)).GetHeight()));
}

bool CompareRectangle(const Rectanglei& a, const Rectanglei& b)
{
  return ( a.GetTopLeftPoint() < b.GetTopLeftPoint() );
}

void Map::OptimizeCache(std::list<Rectanglei>& rectangleCache) const
{
  rectangleCache.sort(CompareRectangle);

  std::list<Rectanglei>::iterator it = rectangleCache.begin(),
    jt = rectangleCache.begin(),
    end = rectangleCache.end(),
    tmp;

  if (jt != end) {
    jt++;
  }
//  std::cout << "Before: " <<  rectangleCache.size()  << std::endl;

  while (it != end && jt != end) {
    if ( (*it).Contains(*jt) ) {
    //   std::cout << "X: " << (*jt).GetPositionX() << " ; " << (*jt).GetBottomRightPoint().GetX() << " - " ;
//       std::cout << "Y: " << (*jt).GetPositionY() << " ; " << (*jt).GetBottomRightPoint().GetY();
//       std::cout << std::endl;
      tmp = jt;
      ++tmp;
      rectangleCache.erase(jt);
      jt = tmp;

    } else if ( (*jt).Contains(*it) ) {
//       std::cout << "X: " << (*it).GetPositionX() << " ; " << (*it).GetBottomRightPoint().GetX() << " - " ;
//       std::cout << "Y: " << (*it).GetPositionY() << " ; " << (*it).GetBottomRightPoint().GetY();
//       std::cout << std::endl;
      tmp = it;
      if (tmp == rectangleCache.begin())
      {
        rectangleCache.erase(it);
        it = rectangleCache.begin();
        if (jt == it)
          jt++;
      }
      else
      {
        --tmp;
        rectangleCache.erase(it);
        it = tmp;
      }
    } else {
      it++;
      jt++;
    }
  }
//   std::cout << "After : " <<  rectangleCache.size()  << std::endl;

//   std::cout << "//#############################" <<std::endl;
}
