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
 * Sprite:     Simple sprite management
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL.h>
#include <vector>
#include "include/base.h"
#include "spritecache.h"
#include "spriteanimation.h"

// Forward declarations
class SpriteFrame;

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
private:
  bool smooth;

public:
  SpriteCache cache;
  SpriteAnimation animation;

public:
  explicit Sprite(bool _smooth=false);
  explicit Sprite(const Surface& surface, bool _smooth=false);
  Sprite(const Sprite &other);

  void Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y);
  Surface GetSurface() const;

  // Frame number
  unsigned int GetCurrentFrame() const;
  void SetCurrentFrame( unsigned int frame_no);
  unsigned int GetFrameCount() const;

  // Antialiasing
  void SetAntialiasing(bool on);
  bool IsAntialiased() const;

  // Size
  unsigned int GetWidth() const;
  unsigned int GetWidthMax() const; // gives height of the surface
                                     // (takes rotations into acount)
  unsigned int GetHeight() const;
  unsigned int GetHeightMax() const; // gives height of the surface
                                     // (takes rotations into acount)
  Point2i GetSize() const;
  Point2i GetSizeMax() const;

  void GetScaleFactors( float &_scale_x, float &_scale_y) const;
  void SetSize(unsigned int w, unsigned int h);
  void SetSize(const Point2i &size);
  void Scale( float scale_x, float scale_y);
  void ScaleSize(int width, int height);
  void ScaleSize(const Point2i& size);

  // Rotation
  void SetRotation_rad( double angle_rad);
  const double &GetRotation_rad() const;
  void SetRotation_HotSpot( const Point2i& new_hotspot);
  void SetRotation_HotSpot( const Rotation_HotSpot rhs) { rot_hotspot = rhs; };
  const Point2i& GetRotationPoint() { return rotation_point; };

  SpriteFrame& operator[] (unsigned int frame_no);
  const SpriteFrame& operator[] (unsigned int frame_no) const;
  const SpriteFrame& GetCurrentFrameObject() const;

  // Prepare animation
  void AddFrame( const Surface& surf, unsigned int delay = 100);
  void SetFrameSpeed(unsigned int nv_fs);

  // Animation
  void Start();
  void Update();
  void Finish();
  bool IsFinished() const;

  // Alpha
  void SetAlpha( float alpha); // Can't be combined with per pixel alpha
  float GetAlpha() const;

  // Cache
  void EnableRotationCache(unsigned int cache_size);
  void EnableFlippingCache();

  // Show flag
  void Show();
  void Hide();

  // Draw
  void Blit(Surface &dest, uint pox_x, uint pos_y);
  void Blit(Surface &dest, const Point2i &pos);
  void Blit(Surface &dest, const Rectanglei &srcRect, const Point2i &destPos);
  void Blit(Surface &dest, int pox_x, int pos_y, int src_x, int src_y, uint w, uint h);
  void Draw(const Point2i &pos);
  void DrawXY(const Point2i &pos);

   void RefreshSurface();

private:
   Surface current_surface;
   bool show;
   // Frames
   unsigned int current_frame;
   int frame_width_pix,frame_height_pix;
   std::vector<SpriteFrame> frames;

   // Gfx
   float alpha;
   float scale_x,scale_y;
   double rotation_rad;
   Point2i rhs_pos;
   Rotation_HotSpot rot_hotspot;
   Point2i rotation_point;

private:
   void Constructor();
   void Calculate_Rotation_Offset(const Surface& tmp_surface);
};

#endif /* _SPRITE_H */
