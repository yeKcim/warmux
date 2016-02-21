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
 * Virtual class to handle man/machine interaction
 *****************************************************************************/

#include <SDL_events.h>
#include "interface/man_machine_interface.h"
#include "interface/interface.h"
#include "character/character.h"
#include "ai/ai_engine_stupid.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "menu/options_menu.h"
#include "network/chat.h"
#include "network/network.h"
#include "object/objbox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "sound/jukebox.h"
#include "weapon/weapons_list.h"

#define SCROLL_KEYBOARD 20 // pixel

void ManMachineInterface::Reset()
{
  for (int i = 0; i != 256; i++)
    PressedKeys[i] = false ;
}

bool ManMachineInterface::IsRegistredEvent(uint8 event_type)
{
  std::list<uint8>::iterator it;
  for(it = registred_event.begin(); it != registred_event.end(); it ++) {
    if(event_type == (*it))
      return true;
  }
  return false;
}

// Get the key associated to an action.
int ManMachineInterface::GetKeyAssociatedToAction(Key_t at) const
{
  std::map<int, Key_t>::const_iterator it;
  for (it= layout.begin(); it != layout.end(); it++) {
    if (it->second == at) {
      return it->first;
    }
  }
  return 0;
}

bool ManMachineInterface::MoveCamera(const Key_t &key) const
{
  bool r = true;

  switch(key) {
  case KEY_MOVE_RIGHT:
    Camera::GetInstance()->SetXY(Point2i(SCROLL_KEYBOARD, 0));
    break;
  case KEY_MOVE_LEFT:
    Camera::GetInstance()->SetXY(Point2i(-SCROLL_KEYBOARD, 0));
    break;
  case KEY_UP:
    Camera::GetInstance()->SetXY(Point2i(0, -SCROLL_KEYBOARD));
    break;
  case KEY_DOWN:
    Camera::GetInstance()->SetXY(Point2i(0, SCROLL_KEYBOARD));
    break;
  default:
    r = false;
    break;
  }

  if (r)
    Camera::GetInstance()->SetAutoCrop(false);

  return r;
}

// Handle a pressed key
void ManMachineInterface::HandleKeyPressed(const Key_t &key)
{
  SDLMod mod = SDL_GetModState();
  if (mod & KMOD_CTRL) {
    if (MoveCamera(key)) {
      PressedKeys[key] = true;
      return;
    }
  }

  // Managing keys related to character moves
  // Available only when local
  if (!ActiveTeam().IsLocal()) return;
  if (Game::GetInstance()->ReadState() == Game::END_TURN) return;
  if (ActiveCharacter().IsDead()) return;

  bool shift = !!(SDL_GetModState() & KMOD_SHIFT);
  if (Game::GetInstance()->ReadState() == Game::HAS_PLAYED) {
    switch (key) {

    case KEY_MOVE_RIGHT:
      ActiveCharacter().HandleKeyPressed_MoveRight(shift);
      break;
    case KEY_MOVE_LEFT:
      ActiveCharacter().HandleKeyPressed_MoveLeft(shift);
      break;
    case KEY_UP:
      ActiveCharacter().HandleKeyPressed_Up(shift);
      break;
    case KEY_DOWN:
      ActiveCharacter().HandleKeyPressed_Down(shift);
      break;
    case KEY_JUMP:
      ActiveCharacter().HandleKeyPressed_Jump(shift);
      break;
    case KEY_HIGH_JUMP:
      ActiveCharacter().HandleKeyPressed_HighJump(shift);
      break;
    case KEY_BACK_JUMP:
      ActiveCharacter().HandleKeyPressed_BackJump(shift);
      break;
    case KEY_SHOOT:
      // Shoot key is not accepted in HAS_PLAYED state
      return;
    default:
      // key not supported
      return;
    }
  } else if (Game::GetInstance()->ReadState() == Game::PLAYING) {

    // Movements are managed by weapons because sometimes it overrides the keys
    switch (key) {

    case KEY_MOVE_RIGHT:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveRight(shift);
      break;
    case KEY_MOVE_LEFT:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveLeft(shift);
      break;
    case KEY_UP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Up(shift);
      break;
    case KEY_DOWN:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Down(shift);
      break;
    case KEY_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Jump(shift);
      break;
    case KEY_HIGH_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_HighJump(shift);
      break;
    case KEY_BACK_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_BackJump(shift);
      break;
    case KEY_SHOOT:
      if (Game::GetInstance()->ReadState() == Game::PLAYING) {
        ActiveTeam().AccessWeapon().HandleKeyPressed_Shoot(shift);
        break;
      }
    default:
      // key not supported
      return;
    }
  }

  PressedKeys[key] = true ;
}

