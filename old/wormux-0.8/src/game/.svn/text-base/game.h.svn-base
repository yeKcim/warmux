/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
#include "include/singleton.h"
#include "network/chat.h"

// Forward declarations
class Character;
class ObjBox;
class FramePerSecond;
class PhysicalObj;

class Game : public Singleton<Game>
{
public:
  typedef enum {
    PLAYING = 0,
    HAS_PLAYED = 1,
    END_TURN = 2
  } game_loop_state_t;

  typedef enum {
    CLASSIC = 0,
    BLITZ   = 1
  } game_mode_t;

protected:
  virtual void Run();         // Main loop

  bool IsAnythingMoving() const;
  void MainLoop();
  void ApplyDiseaseDamage() const;
  int  NbrRemainingTeams() const;
  bool MenuQuitPause() const;

  game_loop_state_t   state;
  bool                give_objbox;
  uint                pause_seconde;

  friend class Singleton<Game>;
  Game();
  virtual ~Game();

private:
  static game_mode_t  mode;


  bool                isGameLaunched;
  ObjBox              *current_ObjBox;
  // Set the user requested a pause/end of the game
  bool                ask_for_menu;

  FramePerSecond      *fps;

  // Time to wait between 2 loops
  int                 delay;
  // Time to display the next frame
  uint                time_of_next_frame;
  // Time to compute the next physic engine frame
  uint                time_of_next_phy_frame;

  static uint         last_unique_id;

  void Draw();        // Draw to screen
  void MessageLoading() const;
  void UnloadDatas() const;

  // Input management (keyboard/mouse)
  void RefreshInput();
  void IgnorePendingInputEvents() const;

  void PingClient() const;

  void CallDraw();

  // Refresh all objects (position, state ...)
  void RefreshObject() const;

  PhysicalObj* GetMovingObject() const;

  void MessageEndOfGame() const;

  // we were playing in network but now we are alone
  bool HasBeenNetworkDisconnected() const;

  virtual void RefreshClock() = 0;
  virtual void __SetState_PLAYING() = 0;
  virtual void __SetState_HAS_PLAYED() = 0;
  virtual void __SetState_END_TURN() = 0;

public:
  static Game * GetInstance();
  static void SetMode(game_mode_t m) { CleanUp(); mode = m; };
  static std::string GetUniqueId();
  static void ResetUniqueIds();

  bool                character_already_chosen;
  Chat                chatsession;

  void Start();
  void Init();

  // Get remaining time to play
  virtual uint GetRemainingTime() const = 0;
  virtual bool IsGameFinished() const = 0;
  virtual void EndOfGame() = 0;

  // Read/Set State
  game_loop_state_t ReadState() const { return state; }
  void SetState(game_loop_state_t new_state, bool begin_game=false) const;
  bool IsGameLaunched() const;

  void UserAsksForMenu() { ask_for_menu = true; };
  void Really_SetState(game_loop_state_t new_state); // called by the action_handler

  // Signal death of a player
  void SignalCharacterDeath (const Character *character) const;

  // Signal character damage
  void SignalCharacterDamage(const Character *character) const;

  // Bonus box handling
  bool NewBox();
  void AddNewBox(ObjBox *);
  void SetCurrentBox(ObjBox * current_box) { current_ObjBox = current_box; };
  ObjBox * GetCurrentBox() { return current_ObjBox; };
};
#endif // GAME_H
