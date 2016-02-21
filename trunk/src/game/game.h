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
 * Main Class manage the game : initialization, drawing, components management
 * and game loop.
 *****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include <vector>

#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "network/chat.h"
#include "sound/sound_sample.h"
#include "graphic/text.h"

// Forward declarations
class Character;
class ObjBox;
class FramePerSecond;
class PhysicalObj;
class WeaponsList;
class Menu;

class Game : public Singleton<Game>
{
public:
  typedef enum {
    PLAYING = 0,
    HAS_PLAYED = 1,
    END_TURN = 2
  } game_loop_state_t;

  typedef enum {
    NO_REQUEST,
    START_PAUSE,
    END_PAUSE
  } PauseRequest;

protected:
  virtual bool Run();         // Main loop

  bool IsAnythingMoving() const;
  void MainLoop();
  void ApplyDiseaseDamage() const;
  uint RemainingGroups() const;
  bool MenuQuitPause();
  void MenuHelpPause();

  SoundSample         countdown_sample;
  game_loop_state_t   state;
  bool                give_objbox;
  uint                last_clock_update;
  bool                benching;
  std::vector< std::pair<float, float> > bench_res;

  friend class Singleton<Game>;
  friend bool GameIsRunning();
  Game();
  virtual ~Game();

private:
  static std::string  current_rules;
  std::string         current_mode;

  bool                isGameLaunched;
  ObjBox              *current_ObjBox;
  // Set that the user requested a pause/end of the game
  bool                ask_for_menu;
  bool                ask_for_help_menu;
  bool                ask_for_end;
  PauseRequest        request_pause;
  Double              request_speed;
  uint                request_time;

  FramePerSecond      *fps;

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
  void InitGameData_RePlay();
  void EndInitGameData_NetGameMaster();
  void EndInitGameData_NetClient();
  void InitMap();
  void InitWeapons();
  void InitTeams();
  void InitSounds();
  void InitData();
  void InitInterface();
  void WaitForOtherPlayers();

  void UnloadDatas(bool game_finished) const;

  // Input management (keyboard/mouse)
  void RefreshInput();
  static void IgnorePendingInputEvents();

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

  bool IsGameLaunched() const { return isGameLaunched; }

  // Menus might be launched while a game is running
  static Menu *menu;

public:
  static Game * GetInstance();
  static std::string GetUniqueId();
  static void ResetUniqueIds() { last_unique_id = 0; }
  uint GetCurrentTurn();
  WeaponsList * GetWeaponsList() const { return weapons_list; }
  void UpdateTranslation();

  Chat                chatsession;

  void DisplayError(const std::string &msg);

  // Set mode
  static Game * UpdateGameRules();

  uint Start(bool bench = false);

  bool IsCharacterAlreadyChosen() const { return character_already_chosen; }
  void SetCharacterChosen(bool chosen);

  // Get remaining time to play
  virtual uint GetRemainingTime() const = 0;
  virtual bool IsGameFinished() const = 0;
  virtual void EndOfGame() = 0;

  // Read/Set State
  game_loop_state_t ReadState() const { return state; }
  void SetState(game_loop_state_t new_state, bool begin_game=false);

  void UserAsksForEnd() { ask_for_end = true; }
  void UserAsksForMenu() { ask_for_menu = true; }
  void UserAsksForHelpMenu() { ask_for_help_menu = true; }
  void RequestPause(bool pause) { request_pause = pause ? START_PAUSE : END_PAUSE; }
  void RequestSpeed(const Double& speed) { request_speed = speed; }
  void RequestTime(uint time) { request_time = time; }

  // Signal death of a player
  void SignalCharacterDeath(const Character *character, const Character* killer = NULL);

  // Signal character damage
  void SignalCharacterDamage(const Character *character);

  // Bonus box handling
  bool NewBox();
  void SetCurrentBox(ObjBox * current_box) { current_ObjBox = current_box; };
  ObjBox * GetCurrentBox() { return current_ObjBox; };
  void RequestBonusBoxDrop();

  static Menu* GetCurrentMenu() { return menu; }
  const std::vector< std::pair<float,float> >& GetBenchResults() const { return bench_res; }

  float GetLastFrameRate() const;
};

inline bool GameIsRunning() { return (Game::singleton) ? Game::singleton->IsGameLaunched() : false; }

#endif // GAME_H
