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
 * Main Class manage the game : initialization, drawing, components management
 * and game loop.
 *****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "include/base.h"
#include "network/chat.h"

// Forward declarations
class Character;
class ObjBox;
class PhysicalObj;
class FramePerSecond;
class Question;

class Game
{
  /* If you need this, implement it (correctly)*/
  Game(const Game&);
  Game operator=(const Game&);
  /**********************************************/

public:
  typedef enum {
    PLAYING = 0,
    HAS_PLAYED = 1,
    END_TURN = 2
  } game_loop_state_t;


private:
  bool isGameLaunched;

  static Game * singleton;

  // Set the user requested an end of the game
  bool want_end_of_game;

  game_loop_state_t state;
  uint pause_seconde;
  uint duration;
  ObjBox * current_ObjBox;
  bool give_objbox;

  FramePerSecond *fps;

  // Time to wait between 2 loops
  int delay;
  // Time to display the next frame
  uint time_of_next_frame;
  // Time to compute the next physic engine frame
  uint time_of_next_phy_frame;

  static uint last_unique_id;
public:
  static Game * GetInstance();

  void Start();
  void UnloadDatas() const;

  bool IsGameLaunched() const;

  static std::string GetUniqueId();
  static void ResetUniqueIds();

  bool character_already_chosen;
  Chat chatsession;

  void Init();

  // Draw to screen
  void Draw();

  // Main loop
  void Run();

  // Read/Set State
  game_loop_state_t ReadState() const { return state; }
  void SetState(game_loop_state_t new_state, bool begin_game=false) const;
  bool IsGameFinished() const;

  void UserWantEndOfGame() { want_end_of_game = true; };
  void Really_SetState(game_loop_state_t new_state); // called by the action_handler

  // Get remaining time to play
  uint GetRemainingTime() const;
  // Signal death of a player
  void SignalCharacterDeath (const Character *character) const;

  // Signal character damage
  void SignalCharacterDamage(const Character *character) const;

  // Bonus box handling
  bool NewBox();
  void AddNewBox(ObjBox *);
  void SetCurrentBox(ObjBox * current_box) { current_ObjBox = current_box; };
  ObjBox * GetCurrentBox() { return current_ObjBox; };

private:
  Game();
  ~Game();

  void MessageLoading() const;

  // Refresh all objects (position, state ...)
  void RefreshObject() const;
  void RefreshClock();

  // Input management (keyboard/mouse)
  void RefreshInput();
  void IgnorePendingInputEvents() const;

  void PingClient() const;

  void CallDraw();

  PhysicalObj* GetMovingObject() const;
  bool IsAnythingMoving() const;
  void ApplyDiseaseDamage() const;
  void ApplyDeathMode() const;

  void __SetState_PLAYING();
  void __SetState_HAS_PLAYED();
  void __SetState_END_TURN();

  void DisplayPause() const;
  bool AskQuit() const;
  void MessageEndOfGame() const;
  int NbrRemainingTeams() const;
  void EndOfGame();
  void MainLoop();
};
#endif