// Handle a released key
void ManMachineInterface::HandleKeyReleased(const Key_t &key)
{
  PressedKeys[key] = false;
  // Here we manage only actions which are activated on KEY_RELEASED event.

  // hack to interrupt AI
  if (ActiveTeam().IsLocalAI() && key == KEY_SHOOT)
  {
    AIStupidEngine::GetInstance()->ForceEndOfTurn();
  }

  { // Managing keys related to interface (no game interaction)
    // Always available
    switch(key){
      // Managing interface
    case KEY_QUIT:
      Game::GetInstance()->UserWantEndOfGame();
      return;
    case KEY_PAUSE:
      if (!Network::IsConnected())
        Time::GetInstance()->TogglePause();
      return;
    case KEY_FULLSCREEN:
      AppWormux::GetInstance()->video->ToggleFullscreen();
      return;
    case KEY_CHAT:
      if(Network::IsConnected())
        Game::GetInstance()->chatsession.ShowInput();
      return;
    case KEY_CENTER:
      Camera::GetInstance()->CenterOnActiveCharacter();
      return;
    case KEY_TOGGLE_INTERFACE:
      Interface::GetInstance()->EnableDisplay (!Interface::GetInstance()->IsDisplayed());
      return;
    case KEY_MENU_OPTIONS_FROM_GAME: {
      OptionMenu options_menu;
      options_menu.Run();
      return;
    }
    default:
      break;
    }
  }

  bool shift = !!(SDL_GetModState() & KMOD_SHIFT);

  // Managing shoot key
  // Drop bonus box or medkit when outside a turn
  // Shoot when in turn
  if (key == KEY_SHOOT) {

    if (Game::GetInstance()->ReadState() == Game::END_TURN) {
      ObjBox* current_box = Game::GetInstance()->GetCurrentBox();
      if (current_box != NULL) {
        Action * a = new Action(Action::ACTION_DROP_BONUS_BOX);
        current_box->StoreValue(a);
        ActionHandler::GetInstance()->NewAction(a);
      }
    } else if (Game::GetInstance()->ReadState() == Game::PLAYING &&
               ActiveTeam().IsLocal() &&
               !ActiveCharacter().IsDead()) {
      ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot(shift);
    }
    return;
  }

  { // Managing keys related to character moves
    // Available only when local
    if (!ActiveTeam().IsLocal()) return;
    if (ActiveCharacter().IsDead()) return;
    if (Game::GetInstance()->ReadState() == Game::END_TURN) return;

    if (Game::GetInstance()->ReadState() == Game::HAS_PLAYED) {
      switch (key) {
      case KEY_MOVE_RIGHT:
        ActiveCharacter().HandleKeyReleased_MoveRight(shift);
        return;
      case KEY_MOVE_LEFT:
        ActiveCharacter().HandleKeyReleased_MoveLeft(shift);
        return;
      case KEY_UP:
        ActiveCharacter().HandleKeyReleased_Up(shift);
        return;
      case KEY_DOWN:
        ActiveCharacter().HandleKeyReleased_Down(shift);
        return;
      case KEY_JUMP:
        ActiveCharacter().HandleKeyReleased_Jump(shift);
        return;
      case KEY_HIGH_JUMP:
        ActiveCharacter().HandleKeyReleased_HighJump(shift);
        return;
      case KEY_BACK_JUMP:
        ActiveCharacter().HandleKeyReleased_BackJump(shift);
        return;
      case KEY_SHOOT:
      // Shoot key is not accepted in HAS_PLAYED state
        return;
      default:
        // Key not supported
        return;
      }
    } else if  (Game::GetInstance()->ReadState() == Game::PLAYING) {

      // Movements are managed by weapons because sometimes it overrides the keys
      switch (key) {

      case KEY_MOVE_RIGHT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveRight(shift);
        return;
      case KEY_MOVE_LEFT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveLeft(shift);
        return;
      case KEY_UP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Up(shift);
        return;
      case KEY_DOWN:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Down(shift);
        return;
      case KEY_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Jump(shift);
        return;
      case KEY_HIGH_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_HighJump(shift);
        return;
      case KEY_BACK_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_BackJump(shift);
        return;

        // Shoot key
      case KEY_SHOOT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot(shift);
        return;

        // Other keys usefull for weapons
      case KEY_WEAPON_1:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num1(shift);
        return;
      case KEY_WEAPON_2:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num2(shift);
        return;
      case KEY_WEAPON_3:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num3(shift);
        return;
      case KEY_WEAPON_4:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num4(shift);
        return;
      case KEY_WEAPON_5:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num5(shift);
        return;
      case KEY_WEAPON_6:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num6(shift);
        return;
      case KEY_WEAPON_7:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num7(shift);
        return;
      case KEY_WEAPON_8:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num8(shift);
        return;
      case KEY_WEAPON_9:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num9(shift);
        return;
      case KEY_WEAPON_LESS:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Less(shift);
        return;
      case KEY_WEAPON_MORE:
        ActiveTeam().AccessWeapon().HandleKeyReleased_More(shift);
        return;
      default:
        break;
      }
    }
  }

  { // Managing keys related to change of character or weapon

    if (Game::GetInstance()->ReadState() != Game::PLAYING ||
        !ActiveTeam().GetWeapon().CanChangeWeapon())
      return;

    Weapon::category_t weapon_sort = Weapon::INVALID;

    switch(key) {

    case KEY_NEXT_CHARACTER:
      {
        if (GameMode::GetInstance()->AllowCharacterSelection()) {
          SDLMod mod = SDL_GetModState();
          if (mod & KMOD_CTRL) {
            Action * previous_character = new Action(Action::ACTION_PLAYER_PREVIOUS_CHARACTER);
            previous_character->StoreActiveCharacter();
            ActiveTeam().PreviousCharacter();
            previous_character->StoreActiveCharacter();
            ActionHandler::GetInstance()->NewAction(previous_character);
          } else {
            Action * next_character = new Action(Action::ACTION_PLAYER_NEXT_CHARACTER);
            next_character->StoreActiveCharacter();
            ActiveTeam().NextCharacter();
            next_character->StoreActiveCharacter();
            ActionHandler::GetInstance()->NewAction(next_character);
          }
        }
      }
      return;

    case KEY_WEAPONS1:
      weapon_sort = Weapon::HEAVY;
      break;
    case KEY_WEAPONS2:
      weapon_sort = Weapon::RIFLE;
      break;
    case KEY_WEAPONS3:
      weapon_sort = Weapon::THROW;
      break;
    case KEY_WEAPONS4:
      weapon_sort = Weapon::SPECIAL;
      break;
    case KEY_WEAPONS5:
      weapon_sort = Weapon::DUEL;
      break;
    case KEY_WEAPONS6:
      weapon_sort = Weapon::MOVE;
      break;
    case KEY_WEAPONS7:
      weapon_sort = Weapon::TOOL;
      break;
    default:
      // Key not supported
      return;
    }

    if ( weapon_sort != Weapon::INVALID ) {
      Weapon::Weapon_type weapon;
      if (WeaponsList::GetInstance()->GetWeaponBySort(weapon_sort, weapon))
        {
          ASSERT (weapon >= Weapon::WEAPON_FIRST && weapon <= Weapon::WEAPON_LAST);
          ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PLAYER_CHANGE_WEAPON, weapon));
        }
    }
  }
}

