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
 * Refresh water that may be placed in bottom of the ground.
 *****************************************************************************/

#include "map/water.h"
#include <assert.h>
#include <SDL.h>
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "game/time.h"
#include "interface/interface.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"

const uint GO_UP_TIME = 1; // min
const uint GO_UP_STEP = 15; // pixels
const uint GO_UP_OSCILLATION_TIME = 30; // seconds
const uint GO_UP_OSCILLATION_NBR = 30; // amplitude
const float t = (GO_UP_OSCILLATION_TIME*1000.0);
const float a = GO_UP_STEP/t;
const float b = 1.0;
int pattern_height = 0; // TODO: relocate

/*
 * Water consists of 1) water.png texture, which is the actual wave and
 * 2) water_bottom.png, which is water below the wave. Composing the water in
 * this way is done because of optimization purposes.
 *
 * The water consists of three waves, which are drawn using SIN functions in
 * different phases.
 *
 * The water is drawn in 180 x 128 blocks. The size 180 comes from SIN(2x)
 * cycle. The pattern surface is rendered using water.png (and SIN functions)
 * and water_bottom.png.
 */
void Water::Init(){
   Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

   surface = resource_manager.LoadImage(res, "gfx/water");
   surface.SetAlpha(0, 0);

   bottom = resource_manager.LoadImage(res, "gfx/water_bottom");
   bottom.SetAlpha(0, 0);

   pattern_height = bottom.GetHeight();

   pattern.NewSurface(Point2i(pattern_width, pattern_height),
                      SDL_SWSURFACE|SDL_SRCALPHA, true);
   /* Convert the pattern into the same format than surface. This allow not to
    * need conversions on fly and thus saves CPU */
   pattern.SetSurface(
       SDL_ConvertSurface(pattern.GetSurface(),
                          surface.GetSurface()->format,
                          SDL_SWSURFACE|SDL_SRCALPHA),
       true /* free old one */);

   // Turn on transparency for water bottom texture
   bottom.SetSurface(
       SDL_ConvertSurface(bottom.GetSurface(),
                          bottom.GetSurface()->format,
                          SDL_SWSURFACE|SDL_SRCALPHA),
       true);


   shift1 = 0;
   resource_manager.UnLoadXMLProfile(res);
}

void Water::Reset(){
  actif = ActiveMap().UseWater();
  if(!actif) return;
  Init();
  hauteur_eau = WATER_INITIAL_HEIGHT;
  temps_montee = GO_UP_TIME * 60 * 1000;
  Refresh(); // Calculate first height position
}

void Water::Free(){
  if(!actif)
    return;
  bottom.Free();
  surface.Free();
  pattern.Free();
  pattern_height = 0;
}

void Water::Refresh(){
  if (!actif)
    return;

  height_mvt = 0;

  // Height Calculation:
  Time * global_time = Time::GetInstance();
  if (temps_montee < global_time->Read())
  {
    if(temps_montee + GO_UP_OSCILLATION_TIME * 1000 > global_time->Read()){
      uint dt=global_time->Read()- temps_montee;
      height_mvt = GO_UP_STEP +
        (uint)(((float)GO_UP_STEP *
               sin(((float)(dt*(GO_UP_OSCILLATION_NBR-0.25))
                   / GO_UP_OSCILLATION_TIME/1000.0)*2*M_PI)
               )/(a*dt+b));
    }
    else{
      temps_montee += GO_UP_TIME * 60 * 1000;
      hauteur_eau += GO_UP_STEP;
    }
  }

}

void Water::Draw(){
  if (!actif)
    return;

  /* Now the wave has changed, we need to build the new image pattern */
  pattern.SetAlpha(0, 0);
  pattern.Fill(0x00000000);

  /* Locks on SDL_Surface must be taken when accessing pixel member */
  SDL_LockSurface(surface.GetSurface());
  SDL_LockSurface(pattern.GetSurface());
  SDL_LockSurface(bottom.GetSurface());

  /* Copy directly the surface image into the pattern image. This doesn't use
   * blit in order to save CPU but it makes this code not really easy to read...
   * The copy is done pixel per pixel */
  uint bpp = surface.GetSurface()->format->BytesPerPixel;

  double degree = static_cast<double>(2*M_PI/360.0);
  double angle1 = -shift1;
  double angle2 = shift1;
  double a = 5, b = 8;
  int wave_height[3];
  const int wave_inc = 5;
  const int wave_count = 3;

  for (uint x = 0; x < pattern_width; x++)
  {
    assert (wave_count == 3);
    wave_height[0] = static_cast<int>(sin(angle1)*a + sin(angle2)*b);
    wave_height[1] = static_cast<int>(sin(angle1+M_PI)*a + sin(angle2+10*degree)*b);
    wave_height[2] = static_cast<int>(sin(angle1+M_PI/2)*a + sin(angle2+20*degree)*b);

    int top = std::max(wave_height[0], wave_height[1]);
    height[x] = std::max(top, wave_height[2]);

    int l = (int)(a + b) * 2 + wave_inc * wave_count + 32; // 32 = pattern slide length (in texture)
    assert(l < pattern_height);
    Uint32 pitch = pattern.GetSurface()->pitch;
    Uint8 *dst = (Uint8*)pattern.GetSurface()->pixels + l*pitch;
    const Uint8 *src = (Uint8*)bottom.GetSurface()->pixels + l*pitch;
    for (; l < pattern_height; l++)
    {
      memcpy(dst, src, bpp * pattern_width);
      dst += pitch;
      src += pitch;
    }

    int wave;
    for (wave = 0; wave < wave_count; wave++)
    {
      dst = (Uint8*)pattern.GetSurface()->pixels + x * bpp
          + (wave_height[wave]+15+wave_inc*wave) * pitch;
      src = (Uint8*)surface.GetSurface()->pixels;
      for (uint y=0; y<(uint)surface.GetHeight(); y++)
      {
        memcpy(dst, src, bpp);
        dst += pitch;
        src += bpp;
      }
    }

    angle1 += 2*degree;
    angle2 += 4*degree;
  }

  shift1 += 4*degree;

  SDL_UnlockSurface(bottom.GetSurface());
  SDL_UnlockSurface(pattern.GetSurface());
  SDL_UnlockSurface(surface.GetSurface());

  pattern.SetAlpha(SDL_SRCALPHA, 0);
  int x0 = Camera::GetInstance()->GetPosition().x % pattern_width;

  int r = 0;
  for(int y = world.GetHeight() - (hauteur_eau + height_mvt) - 20;
      y < (int)Camera::GetInstance()->GetPosition().y + (int)Camera::GetInstance()->GetSize().y;
      y += pattern_height)
  {
    Surface *bitmap = r ? &bottom : &pattern;
    for(int x = Camera::GetInstance()->GetPosition().x - x0;
        x < Camera::GetInstance()->GetPosition().x + Camera::GetInstance()->GetSize().x;
        x += pattern_width)
    {
      AbsoluteDraw(*bitmap, Point2i(x, y));
    }
    r++;
  }
}

int Water::GetHeight(int x) const
{
  if (IsActive())
    return height[x % pattern_width]
           + world.GetHeight()
           - (hauteur_eau + height_mvt);
  else
    return world.GetHeight();
}
