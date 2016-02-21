/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "map.h"
//-----------------------------------------------------------------------------
#include <iostream>
#include "../game/config.h"
#include "../include/constant.h"
#include "../object/bonus_box.h"
#include "../tool/i18n.h"
#include "../game/time.h"
#include "wind.h"
#include "camera.h"
#include "maps_list.h"
#include "../tool/stats.h"
#include "../include/global.h"

using namespace std;
using namespace Wormux;
//-----------------------------------------------------------------------------
const double DST_MIN_ENTRE_VERS = 50.0;

const uint AUTHOR_INFO_TIME = 5000; // ms
const uint AUTHOR_INFO_X = 100;
const uint AUTHOR_INFO_Y = 50;
//-----------------------------------------------------------------------------
Map world;
//-----------------------------------------------------------------------------

Map::Map()
{
  dst_min_entre_vers = DST_MIN_ENTRE_VERS;

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


//-----------------------------------------------------------------------------

void Map::Reset()
{
  sky.Reset();
  ground.Reset();
  water.Reset();
  wind.Reset();

  // Configure le jeu selon que le terrain soit ouvert ou non
  bool ouvert = ground.EstOuvert();
  caisse.Active (ouvert);
  config.exterieur_monde_vide = ouvert;

  delete author_info1; author_info1 = NULL;
  delete author_info2; author_info2 = NULL;

  to_redraw->clear();
  to_redraw_now->clear();
  to_redraw_particles->clear();
  to_redraw_particles_now->clear();
}

//-----------------------------------------------------------------------------

void Map::Refresh()
{
  water.Refresh();
  wind.Refresh();
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void Map::ToRedrawOnMap(Rectanglei r)
{
  to_redraw->push_back(r);

}

//-----------------------------------------------------------------------------

void Map::ToRedrawOnScreen(Rectanglei r)
{
  to_redraw->push_back(Rectanglei(r.x+camera.GetX(), 
				  r.y+camera.GetY(), r.w, r.h));
}

//-----------------------------------------------------------------------------

void Map::SwitchDrawingCache()
{
  std::list<Rectanglei> *tmp = to_redraw_now;
  to_redraw_now = to_redraw;
  to_redraw = tmp;
  to_redraw->clear();
}

//-----------------------------------------------------------------------------

void Map::SwitchDrawingCacheParticles()
{
  std::list<Rectanglei> *tmp = to_redraw_particles_now;
  to_redraw_particles_now = to_redraw_particles;
  to_redraw_particles = tmp;
  to_redraw_particles->clear();
}

//-----------------------------------------------------------------------------

void Map::Creuse (uint x, uint y, SDL_Surface *surface)
{
   ground.Dig (x, y, surface);
   to_redraw->push_back(Rectanglei(x, y, surface->w, surface->h));
}

//-----------------------------------------------------------------------------

void Map::DrawSky()
{ 
  SwitchDrawingCache();
  SwitchDrawingCacheParticles();
  sky.Draw(); 
}

//-----------------------------------------------------------------------------

void Map::DrawWater()
{ water.Draw(); }

//-----------------------------------------------------------------------------

void Map::Draw()
{ 
  std::list<Rectanglei> *tmp = to_redraw;
  to_redraw_particles->clear();
  to_redraw = to_redraw_particles;
  
  wind.DrawParticles();
  to_redraw = tmp;

  ground.Draw(); 
}

//-----------------------------------------------------------------------------

bool Map::EstHorsMondeX(int x) const
{ return ((x < 0) || ((int)GetWidth() <= x)) && !TerrainActif().infinite_bg; }

bool Map::EstHorsMondeY(int y) const
{ return (((y < 0) && !TerrainActif().infinite_bg) || ((int)GetHeight() <= y)); }

bool Map::EstHorsMondeXlarg(int x, uint larg) const
{ return ((x+(int)larg-1 < 0) || ((int)GetWidth() <= x)) && !TerrainActif().infinite_bg; }

bool Map::EstHorsMondeYhaut(int y, uint haut) const
{ return ((y+(int)haut-1 < 0  && !TerrainActif().infinite_bg) || ((int)GetHeight() <= y)); }

bool Map::EstHorsMondeXY(int x, int y) const
{ return EstHorsMondeX(x) || EstHorsMondeY(y) && !TerrainActif().infinite_bg; }

bool Map::EstHorsMonde (const Point2i &pos) const
{ return EstHorsMondeXY (pos.x, pos.y); }

//-----------------------------------------------------------------------------

bool Map::EstDansVide(int x, int y)
{ return ground.EstDansVide (x,y); }

//-----------------------------------------------------------------------------

bool Map::LigneH_EstDansVide (int ox, int y, int width)
{ 
  // Traite une ligne
  for (int i=0; i<width; i++) 
     {
	if (!EstDansVide(ox+i, (uint)y)) 
	  {
	     return false;
	  }
     }
   
   return true;
}

//-----------------------------------------------------------------------------

bool Map::LigneV_EstDansVide (int x, int top, int bottom)
{ 
  assert (top <= bottom);

  // Vérifie qu'on reste dans le monde
  if (EstHorsMondeX(x) || EstHorsMondeYhaut(top, bottom-top+1))
    return config.exterieur_monde_vide;
  if (top < 0) top = 0;
  if ((int)GetHeight() <= bottom) bottom = GetHeight()-1;

  // Traite une ligne
  for (uint iy=(uint)top; iy<=(uint)bottom; iy++) 
  {
    if (!EstDansVide((uint)x, iy)) return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool Map::RectEstDansVide (const Rectanglei &prect)
{
   Rectanglei rect(prect);

   // Clip rectangle in the the world area
   rect.Clip( Rectanglei(0,0,GetWidth(),GetHeight())); 
   
   // Check line by line
   for (int i=rect.y; i < rect.y+rect.h; i++)
     {
	if (!LigneH_EstDansVide (rect.x, i, rect.w))
	  {
	     return false;
	  }
     }
   
   return true;
}

//-----------------------------------------------------------------------------

bool Map::EstDansVide_haut (const PhysicalObj &obj, int dx, int dy)
{
  return LigneH_EstDansVide (obj.GetTestRect().x+dx,
			     obj.GetTestRect().y+obj.GetTestRect().h+dy,
			     obj.GetTestRect().w);
}

//-----------------------------------------------------------------------------

bool Map::EstDansVide_bas (const PhysicalObj &obj, int dx, int dy)
{
  return LigneH_EstDansVide (obj.GetTestRect().x+dx,
			     obj.GetTestRect().y+dy,
			     obj.GetTestRect().w);
}

//-----------------------------------------------------------------------------

bool Map::IsInVacuum_left (const PhysicalObj &obj, int dx, int dy)
{
  return LigneV_EstDansVide (obj.GetTestRect().x+dx,
			     obj.GetTestRect().y+dy,
			     obj.GetTestRect().y+obj.GetTestRect().h+dy);
}

//-----------------------------------------------------------------------------

bool Map::IsInVacuum_right (const PhysicalObj &obj, int dx, int dy)
{
  return LigneV_EstDansVide (obj.GetTestRect().x+obj.GetTestRect().w+dx,
			     obj.GetTestRect().y+dy,
			     obj.GetTestRect().y+obj.GetTestRect().h+dy);
}

//-----------------------------------------------------------------------------

void Map::DrawAuthorName()
{
  if (AUTHOR_INFO_TIME < global_time.Read()) {
    if (author_info1 != NULL) {
      delete author_info1;
      delete author_info2;
      author_info1 = author_info2 = NULL;
    }
    return;
  }

  if (author_info1 == NULL) {
    std::string txt;
    txt  = Format(_("Map %s, a creation of :"),
		  lst_terrain.TerrainActif().name.c_str());
    author_info1 = new Text(txt, white_color, &global().small_font());
    txt = lst_terrain.TerrainActif().author_info;
    author_info2 = new Text(txt, white_color, &global().small_font());
  }
  
  author_info1->DrawTopLeft(AUTHOR_INFO_X,AUTHOR_INFO_Y);
  author_info2->DrawTopLeft(AUTHOR_INFO_X,AUTHOR_INFO_Y+global().small_font().GetHeight());
}

//-----------------------------------------------------------------------------
