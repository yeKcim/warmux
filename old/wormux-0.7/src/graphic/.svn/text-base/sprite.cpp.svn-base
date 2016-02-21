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
 *****************************************************************************/

#include "sprite.h"
#include <SDL.h>
#include <SDL_rotozoom.h>
#include <iostream>
#include "surface.h"
#include "../game/game.h"
#include "../include/app.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../tool/rectangle.h"
#include "../tool/debug.h"

#define BUGGY_SDLGFX

Sprite::Sprite() :
  cache(*this),
  animation(*this)
{
  Constructor();
}

Sprite::Sprite( Surface surface ) :
  cache(*this),
  animation(*this)
{
   Constructor();
   frame_width_pix = surface.GetWidth();
   frame_height_pix = surface.GetHeight();
   frames.push_back( SpriteFrame(surface));
}

Sprite::Sprite(const Sprite &other) :
  cache(*this),
  animation(other.animation,*this)
{
  frame_width_pix = other.frame_width_pix;
  frame_height_pix = other.frame_height_pix;
  scale_x = other.scale_x;
  scale_y = other.scale_y;
  alpha = other.alpha;
  rotation_deg = other.rotation_deg;
  current_frame = other.current_frame;
  rot_hotspot = other.rot_hotspot;
  show = other.show;

  for(unsigned int f=0;f<other.frames.size();f++)
    AddFrame(other.frames[f].surface,other.frames[f].delay);
}

void Sprite::Constructor() {
  show = true;
  current_frame = 0;
  frame_width_pix = frame_height_pix = 0;
  alpha = 1.0f;
  scale_x = scale_y = 1.0f;
  rotation_deg = 0.0f;   
  rot_hotspot = center;
}

void Sprite::Init(Surface& surface, const Point2i &frameSize, int nb_frames_x, int nb_frames_y){
   Point2i f;

   this->frame_width_pix = frameSize.x;
   this->frame_height_pix = frameSize.y;

   surface.SetAlpha( 0, 0);
   
   for( f.y = 0; f.y < nb_frames_y; f.y++)
     for( f.x = 0; f.x < nb_frames_x; f.x++){
       Surface new_surf = Surface(frameSize, SDL_SWSURFACE|SDL_SRCALPHA, true);
       Rectanglei sr(f * frameSize, frameSize);
	  
       new_surf.Blit( surface, sr, Point2i(0, 0));
       frames.push_back( SpriteFrame(new_surf));
     }
}

void Sprite::AddFrame(const Surface &surf, unsigned int delay){
	  frames.push_back( SpriteFrame(surf, delay) );
}

void Sprite::SetSize(unsigned int w, unsigned int h){
  assert(frame_width_pix == 0 && frame_height_pix == 0)
	frame_width_pix = w;
	frame_height_pix = h;
}

void Sprite::SetSize(const Point2i &size){
	SetSize(size.x, size.y);
}

unsigned int Sprite::GetWidth() const{
   return (uint)((float)frame_width_pix * (scale_x>0?scale_x:-scale_x));
}

unsigned int Sprite::GetHeight() const{
   return (uint)((float)frame_height_pix * (scale_y>0?scale_y:-scale_y));
}

Point2i Sprite::GetSize() const{
	return Point2i(GetWidth(), GetHeight());
}

unsigned int Sprite::GetFrameCount(){
   return frames.size();
}

void Sprite::SetCurrentFrame( unsigned int frame_no){
  assert (frame_no < frames.size());
  if (current_frame != frame_no) {
    cache.InvalidLastFrame();
    MSG_DEBUG("sprite", "Set current frame : %d", frame_no);
  }
  current_frame = frame_no;
}

unsigned int Sprite::GetCurrentFrame() const{
   return current_frame;
}

SpriteFrame& Sprite::operator[] (unsigned int index){
  return frames.at(index);
}

const SpriteFrame& Sprite::operator[] (unsigned int index) const{
  return frames.at(index);
}

const SpriteFrame& Sprite::GetCurrentFrameObject() const{
   return frames[current_frame];
}

void Sprite::Scale( float scale_x, float scale_y){
   if(this->scale_x == scale_x && this->scale_y == scale_y)
	   return;
   this->scale_x = scale_x;
   this->scale_y = scale_y;
   cache.InvalidLastFrame();
}

void Sprite::ScaleSize(int width, int height){
  Scale(float(width)/float(frame_width_pix),
        float(height)/float(frame_height_pix));
}

void Sprite::ScaleSize(Point2i size){
	ScaleSize(size.x, size.y);
}

void Sprite::GetScaleFactors( float &scale_x, float &scale_y){
   scale_x = this->scale_x;
   scale_y = this->scale_y;
}

void Sprite::SetFrameSpeed(unsigned int nv_fs){
   for ( unsigned int f = 0 ; f < frames.size() ; f++)
     frames[f].delay = nv_fs;
}

