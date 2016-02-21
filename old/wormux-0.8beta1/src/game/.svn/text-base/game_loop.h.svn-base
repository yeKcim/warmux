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
 * Game loop : drawing and data handling
 *****************************************************************************/

#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "graphic/fps.h"
#include "include/base.h"
#include "character/character.h"
#include "network/chat.h"
#include "object/bonus_box.h"
#include "object/medkit.h"

class GameLoop
{
  /* If you need this, implement it (correctly)*/
  GameLoop(const GameLoop&);
  const GameLoop& operator=(const GameLoop&);
  /**********************************************/

public:
  typedef enum {
    PLAYING = 0,
    HAS_PLAYED = 1,
    END_TURN = 2
  } game_loop_state_t;

private:
  game_loop_state_t state;
  uint pause_seconde;
  uint duration;
  ObjBox * current_ObjBox;
  bool give_objbox;

  FramePerSecond fps;

  static GameLoop * singleton;
  GameLoop();

public:
  static GameLoop * GetInstance();

  bool character_already_chosen;
  Chat chatsession;

  void Init();

  // Draw to screen
  void Draw();

  // Main loop
  void Run();

  // Read/Set State
  game_loop_state_t ReadState() const { return state; }
  void SetState(game_loop_state_t new_state, bool begin_game=false);
  void Really_SetState(game_loop_state_t new_state); // called by the action_handler

  // Signal death of a player
  void SignalCharacterDeath (Character *character);

  // Signal character damage
  void SignalCharacterDamage(Character *character);

  void SetCurrentBox(ObjBox * current_box);
  ObjBox * GetCurrentBox() const;

private:

  // Refresh all objects (position, state ...)
  void RefreshObject();
  void RefreshClock();

  // Input management (keyboard/mouse)
  void RefreshInput();
  void IgnorePendingInputEvents();

  void PingClient();

  void CallDraw();

  PhysicalObj* GetMovingObject();
  bool IsAnythingMoving();
  void ApplyDiseaseDamage();
  void ApplyDeathMode();

  void __SetState_PLAYING();
  void __SetState_HAS_PLAYED();
  void __SetState_END_TURN();

  void EndOfGame();
};
#endif
