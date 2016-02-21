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
 * Refresh water that may be placed in bottom of the ground.
 *****************************************************************************/

#include "game/game_mode.h"
#include <assert.h>
#include <SDL.h>
#include "game/game_time.h"
#include "graphic/video.h"
#include "interface/interface.h"
#include "map/camera.h"
#include "map/map.h"
#include "map/maps_list.h"
#include "map/water.h"
#include "particles/particle.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/string_tools.h"

#define GO_UP_TIME 1  // min
#define GO_UP_STEP 15 // pixels
#define GO_UP_OSCILLATION_TIME  30 // seconds
#define GO_UP_OSCILLATION_NBR  30 // amplitude
#define MS_BETWEEN_SHIFTS  20
#define PATTERN_HEIGHT 64
#define WAVE_INC   5
#define WAVE_HEIGHT_A  5
#define WAVE_HEIGHT_B  8

static const Double DEGREE = TWO_PI/360;

Water::Water()
  : type_color(NULL)
  , height_mvt(0)
  , shift1(0)
  , water_height(0)
  , time_raise(0)
  , water_type("no")
  , m_last_preview_redraw(0)
  , next_wave_shift(0)
{
  memset(height, 0, sizeof(height));
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
 * The water is drawn in patterns of 180 x 128 block. The value 180 comes
 * from SIN(2x) period.
 *
 * The pattern surface is rendered using water.png and SIN functions, while
 * the bottom is just painted with a transparent color.
 */
void Water::Init()
{
  ASSERT(water_type != "no");

  std::string image = "gfx/";
  image += water_type;

  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);

  type_color = new Color(GetResourceManager().LoadColor(res, "water_colors/" + water_type));
#ifdef HAVE_HANDHELD
  surface = GetResourceManager().LoadImage(res, image, false);
  pattern.NewSurface(Point2i(PATTERN_WIDTH, PATTERN_HEIGHT),
                     SDL_SWSURFACE|SDL_SRCCOLORKEY, false);
  pattern.SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, 0);
#else
  surface = GetResourceManager().LoadImage(res, image, true);
  pattern.NewSurface(Point2i(PATTERN_WIDTH, PATTERN_HEIGHT),
                     SDL_SWSURFACE|SDL_SRCALPHA, true);
#endif

  shift1 = 0;
  next_wave_shift = 0;
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
  if (!IsActive()) {
    return;
  }

  surface.Free();
  pattern.Free();
}

void Water::Refresh()
{
  if (!IsActive()) {
    return;
  }

  height_mvt = 0;
  uint now = GameTime::GetInstance()->Read();

  if (next_wave_shift <= now) {
    shift1 += 2*DEGREE;
    next_wave_shift += MS_BETWEEN_SHIFTS;
  }

  // Height Calculation:
  if (time_raise < now) {
    m_last_preview_redraw = now;
    if (time_raise + GO_UP_OSCILLATION_TIME * 1000 > now) {
      static const Double A = GO_UP_STEP/(GO_UP_OSCILLATION_TIME*(Double)1000.0);
      uint dt = now - time_raise;
      height_mvt = GO_UP_STEP + (int)((GO_UP_STEP *
                 sin(((dt* (GO_UP_OSCILLATION_NBR-(Double)0.25))
                      / (GO_UP_OSCILLATION_TIME*(Double)1000.0))*TWO_PI)
                 )/(A*dt+ONE));
    } else {
      time_raise += GO_UP_TIME * 60 * 1000;
      water_height += GO_UP_STEP;
    }
  }

  CalculateWaveHeights();
}

void Water::CalculateWaveHeights()
{
  Double angle1 = -shift1;
  Double angle2 = shift1;

  for (uint x = 0; x < PATTERN_WIDTH; x++) {
    // TODO: delete the first dimension of wave_height (now unused)
    height[x] = static_cast<int>(sin(angle1)*WAVE_HEIGHT_A + sin(angle2)*WAVE_HEIGHT_B);

    angle1 += 2*DEGREE;
    angle2 += 4*DEGREE;
  }
}


