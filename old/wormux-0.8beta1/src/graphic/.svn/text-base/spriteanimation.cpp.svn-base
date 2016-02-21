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
 * Sprite animation control.
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de) 
 *             Initial version
 *****************************************************************************/

#include "spriteanimation.h"
#include "game/time.h"
#include "sprite.h"

SpriteAnimation::SpriteAnimation(Sprite &p_sprite) :
  sprite(p_sprite)
{
   last_update = Time::GetInstance()->Read();
   speed_factor = 1.0f;
   frame_delta = 1;
   finished = false;
   show_on_finish = show_last_frame;
   loop = true;
   pingpong = false;
}

SpriteAnimation::SpriteAnimation(const SpriteAnimation &other, Sprite &p_sprite) :
  sprite(p_sprite)
{
   last_update = other.last_update;
   speed_factor = other.speed_factor;
   frame_delta = other.frame_delta;
   finished = other.finished;
   show_on_finish = other.show_on_finish;
   loop = other.loop;
   pingpong = other.pingpong;
}

void SpriteAnimation::SetSpeedFactor( float nv_speed){
  speed_factor = nv_speed;
}

void SpriteAnimation::Start(){
   finished = false;
   last_update = Time::GetInstance()->Read();
}

void SpriteAnimation::SetPlayBackward(bool enable){
  if (enable)
    frame_delta = -1;
  else 
    frame_delta = 1;
}

void SpriteAnimation::Finish() {
  finished = true;
}  

void SpriteAnimation::Update(){
  if (finished) return;
  
  Time * global_time = Time::GetInstance();
  if (global_time->Read() < (last_update + sprite.GetCurrentFrameObject().delay))
     return;
  const unsigned int current_frame = sprite.GetCurrentFrame();
  const unsigned int frame_count = sprite.GetFrameCount();

  //Delta to next frame used to enable frameskip
  //if delay between 2 frame is < fps
  int delta_to_next_f = (int)((float)((global_time->Read() - last_update) / sprite.GetCurrentFrameObject().delay) * speed_factor);
  last_update += (int)((float)(delta_to_next_f * sprite.GetCurrentFrameObject().delay) / speed_factor);

  //Animation is finished, when last frame have been fully played
  bool finish;
  if (frame_delta < 0)
    finish = ((int)current_frame + frame_delta * delta_to_next_f) <= -1;
  else
    finish = frame_count <= (current_frame + frame_delta * delta_to_next_f);

  if (finish && !loop && (!pingpong || frame_delta < 0))
     Finish();
  else
  {
    unsigned int next_frame = ( current_frame + frame_delta * delta_to_next_f ) % frame_count;

    if(pingpong)
    {
      if( frame_delta>0 && ( current_frame + frame_delta * delta_to_next_f ) >= frame_count)
      {
        next_frame = frame_count - next_frame -2;
        frame_delta = - frame_delta;
      }
      else
      if( frame_delta<0 && ( (int)current_frame + frame_delta * delta_to_next_f ) <= -1)
      {
        next_frame = (-((int)current_frame + frame_delta * delta_to_next_f )) % frame_count;
        frame_delta = - frame_delta;
      }
    }

    if(next_frame != current_frame)
    {
      if(next_frame >= frame_count)
        next_frame = 0;
      sprite.SetCurrentFrame(next_frame);
    }
  }
}

void SpriteAnimation::SetShowOnFinish(SpriteShowOnFinish show) { 
  show_on_finish = show; 
  loop = false;
}

bool SpriteAnimation::IsFinished() const { 
  return finished; 
}
  
void SpriteAnimation::SetLoopMode(bool enable) { 
  loop = enable; 
}

void SpriteAnimation::SetPingPongMode(bool enable) {
  pingpong = enable; 
}

SpriteAnimation::SpriteShowOnFinish SpriteAnimation::GetShowOnFinish() const {
  return show_on_finish;
}

