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
 * Refresh de l'eau pouvant apparaitre en bas du terrain.
 *****************************************************************************/

#include "water.h"
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../graphic/video.h"
#include "../game/time.h"
#include "map.h"
#include "maps_list.h"
#include "camera.h"
#include "../interface/interface.h"
#include "../tool/resource_manager.h"
#include "../include/app.h"
using namespace Wormux;
//-----------------------------------------------------------------------------

// Vitesse d'animation des vagues
const uint WAVE_TIME=10;
// le pas d'avance des vagues en pixels
const uint WAVE_STEP=1;
const uint WAVE_HEIGHT = 5;

const uint GO_UP_TIME = 1; // min
const uint GO_UP_STEP = 15; // pixels
const uint GO_UP_OSCILLATION_TIME = 30; // seconds
const uint GO_UP_OSCILLATION_NBR = 30; // amplitude
const float t = (GO_UP_OSCILLATION_TIME*1000.0);
const float a = GO_UP_STEP/t;
const float b = 1.0;

//-----------------------------------------------------------------------------

void Water::Init()
{ 
   Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
   surface = resource_manager.LoadImage(res, "gfx/water");
   SDL_SetAlpha(surface, 0, 0);
   pattern = CreateRGBASurface(180, surface->h + 40, SDL_SWSURFACE|SDL_SRCALPHA);
   shift1 = 0;
}

//-----------------------------------------------------------------------------

void Water::Reset()
{
  actif = lst_terrain.TerrainActif().use_water;
  if(!actif) return;
  Init();
  hauteur_eau = WATER_INITIAL_HEIGHT;
  vague = 0;
  temps_eau = 0;
  temps_montee = GO_UP_TIME * 60 * 1000;
  height.clear();
  height.assign(180, 0);
  Refresh(); // Calculate first height position
}

//-----------------------------------------------------------------------------

void Water::Free()
{
  if(!actif) return;
  SDL_FreeSurface(surface);
  SDL_FreeSurface(pattern);
  height.clear();
}

//-----------------------------------------------------------------------------

void Water::Refresh()
{
  if (!actif) return;

  height_mvt = 0;

  ////////  Height Calculation:
  if (temps_montee < Wormux::global_time.Read())
  {
    if(temps_montee + GO_UP_OSCILLATION_TIME * 1000 > Wormux::global_time.Read())
    {
      uint dt=Wormux::global_time.Read()- temps_montee;
      height_mvt = GO_UP_STEP + (uint)(((float)GO_UP_STEP * sin(((float)(dt*(GO_UP_OSCILLATION_NBR-0.25))/GO_UP_OSCILLATION_TIME/1000.0)*2*M_PI))/(a*dt+b));
///;
    }
    else
    {
      temps_montee += GO_UP_TIME * 60 * 1000;
      hauteur_eau += GO_UP_STEP;
    }
  }

  ////////  Wave calculation:
  // on rempli le sol avec de l'eau
  if (WAVE_TIME < (Wormux::global_time.Read() - temps_eau))
  {
    temps_eau = Wormux::global_time.Read();
    vague += WAVE_STEP;
    if (surface->w <= vague) vague=0;
  }

  int x = -surface->w+vague;
  int y = world.GetHeight()-(hauteur_eau + height_mvt);

  double decree = (double) 2*M_PI/360;

  double angle1 = 0;
  double angle2 = shift1;
  do 
  {
    int offset=0;
    double y_pos = y + sin(angle1)*10 + sin(angle2)*10;
//    do 
//    {
      if (0<=x+offset) height.at(x+offset) = (int)y_pos;
//      offset += 180;
//    } while ((uint)offset+x < world.GetWidth());

    angle1 += 2*decree;
    angle2 += 4*decree;
    x++;
  } while ((uint)x < 180);

  shift1 += 4*decree;
}

//-----------------------------------------------------------------------------

void Water::Draw()
{
  if (!actif) return;

/*  for(uint x=0; x<world.GetWidth(); x++)
  for(uint y=height.at(x); y<world.GetHeight(); y+=surface->h)
  {
     AbsoluteDraw( surface, x, y);
  }
*/
  // Compute 1 pattern:
  SDL_SetAlpha(pattern, 0, 0);
  SDL_FillRect(pattern, NULL, 0x00000000);

  int y0 = world.GetHeight()-(hauteur_eau + height_mvt)-20;

  for(uint x=0; x<180; x++)
  {
    SDL_Rect dst = {x, height.at(x) - y0, surface->w, surface->h};
    SDL_BlitSurface(surface,NULL, pattern,&dst);
  }
  SDL_SetAlpha(pattern, SDL_SRCALPHA, 0);

  int x0 = (int)camera.GetX();
  while(x0<0)
    x0+=180;
  while(x0>180)
    x0-=180;

  for(int x=(int)camera.GetX()-x0;x<(int)camera.GetX()+(int)camera.GetWidth();x+=180)
  for(int y=y0;y<(int)camera.GetY()+(int)camera.GetHeight();y+=surface->h)
  {
    AbsoluteDraw(pattern, x, y);
  }
}

//-----------------------------------------------------------------------------

int Water::GetHeight(int x)
{
  if (IsActive())
  {
    while(x<0)
      x += 180;
    while(x>=180)
      x -= 180;
    return height.at(x);
  }
  else
    return world.GetHeight();
}

//-----------------------------------------------------------------------------

bool Water::IsActive() {return actif;}

//-----------------------------------------------------------------------------
