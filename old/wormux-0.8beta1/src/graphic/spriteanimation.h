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

#ifndef _SPRITE_ANIMATION_H
#define _SPRITE_ANIMATION_H

#include "include/base.h"

class Sprite;

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
  unsigned int last_update;
  float speed_factor;
  int frame_delta; // Used in Update() to get next frame

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
  void Finish();
  bool IsFinished() const;

  // Control speed
  void SetSpeedFactor(float nv_speed);

  // Control options
  void SetPlayBackward(bool enable);
  void SetLoopMode(bool enable);
  void SetPingPongMode(bool enable);
  void SetShowOnFinish(SpriteShowOnFinish show);
  SpriteShowOnFinish GetShowOnFinish() const;
};

#endif /* _SPRITE_ANIMATION_H */
