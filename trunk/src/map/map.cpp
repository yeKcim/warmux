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
 * Monde ou plateau de jeu.
 *****************************************************************************/

#include <iostream>
#include "map/map.h"
#include "object/physical_obj.h"
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "map/camera.h"
#include "map/maps_list.h"
#include "map/wind.h"
#include "game/game_time.h"
#include "object/objbox.h"
#include "tool/math_tools.h"

const Double MINIMUM_DISTANCE_BETWEEN_CHARACTERS = 50.0;

const uint AUTHOR_INFO_TIME = 5000; // ms
const uint AUTHOR_INFO_X = 100;
const uint AUTHOR_INFO_Y = 50;

Map::Map() : author_info1(NULL), author_info2(NULL)
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
  if (author_info1)
    delete author_info1;
  if (author_info2)
    delete author_info2;
}

void Map::Reset()
{
  sky.Reset();
  ground.Reset();
  water.Reset();
  Wind::GetRef().Reset();

  if (author_info1)
    delete author_info1;
  author_info1 = NULL;
  if (author_info2)
    delete author_info2;
  author_info2 = NULL;

  to_redraw->clear();
  to_redraw_now->clear();
  to_redraw_particles->clear();
  to_redraw_particles_now->clear();
}

void Map::Refresh()
{
  water.Refresh();
  Wind::GetRef().Refresh();
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

//  0: nothing
//  1: usual optimize cache
//  2: immediately tries to remove strictly included rectangles
//  4: try to perform a split; must add flag 2
//  8: try to perform another split; must add flag 2
// 16: erase rectangles from the particle list if they are contained by
//     rectangles of the other list
#define ADD_CHECK_LEVEL 1

#if ADD_CHECK_LEVEL & ~1
void Map::AddRectangle(std::list<Rectanglei>::iterator it, const Rectanglei& rect)
#else
void Map::AddRectangle(std::list<Rectanglei>::iterator /*it*/, const Rectanglei& rect)
#endif
{
  MSG_DEBUG("map.redraw", "Checking %ix%i at (%i,%i)\n",
            rect.GetSizeX(), rect.GetSizeY(), rect.GetPositionX(), rect.GetPositionY());

  if (rect.GetSizeX()<1 || rect.GetSizeY()<1)
    return;

#if ADD_CHECK_LEVEL&2
  Point2i TL = rect.GetTopLeftPoint();
  Point2i BR = rect.GetBottomRightPoint();

  while (it != to_redraw->end()) {
    Point2i iTL = it->GetTopLeftPoint();
    Point2i iBR = it->GetBottomRightPoint();

    // Is TL inside?
    if (iTL<=TL && TL<=iBR) {
      // TL inside, is BR?
      if (TL<=BR && BR<=iBR) {
        // BR also in
        MSG_DEBUG("map.redraw", "Inside %ix%i at (%i,%i)\n",
                  it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY());
        return;
      }
#if ADD_CHECK_LEVEL&4
      else {
        Rectanglei n(iTL, Point2i(BR-iTL+1));
        if (n.GetSizeX()*n.GetSizeY() - rect.GetSizeX()*rect.GetSizeY() < 20)
          *it = n;
        else {
          // iTL outside, ie higher or more to the left
          // iTL
          //  +-----+
          //  |TL   |
          //  | +---+-+
          //  | |iBR| |
          //  +-+---+-+
          //    +-----+BR
          MSG_DEBUG("map.redraw", "Overlap with %ix%i at (%i,%i)\n",
                    it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY());
          AddRectangle(it, Rectanglei(Point2i(iBR.GetX()+1, TL.GetY()), Point2i(BR.GetX()-iBR.GetX(), iBR.GetY()-TL.GetY()+1)));
          AddRectangle(it, Rectanglei(Point2i(TL.GetX(), iBR.GetY()+1), Point2i(rect.GetSizeX(), BR.GetY()-iBR.GetY())));
          return;
        }
      }
#endif
    }

    // Is BR inside?
#if ADD_CHECK_LEVEL&8
    if (iTL<=BR && BR<=iBR) {
      Rectanglei n(TL, Point2i(iBR-TL+1));
      if (n.GetSizeX()*n.GetSizeY() - rect.GetSizeX()*rect.GetSizeY() < 20)
        *it = n;
      else {
        // BR inside, we know from above that TL isn't

        // Swap previous image
        MSG_DEBUG("map.redraw", "Overlap with %ix%i at (%i,%i)\n",
                  it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY());
        AddRectangle(it, Rectanglei(TL, Point2i(rect.GetSizeX(), iTL.GetY()-TL.GetY())));
        AddRectangle(it, Rectanglei(Point2i(TL.GetX(), iTL.GetY()), Point2i(iTL.GetX()-TL.GetX(), BR.GetY()-iTL.GetY())));
        return;
      }
    }
#endif

    // Is "it" inside?
    if (TL<=iTL && iTL<=BR) {
      // iTL inside, is iBR?
      if (TL<=iBR && iBR<=BR) {
        // iBR is, remove
        MSG_DEBUG("map.redraw", "Removing %ix%i at (%i,%i)\n",
                  it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY());
        it = to_redraw->erase(it);
        continue;
      }
    }

    ++it;
  }
  MSG_DEBUG("map.redraw", "No overlap\n");
#endif

  to_redraw->push_back(rect);
}

void Map::ToRedrawOnScreen(Rectanglei r)
{
  assert(!r.IsSizeZero());
  r.SetPosition(r.GetPosition() + Camera::GetInstance()->GetPosition());
  AddRectangle(to_redraw->begin(), r);
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
  ground.Dig(position, surface);
  AddRectangle(to_redraw->begin(), Rectanglei(position, surface.GetSize()));
}

void Map::Dig(const Point2i& center, uint radius)
{
  ground.Dig(center, radius);
  radius += EXPLOSION_BORDER_SIZE;
  Point2i ra(radius, radius);
  AddRectangle(to_redraw->begin(), Rectanglei(center - ra, 2*ra));
}

void Map::PutSprite(const Point2i& pos, Sprite* spr)
{
  ground.PutSprite(pos, spr);
  AddRectangle(to_redraw->begin(), Rectanglei(pos, spr->GetSizeMax()));
}

void Map::MergeSprite(const Point2i& pos, Sprite * spr)
{
  Surface& tmp = spr->GetSurface();
  ground.MergeSprite(pos, tmp);
  AddRectangle(to_redraw->begin(), Rectanglei(pos, spr->GetSizeMax()));
}

void Map::DrawSky(bool redraw_all)
{
  SwitchDrawingCache();
  SwitchDrawingCacheParticles();

#if ADD_CHECK_LEVEL&8
  std::list<Rectanglei>& first = *to_redraw_now;
  std::list<Rectanglei>& second = *to_redraw_particles_now;
  for (std::list<Rectanglei>::iterator it1 = first.begin();
       it1 != first.end(); ++it1) {
    for (std::list<Rectanglei>::iterator it2 = second.begin();
         it2 != second.end(); it2++) {
      if (it1->Contains(*it2)) {
        it2 = second.erase(it2);
        //printf("2\n");
        if (it2 == second.end())
          break;
      }
    }

    if (it1 == first.end())
      break;
  }
  //printf("\n");
#endif

#if ADD_CHECK_LEVEL&1
  OptimizeCache(*to_redraw_now);
#endif
  // Particles never overlap, so don't optimize it

  sky.Draw(redraw_all);
}

void Map::Draw(bool redraw_all)
{
  // This is necessary because the WindParticles Sprites will set
  // the Rectanglei's to redraw in the to_redraw list using ToRedrawOnScreen
  std::list<Rectanglei> *tmp = to_redraw;
  to_redraw_particles->clear();
  to_redraw = to_redraw_particles;

  Wind::GetRef().DrawParticles();
  to_redraw = tmp;

//  Done from DrawSky
//  OptimizeCache(*to_redraw_now);

  ground.Draw(redraw_all);
}

bool Map::HorizontalLine_IsInVacuum(int ox, int y, int width) const
{
  // Process a line
  for (int i=0; i<width; ++i)
    if (!IsInVacuum(ox+i, y))
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
  if (GetHeight() <= bottom) bottom = GetHeight()-1;

  // Traite une ligne
  for (int iy=top; iy<=bottom; iy++) {
    if (!IsInVacuum(x, iy)) return false;
  }
  return true;
}

bool Map::RectIsInVacuum(const Rectanglei &prect) const
{
  // only check whether the border touch the ground
  Rectanglei rect(prect);

  // Clip rectangle in the the world area
  rect.Clip(Rectanglei(Point2i(), GetSize()));

  // Only check the borders of the rectangle
  if (rect.GetSizeX()==0 || rect.GetSizeY()==0)
    return true;

  if (!HorizontalLine_IsInVacuum (rect.GetPositionX(), rect.GetPositionY(), rect.GetSizeX()))
    return false;

  if (rect.GetSizeY() > 1) {
    if (!HorizontalLine_IsInVacuum(rect.GetPositionX(), rect.GetPositionY() + rect.GetSizeY() - 1, rect.GetSizeX()))
      return false;
    if (!VerticalLine_IsInVacuum(rect.GetPositionX(), rect.GetPositionY(),
                                 rect.GetPositionY() + rect.GetSizeY() -1))
      return false;

    if (rect.GetSizeX() > 1)
      if(!VerticalLine_IsInVacuum(rect.GetPositionX()+rect.GetSizeX()-1, rect.GetPositionY(),
                                  rect.GetPositionY() + rect.GetSizeY() -1))
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
  rect.Clip(Rectanglei(Point2i(), GetSize()));

  // Check line by line
  for (int i = rect.GetPositionY(); i < rect.GetPositionY() + rect.GetSizeY(); i++)
    if (!HorizontalLine_IsInVacuum (rect.GetPositionX(), i, rect.GetSizeX()))
      return false;

  return true;
}

bool Map::IsInVacuum_top(const PhysicalObj &obj, int dx, int dy) const
{
  const Rectanglei& rect = obj.GetTestRect();
  return HorizontalLine_IsInVacuum(rect.GetPositionX() + dx,
                                   rect.GetPositionY() + rect.GetSizeY()-1 + dy,
                                   rect.GetSizeX());
}

bool Map::IsInVacuum_bottom(const PhysicalObj &obj, int dx, int dy) const
{
  const Rectanglei& rect = obj.GetTestRect();
  return HorizontalLine_IsInVacuum(rect.GetPositionX() + dx,
                                   rect.GetPositionY() + dy,
                                   rect.GetSizeX());
}

bool Map::IsInVacuum_left(const PhysicalObj &obj, int dx, int dy) const
{
  const Rectanglei& rect = obj.GetTestRect();
  return VerticalLine_IsInVacuum(rect.GetPositionX() + dx,
                                 rect.GetPositionY() + dy,
                                 rect.GetPositionY() + rect.GetSizeY()-1 + dy);
}

bool Map::IsInVacuum_right(const PhysicalObj &obj, int dx, int dy) const
{
  const Rectanglei& rect = obj.GetTestRect();
  return VerticalLine_IsInVacuum(rect.GetPositionX() + rect.GetSizeX()-1 + dx,
                                 rect.GetPositionY() + dy,
                                 rect.GetPositionY() + rect.GetSizeY()-1 + dy);
}

void Map::DrawAuthorName()
{
  if (AUTHOR_INFO_TIME < GameTime::GetInstance()->Read()) {
    if (author_info1 != NULL) {
      delete author_info1;
      delete author_info2;
      author_info1 = author_info2 = NULL;
    }
    return;
  }

  if (author_info1 == NULL) {
    InfoMapBasicAccessor *basic = ActiveMap()->LoadBasicInfo();
    std::string txt = Format(_("Map %s, a creation of: "), basic->ReadFullMapName().c_str());
    author_info1 = new Text(txt, white_color, Font::FONT_SMALL, Font::FONT_BOLD, true);
    txt = basic->ReadAuthorInfo();
    author_info2 = new Text(txt, white_color, Font::FONT_SMALL, Font::FONT_BOLD, true);
  }

  /* FIXME use a real layout here... not calculated positions */
  author_info1->DrawLeftTop(Point2i(AUTHOR_INFO_X,AUTHOR_INFO_Y));
  author_info2->DrawLeftTop(Point2i(AUTHOR_INFO_X,AUTHOR_INFO_Y+(*Font::GetInstance(Font::FONT_SMALL)).GetHeight()));
}

static inline bool CompareRectangle(const Rectanglei& a, const Rectanglei& b)
{
  return a.GetTopLeftPoint() < b.GetTopLeftPoint();
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

  while (it != end && jt != end) {
    if ( (*it).Contains(*jt) ) {
      jt = rectangleCache.erase(jt);
      MSG_DEBUG("map.redraw", "%ix%i at (%i,%i) in %ix%i at (%i,%i)\n",
                jt->GetSizeX(), jt->GetSizeY(), jt->GetPositionX(), jt->GetPositionY(),
                it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY());
    } else if ( (*jt).Contains(*it) ) {
      tmp = it;
      MSG_DEBUG("map.redraw", "%ix%i at (%i,%i) in %ix%i at (%i,%i)\n",
                it->GetSizeX(), it->GetSizeY(), it->GetPositionX(), it->GetPositionY(),
                jt->GetSizeX(), jt->GetSizeY(), jt->GetPositionX(), jt->GetPositionY());
      if (tmp == rectangleCache.begin()) {
        rectangleCache.erase(it);
        it = rectangleCache.begin();
        if (jt == it)
          jt++;
      } else {
        --tmp;
        rectangleCache.erase(it);
        it = tmp;
      }
    } else {
      it++;
      jt++;
    }
  }
  MSG_DEBUG("map.redraw", "=======\n");
}

// traces ray, determining the collision point (if any)
// if no collision detected, TraceResult is left uninitialized
bool Map::TraceRay(const Point2i &start, const Point2i & end, TraceResult & tr, uint trace_flags)
{
  Point2d diff = end - start;
  Point2d delta = diff.GetNormal();
  Double length = diff.Norm();

  // FIXME: use some Bresenham-like algorithm
  Point2i prev_point = start;
  Point2i new_point = prev_point;
  Point2d iterated_point = start;
  while (!IsOutsideWorld(new_point) && length >= 0) {
    if (!IsInVacuum(new_point)) {
      if (trace_flags & COMPUTE_HIT) {
        tr.m_fraction = ONE - (length / diff.Norm());

        if (trace_flags & RETURN_LAST_IN_VACUUM_AS_HIT) {
          tr.m_hit = prev_point; // unless start is in vacuum, it's always in vacuum
        }
        else {
          tr.m_hit = new_point;
        }

        MSG_DEBUG( "map.collision", "tracing (%d,%d)->(%d,%d), collision at (%d,%d)",
          start.x, start.y,
          end.x, end.y,
          tr.m_hit.x, tr.m_hit.y
        );
      }
      return true ;
    };

    prev_point = new_point;
    iterated_point += delta;

    // not using automatic conversions to preserve call to round()
    // which was in the original find_first_contact function in Grapple
    new_point.x = (int)round(iterated_point.x);
    new_point.y = (int)round(iterated_point.y );
    length -= 1;
  }

  return false;
}
