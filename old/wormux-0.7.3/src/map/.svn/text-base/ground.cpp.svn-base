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
#include <iostream>
#include <SDL_video.h>
#include <SDL_gfxPrimitives.h>
#include <limits.h>
#include "camera.h"
#include "map.h"
#include "maps_list.h"
#include "../graphic/surface.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"

Ground::Ground()
{ //FIXME (a effacer) 
}

void Ground::Init(){
  std::cout << "o " << _("Ground initialization...") << ' ';
  std::cout.flush();
  
  // Charge les données du terrain
  Surface m_image = lst_terrain.TerrainActif().LitImgTerrain();
  LoadImage ( m_image );

  // Vérifie la taille du terrain
  assert(Constants::MAP_MIN_SIZE <= GetSize());
  assert(GetSizeX()*GetSizeY() <= Constants::MAP_MAX_SIZE);
  
  // Vérifie si c'est un terrain ouvert ou fermé
  ouvert = lst_terrain.TerrainActif().is_opened;

  std::cout << _("done") << std::endl;
}

void Ground::Reset(){
  Init();
  lastPos.SetValues(INT_MAX, INT_MAX);
}

// Lit la valeur alpha du pixel (x,y)
bool Ground::IsEmpty(const Point2i &pos){ 
	assert( !world.EstHorsMondeXY(pos.x, pos.y) );
	if( TerrainActif().infinite_bg){
		if( !Contains(pos) )
			return true;
	}

	// Lit le monde
	return GetAlpha( pos ) != 255; // IsTransparent
}

//Renvoie l'angle entre la tangeante au terrain en (x,y) et l'horizontale.
//l'angle est toujours > 0.
//Renvoie -1.0 s'il n'y a pas de tangeante (si le pixel(x,y) ne touche
//aucun autre morceau de terrain)
double Ground::Tangeante(int x,int y){
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
  if( !IsEmpty(Point2i(x-1,y-1) )
  &&( IsEmpty(Point2i(x-1,y))
  || IsEmpty(Point2i(x,y-1))))
  {
    p_x=x-1;
    p_y=y-1;
    return true;
  }
  //regarde en haut
  if(x != pas_bon_x
  || y-1 != pas_bon_y)
  if(!IsEmpty(Point2i(x,y-1))
  &&(IsEmpty(Point2i(x-1,y-1))
  || IsEmpty(Point2i(x+1,y-1))))
  {
    p_x=x;
    p_y=y-1;
    return true;
  }
  //regarde en haut à droite
  if(x+1 != pas_bon_x
  || y-1 != pas_bon_y)
  if(!IsEmpty(Point2i(x+1,y-1))
  &&(IsEmpty(Point2i(x,y-1))
  || IsEmpty(Point2i(x+1,y))))
  {
    p_x=x+1;
    p_y=y-1;
    return true;
  }
  //regarde à droite
  if(x+1 != pas_bon_x
  || y != pas_bon_y)
  if(!IsEmpty(Point2i(x+1,y))
  &&(IsEmpty(Point2i(x+1,y-1))
  || IsEmpty(Point2i(x,y+1))))
  {
    p_x=x+1;
    p_y=y;
    return true;
  }
  //regarde en bas à droite
  if(x+1 != pas_bon_x
  || y+1 != pas_bon_y)
  if(!IsEmpty(Point2i(x+1,y+1))
  &&(IsEmpty(Point2i(x+1,y))
  || IsEmpty(Point2i(x,y+1))))
  {
    p_x=x+1;
    p_y=y+1;
    return true;
  }
  //regarde en bas
  if(x != pas_bon_x
  || y+1 != pas_bon_y)
  if(!IsEmpty(Point2i(x,y+1))
  &&(IsEmpty(Point2i(x-1,y+1))
  || IsEmpty(Point2i(x+1,y+1))))
  {
    p_x=x;
    p_y=y+1;
    return true;
  }
  //regarde en bas à gauche
  if(x-1 != pas_bon_x
  || y+1 != pas_bon_y)
  if(!IsEmpty(Point2i(x-1,y+1))
  &&(IsEmpty(Point2i(x-1,y))
  || IsEmpty(Point2i(x,y+1))))
  {
    p_x=x-1;
    p_y=y+1;
    return true;
  }
  //regarde à gauche
  if(x-1 == pas_bon_x
  && y == pas_bon_y)
  if(!IsEmpty(Point2i(x-1,y))
  &&(IsEmpty(Point2i(x-1,y-1))
  || IsEmpty(Point2i(x-1,y+1))))
  {
    p_x=x-1;
    p_y=y;
    return true;
  }
  return false;
}

void Ground::Draw()
{
  AppWormux * app = AppWormux::GetInstance();

  Point2i cPos = camera.GetPosition();
  Point2i windowSize = app->video.window.GetSize();
  Point2i margin = (windowSize - GetSize())/2;
  
  if( camera.HasFixedX() ){// ground is less wide than screen !
    app->video.window.BoxColor( Rectanglei(0, 0, margin.x, windowSize.y), black_color);
    app->video.window.BoxColor( Rectanglei(windowSize.x - margin.x, 0, margin.x, windowSize.y), black_color);
  }

  if( camera.HasFixedY() ){// ground is less wide than screen !
    app->video.window.BoxColor( Rectanglei(0, 0, windowSize.x, margin.y), black_color);
    app->video.window.BoxColor( Rectanglei(0, windowSize.y - margin.y, windowSize.x, margin.y), black_color);
  }

  if( lastPos != cPos ){
    lastPos = cPos;
    DrawTile();
    return;
  }

  RedrawParticleList(*world.to_redraw_now);

  // Draw on top of sky (redisplayed on top of particles)
  RedrawParticleList(*world.to_redraw_particles_now);

  // Draw on top of new position of particles (redisplayed on top of particles)
  RedrawParticleList(*world.to_redraw_particles);
}

void Ground::RedrawParticleList(std::list<Rectanglei> &list){
	std::list<Rectanglei>::iterator it;

	for( it = list.begin(); it != list.end(); ++it )
		DrawTile_Clipped(*it);
}
