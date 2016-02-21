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
 * Sprite cache.
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#include "graphic/spritecache.h"
#include <SDL_rotozoom.h>

#define BUGGY_SDLGFX

SpriteFrameCache::SpriteFrameCache() {
  use_rotation = false;
}
#include <iostream>
void SpriteFrameCache::CreateRotationCache(Surface &surface, unsigned int cache_size){
  ASSERT (use_rotation == false);
  use_rotation = true;

  rotated_surface.push_back( surface );
  for(unsigned int i=1 ; i< cache_size ; i++){
    double angle = 2* M_PI * (1 /* to inverte rotation angle */ - static_cast<double>(i) / static_cast<double>(cache_size));
    rotated_surface.push_back( surface.RotoZoom(angle, 1.0, 1.0, SMOOTHING_ON) );
  }
}

Surface SpriteFrameCache::GetFlippedSurfaceForAngle(double angle) const
{
  double angle_tmp = angle;
  while(angle_tmp >= 2 * M_PI)
    angle_tmp -= 2 * M_PI;
  while(angle_tmp < 0.0)
    angle_tmp += 2 * M_PI;
  int index = static_cast<uint>(angle_tmp*static_cast<double>(rotated_flipped_surface.size()) / (2*M_PI));
  return rotated_flipped_surface[index];
}

Surface SpriteFrameCache::GetSurfaceForAngle(double angle) const
{
  double angle_tmp = angle;
  while(angle_tmp >= 2 * M_PI)
    angle_tmp -= 2 * M_PI;
  while(angle_tmp < 0.0)
    angle_tmp += 2 * M_PI;

  int index = static_cast<uint>(angle_tmp*static_cast<double>(rotated_surface.size()) / (2*M_PI));
  return rotated_surface[index];
}

void SpriteFrameCache::CreateFlippingCache(Surface &surface)
{
  ASSERT (flipped_surface.IsNull());
  flipped_surface = surface.RotoZoom( 0.0, -1.0, 1.0, SMOOTHING_OFF);
  if (use_rotation)
  {
    ASSERT (rotated_surface.size() != 0);
    ASSERT (rotated_flipped_surface.size() == 0);
    rotated_flipped_surface.push_back( flipped_surface );
    const unsigned int n = rotated_surface.size();
    for(unsigned int i=1 ; i<n; i++)
    {
      double angle = 2 * M_PI * (1 - (float) i / (float) n);
      rotated_flipped_surface.push_back( surface.RotoZoom(angle, -1.0, 1.0, SMOOTHING_ON) );
    }
  }
}




SpriteCache::SpriteCache(Sprite &p_sprite) :
  sprite(p_sprite)
{
  have_rotation_cache = false;
  have_flipping_cache = false;
  have_lastframe_cache = false;
  rotation_cache_size = 0;
}

#if 0
SpriteCache::SpriteCache(Sprite &p_sprite, const SpriteCache &other)  :
  sprite(p_sprite),
  frames(other.frames)
{
  have_rotation_cache = false;
  have_flipping_cache = false;
  have_lastframe_cache = false;
  rotation_cache_size = 0;

  for ( unsigned int f = 0 ; f < other.frames.size() ; f++)
  {
    Surface new_surf = Surface(frame_width_pix, frame_height_pix, SDL_SWSURFACE|SDL_SRCALPHA, true);

        // Disable per pixel alpha on the source surface
    // in order to properly copy the alpha chanel to the destination suface
        // see the SDL_SetAlpha man page for more infos (RGBA->RGBA without SDL_SRCALPHA)
        other.frames[f].surface.SetAlpha( 0, 0);
        new_surf.Blit( other.frames[f].surface, NULL, NULL);

        // re-enable the per pixel alpha in the
        other.frames[f].surface.SetAlpha( SDL_SRCALPHA, 0);
    frames.push_back( SpriteFrame(new_surf,other.frames[f].delay));
  }

  if(other.have_rotation_cache)
    EnableRotationCache(other.rotation_cache_size);
  if(other.have_flipping_cache)
    EnableFlippingCache();
  if(other.have_lastframe_cache)
    EnableLastFrameCache();
}
#endif

void SpriteCache::EnableRotationCache(std::vector<SpriteFrame> &sprite_frames, unsigned int cache_size){
  //For each frame, we pre-render 'cache_size' rotated surface
  //At runtime the prerender Surface with the nearest angle to what is asked is displayed
  ASSERT(1 < cache_size && cache_size <= 360);
  ASSERT(!have_lastframe_cache);
  ASSERT(!have_flipping_cache); //Always compute rotation cache before flipping cache!
  ASSERT(!have_rotation_cache);
  have_rotation_cache = true;

  if (frames.empty())
    frames.resize( sprite_frames.size() );
  ASSERT( frames.size() == sprite_frames.size() );
  rotation_cache_size = cache_size;

  for ( unsigned int f = 0 ; f < frames.size() ; f++)
  {
    frames[f].CreateRotationCache(sprite_frames[f].surface, cache_size);
  }
}

void SpriteCache::EnableFlippingCache(std::vector<SpriteFrame> &sprite_frames){
  //For each frame, we pre-render the flipped frame
  ASSERT(!have_flipping_cache);
  ASSERT(!have_lastframe_cache);

  if (frames.empty())
    frames.resize( sprite_frames.size() );
  ASSERT( frames.size() == sprite_frames.size() );

  have_flipping_cache = true;

  for ( unsigned int f = 0 ; f < frames.size() ; f++)
    frames[f].CreateFlippingCache(sprite_frames[f].surface);
}

void SpriteCache::EnableLastFrameCache(){
  //The result of the last call to SDLgfx is kept in memory
  //to display it again if rotation / scale / alpha didn't changed
  ASSERT(!have_rotation_cache);
  ASSERT(!have_flipping_cache);
  have_lastframe_cache = true;
}

void SpriteCache::DisableLastFrameCache(){
  //The result of the last call to SDLgfx is kept in memory
  //to display it again if rotation / scale / alpha didn't changed
  ASSERT(!have_rotation_cache);
  ASSERT(!have_flipping_cache);
  have_lastframe_cache = false;
}

void SpriteCache::InvalidLastFrame(){
  //Free lastframe_cache if the next frame to be displayed
  //is not the same as the last one.
  if(!have_lastframe_cache)
    return;
  last_frame.Free();
}

