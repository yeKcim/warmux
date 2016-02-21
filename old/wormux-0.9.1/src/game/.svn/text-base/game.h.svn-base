/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include <WORMUX_singleton.h>
#include "network/chat.h"
#include "sound/sound_sample.h"
#include "graphic/text.h"

// Forward declarations
class Character;
class ObjBox;
class FramePerSecond;
class PhysicalObj;
class WeaponsList;

class Game : public Singleton<Game>
{
public:
  typedef enum {
    PLAYING = 0,
    HAS_PLAYED = 1,
    END_TURN = 2
  } game_loop_state_t;

protected:
  virtual bool Run();         // Main loop

  bool IsAnythingMoving() const;
  void MainLoop();
  void ApplyDiseaseDamage() const;
  int  NbrRemainingTeams() const;
  bool MenuQuitPause() const;

  SoundSample         countdown_sample;
  game_loop_state_t   state;
  bool                give_objbox;
  uint                last_clock_update;

  friend class Singleton<Game>;
  Game();
  virtual ~Game();

private:
  static std::string  current_rules;

  bool                isGameLaunched;
  ObjBox              *current_ObjBox;
  // Set that the user requested a pause/end of the game
  bool                ask_for_menu;

  FramePerSecond      *fps;

  // Time to wait between 2 loops
  int                 delay;
  // Time to display the next frame
  uint                time_of_next_frame;
  // Time to compute the next physic engine frame
  uint                time_of_next_phy_frame;

  bool                character_already_chosen;

  static uint         last_unique_id;
  uint                m_current_turn;
  Text                waiting_for_network_text;
  WeaponsList *       weapons_list;

  void Draw();        // Draw to screen

  // Initialization
  void InitEverything();
  void InitGameData_NetGameMaster();
  void EndInitGameData_NetGameMaster();
  void EndInitGameData_NetClient();
  void InitMap();
  void InitTeams();
  void InitSounds();
  void InitData();
  void InitInterface();
  void WaitForOtherPlayers();

  void UnloadDatas(bool game_finished) const;

  // Input management (keyboard/mouse)
  void RefreshInput();
  void IgnorePendingInputEvents() const;

  void PingClient() const;

  void CallDraw();

  // Refresh all objects (position, state ...)
  void RefreshObject() const;

  void RefreshActions() const;

  PhysicalObj* GetMovingObject() const;

  void MessageEndOfGame() const;

  // we were playing in network but now we are alone
  bool HasBeenNetworkDisconnected() const;

  virtual void RefreshClock() = 0;
  virtual void __SetState_PLAYING() = 0;
  virtual void __SetState_HAS_PLAYED() = 0;
  virtual void __SetState_END_TURN() = 0;

  bool IsGameLaunched() const;

public:
  static Game * GetInstance();
  static std::string GetUniqueId();
  static void ResetUniqueIds();
  static bool IsRunning();
  uint GetCurrentTurn();
  WeaponsList * GetWeaponsList() { return weapons_list; }
  void UpdateTranslation();

  Chat                chatsession;

  void DisplayError(const std::string &msg);

  // Set mode
  static Game * UpdateGameRules();

  void Start();

  bool IsCharacterAlreadyChosen() const;
  void SetCharacterChosen(bool chosen);

  // Get remaining time to play
  virtual uint GetRemainingTime() const = 0;
  virtual bool IsGameFinished() const = 0;
  virtual void EndOfGame() = 0;

  // Read/Set State
  game_loop_state_t ReadState() const { return state; }
  void SetState(game_loop_state_t new_state, bool begin_game=false);

  void UserAsksForMenu() { ask_for_menu = true; };

  // Signal death of a player
  void SignalCharacterDeath (const Character *character);

  // Signal character damage
  void SignalCharacterDamage(const Character *character);

  // Bonus box handling
  bool NewBox();
  void SetCurrentBox(ObjBox * current_box) { current_ObjBox = current_box; };
  ObjBox * GetCurrentBox() { return current_ObjBox; };
  void RequestBonusBoxDrop();
};
#endif // GAME_H