// Refresh keys which are still pressed.
void ManMachineInterface::Refresh() const
{
  //Treat KEY_REFRESH events:
  for (int i = 0; i < 256; i++) {

    if (PressedKeys[i]) {
      Key_t key = static_cast<Key_t>(i);

      if (SDL_GetModState()&KMOD_CTRL && MoveCamera(key))
        continue;

      // Managing keys related to character moves
      // Available only when local
      if (!ActiveTeam().IsLocal()) return;
      if (ActiveCharacter().IsDead()) return;
      if (Game::GetInstance()->ReadState() == Game::END_TURN) return;

      // Movements are managed by weapons because sometimes it overrides the keys
      bool shift = !!(SDL_GetModState() & KMOD_SHIFT);
      switch (key) {

      case KEY_MOVE_RIGHT:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_MoveRight(shift);
        break;
      case KEY_MOVE_LEFT:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_MoveLeft(shift);
        break;
      case KEY_UP:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_Up(shift);
        break;
      case KEY_DOWN:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_Down(shift);
        break;
      case KEY_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_Jump(shift);
        break;
      case KEY_HIGH_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_HighJump(shift);
        break;
      case KEY_BACK_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyRefreshed_BackJump(shift);
        break;
      case KEY_SHOOT:
        if (Game::GetInstance()->ReadState() == Game::PLAYING) {
          ActiveTeam().AccessWeapon().HandleKeyRefreshed_Shoot(shift);
        }
        break;
      default:
        break;
      }
    }
  }
}
