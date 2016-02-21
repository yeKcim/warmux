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

#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL.h>
#include <vector>
#include <assert.h>

#include <WARMUX_base.h>
#include <WARMUX_debug.h>

#include "spritecache.h"
#include "spriteanimation.h"

typedef enum {
  top_left,
  top_center,
  top_right,
  left_center,
  center,
  right_center,
  bottom_left,
  bottom_center,
  bottom_right,
  user_defined // Sprite::rot_hotspot is set to this value, when the hotspot is set as a Point2i
} Rotation_HotSpot;

class Sprite
{
  bool fixed;
  bool flipped;

  SpriteCache cache;

  // Surface growing as need be for any temporary operation inside Sprite members
  // that need a SDL_SWSURFACE Surface without alpha.
  // This surface is shared by all Sprite objects to save memory, but beware
  // that its use is not thread-safe!
  static Surface scratch;

  // Use that to grow the scratch Surface as need be
  void CheckScratch(const Point2i& size);

public:
  SpriteAnimation animation;

  explicit Sprite();
  explicit Sprite(const Surface& surface);
  Sprite(const Sprite &other);

  void Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y);
  Surface& GetSurface()
  {
    ASSERT(!current_surface.IsNull());
    return current_surface;
  }
  bool IsFlipped() const { return flipped; }

  // Frame number
  uint GetCurrentFrame() const { return current_frame; }
  void SetCurrentFrame(uint frame_no)
  {
    assert(frame_no < cache.size());
    if (current_frame == frame_no)
      return;

    InvalidLastFrame();
    MSG_DEBUG("sprite", "Set current frame : %d", frame_no);
    current_frame = frame_no;
  }
  uint GetFrameCount() const { return cache.size(); };

  // Access current frame
  uint GetCurrentDelay() const { return cache[current_frame].delay; }

  // Size
  uint GetWidth() const { return uround(frame_width_pix *  scale_x); }
  // gives height of the surface (takes rotations into acount)
  uint GetWidthMax() const
  {
    if (!current_surface.IsNull())
      return current_surface.GetWidth();
    else
      return GetWidth();
  }
  uint GetHeight() const { return uround(frame_height_pix * scale_y); }
  // gives height of the surface (takes rotations into acount)
  uint GetHeightMax() const
  {
    if (!current_surface.IsNull())
      return current_surface.GetHeight();
    else
      return GetHeight();
  }
  Point2i GetSize() const { return Point2i(GetWidth(), GetHeight()); };
  Point2i GetSizeMax() const { return Point2i(GetWidthMax(), GetHeightMax()); };

  void GetScaleFactors(Double &_scale_x, Double &_scale_y) const
  {
    _scale_x = scale_x;
    _scale_y = scale_y;
  }
  Double GetScaleX(void) const { return scale_x; }
  Double GetScaleY(void) const { return scale_y; }
  void SetSize(uint w, uint h)
  {
    ASSERT(frame_width_pix == 0 && frame_height_pix == 0);

    frame_width_pix = w;
    frame_height_pix = h;
  }
  void SetSize(const Point2i &size) { SetSize(size.x, size.y); };

  void Scale(Double _scale_x, Double _scale_y)
  {
    assert(_scale_x>=0);
    assert(_scale_y>=0);
    if (scale_x==_scale_x && _scale_y==scale_y)
      return;
    scale_x = _scale_x;
    scale_y = _scale_y;
    InvalidLastFrame();
  }
  void ScaleSize(int width, int height)
  {
    Scale(Double(width)/frame_width_pix, Double(height)/frame_height_pix);
  }
  void ScaleSize(const Point2i& size) { ScaleSize(size.x, size.y); };

  // Rotation
  void SetRotation_rad(Double angle_rad);
  const Double &GetRotation_rad() const
  {
    assert(rotation_rad > -TWO_PI && rotation_rad <= TWO_PI);
    return rotation_rad;
  }
  void SetRotation_HotSpot(const Point2i& new_hotspot);
  void SetRotation_HotSpot(const Rotation_HotSpot rhs) { rot_hotspot = rhs; };
  const Point2i& GetRotationPoint() const { return rotation_point; };

  // Prepare animation
  void AddFrame(const Surface& surf, uint delay = 100) { cache.AddFrame(surf, delay); }
  void SetFrameSpeed(uint nv_fs) { cache.SetDelay(nv_fs); }

  // Animation
  void Start();
  void Update() { animation.Update(); };
  void Finish();
  bool IsFinished() const { return animation.IsFinished(); };

  // Alpha
   // Can't be combined with per pixel alpha
  void SetAlpha(Double _alpha)
  {
    assert(_alpha >= ZERO && _alpha <= ONE);
    this->alpha = _alpha;
  }
  Double GetAlpha() const { return alpha; };

  // Cache handling
  void EnableCaches(bool flipped, uint num=0, const Double& min=ZERO, const Double& max=TWO_PI)
  {
    cache.EnableCaches(flipped, num, min, max);
  }
  void SetFlipped(bool f)
  {
    if (flipped == f)
      return;
    ASSERT(!f || cache.HasFlippedCache());
    flipped = f;
    InvalidLastFrame();
  }

  // Show flag
  void Show() { show = true; };
  void Hide() { show = false; };

  // Draw
  void Blit(Surface &dest, uint pos_x, uint pos_y)
  {
    RefreshSurface(); // Might be NULL here
    Blit(dest, pos_x, pos_y, 0, 0, current_surface.GetWidth(), current_surface.GetHeight());
  }
  void Blit(Surface &dest, const Point2i &pos) { Blit(dest, pos.GetX(), pos.GetY()); };
  void Blit(Surface &dest, const Rectanglei &srcRect, const Point2i &destPos)
  {
    Blit(dest, destPos.GetX(), destPos.GetY(), srcRect.GetPositionX(),
         srcRect.GetPositionY(), srcRect.GetSizeX(), srcRect.GetSizeY());
  }
  void Blit(Surface &dest, int pox_x, int pos_y, int src_x, int src_y, uint w, uint h);
  void Draw(const Point2i &pos);
  void DrawXY(const Point2i &pos);

  void RefreshSurface();
  void FixParameters(bool force_ckey = false);

private:
  Surface current_surface;
  bool show;
  // Frames
  uint current_frame;
  int frame_width_pix,frame_height_pix;

  // Gfx
  Double alpha;
  Double scale_x,scale_y;
  Double rotation_rad;
  Point2i rhs_pos;
  Rotation_HotSpot rot_hotspot;
  Point2i rotation_point;

  void Constructor();
  void Calculate_Rotation_Offset(const Surface& tmp_surface);
  void InvalidLastFrame() { current_surface.Free(); }
};

#endif /* _SPRITE_H */
