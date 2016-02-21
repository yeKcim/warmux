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
 * Terrain de jeu.
 *****************************************************************************/

#include "ground.h"
//-----------------------------------------------------------------------------
#include <iostream>
#include <SDL_gfxPrimitives.h>
#include "map.h"
#include "maps_list.h"
#include "../map/camera.h"
#include "../graphic/video.h"
#include <limits.h>

#include "../include/constant.h"
#include "../include/app.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"
#include <SDL_video.h>

#ifdef DEBUG
//#  define DESSINE_BORDURE_CANVAS
#endif

//-----------------------------------------------------------------------------
namespace Wormux {

//-----------------------------------------------------------------------------

Ground::Ground()
{ //FIXME (a effacer) 
}

//-----------------------------------------------------------------------------

void Ground::Init()
{
  std::cout << "o " << _("Ground initialization...") << ' ';
  std::cout.flush();
  
  // Charge les données du terrain
  SDL_Surface *m_image = lst_terrain.TerrainActif().LitImgTerrain();
  LoadImage ( m_image);
  // delete m_image; -> Done after Terrain initialization

  // Vérifie la taille du terrain
  assert (LARG_MIN_TERRAIN <= GetWidth());
  assert (HAUT_MIN_TERRAIN <= GetHeight());
  assert ((ulong)GetWidth()*GetHeight() <= TAILLE_MAX_TERRAIN);
  
  // Vérifie si c'est un terrain ouvert ou fermé
  ouvert = lst_terrain.TerrainActif().is_opened;

  std::cout << _("done") << std::endl;
}

//-----------------------------------------------------------------------------

void Ground::Reset()
{
  Init();
  lastx = lasty = INT_MAX;
}

//-----------------------------------------------------------------------------

// Lit la valeur alpha du pixel (x,y)
bool Ground::EstDansVide (int x, int y)
{ 
  // En dehors du monde : c'est vide :-p
  //  if (monde.EstHorsMondeXY(x,y)) return config.exterieur_monde_vide;
  assert (!world.EstHorsMondeXY(x,y));
  if(TerrainActif().infinite_bg)
  {
    if(x < 0 || y<0 || x>static_cast<int>(GetWidth()) || y>static_cast<int>(GetHeight()))
      return true;
  }

  // Lit le monde
   return EstTransparent( GetAlpha(x,y) );
}

//-----------------------------------------------------------------------------
//Renvoie l'angle entre la tangeante au terrain en (x,y) et l'horizontale.
//l'angle est toujours > 0.
//Renvoie -1.0 s'il n'y a pas de tangeante (si le pixel(x,y) ne touche
//aucun autre morceau de terrain)
double Ground::Tangeante(int x,int y)
{
  //Approxiamtion:on renvoie la corde de la courbe formée
  //par le terrain...

  //On cherche deux points du terrain autour de (x,y), à la limite entre le terrain
  //et le vide:
  //(p1 = 1er point à gauche
  // p2 = 1er point à droite
  // p3 = 2em point à gauche
  // p4 = 2em point à droite)
  Point2i p1,p2,p3,p4;
  if(!PointContigu(x,y, p1.x,p1.y, -1,-1))
    return -1.0;
  
  if(!PointContigu(x,y, p2.x,p2.y, p1.x,p1.y))
  {
    p2.x = x;
    p2.y = y;
  }

  if(!PointContigu(p1.x,p1.y, p3.x,p3.y, x,y))
  {
    p3.x = p1.x;
    p3.y = p1.y;
  }
  if(!PointContigu(p2.x,p2.y, p4.x,p4.y, x,y))
  {
    p4.x = p2.x;
    p4.y = p2.y;
  }

  if(p3.x == p4.x)
    return M_PI / 2.0;
  if(p3.y == p4.y)
    return M_PI;

  assert (p3.x != p4.x);
  
  double tangeante = atan((double)(p4.y-p3.y)/(double)(p4.x-p3.x));

  while(tangeante <= 0.0)
    tangeante += M_PI;
  while(tangeante > M_PI)
    tangeante -= M_PI;

  return tangeante;
}

//-----------------------------------------------------------------------------
bool Ground::PointContigu(int x,int y,  int & p_x,int & p_y,
                           int pas_bon_x,int pas_bon_y)
{
  //Cherche un pixel autour du pixel(x,y) qui est à la limite entre
  //le terrin et le vide.
  //renvoie true (+ p_x et p_y) si on a trouvé qqch, sinon false
  if(world.EstHorsMonde(Point2i(x-1,y))
  || world.EstHorsMonde(Point2i(x+1,y))
  || world.EstHorsMonde(Point2i(x,y-1))
  || world.EstHorsMonde(Point2i(x,y+1)) )
    return false;
   
  //regarde en haut à gauche
  if(x-1 != pas_bon_x
  || y-1 != pas_bon_y)
  if(!EstDansVide(x-1,y-1)
  &&(EstDansVide(x-1,y)
  || EstDansVide(x,y-1)))
  {
    p_x=x-1;
    p_y=y-1;
    return true;
  }
  //regarde en haut
  if(x != pas_bon_x
  || y-1 != pas_bon_y)
  if(!EstDansVide(x,y-1)
  &&(EstDansVide(x-1,y-1)
  || EstDansVide(x+1,y-1)))
  {
    p_x=x;
    p_y=y-1;
    return true;
  }
  //regarde en haut à droite
  if(x+1 != pas_bon_x
  || y-1 != pas_bon_y)
  if(!EstDansVide(x+1,y-1)
  &&(EstDansVide(x,y-1)
  || EstDansVide(x+1,y)))
  {
    p_x=x+1;
    p_y=y-1;
    return true;
  }
  //regarde à droite
  if(x+1 != pas_bon_x
  || y != pas_bon_y)
  if(!EstDansVide(x+1,y)
  &&(EstDansVide(x+1,y-1)
  || EstDansVide(x,y+1)))
  {
    p_x=x+1;
    p_y=y;
    return true;
  }
  //regarde en bas à droite
  if(x+1 != pas_bon_x
  || y+1 != pas_bon_y)
  if(!EstDansVide(x+1,y+1)
  &&(EstDansVide(x+1,y)
  || EstDansVide(x,y+1)))
  {
    p_x=x+1;
    p_y=y+1;
    return true;
  }
  //regarde en bas
  if(x != pas_bon_x
  || y+1 != pas_bon_y)
  if(!EstDansVide(x,y+1)
  &&(EstDansVide(x-1,y+1)
  || EstDansVide(x+1,y+1)))
  {
    p_x=x;
    p_y=y+1;
    return true;
  }
  //regarde en bas à gauche
  if(x-1 != pas_bon_x
  || y+1 != pas_bon_y)
  if(!EstDansVide(x-1,y+1)
  &&(EstDansVide(x-1,y)
  || EstDansVide(x,y+1)))
  {
    p_x=x-1;
    p_y=y+1;
    return true;
  }
  //regarde à gauche
  if(x-1 == pas_bon_x
  && y == pas_bon_y)
  if(!EstDansVide(x-1,y)
  &&(EstDansVide(x-1,y-1)
  || EstDansVide(x-1,y+1)))
  {
    p_x=x-1;
    p_y=y;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

void Ground::Draw()
{
  int cx = camera.GetX();
  int cy = camera.GetY();  
  
  if (camera.HasFixedX()) {// ground is less wide than screen !
    uint margin = (video.GetWidth()-GetWidth())/2;
    boxRGBA(app.sdlwindow, 0, 0,margin, video.GetHeight(),
	    0, 0, 0, 255); 
    boxRGBA(app.sdlwindow, video.GetWidth()-margin, 0, video.GetWidth(), video.GetHeight(),
	    0, 0, 0, 255); 
  }

  if (camera.HasFixedY()) {// ground is less wide than screen !
    uint margin = (video.GetHeight()-GetHeight())/2;
    boxRGBA(app.sdlwindow, 0, 0, video.GetWidth(), margin,
	    0, 0, 0, 255); 
    boxRGBA(app.sdlwindow, 0, video.GetHeight()-margin, video.GetWidth(), video.GetHeight(),
	    0, 0, 0, 255); 
  }

  if (lastx != cx || lasty != cy)
  {
    lastx = cx;
    lasty = cy;
    DrawTile();
    return;
  }
  lastx = cx;
  lasty = cy; 

  std::list<Rectanglei>::iterator
    it=world.to_redraw_now->begin(),
    end=world.to_redraw_now->end();
  for (; it != end; ++it) DrawTile_Clipped(*it);

  // Draw on top of sky (redisplayed on top of particles)
  it=world.to_redraw_particles_now->begin();
  end=world.to_redraw_particles_now->end();
  for (; it != end; ++it) DrawTile_Clipped(*it);

  // Draw on top of new position of particles (redisplayed on top of particles)
  it=world.to_redraw_particles->begin();
  end=world.to_redraw_particles->end();
  for (; it != end; ++it) DrawTile_Clipped(*it);
}

//-----------------------------------------------------------------------------
} // namespace Wormux
