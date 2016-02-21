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
 * Sprite:     Simple sprite management
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#include <iostream>
#include <SDL.h>

#include <WARMUX_rectangle.h>

#include "graphic/sprite.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "tool/math_tools.h"
#include "tool/string_tools.h"

// Scratch surface without alpha, not thread-safe!
Surface Sprite::scratch;

Sprite::Sprite()
  : fixed(false)
  , flipped(false)
  , cache(*this)
  , animation(*this)
{
  Constructor();
}

Sprite::Sprite(const Surface& surface)
  : fixed(false)
  , flipped(false)
  , cache(*this)
  , animation(*this)
{
  Constructor();
  frame_width_pix = surface.GetWidth();
  frame_height_pix = surface.GetHeight();
  AddFrame(surface);
}

Sprite::Sprite(const Sprite &other)
  : fixed(other.fixed)
  , flipped(other.flipped)
  , cache(*this)
  , animation(other.animation, *this)
{
  Constructor();
  current_surface = other.current_surface;
  show = other.show;
  current_frame = other.current_frame;
  frame_width_pix = other.frame_width_pix;
  frame_height_pix = other.frame_height_pix;
  alpha = other.alpha;
  scale_x = other.scale_x;
  scale_y = other.scale_y;
  rotation_rad = other.rotation_rad;
  rhs_pos = other.rhs_pos;
  rot_hotspot = other.rot_hotspot;
  rotation_point = other.rotation_point;

  cache.SetFrames(other.cache);
}

void Sprite::Constructor()
{
  show = true;
  current_frame = 0;
  frame_width_pix = frame_height_pix = 0;
  alpha = 1.0f;
  scale_x = scale_y = 1.0f;
  rotation_rad = 0.0f;
  SetRotation_HotSpot(center);
}

void Sprite::Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y)
{
  Point2i f;

  frame_width_pix = frameSize.x;
  frame_height_pix = frameSize.y;

  surface.SetAlpha(0, 0);

  Surface new_surf(frameSize, SDL_SWSURFACE|SDL_SRCALPHA, true);
  for (f.y=0; f.y<nb_frames_y; f.y++) {
    for (f.x=0; f.x<nb_frames_x; f.x++) {
      new_surf.Blit(surface, Rectanglei(f * frameSize, frameSize), Point2i(0, 0));
      cache.AddFrame(new_surf.DisplayFormatAlpha());
    }
  }
}

void Sprite::SetRotation_rad(Double angle_rad)
{
  angle_rad = RestrictAngle(angle_rad);

  if (rotation_rad == angle_rad)
    return;

  rotation_rad = angle_rad;
  InvalidLastFrame();
}

void Sprite::SetRotation_HotSpot(const Point2i& new_hotspot)
{
  rot_hotspot = user_defined;
  rhs_pos = new_hotspot;

  if (rhs_pos * 2 == GetSize())
    rot_hotspot = center; // avoid using Calculate_Rotation_Offset, thus avoiding a division by zero
}

void Sprite::Calculate_Rotation_Offset(const Surface & tmp_surface)
{
  const Surface & surface = cache[current_frame].normal.surface;

  // Calculate offset of the surface depending on hotspot rotation position :
  int surfaceHeight = surface.GetHeight();
  int surfaceWidth = surface.GetWidth();
  int halfHeight = surfaceHeight >> 1;
  int halfWidth = surfaceWidth >> 1;

  //Do as if hotspot is center of picture:
  rotation_point.x = halfWidth  - (tmp_surface.GetWidth()  >> 1);
  rotation_point.y = halfHeight - (tmp_surface.GetHeight() >> 1);

  if (rot_hotspot == center || !rotation_rad.IsNotZero()) {
    return;
  }

  if (rot_hotspot != user_defined) {
    switch(rot_hotspot) {
    case top_left:      rhs_pos = Point2i( 0,              0);               break;
    case top_center:    rhs_pos = Point2i( halfWidth,      0);               break;
    case top_right:     rhs_pos = Point2i( surfaceWidth,   0);               break;
    case left_center:   rhs_pos = Point2i( 0,              halfHeight);      break;
    case center:        rhs_pos = Point2i( halfWidth,      halfHeight);      break;
    case right_center:  rhs_pos = Point2i( surfaceWidth,   halfHeight);      break;
    case bottom_left:   rhs_pos = Point2i( 0,              surfaceHeight);   break;
    case bottom_center: rhs_pos = Point2i( halfWidth,      surfaceHeight);   break;
    case bottom_right:  rhs_pos = Point2i( surfaceWidth,   surfaceHeight);   break;
    default:
      ASSERT(false);
    }
  }

  Point2i rhs_pos_tmp;
  if (flipped) {
    rhs_pos_tmp.x = int(rhs_pos.x * -scale_x);
    surfaceWidth  = int(surfaceWidth  * -scale_x);
  } else {
    rhs_pos_tmp.x = int(rhs_pos.x * scale_x);
    surfaceWidth  = int(surfaceWidth  * scale_x);
  }
  rhs_pos_tmp.y = int(rhs_pos.y * scale_y);
  surfaceHeight = int(surfaceHeight * scale_y);
  //Calculate the position of the hotspot after a rotation around the center of the surface:

  Point2i center(surfaceWidth >> 1, surfaceHeight >> 1);
  // Don't let the compiler any choice with which types the resulting program will calculate with.
  Point2i old_hotspot_delta_i = center - rhs_pos_tmp;
  Point2d old_hotspot_delta = old_hotspot_delta_i;
  Double rhs_dst = old_hotspot_delta.Norm();
  Double rhs_angle = 0;
  if (rhs_dst.IsNotZero())
    rhs_angle = -acos(-old_hotspot_delta.x / rhs_dst);
  if (halfHeight /*surfaceHeight/2*/ - rhs_pos.y < 0)
    rhs_angle = -rhs_angle;

  rhs_angle += rotation_rad;

  Point2d new_hotspot_delta = Point2d::FromPolarCoordinates(rhs_dst, rhs_angle);
  Point2i new_hotspot_delta_i = new_hotspot_delta;
  Point2i new_hotspot = center + new_hotspot_delta_i;

  rotation_point -= new_hotspot;
  rotation_point += rhs_pos_tmp;
}

