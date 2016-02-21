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
 * Sprite:     Simple sprite management
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 * TODO:       Scale,Rotation...
 *****************************************************************************/

#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL.h>
#include <vector>

#ifdef DEBUG
//#define DBG_SPRITE

#ifdef DBG_SPRITE
#include "text.h"
#endif
#endif //DEBUG

// Unused
//SDL_Surface *newFlippedSurface(SDL_Surface *src, int fliph, int flipv);

enum Rotation_HotSpot {top_left, top_center, top_right, left_center, center, right_center, bottom_left, bottom_center, bottom_right};

 
class SpriteFrame
{
public:
  SpriteFrame(SDL_Surface *surface, unsigned int delay=100);
  SDL_Surface *surface;
  SDL_Surface *flipped_surface;
  SDL_Surface** rotated_surface;
  SDL_Surface** rotated_flipped_surface;
  unsigned int delay; // in millisecond
};

class Sprite
{
  public:
typedef enum {
  show_first_frame,
  show_last_frame,
  show_blank
} SpriteShowOnFinish;
	
 public:
   Sprite();
   Sprite( const Sprite &other);
   Sprite( SDL_Surface *surface);
   ~Sprite();
   void Init( SDL_Surface *surface, int frame_width, int frame_height, int nb_frames_x, int nb_frames_y);
   void AddFrame(SDL_Surface* surf, unsigned int delay);
   void EnableRotationCache(unsigned int cache_size);
   void EnableFlippingCache();
   void EnableLastFrameCache();

   // Get/Set physical characterisics
	void SetSize(unsigned int w, unsigned int h);
   unsigned int GetWidth();
   unsigned int GetHeight();
   unsigned int GetFrameCount();
   
   // Get/Set sprite parameters
   void SetCurrentFrame( unsigned int frame_no);    
   unsigned int GetCurrentFrame() const;
   SpriteFrame& operator[] (unsigned int frame_no);
   const SpriteFrame& operator[] (unsigned int frame_no) const;
   const SpriteFrame& GetCurrentFrameObject() const;

   void Start();
   void Finish();
   void SetPlayBackward(bool enable);
   void SetLoopMode(bool enable=true) { loop = enable; };
   void SetPingPongMode(bool enable=true) { pingpong = enable; };
   void SetShowOnFinish(SpriteShowOnFinish show);

   void Scale( float scale_x, float scale_y);
   void ScaleSize(int width, int height);
   void GetScaleFactors( float &scale_x, float &scale_y);

   void SetRotation_deg( float angle_deg);
   void SetRotation_HotSpot( Rotation_HotSpot rhs) {rot_hotspot = rhs;};

   void SetAlpha( float alpha); // Can't be combined with per pixel alpha
   float GetAlpha();

   void SetFrameSpeed(unsigned int nv_fs);
   void SetSpeedFactor(float nv_speed);

   void Show();
   void Hide();

     
   void Blit( SDL_Surface *dest, unsigned int pox_x, unsigned int pos_y);
   void Draw(int pos_x, int pos_y);
   void Update();
   bool IsFinished() const;

 private:
   unsigned int last_update;
   bool finished;
   bool show;
   bool loop;
   bool pingpong;
   SpriteShowOnFinish show_on_finish;
   int frame_width_pix,frame_height_pix;
   float scale_x,scale_y;
   float rotation_deg;
   float alpha;
   float speed_factor;
   unsigned int current_frame;
   int frame_delta; // Used in Update() to get next frame
   bool backward;
   std::vector<SpriteFrame> frames;

   Rotation_HotSpot rot_hotspot;
   void Calculate_Rotation_Offset(int & rot_x, int & rot_y, SDL_Surface* tmp_surface);

   //Cache members / functions
   bool have_rotation_cache;
   unsigned int rotation_cache_size;

   bool have_flipping_cache;

   bool have_lastframe_cache;
   SDL_Surface* last_frame;
   void LastFrameModified();

#ifdef DBG_SPRITE
   Text* info;
#endif
};

#endif /* _SPRITE_H */