void Sprite::SetAlpha( float alpha){
  assert(alpha >= 0.0 && alpha <= 1.0);
  if(this->alpha == alpha)
    return;
  this->alpha = alpha;
}

float Sprite::GetAlpha(){
  return alpha;
}

void Sprite::SetRotation_deg( float angle_deg){
   while(angle_deg >= 360.0)
     angle_deg -= 360.0;
   while(angle_deg < 0.0)
     angle_deg += 360.0;

   if(rotation_deg == angle_deg) return;

   rotation_deg = angle_deg;
   cache.InvalidLastFrame();
}

void Sprite::Calculate_Rotation_Offset(int & rot_x, int & rot_y, Surface& tmp_surface){
    const SpriteFrame& frame = GetCurrentFrameObject();
    const Surface &surface = frame.surface;
    // Calculate offset of the depending on hotspot rotation position :
  
    int surfaceHeight = surface.GetHeight();
	int surfaceWidth = surface.GetWidth();

   //Do as if hotspot is center of picture:
   rot_x = surfaceWidth / 2 - tmp_surface.GetWidth() / 2;
   rot_y = surfaceHeight / 2 - tmp_surface.GetHeight() / 2;

   if(rot_hotspot == center)
      return;

   //TODO:Rewrite this function... It's quite dirty, i know :p
   //Distance between center of picture and hotspot:
   // Works with scale == 1 or -1

   float d = 0.0 ;
   //Angle between center of picture and hotspot:
   float d_angle = 0.0;

   //Rotoation angle in radians:
   float angle = rotation_deg * M_PI / 180.0;

   switch(rot_hotspot)
   {
   case top_left:
   case top_right:
   case bottom_left:
   case bottom_right:
      d = sqrt(surfaceWidth * surfaceWidth +
               surfaceHeight * surfaceHeight) / 2;
      break;
   case top_center:
   case bottom_center:
      d = surfaceHeight / 2;
      break;
   case left_center:
   case right_center:
      d = surfaceWidth / 2;
      break;
   default: break;
   }

   switch(rot_hotspot)
   {
   case top_center:
      d_angle = 0.0;
      break;
   case top_left:
      if(surfaceWidth<surfaceHeight)
        d_angle = atan(surfaceWidth / surfaceHeight);
      else
        d_angle = atan(surfaceHeight / surfaceWidth);
      break;
   case left_center:
      d_angle = M_PI_2;
      break;
   case bottom_left:
      if(surfaceWidth<surfaceHeight)
        d_angle = M_PI - atan(surfaceWidth / surfaceHeight);
      else
        d_angle = M_PI - atan(surfaceHeight / surfaceWidth);
      break;
   case bottom_center:
      d_angle = M_PI;
      break;
   case bottom_right:
      if(surfaceWidth<surfaceHeight)
        d_angle = M_PI + atan(surfaceWidth / surfaceHeight);
      else
        d_angle = M_PI + atan(surfaceHeight / surfaceWidth);
      break;
   case right_center:
      d_angle =  - M_PI_2;
      break;
   case top_right:
      if(surfaceWidth<surfaceHeight)
        d_angle = - atan(surfaceWidth / surfaceHeight);
      else
        d_angle = - atan(surfaceHeight / surfaceWidth);
      break;
   default: break;
   }

   if(this->scale_y > 0.0)
      d_angle += M_PI_2;
   else
      d_angle = - d_angle - M_PI_2;

   rot_x -= static_cast<int>(cos(angle - d_angle) * d);
   rot_y -= static_cast<int>(sin(angle - d_angle) * d);

   switch(rot_hotspot)
   {
   case top_left:
   case left_center:
   case bottom_left:
      rot_x -= static_cast<int>(scale_y * surfaceWidth / 2);
      break;
   case top_right:
   case right_center:
   case bottom_right:
      rot_x += static_cast<int>(scale_y * surfaceWidth / 2);
      break;
   default: break;
   }

   switch(rot_hotspot)
   {
   case top_left:
   case top_center:
   case top_right:
      rot_y -= surfaceHeight / 2;
      break;
   case bottom_left:
   case bottom_center:
   case bottom_right:
      rot_y += surfaceHeight / 2;
      break;
   default: break;
   }
}

void Sprite::Start(){
   show = true;
   animation.Start();
   cache.InvalidLastFrame();
}

void Sprite::Blit( Surface &dest, uint pos_x, uint pos_y){
  RefreshSurface();
    Blit(dest, pos_x, pos_y, 0, 0, current_surface.GetWidth(), current_surface.GetHeight());
}

void Sprite::Blit( Surface &dest, const Point2i &pos){
	Blit(dest, pos.GetX(), pos.GetY());
}