void Sprite::Start()
{
  show = true;
  animation.Start();
  InvalidLastFrame();
}

void Sprite::CheckScratch(const Point2i& size)
{
  int w = scratch.IsNull() ? 0 : scratch.GetWidth();
  int h = scratch.IsNull() ? 0 : scratch.GetHeight();
  if (w<size.GetX() || h<size.GetY()) {
    w = std::max(size.GetX(), w);
    h = std::max(size.GetY(), h);
    scratch.NewSurface(Point2i(w, h), SDL_SWSURFACE, false);
  }
  scratch.SetAlpha(SDL_SRCALPHA, 0);
}

void Sprite::Blit(Surface &dest, int pos_x, int pos_y, int src_x, int src_y, uint w, uint h)
{
  if (!show)
    return;

  RefreshSurface();
  Rectanglei srcRect(src_x, src_y, w, h);
  Rectanglei dstRect(pos_x + rotation_point.x, pos_y + rotation_point.y, w, h);

  if (alpha == ONE) {
    dest.Blit(current_surface, srcRect, dstRect.GetPosition());
  } else if (current_surface.GetSurface()->format->Amask) {
    CheckScratch(srcRect.GetSize());
    scratch.Blit(dest, dstRect, Point2i(0,0));
    scratch.SetAlpha(SDL_SRCALPHA, alpha * 255);
    scratch.Blit(current_surface, srcRect, Point2i(0,0));
    dest.Blit(scratch, srcRect, dstRect.GetPosition());
  } else {
    // Surface doesn't have alpha, do a simple blit
    current_surface.SetAlpha(SDL_SRCALPHA, alpha * 255);
    dest.Blit(current_surface, srcRect, dstRect.GetPosition());
  }

  // For the cache mechanism
  GetWorld().ToRedrawOnScreen(dstRect);
}

void Sprite::Finish()
{
  animation.Finish();
  switch(animation.GetShowOnFinish())
  {
  case SpriteAnimation::show_first_frame:
    current_frame = 0;
    break;
  case SpriteAnimation::show_blank:
    show = false;
    break;
  default:
  case SpriteAnimation::show_last_frame:
    current_frame = cache.size()-1;
    break;
  }
  InvalidLastFrame();
}

void Sprite::Draw(const Point2i &pos)
{
  DrawXY(pos - Camera::GetInstance()->GetPosition());
}

void Sprite::DrawXY(const Point2i &pos)
{
  Blit(GetMainWindow(), pos);
}

void Sprite::RefreshSurface()
{
  if (!current_surface.IsNull())
    return;

  assert(scale_x >= 0);
  SpriteSubframeCache& frame = (flipped) ? cache[current_frame].flipped
                                         : cache[current_frame].normal;
  Double angle;
  Surface tmp;
  if (rotation_rad.IsNotZero() && cache.HasRotationCache()) {
    tmp = frame.GetSurfaceForAngle(rotation_rad);
    angle = ZERO;
  } else {
    tmp = frame.surface;
    angle = -rotation_rad;
  }
  // RotoZoom already checks angle and scale to do the least work
  current_surface = tmp.RotoZoom(angle, scale_x, scale_y);

  // Calculate offset of the sprite depending on hotspot rotation position :
  rotation_point.x=0;
  rotation_point.y=0;
  if (rot_hotspot != center || rotation_rad.IsNotZero())
    Calculate_Rotation_Offset(current_surface);
}

void Sprite::FixParameters(bool force_ckey)
{
  cache.FixParameters(rotation_rad, scale_x, scale_y, force_ckey);
  frame_width_pix = GetWidth();
  frame_height_pix = GetHeight();
  scale_x = ONE;
  scale_y = ONE;
  rotation_rad = 0;
  fixed = true;
}
