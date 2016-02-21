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
 * Sprite animation control.
 ******************************************************************************
 * 2005/09/21: Jean-Christophe Duberga (jcduberga@gmx.de)
 *             Initial version
 *****************************************************************************/

#include "game/game_time.h"
#include "graphic/spriteanimation.h"
#include "graphic/sprite.h"
#include "network/randomsync.h"

SpriteAnimation::SpriteAnimation(Sprite & p_sprite) :
  sprite(p_sprite),
  last_update(GameTime::GetInstance()->Read()),
  speed_factor(1<<SPEED_BITS),
  frame_delta(1),
  loop_wait(0),
  loop_wait_random(0),
  finished(false),
  show_on_finish(show_last_frame), //(enum)
  loop(true),
  pingpong(false)
{
}

SpriteAnimation::SpriteAnimation(const SpriteAnimation & other, Sprite & p_sprite) :
  sprite(p_sprite),
  last_update(other.last_update),
  speed_factor(other.speed_factor),
  frame_delta(other.frame_delta),
  loop_wait(other.loop_wait),
  loop_wait_random(other.loop_wait_random),
  finished(other.finished),
  show_on_finish(other.show_on_finish), //(enum)
  loop(other.loop),
  pingpong(other.pingpong)
{
}

void SpriteAnimation::Start()
{
   finished = false;
   last_update = GameTime::GetInstance()->Read();
}

void SpriteAnimation::Update()
{
  if (finished) { 
    return;
  }

  if (GameTime::GetInstance()->Read() < last_update + sprite.GetCurrentDelay()) {
    return;
  }

  uint current_frame = sprite.GetCurrentFrame();
  uint frame_count = sprite.GetFrameCount();

  //Delta to next frame used to enable frameskip
  //if delay between 2 frame is < fps
  int delta_to_next_f = (((GameTime::GetInstance()->Read() - last_update) / sprite.GetCurrentDelay()) * speed_factor)>>SPEED_BITS;
  last_update += ((delta_to_next_f<<SPEED_BITS) * sprite.GetCurrentDelay()) / speed_factor;

  //Animation is finished, when last frame have been fully played
  bool finish;

  int  delta = frame_delta * delta_to_next_f;
  if (frame_delta < 0) {
    finish = (int(current_frame) + delta <= -1);
  } else {
    finish = frame_count <= current_frame + delta;
  }

  if (finish && !loop && (!pingpong || frame_delta < 0)) {
     Finish();
  } else {
    uint next_frame = (current_frame + delta) % frame_count;

    if (pingpong) {
      if (frame_delta>0 && current_frame + delta >= frame_count) {
        next_frame = frame_count - next_frame -2;
        frame_delta = - frame_delta;
      } else if (frame_delta < 0 && int(current_frame) + delta <= -1) {
        next_frame = (delta-(int)current_frame) % frame_count;
        frame_delta = - frame_delta;
        CalculateWait();
      }
    }

    if (next_frame != current_frame) {
      if (next_frame >= frame_count) {
        next_frame = 0;
        CalculateWait();
      }
      sprite.SetCurrentFrame(next_frame);
    }
  }
}

void SpriteAnimation::CalculateWait()
{
  MSG_DEBUG("eye", "CalculateWait stat   :  wait = %d , random = %d", loop_wait, loop_wait_random);
  MSG_DEBUG("eye", "CalculateWait 1 : %d", last_update);

  if (loop_wait != 0) {
    last_update += loop_wait;
    if (loop_wait_random != 0) {
      MSG_DEBUG("random.get","SpriteAnimation::CalculateWait()");
      last_update +=  RandomSync().GetInt(0, loop_wait_random) - loop_wait_random/2;
    }
  }
  MSG_DEBUG("eye", "CalculateWait 2 : %d", last_update);
}
