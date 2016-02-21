/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
#include "game/game_mode.h"
#include "game/time.h"
#include "interface/interface.h"
#include "particles/particle.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"

const uint GO_UP_TIME = 1; // min
const uint GO_UP_STEP = 15; // pixels
const uint GO_UP_OSCILLATION_TIME = 30; // seconds
const uint GO_UP_OSCILLATION_NBR = 30; // amplitude
const float t = (GO_UP_OSCILLATION_TIME*1000.0);
const float a = GO_UP_STEP/t;
const float b = 1.0;

int Water::pattern_height = 0;

Water::Water() :
  type_color(NULL),
  height_mvt(0),
  shift1(0),
  water_height(0),
  time_raise(0),
  water_type(NO_WATER),
  m_last_preview_redraw(0)
{
  for (uint i = 0; i < pattern_width; i++) {
    height[i] = 0;
  }
}

Water::~Water()
{
  if (type_color)
    delete type_color;
}

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
void Water::Init()
{
  if (water_type == NO_WATER)
    return;

  std::string image = "gfx/";
  std::string water_name = GetWaterName(water_type);
  image += water_name;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  surface = GetResourceManager().LoadImage(res, image);
  surface.SetAlpha(0, 0);

  image += "_bottom";

  if (water_type != NO_WATER)
    type_color = new Color(GetResourceManager().LoadColor(res, "water_colors/" + water_name));
  else
    type_color = NULL;
  bottom = GetResourceManager().LoadImage(res, image);
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
  GetResourceManager().UnLoadXMLProfile(res);
}

void Water::Reset()
{
  water_type = ActiveMap()->GetWaterType();
  if (type_color)
    delete type_color;
  type_color = NULL;

  if (!IsActive())
    return;

  Init();
  water_height = WATER_INITIAL_HEIGHT;
  time_raise = 1000 * GameMode::GetInstance()->duration_before_death_mode;

  Refresh(); // Calculate first height position
}

void Water::Free()
{
  if (!IsActive())
    return;

  bottom.Free();
  surface.Free();
  pattern.Free();
  pattern_height = 0;
}

void Water::Refresh()
{
  if (!IsActive())
    return;

  height_mvt = 0;

  // Height Calculation:
  Time * global_time = Time::GetInstance();
  if (time_raise < global_time->Read())
  {
    m_last_preview_redraw = global_time->Read();
    if (time_raise + GO_UP_OSCILLATION_TIME * 1000 > global_time->Read()) {
      uint dt = global_time->Read() - time_raise;
      height_mvt = GO_UP_STEP +
        (uint)(((float)GO_UP_STEP *
               sin(((float)(dt*(GO_UP_OSCILLATION_NBR-0.25))
                   / GO_UP_OSCILLATION_TIME/1000.0)*2*M_PI)
               )/(a*dt+b));
    }
    else{
      time_raise += GO_UP_TIME * 60 * 1000;
      water_height += GO_UP_STEP;
    }
  }

}

void Water::Draw()
{
  if (!IsActive())
    return;

  int screen_bottom = (int)Camera::GetInstance()->GetPosition().y + (int)Camera::GetInstance()->GetSize().y;
  int water_top = GetWorld().GetHeight() - (water_height + height_mvt) - 20;

  if ( screen_bottom < water_top )
    return; // save precious CPU time

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
  for(int y = water_top;
      y < screen_bottom;
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

bool Water::IsActive() const
{
  return water_type != NO_WATER;
}

int Water::GetHeight(int x) const
{
  if (IsActive())
    return height[x % pattern_width]
           + GetWorld().GetHeight()
           - (water_height + height_mvt);
  else
    return GetWorld().GetHeight();
}

uint Water::GetSelfHeight() const
{
  return water_height+(pattern_height/2);
}

const Color* Water::GetColor() const
{
  return type_color;
}

void Water::Splash(const Point2i& pos) const
{
  switch (water_type) {
  case NO_WATER:
    break;
  case WATER:
    ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_WATER, true, -1, 20);
    break;
  case LAVA:
    ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_LAVA, true, -1, 20);
    break;
  case RADIOACTIVE:
    ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_RADIOACTIVE, true, -1, 20);
    break;
  case DIRTY:
    ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_DIRTYWATER, true, -1, 20);
    break;
  case CHOCOLATE:
    ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_CHOCOLATEWATER, true, -1, 20);
    break;
  case MAX_WATER_TYPE:
    ASSERT(false);
    break;
  }
}

void Water::Smoke(const Point2i& pos) const
{
  ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 2, particle_SMOKE, true, 0, 1);
}

// =================== static methods

Water::Water_type Water::GetWaterType(const std::string & water)
{
  if (water == "no") {
    return NO_WATER;
  } else if (water == "water") {
    return WATER;
  } else if (water == "lava") {
    return LAVA;
  } else if (water == "radioactive") {
    return RADIOACTIVE;
  } else if (water == "dirtywater") {
    return DIRTY;
  } else if (water == "chocolate") {
    return CHOCOLATE;
  } else { // Unsupported water type
    fprintf(stderr, "WARNING: map using invalid water type %s: valid water types are no, water, lava, radioactive\n",
	    water.c_str());
  }
  return NO_WATER;
}

const std::string Water::GetWaterName(const Water::Water_type water_type)
{
  switch (water_type) {
  case NO_WATER:
    return "no";
    break;
  case WATER:
    return "water";
    break;
  case LAVA:
    return "lava";
    break;
  case RADIOACTIVE:
    return "radioactive";
    break;
  case DIRTY:
    return "dirtywater";
    break;
  case CHOCOLATE:
    return "chocolate";
    break;
  case MAX_WATER_TYPE:
    ASSERT(false);
    break;
  }
  ASSERT(false);
  return "";
}
