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

#ifndef _SPRITE_ANIMATION_H
#define _SPRITE_ANIMATION_H

#include <WARMUX_base.h>
#include <WARMUX_debug.h>

class Sprite;

#define SPEED_BITS  8

class SpriteAnimation
{
public:
  typedef enum {
    show_first_frame,
    show_last_frame,
    show_blank
  } SpriteShowOnFinish;

private:
  Sprite &sprite;

  // Speed
  uint last_update;
  int speed_factor;
  int frame_delta; // Used in Update() to get next frame
  int loop_wait;
  int loop_wait_random;

  // State
  bool finished;
  SpriteShowOnFinish show_on_finish;

  // Options
  bool loop;
  bool pingpong;

public:
  SpriteAnimation(Sprite &sprite);
  SpriteAnimation(const SpriteAnimation &other,Sprite &sprite);

  // Control animation
  void Start();
  void Update();
  void Finish() { finished = true; }
  bool IsFinished() const { return finished; }
  void CalculateWait();

  // Control speed
  void SetSpeedFactor(Double nv_speed) { speed_factor = nv_speed<<SPEED_BITS; }

  // Control options
  void SetPlayBackward(bool enable) { frame_delta = enable ? -1 : 1; }
  void SetLoopMode(bool enable) { loop = enable; }
  void SetPingPongMode(bool enable) { pingpong = enable; }
  void SetLoopWaitRandom(int time)
  {
    MSG_DEBUG("eye", "SetLoopWaitRandom  : %d -> %d", loop_wait_random, time);
    loop_wait_random = time;
  }
  void SetLoopWait(int time)
  {
    MSG_DEBUG("eye", "SetLoopWait  : %d -> %d", loop_wait, time);
    loop_wait = time;
  }
  void SetShowOnFinish(SpriteShowOnFinish show) { show_on_finish = show; loop = false; }
  SpriteShowOnFinish GetShowOnFinish() const { return show_on_finish; }
};

#endif /* _SPRITE_ANIMATION_H */
