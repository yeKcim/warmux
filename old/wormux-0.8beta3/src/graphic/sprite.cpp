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

#include "graphic/sprite.h"
#include <SDL.h>
#include <SDL_rotozoom.h>
#include <iostream>
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"
#include "map/map.h"
#include "tool/rectangle.h"
#include "tool/debug.h"
#include "graphic/spriteframe.h"

#define BUGGY_SDLGFX

Sprite::Sprite() :
  cache(*this),
  animation(*this)
{
  Constructor();
}

Sprite::Sprite(const Surface& surface ) :
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
  rotation_rad = other.rotation_rad;
  current_frame = other.current_frame;
  rot_hotspot = other.rot_hotspot;
  show = other.show;

  for(unsigned int f=0;f<other.frames.size();f++)
    AddFrame(other.frames[f].surface,other.frames[f].delay);

  if(other.cache.have_lastframe_cache)
    cache.EnableLastFrameCache();
  if(other.cache.have_rotation_cache)
    EnableRotationCache(other.cache.rotation_cache_size);
  if(other.cache.have_flipping_cache)
    EnableFlippingCache();
}

void Sprite::Constructor() {
  show = true;
  current_frame = 0;
  frame_width_pix = frame_height_pix = 0;
  alpha = 1.0f;
  scale_x = scale_y = 1.0f;
  rotation_rad = 0.0f;
  SetRotation_HotSpot(center);
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
  ASSERT(frame_width_pix == 0 && frame_height_pix == 0)

  frame_width_pix = w;
  frame_height_pix = h;
}

void Sprite::SetSize(const Point2i &size){
        SetSize(size.x, size.y);
}

unsigned int Sprite::GetWidth() const{
   return static_cast<uint>(frame_width_pix * (scale_x>0?scale_x:-scale_x));
}

unsigned int Sprite::GetWidthMax() const{
  if(!current_surface.IsNull() )
    return current_surface.GetWidth();
  else
    return GetWidth();
}

unsigned int Sprite::GetHeight() const{
   return static_cast<uint>(frame_height_pix * (scale_y>0?scale_y:-scale_y));
}

unsigned int Sprite::GetHeightMax() const{
  if(!current_surface.IsNull() )
    return current_surface.GetHeight();
  else
    return GetHeight();
}

Point2i Sprite::GetSize() const{
        return Point2i(GetWidth(), GetHeight());
}

Point2i Sprite::GetSizeMax() const{
        return Point2i(GetWidthMax(), GetHeightMax());
}

unsigned int Sprite::GetFrameCount() const{
   return frames.size();
}

void Sprite::SetCurrentFrame( unsigned int frame_no){
  ASSERT (frame_no < frames.size());
  if (current_frame != frame_no) {
    cache.InvalidLastFrame();
    MSG_DEBUG("sprite", "Set current frame : %d", frame_no);
  }
  current_frame = frame_no;
}