void Water::CalculateWavePattern()
{
  pattern.Fill(0x00000000);

  /* Locks on SDL_Surface must be taken when accessing pixel member */
  surface.Lock();
  pattern.Lock();

  /* Copy directly the surface image into the pattern image. This doesn't use
   * blit in order to save CPU but it makes this code not really easy to read...
   * The copy is done pixel per pixel */
  uint         bpp        = surface.GetBytesPerPixel();
  int          spitch     = surface.GetPitch();
  int          dpitch     = pattern.GetPitch();

  switch (bpp)
  {
  case 2:
    {
      spitch >>= 1;
      dpitch >>= 1;
      const Uint16 *src_origin = (Uint16 *)surface.GetPixels();
      Uint16       *dst_origin = (Uint16 *)pattern.GetPixels()
                               + (15 + WAVE_INC*(WAVE_COUNT-1)) * dpitch;

      for (uint x = 0; x < PATTERN_WIDTH; x++) {
        const Uint16 *src = src_origin;
        Uint16       *dst = dst_origin + x + height[x] * dpitch;
        for (int y=0; y < surface.GetHeight(); y++) {
          dst[0] = src[0];
          dst += dpitch;
          src += spitch;
        }
      }
      break;
    }
  case 4:
    {
      spitch >>= 2;
      dpitch >>= 2;
      const Uint32 *src_origin = (Uint32 *)surface.GetPixels();
      Uint32       *dst_origin = (Uint32 *)pattern.GetPixels()
                               + (15 + WAVE_INC*(WAVE_COUNT-1)) * dpitch;

      for (uint x = 0; x < PATTERN_WIDTH; x++) {
        const Uint32 *src = src_origin;
        Uint32       *dst = dst_origin + x + height[x] * dpitch;
        for (int y=0; y < surface.GetHeight(); y++) {
          dst[0] = src[0];
          dst += dpitch;
          src += spitch;
        }
      }
      break;
    }
  default:
    Error("Unexpected surface format");
  }

  pattern.Unlock();
  surface.Unlock();
}

void Water::Draw()
{
  if (!IsActive()) {
    return;
  }

  const Camera* cam = Camera::GetConstInstance();
  int screen_bottom = (int)cam->GetPosition().y + (int)cam->GetSize().y;
  int water_top = GetWorld().GetHeight() - (water_height + height_mvt) - 20;

  if (screen_bottom < water_top) {
    return; // save precious CPU time
  }

  int x0 = cam->GetPosition().x % PATTERN_WIDTH;
  int cameraRightPosition = cam->GetPosition().x + cam->GetSize().x;
  int y = water_top + (WAVE_HEIGHT_A + WAVE_HEIGHT_B) * 2 + WAVE_INC;

  int h = cam->GetSize().y - (y - cam->GetPosition().y);
  if (h > 0) {
    Rectanglei bottom(0, cam->GetSize().y-h, cam->GetSize().x, h);
    GetMainWindow().BoxColor(bottom, *type_color);
    bottom.SetPosition(bottom.GetPosition() + cam->GetPosition());
    GetWorld().ToRedrawOnMap(bottom);
  }

  CalculateWavePattern();
  y = water_top;
  for (int wave = 0; wave < WAVE_COUNT; wave++) {
    for (int x = cam->GetPosition().x - x0 - ((PATTERN_WIDTH/4) * wave);
         x < cameraRightPosition;
         x += PATTERN_WIDTH) {
      AbsoluteDraw(pattern, Point2i(x, y));
    }
    y += wave * WAVE_INC;
  }
}

int Water::GetHeight(int x) const
{
  if (IsActive()) {
    //printf("Height would have been %i\n", height[x % PATTERN_WIDTH]);
    int h = height[x % PATTERN_WIDTH];
    return h + GetWorld().GetHeight() - int(water_height + height_mvt);
  } else {
    return GetWorld().GetHeight();
  }
}

void Water::Splash(const Point2i& pos) const
{
  if (water_type == "no")
    return;

  ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 5, particle_WATER, true, -1, 20);
}

void Water::Smoke(const Point2i& pos) const
{
  ParticleEngine::AddNow(Point2i(pos.x, pos.y-5), 2, particle_SMOKE, true, 0, 1);
}