void Sprite::Blit( Surface &dest, const Rectanglei &srcRect, const Point2i &destPos){
	Blit(dest, destPos.GetX(), destPos.GetY(), srcRect.GetPositionX(), srcRect.GetPositionY(), srcRect.GetSizeX(), srcRect.GetSizeY() );
}

void Sprite::Blit( Surface &dest, int pos_x, int pos_y, int src_x, int src_y, uint w, uint h){
  if (!show)
	return;

  RefreshSurface();

   // Calculate offset of the sprite depending on hotspot rotation position :
  int rot_x=0;
  int rot_y=0;
  if( rotation_deg!=0.0 )
    Calculate_Rotation_Offset(rot_x, rot_y, current_surface);

  Rectanglei srcRect (src_x, src_y, w, h);
  Rectanglei dstRect (pos_x + rot_x, pos_y + rot_y, w, h);
  
  if(alpha == 1.0)
    dest.Blit(current_surface, srcRect, dstRect.GetPosition());
  else
  {
    Surface surf_alpha;
    surf_alpha.NewSurface(srcRect.GetSize(),SDL_SWSURFACE,false);
    surf_alpha.Blit(dest,dstRect,Point2i(0,0));
    surf_alpha.SetAlpha(SDL_SRCALPHA, (int)(alpha * 255.0));
    surf_alpha.Blit(current_surface,srcRect,Point2i(0,0));
    dest.Blit(surf_alpha, srcRect, dstRect.GetPosition());
  }

  // For the cache mechanism
  if( Game::GetInstance()->IsGameLaunched() )
    world.ToRedrawOnScreen( dstRect );
}

void Sprite::Finish(){
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
    current_frame = frames.size()-1;
    break;      
  }
  cache.InvalidLastFrame();
}

void Sprite::Update(){
  animation.Update();
}    

void Sprite::Draw(const Point2i &pos){
	if( !show )
		return;

	Blit(AppWormux::GetInstance()->video.window, pos - camera.GetPosition() );	
}

void Sprite::Show() { show = true; }
void Sprite::Hide() { show = false; }
bool Sprite::IsFinished() const { return animation.IsFinished(); }
  
void Sprite::EnableRotationCache(unsigned int cache_size) {
  cache.EnableRotationCache(frames, cache_size);
}

void Sprite::EnableFlippingCache() {
  cache.EnableFlippingCache(frames);
}

void Sprite::RefreshSurface()
{
  current_surface.Free();

  if(!cache.have_rotation_cache && !cache.have_flipping_cache)
  {
    if(!cache.have_lastframe_cache)
      current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_OFF);
    else
    {
      if(cache.last_frame.IsNull() )
      {
#ifdef BUGGY_SDLGFX
        if(rotation_deg != 0.0 || (scale_x != 1.0 && scale_y == 1.0))
        {
		  current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_OFF);
          cache.last_frame = current_surface;
        }
        else
        if(scale_x != 1.0 || scale_y != 1.0)
        {
#endif
		  current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_ON);
          cache.last_frame = current_surface;
#ifdef BUGGY_SDLGFX
        }
        else
        {
          current_surface = frames[current_frame].surface;
          cache.last_frame.Free();
        }
#endif
      }
      else
      {
        current_surface = cache.last_frame;
      }
    }
  }
  else
  {
    if(cache.have_flipping_cache && !cache.have_rotation_cache)
    {
      if(rotation_deg != 0.0 || scale_y != 1.0 || (scale_x != 1.0 && scale_x != -1.0))
      {
        current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_OFF );
      }
      else
      if(scale_x == 1.0)
        current_surface = frames[current_frame].surface;
      else
        current_surface = cache.frames[current_frame].flipped_surface;
    }
    else
    if(!cache.have_flipping_cache && cache.have_rotation_cache)
    {
      if(scale_x != 1.0 || scale_y != 1.0)
        current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_OFF);
      else
        current_surface = cache.frames[current_frame].rotated_surface[(unsigned int)rotation_deg*cache.rotation_cache_size/360];
    }
    else
    {
      //cache.have_flipping_cache==true && cache.have_rotation_cache==true
      if((scale_x != 1.0 && scale_x != -1.0)  || scale_y != 1.0)
        current_surface = frames[current_frame].surface.RotoZoom( -rotation_deg, scale_x, scale_y, SMOOTHING_OFF);
      else
      {
        //Scale_y == 1.0
        if(scale_x == 1.0)
          current_surface = cache.frames[current_frame].rotated_surface[(unsigned int)rotation_deg*cache.rotation_cache_size/360];
        else
          current_surface = cache.frames[current_frame].rotated_flipped_surface[(unsigned int)rotation_deg*cache.rotation_cache_size/360];
      }
    }
  }
  assert( !current_surface.IsNull() );
}

Surface Sprite::GetSurface() {
  assert ( !current_surface.IsNull() );
  return current_surface;
}