unsigned int Sprite::GetCurrentFrame() const{
  ASSERT(current_frame < frames.size());
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

void Sprite::Scale( float _scale_x, float _scale_y){
   this->scale_x = _scale_x;
   this->scale_y = _scale_y;
   cache.InvalidLastFrame();
}

void Sprite::ScaleSize(int width, int height){
  Scale(float(width)/float(frame_width_pix),
        float(height)/float(frame_height_pix));
}

void Sprite::ScaleSize(const Point2i& size){
        ScaleSize(size.x, size.y);
}

void Sprite::GetScaleFactors( float &_scale_x, float &_scale_y) const {
   _scale_x = this->scale_x;
   _scale_y = this->scale_y;
}

void Sprite::SetFrameSpeed(unsigned int nv_fs){
   for ( unsigned int f = 0 ; f < frames.size() ; f++)
     frames[f].delay = nv_fs;
}

void Sprite::SetAlpha( float _alpha){
  ASSERT(_alpha >= 0.0 && _alpha <= 1.0);
  this->alpha = _alpha;
}

float Sprite::GetAlpha() const {
  return alpha;
}

void Sprite::SetRotation_rad( double angle_rad){
   while(angle_rad > 2*M_PI)
     angle_rad -= 2 * M_PI;
   while(angle_rad <= -2*M_PI)
     angle_rad += 2 * M_PI;

   if(rotation_rad == angle_rad) return;

   rotation_rad = angle_rad;
   cache.InvalidLastFrame();
}

const double &Sprite::GetRotation_rad() const
{
  ASSERT(rotation_rad > -2*M_PI && rotation_rad <= 2*M_PI);
  return rotation_rad;
}

void Sprite::SetRotation_HotSpot( const Point2i& new_hotspot)
{
  rot_hotspot = user_defined;
  rhs_pos = new_hotspot;

  if( rhs_pos.x * 2 == static_cast<int>(GetWidth()) &&
      rhs_pos.y * 2 == static_cast<int>(GetHeight())  )
    rot_hotspot = center; // avoid using Calculate_Rotation_Offset, thus avoiding a division by zero
}

void Sprite::Calculate_Rotation_Offset(const Surface& tmp_surface){
  const SpriteFrame& frame = GetCurrentFrameObject();
  const Surface &surface = frame.surface;
  // Calculate offset of the depending on hotspot rotation position :

  int surfaceHeight = surface.GetHeight();
  int surfaceWidth = surface.GetWidth();

  //Do as if hotspot is center of picture:
  rotation_point.x = surfaceWidth  / 2 - tmp_surface.GetWidth()  / 2;
  rotation_point.y = surfaceHeight / 2 - tmp_surface.GetHeight() / 2;

  if(rot_hotspot == center)
      return;

  if(rot_hotspot != user_defined)
  {
    switch(rot_hotspot)
    {
    case top_left:      rhs_pos = Point2i( 0,              0);               break;
    case top_center:    rhs_pos = Point2i( surfaceWidth/2, 0);               break;
    case top_right:     rhs_pos = Point2i( surfaceWidth,   0);               break;
    case left_center:   rhs_pos = Point2i( 0,              surfaceHeight/2); break;
    case center:        rhs_pos = Point2i( surfaceWidth/2, surfaceHeight/2); break;
    case right_center:  rhs_pos = Point2i( surfaceWidth,   surfaceHeight/2); break;
    case bottom_left:   rhs_pos = Point2i( 0,              surfaceHeight);   break;
    case bottom_center: rhs_pos = Point2i( surfaceWidth/2, surfaceHeight);   break;
    case bottom_right:  rhs_pos = Point2i( surfaceWidth,   surfaceHeight);   break;
    default:
      ASSERT(false);
    }
  }

  Point2i rhs_pos_tmp;
  rhs_pos_tmp.x = static_cast<uint>(rhs_pos.x * scale_x);
  rhs_pos_tmp.y = static_cast<uint>(rhs_pos.y * scale_y);
  surfaceWidth  = static_cast<uint>(surfaceWidth  * scale_x);
  surfaceHeight = static_cast<uint>(surfaceHeight * scale_y);

  //Calculate the position of the hotspot after a rotation around the center of the surface:
  float rhs_dst; //Distance between center of the sprite and the hotspot
  double rhs_angle; //Angle of the hotspot _before_ the rotation

  rhs_dst = sqrt(float((surfaceWidth /2 - rhs_pos_tmp.x)*(surfaceWidth /2 - rhs_pos_tmp.x)
                     + (surfaceHeight/2 - rhs_pos_tmp.y)*(surfaceHeight/2 - rhs_pos_tmp.y)));

  if( rhs_dst == 0.0)
    rhs_angle = 0.0;
  else
    rhs_angle = - acos ( float(rhs_pos_tmp.x - surfaceWidth/2) / rhs_dst );

  if(surfaceHeight/2 - rhs_pos.y < 0) rhs_angle = -rhs_angle;

  rhs_angle += rotation_rad;

  Point2i rhs_new_pos =  Point2i(surfaceWidth /2 + static_cast<uint>(cos(rhs_angle) * rhs_dst),
                                 surfaceHeight/2 + static_cast<uint>(sin(rhs_angle) * rhs_dst));

  rotation_point.x -= rhs_new_pos.x;
  rotation_point.y -= rhs_new_pos.y;
  rotation_point.x += rhs_pos_tmp.x;
  rotation_point.y += rhs_pos_tmp.y;
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

  Rectanglei srcRect (src_x, src_y, w, h);
  Rectanglei dstRect (pos_x + rotation_point.x, pos_y + rotation_point.y, w, h);

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
  world.ToRedrawOnScreen(dstRect);
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
  DrawXY(pos - Camera::GetInstance()->GetPosition());
}

void Sprite::DrawXY(const Point2i &pos){
  if( !show )
    return;

  Blit(AppWormux::GetInstance()->video->window, pos);
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
      current_surface = frames[current_frame].surface.RotoZoom(-rotation_rad, scale_x, scale_y, SMOOTHING_OFF);
    else
    {
      if(cache.last_frame.IsNull() )
      {
#ifdef BUGGY_SDLGFX
        if(rotation_rad != 0.0 || (scale_x != 1.0 && scale_y == 1.0))
        {
          current_surface = frames[current_frame].surface.RotoZoom(-rotation_rad , scale_x, scale_y, SMOOTHING_OFF);
          cache.last_frame = current_surface;
        }
        else
        if(scale_x != 1.0 || scale_y != 1.0)
        {
#endif
          current_surface = frames[current_frame].surface.RotoZoom(-rotation_rad, scale_x, scale_y, SMOOTHING_ON);
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
      if(rotation_rad != 0.0 || scale_y != 1.0 || (scale_x != 1.0 && scale_x != -1.0))
      {
        current_surface = frames[current_frame].surface.RotoZoom( rotation_rad, scale_x, scale_y, SMOOTHING_OFF );
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
        current_surface = frames[current_frame].surface.RotoZoom(rotation_rad, scale_x, scale_y, SMOOTHING_OFF);
      else
        current_surface = cache.frames[current_frame].GetSurfaceForAngle(rotation_rad);
    }
    else
    {
      //cache.have_flipping_cache==true && cache.have_rotation_cache==true
      if((scale_x != 1.0 && scale_x != -1.0)  || scale_y != 1.0)
        current_surface = frames[current_frame].surface.RotoZoom( rotation_rad, scale_x, scale_y, SMOOTHING_OFF);
      else
      {
        //Scale_y == 1.0
        if(scale_x == 1.0)
          current_surface = cache.frames[current_frame].GetSurfaceForAngle(rotation_rad);
        else
          current_surface = cache.frames[current_frame].GetFlippedSurfaceForAngle(rotation_rad);
      }
    }
  }
  ASSERT( !current_surface.IsNull() );

  // Calculate offset of the sprite depending on hotspot rotation position :
  rotation_point.x=0;
  rotation_point.y=0;
  if(rot_hotspot != center || rotation_rad!=0.0)
    Calculate_Rotation_Offset(current_surface);
}

Surface Sprite::GetSurface() const {
  ASSERT ( !current_surface.IsNull() );
  return current_surface;
}

