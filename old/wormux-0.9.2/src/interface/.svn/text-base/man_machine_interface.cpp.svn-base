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
 * Virtual class to handle man/machine interaction
 *****************************************************************************/

#include <SDL_events.h>
#include "interface/man_machine_interface.h"
#include "interface/interface.h"
#include "character/character.h"
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
  std::map<int, std::vector<Key_t> >::const_iterator it;
  std::vector<Key_t>::const_iterator itv;
  for (it = layout.begin(); it != layout.end(); it++) {
    std::vector<Key_t> key_list = it->second;
    for (itv = key_list.begin(); itv != key_list.end(); itv++)
    {
      if ( *itv == at)
      {
        return it->first;
      }
    }
  }
  return 0;
}

// Handle a pressed key
void ManMachineInterface::HandleKeyPressed(const Key_t &key)
{
  // Key repeat is useful in the menu, but we are handling it manually
  // during the game
  if (PressedKeys[key]) {
    SDL_EnableKeyRepeat(0,0);
    return;
  }

  switch (key) {
    case KEY_MOVE_CAMERA_RIGHT:
      Camera::GetInstance()->AddLRMoveIntention(INTENTION_MOVE_RIGHT);
      break;
    case KEY_MOVE_CAMERA_LEFT:
      Camera::GetInstance()->AddLRMoveIntention(INTENTION_MOVE_LEFT);
      break;
    case KEY_MOVE_CAMERA_UP:
      Camera::GetInstance()->AddUDMoveIntention(INTENTION_MOVE_UP);
      break;
    case KEY_MOVE_CAMERA_DOWN:
      Camera::GetInstance()->AddUDMoveIntention(INTENTION_MOVE_DOWN);
      break;
    default:
      break;
  }

  // Managing keys related to character moves
  // Available only when local
  if (!ActiveTeam().IsLocalHuman()) return;
  if (Game::GetInstance()->ReadState() == Game::END_TURN) return;
  if (ActiveCharacter().IsDead()) return;

  switch (key) {
    case KEY_MOVE_RIGHT:
      ActiveCharacter().HandleKeyPressed_MoveRight(false);
      break;
    case KEY_MOVE_RIGHT_SLOWLY:
      ActiveCharacter().HandleKeyPressed_MoveRight(true);
      break;
    case KEY_MOVE_LEFT:
      ActiveCharacter().HandleKeyPressed_MoveLeft(false);
      break;
    case KEY_MOVE_LEFT_SLOWLY:
      ActiveCharacter().HandleKeyPressed_MoveLeft(true);
      break;
    case KEY_UP:
      ActiveCharacter().HandleKeyPressed_Up(false);
      break;
    case KEY_UP_SLOWLY:
      ActiveCharacter().HandleKeyPressed_Up(true);
      break;
    case KEY_DOWN:
      ActiveCharacter().HandleKeyPressed_Down(false);
      break;
    case KEY_DOWN_SLOWLY:
      ActiveCharacter().HandleKeyPressed_Down(true);
      break;
    case KEY_JUMP:
      ActiveCharacter().HandleKeyPressed_Jump();
      break;
    case KEY_HIGH_JUMP:
      ActiveCharacter().HandleKeyPressed_HighJump();
      break;
    case KEY_BACK_JUMP:
      ActiveCharacter().HandleKeyPressed_BackJump();
      break;
    case KEY_SHOOT:
      // Shoot key is not accepted in HAS_PLAYED state
      if (Game::GetInstance()->ReadState() == Game::HAS_PLAYED)
        return;
      break;
    default:
      // key not supported
      return;
  }
  if (Game::GetInstance()->ReadState() == Game::PLAYING) {
    switch (key) {
    case KEY_MOVE_RIGHT:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveRight(false);
      break;
    case KEY_MOVE_RIGHT_SLOWLY:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveRight(true);
      break;
    case KEY_MOVE_LEFT:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveLeft(false);
      break;
    case KEY_MOVE_LEFT_SLOWLY:
      ActiveTeam().AccessWeapon().HandleKeyPressed_MoveLeft(true);
      break;
    case KEY_UP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Up(false);
      break;
    case KEY_UP_SLOWLY:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Up(true);
      break;
    case KEY_DOWN:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Down(false);
      break;
    case KEY_DOWN_SLOWLY:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Down(true);
      break;
    case KEY_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Jump();
      break;
    case KEY_HIGH_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_HighJump();
      break;
    case KEY_BACK_JUMP:
      ActiveTeam().AccessWeapon().HandleKeyPressed_BackJump();
      break;
    case KEY_SHOOT:
      ActiveTeam().AccessWeapon().HandleKeyPressed_Shoot();
      break;
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

  { // Managing keys related to interface (no game interaction)
    // Always available
    switch(key){
      // Managing interface
    case KEY_QUIT:
    case KEY_PAUSE:
      Game::GetInstance()->UserAsksForMenu();
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
    case KEY_MINIMAP_FROM_GAME:
      Interface::GetInstance()->ToggleMinimap ();
      return;
    case KEY_MENU_OPTIONS_FROM_GAME: {
      OptionMenu options_menu;
      options_menu.Run();
      return;
    }
    case KEY_MOVE_CAMERA_RIGHT:
      Camera::GetInstance()->RemoveLRMoveIntention(INTENTION_MOVE_RIGHT);
      return;
    case KEY_MOVE_CAMERA_LEFT:
      Camera::GetInstance()->RemoveLRMoveIntention(INTENTION_MOVE_LEFT);
      return;
    case KEY_MOVE_CAMERA_UP:
      Camera::GetInstance()->RemoveUDMoveIntention(INTENTION_MOVE_UP);
      return;
    case KEY_MOVE_CAMERA_DOWN:
      Camera::GetInstance()->RemoveUDMoveIntention(INTENTION_MOVE_DOWN);
      return;
    default:
      break;
    }
  }

  // Managing shoot key
  // Drop bonus box or medkit when outside a turn
  // Shoot when in turn
  if (key == KEY_SHOOT) {

    if (Game::GetInstance()->ReadState() == Game::END_TURN) {
      Game::GetInstance()->RequestBonusBoxDrop();
    } else if (Game::GetInstance()->ReadState() == Game::PLAYING &&
               ActiveTeam().IsLocalHuman() &&
               !ActiveCharacter().IsDead()) {
      ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot();
    }
    return;
  }

  { // Managing keys related to character moves
    // Available only when local
    if (!ActiveTeam().IsLocalHuman()) return;
    if (ActiveCharacter().IsDead()) return;
    if (Game::GetInstance()->ReadState() == Game::END_TURN) return;


    switch (key) {
      case KEY_MOVE_RIGHT:
        ActiveCharacter().HandleKeyReleased_MoveRight(false);
        break;
      case KEY_MOVE_RIGHT_SLOWLY:
        ActiveCharacter().HandleKeyReleased_MoveRight(true);
        break;
      case KEY_MOVE_LEFT:
        ActiveCharacter().HandleKeyReleased_MoveLeft(false);
        break;
      case KEY_MOVE_LEFT_SLOWLY:
        ActiveCharacter().HandleKeyReleased_MoveLeft(true);
        break;
      case KEY_UP:
        ActiveCharacter().HandleKeyReleased_Up(false);
        break;
      case KEY_UP_SLOWLY:
        ActiveCharacter().HandleKeyReleased_Up(true);
        break;
      case KEY_DOWN:
        ActiveCharacter().HandleKeyReleased_Down(false);
        break;
      case KEY_DOWN_SLOWLY:
        ActiveCharacter().HandleKeyReleased_Down(true);
        break;
      case KEY_JUMP:
        ActiveCharacter().HandleKeyReleased_Jump();
        break;
      case KEY_HIGH_JUMP:
        ActiveCharacter().HandleKeyReleased_HighJump();
        break;
      case KEY_BACK_JUMP:
        ActiveCharacter().HandleKeyReleased_BackJump();
        break;
      default:
        if (Game::GetInstance()->ReadState() == Game::HAS_PLAYED)
          return;
        break;
    }
    if (Game::GetInstance()->ReadState() == Game::PLAYING) {
      switch (key) {

      case KEY_MOVE_RIGHT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveRight(false);
        return;
      case KEY_MOVE_RIGHT_SLOWLY:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveRight(true);
        return;
      case KEY_MOVE_LEFT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveLeft(false);
        return;
      case KEY_MOVE_LEFT_SLOWLY:
        ActiveTeam().AccessWeapon().HandleKeyReleased_MoveLeft(true);
        return;
      case KEY_UP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Up(false);
        return;
      case KEY_UP_SLOWLY:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Up(true);
        return;
      case KEY_DOWN:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Down(false);
        return;
      case KEY_DOWN_SLOWLY:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Down(true);
        return;
      case KEY_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Jump();
        return;
      case KEY_HIGH_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_HighJump();
        return;
      case KEY_BACK_JUMP:
        ActiveTeam().AccessWeapon().HandleKeyReleased_BackJump();
        return;

        // Shoot key
      case KEY_SHOOT:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Shoot();
        return;

        // Other keys usefull for weapons
      case KEY_WEAPON_1:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num1();
        return;
      case KEY_WEAPON_2:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num2();
        return;
      case KEY_WEAPON_3:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num3();
        return;
      case KEY_WEAPON_4:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num4();
        return;
      case KEY_WEAPON_5:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num5();
        return;
      case KEY_WEAPON_6:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num6();
        return;
      case KEY_WEAPON_7:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num7();
        return;
      case KEY_WEAPON_8:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num8();
        return;
      case KEY_WEAPON_9:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Num9();
        return;
      case KEY_WEAPON_LESS:
        ActiveTeam().AccessWeapon().HandleKeyReleased_Less();
        return;
      case KEY_WEAPON_MORE:
        ActiveTeam().AccessWeapon().HandleKeyReleased_More();
        return;
      default:
        break;
      }
    }
  }

  { // Managing keys related to change of character or weapon

    if (Game::GetInstance()->ReadState() != Game::PLAYING ||
        !ActiveTeam().GetWeapon().CanChangeWeapon() ||
        !ActiveTeam().IsLocalHuman())
      return;

    Weapon::category_t weapon_sort = Weapon::INVALID;

    switch(key) {

    case KEY_NEXT_CHARACTER:
      {
        if (GameMode::GetInstance()->AllowCharacterSelection()) {
          SDLMod mod = SDL_GetModState();
          if (mod & KMOD_CTRL) {
            ActiveTeam().PreviousCharacter();
          } else {
            ActiveTeam().NextCharacter();
          }
          Action * next_character = new Action(Action::ACTION_PLAYER_CHANGE_CHARACTER);
          uint next_character_index = ActiveCharacter().GetCharacterIndex();
          next_character->Push((int)next_character_index);
          ActionHandler::GetInstance()->NewAction(next_character);
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
      WeaponsList * weapons_list = Game::GetInstance()->GetWeaponsList();
      if (weapons_list->GetWeaponBySort(weapon_sort, weapon))
        {
          ASSERT (weapon >= Weapon::FIRST && weapon <= Weapon::LAST);
          ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PLAYER_CHANGE_WEAPON, weapon));
        }
    }
  }
}

void ManMachineInterface::ClearKeyAction()
{
  layout.clear();
}

int ManMachineInterface::GetKeyFromKeyName(const std::string &name) const
{
  if(name == "unknown") return SDLK_UNKNOWN;

  // The keyboard syms have been cleverly chosen to map to ASCII
  if(name == "backspace") return SDLK_BACKSPACE;
  if(name == "tab") return SDLK_TAB;
  if(name == "clear") return SDLK_CLEAR;
  if(name == "return") return SDLK_RETURN;
  if(name == "pause") return SDLK_PAUSE;
  if(name == "escape") return SDLK_ESCAPE;
  if(name == "space") return SDLK_SPACE;
  if(name == "exclaim") return SDLK_EXCLAIM;
  if(name == "quotedbl") return SDLK_QUOTEDBL;
  if(name == "hash") return SDLK_HASH;
  if(name == "dollar") return SDLK_DOLLAR;
  if(name == "ampersand") return SDLK_AMPERSAND;
  if(name == "quote") return SDLK_QUOTE;
  if(name == "leftparen") return SDLK_LEFTPAREN;
  if(name == "rightparen") return SDLK_RIGHTPAREN;
  if(name == "asterisk") return SDLK_ASTERISK;
  if(name == "plus") return SDLK_PLUS;
  if(name == "comma") return SDLK_COMMA;
  if(name == "minus") return SDLK_MINUS;
  if(name == "period") return SDLK_PERIOD;
  if(name == "slash") return SDLK_SLASH;
  if(name == "0") return SDLK_0;
  if(name == "1") return SDLK_1;
  if(name == "2") return SDLK_2;
  if(name == "3") return SDLK_3;
  if(name == "4") return SDLK_4;
  if(name == "5") return SDLK_5;
  if(name == "6") return SDLK_6;
  if(name == "7") return SDLK_7;
  if(name == "8") return SDLK_8;
  if(name == "9") return SDLK_9;
  if(name == "colon") return SDLK_COLON;
  if(name == "semicolon") return SDLK_SEMICOLON;
  if(name == "less") return SDLK_LESS;
  if(name == "equals") return SDLK_EQUALS;
  if(name == "greater") return SDLK_GREATER;
  if(name == "question") return SDLK_QUESTION;
  if(name == "at") return SDLK_AT;

  //Skip uppercase letters
  if(name == "leftbracket") return SDLK_LEFTBRACKET;
  if(name == "backslash") return SDLK_BACKSLASH;
  if(name == "rightbracket") return SDLK_RIGHTBRACKET;
  if(name == "caret") return SDLK_CARET;
  if(name == "underscore") return SDLK_UNDERSCORE;
  if(name == "backquote") return SDLK_BACKQUOTE;
  if(name == "a") return SDLK_a;
  if(name == "b") return SDLK_b;
  if(name == "c") return SDLK_c;
  if(name == "d") return SDLK_d;
  if(name == "e") return SDLK_e;
  if(name == "f") return SDLK_f;
  if(name == "g") return SDLK_g;
  if(name == "h") return SDLK_h;
  if(name == "i") return SDLK_i;
  if(name == "j") return SDLK_j;
  if(name == "k") return SDLK_k;
  if(name == "l") return SDLK_l;
  if(name == "m") return SDLK_m;
  if(name == "n") return SDLK_n;
  if(name == "o") return SDLK_o;
  if(name == "p") return SDLK_p;
  if(name == "q") return SDLK_q;
  if(name == "r") return SDLK_r;
  if(name == "s") return SDLK_s;
  if(name == "t") return SDLK_t;
  if(name == "u") return SDLK_u;
  if(name == "v") return SDLK_v;
  if(name == "w") return SDLK_w;
  if(name == "x") return SDLK_x;
  if(name == "y") return SDLK_y;
  if(name == "z") return SDLK_z;
  if(name == "delete") return SDLK_DELETE;
  // End of ASCII mapped keysyms

  // International keyboard syms
  if(name == "world_0") return SDLK_WORLD_0;
  if(name == "world_1") return SDLK_WORLD_1;
  if(name == "world_2") return SDLK_WORLD_2;
  if(name == "world_3") return SDLK_WORLD_3;
  if(name == "world_4") return SDLK_WORLD_4;
  if(name == "world_5") return SDLK_WORLD_5;
  if(name == "world_6") return SDLK_WORLD_6;
  if(name == "world_7") return SDLK_WORLD_7;
  if(name == "world_8") return SDLK_WORLD_8;
  if(name == "world_9") return SDLK_WORLD_9;
  if(name == "world_10") return SDLK_WORLD_10;
  if(name == "world_11") return SDLK_WORLD_11;
  if(name == "world_12") return SDLK_WORLD_12;
  if(name == "world_13") return SDLK_WORLD_13;
  if(name == "world_14") return SDLK_WORLD_14;
  if(name == "world_15") return SDLK_WORLD_15;
  if(name == "world_16") return SDLK_WORLD_16;
  if(name == "world_17") return SDLK_WORLD_17;
  if(name == "world_18") return SDLK_WORLD_18;
  if(name == "world_19") return SDLK_WORLD_19;
  if(name == "world_20") return SDLK_WORLD_20;
  if(name == "world_21") return SDLK_WORLD_21;
  if(name == "world_22") return SDLK_WORLD_22;
  if(name == "world_23") return SDLK_WORLD_23;
  if(name == "world_24") return SDLK_WORLD_24;
  if(name == "world_25") return SDLK_WORLD_25;
  if(name == "world_26") return SDLK_WORLD_26;
  if(name == "world_27") return SDLK_WORLD_27;
  if(name == "world_28") return SDLK_WORLD_28;
  if(name == "world_29") return SDLK_WORLD_29;
  if(name == "world_30") return SDLK_WORLD_30;
  if(name == "world_31") return SDLK_WORLD_31;
  if(name == "world_32") return SDLK_WORLD_32;
  if(name == "world_33") return SDLK_WORLD_33;
  if(name == "world_34") return SDLK_WORLD_34;
  if(name == "world_35") return SDLK_WORLD_35;
  if(name == "world_36") return SDLK_WORLD_36;
  if(name == "world_37") return SDLK_WORLD_37;
  if(name == "world_38") return SDLK_WORLD_38;
  if(name == "world_39") return SDLK_WORLD_39;
  if(name == "world_40") return SDLK_WORLD_40;
  if(name == "world_41") return SDLK_WORLD_41;
  if(name == "world_42") return SDLK_WORLD_42;
  if(name == "world_43") return SDLK_WORLD_43;
  if(name == "world_44") return SDLK_WORLD_44;
  if(name == "world_45") return SDLK_WORLD_45;
  if(name == "world_46") return SDLK_WORLD_46;
  if(name == "world_47") return SDLK_WORLD_47;
  if(name == "world_48") return SDLK_WORLD_48;
  if(name == "world_49") return SDLK_WORLD_49;
  if(name == "world_50") return SDLK_WORLD_50;
  if(name == "world_51") return SDLK_WORLD_51;
  if(name == "world_52") return SDLK_WORLD_52;
  if(name == "world_53") return SDLK_WORLD_53;
  if(name == "world_54") return SDLK_WORLD_54;
  if(name == "world_55") return SDLK_WORLD_55;
  if(name == "world_56") return SDLK_WORLD_56;
  if(name == "world_57") return SDLK_WORLD_57;
  if(name == "world_58") return SDLK_WORLD_58;
  if(name == "world_59") return SDLK_WORLD_59;
  if(name == "world_60") return SDLK_WORLD_60;
  if(name == "world_61") return SDLK_WORLD_61;
  if(name == "world_62") return SDLK_WORLD_62;
  if(name == "world_63") return SDLK_WORLD_63;
  if(name == "world_64") return SDLK_WORLD_64;
  if(name == "world_65") return SDLK_WORLD_65;
  if(name == "world_66") return SDLK_WORLD_66;
  if(name == "world_67") return SDLK_WORLD_67;
  if(name == "world_68") return SDLK_WORLD_68;
  if(name == "world_69") return SDLK_WORLD_69;
  if(name == "world_70") return SDLK_WORLD_70;
  if(name == "world_71") return SDLK_WORLD_71;
  if(name == "world_72") return SDLK_WORLD_72;
  if(name == "world_73") return SDLK_WORLD_73;
  if(name == "world_74") return SDLK_WORLD_74;
  if(name == "world_75") return SDLK_WORLD_75;
  if(name == "world_76") return SDLK_WORLD_76;
  if(name == "world_77") return SDLK_WORLD_77;
  if(name == "world_78") return SDLK_WORLD_78;
  if(name == "world_79") return SDLK_WORLD_79;
  if(name == "world_80") return SDLK_WORLD_80;
  if(name == "world_81") return SDLK_WORLD_81;
  if(name == "world_82") return SDLK_WORLD_82;
  if(name == "world_83") return SDLK_WORLD_83;
  if(name == "world_84") return SDLK_WORLD_84;
  if(name == "world_85") return SDLK_WORLD_85;
  if(name == "world_86") return SDLK_WORLD_86;
  if(name == "world_87") return SDLK_WORLD_87;
  if(name == "world_88") return SDLK_WORLD_88;
  if(name == "world_89") return SDLK_WORLD_89;
  if(name == "world_90") return SDLK_WORLD_90;
  if(name == "world_91") return SDLK_WORLD_91;
  if(name == "world_92") return SDLK_WORLD_92;
  if(name == "world_93") return SDLK_WORLD_93;
  if(name == "world_94") return SDLK_WORLD_94;
  if(name == "world_95") return SDLK_WORLD_95;

  // Numeric keypad
  if(name == "kp0") return SDLK_KP0;
  if(name == "kp1") return SDLK_KP1;
  if(name == "kp2") return SDLK_KP2;
  if(name == "kp3") return SDLK_KP3;
  if(name == "kp4") return SDLK_KP4;
  if(name == "kp5") return SDLK_KP5;
  if(name == "kp6") return SDLK_KP6;
  if(name == "kp7") return SDLK_KP7;
  if(name == "kp8") return SDLK_KP8;
  if(name == "kp9") return SDLK_KP9;
  if(name == "kp_period") return SDLK_KP_PERIOD;
  if(name == "kp_divide") return SDLK_KP_DIVIDE;
  if(name == "kp_multiply") return SDLK_KP_MULTIPLY;
  if(name == "kp_minus") return SDLK_KP_MINUS;
  if(name == "kp_plus") return SDLK_KP_PLUS;
  if(name == "kp_enter") return SDLK_KP_ENTER;
  if(name == "kp_equals") return SDLK_KP_EQUALS;

  // Arrows + Home/End pad
  if(name == "up") return SDLK_UP;
  if(name == "down") return SDLK_DOWN;
  if(name == "right") return SDLK_RIGHT;
  if(name == "left") return SDLK_LEFT;
  if(name == "insert") return SDLK_INSERT;
  if(name == "home") return SDLK_HOME;
  if(name == "end") return SDLK_END;
  if(name == "pageup") return SDLK_PAGEUP;
  if(name == "pagedown") return SDLK_PAGEDOWN;

  // Function keys
  if(name == "f1") return SDLK_F1;
  if(name == "f2") return SDLK_F2;
  if(name == "f3") return SDLK_F3;
  if(name == "f4") return SDLK_F4;
  if(name == "f5") return SDLK_F5;
  if(name == "f6") return SDLK_F6;
  if(name == "f7") return SDLK_F7;
  if(name == "f8") return SDLK_F8;
  if(name == "f9") return SDLK_F9;
  if(name == "f10") return SDLK_F10;
  if(name == "f11") return SDLK_F11;
  if(name == "f12") return SDLK_F12;
  if(name == "f13") return SDLK_F13;
  if(name == "f14") return SDLK_F14;
  if(name == "F15") return SDLK_F15;

  // Key state modifier keys
  if(name == "numlock") return SDLK_NUMLOCK;
  if(name == "capslock") return SDLK_CAPSLOCK;
  if(name == "scrollock") return SDLK_SCROLLOCK;
  if(name == "rshift") return SDLK_RSHIFT;
  if(name == "lshift") return SDLK_LSHIFT;
  if(name == "rctrl") return SDLK_RCTRL;
  if(name == "lctrl") return SDLK_LCTRL;
  if(name == "ralt") return SDLK_RALT;
  if(name == "lalt") return SDLK_LALT;
  if(name == "rmeta") return SDLK_RMETA;
  if(name == "lmeta") return SDLK_LMETA;
  if(name == "lsuper") return SDLK_LSUPER;
  if(name == "rsuper") return SDLK_RSUPER;
  if(name == "mode") return SDLK_MODE;
  if(name == "compose") return SDLK_COMPOSE;

  // Miscellaneous function keys
  if(name == "help") return SDLK_HELP;
  if(name == "print") return SDLK_PRINT;
  if(name == "sysreq") return SDLK_SYSREQ;
  if(name == "break") return SDLK_BREAK;
  if(name == "menu") return SDLK_MENU;
  if(name == "power") return SDLK_POWER;
  if(name == "euro") return SDLK_EURO;
  if(name == "undo") return SDLK_UNDO;

  return SDLK_UNKNOWN;
}

std::string ManMachineInterface::GetKeyNameFromKey(int key) const
{
  if(key == SDLK_UNKNOWN) return "unknown";

  // The keyboard syms have been cleverly chosen to map to ASCII
  if(key == SDLK_BACKSPACE) return "backspace";
  if(key == SDLK_TAB) return "tab";
  if(key == SDLK_CLEAR) return "clear";
  if(key == SDLK_RETURN) return "return";
  if(key == SDLK_PAUSE) return "pause";
  if(key == SDLK_ESCAPE) return "escape";
  if(key == SDLK_SPACE) return "space";
  if(key == SDLK_EXCLAIM) return "exclaim";
  if(key == SDLK_QUOTEDBL) return "quotedbl";
  if(key == SDLK_HASH) return "hash";
  if(key == SDLK_DOLLAR) return "dollar";
  if(key == SDLK_AMPERSAND) return "ampersand";
  if(key == SDLK_QUOTE) return "quote";
  if(key == SDLK_LEFTPAREN) return "leftparen";
  if(key == SDLK_RIGHTPAREN) return "rightparen";
  if(key == SDLK_ASTERISK) return "asterisk";
  if(key == SDLK_PLUS) return "plus";
  if(key == SDLK_COMMA) return "comma";
  if(key == SDLK_MINUS) return "minus";
  if(key == SDLK_PERIOD) return "period";
  if(key == SDLK_SLASH) return "slash";
  if(key == SDLK_0) return "0";
  if(key == SDLK_1) return "1";
  if(key == SDLK_2) return "2";
  if(key == SDLK_3) return "3";
  if(key == SDLK_4) return "4";
  if(key == SDLK_5) return "5";
  if(key == SDLK_6) return "6";
  if(key == SDLK_7) return "7";
  if(key == SDLK_8) return "8";
  if(key == SDLK_9) return "9";
  if(key == SDLK_COLON) return "colon";
  if(key == SDLK_SEMICOLON) return "semicolon";
  if(key == SDLK_LESS) return "less";
  if(key == SDLK_EQUALS) return "equals";
  if(key == SDLK_GREATER) return "greater";
  if(key == SDLK_QUESTION) return "question";
  if(key == SDLK_AT) return "at";

  //Skip uppercase letters
  if(key == SDLK_LEFTBRACKET) return "leftbracket";
  if(key == SDLK_BACKSLASH) return "backslash";
  if(key == SDLK_RIGHTBRACKET) return "rightbracket";
  if(key == SDLK_CARET) return "caret";
  if(key == SDLK_UNDERSCORE) return "underscore";
  if(key == SDLK_BACKQUOTE) return "backquote";
  if(key == SDLK_a) return "a";
  if(key == SDLK_b) return "b";
  if(key == SDLK_c) return "c";
  if(key == SDLK_d) return "d";
  if(key == SDLK_e) return "e";
  if(key == SDLK_f) return "f";
  if(key == SDLK_g) return "g";
  if(key == SDLK_h) return "h";
  if(key == SDLK_i) return "i";
  if(key == SDLK_j) return "j";
  if(key == SDLK_k) return "k";
  if(key == SDLK_l) return "l";
  if(key == SDLK_m) return "m";
  if(key == SDLK_n) return "n";
  if(key == SDLK_o) return "o";
  if(key == SDLK_p) return "p";
  if(key == SDLK_q) return "q";
  if(key == SDLK_r) return "r";
  if(key == SDLK_s) return "s";
  if(key == SDLK_t) return "t";
  if(key == SDLK_u) return "u";
  if(key == SDLK_v) return "v";
  if(key == SDLK_w) return "w";
  if(key == SDLK_x) return "x";
  if(key == SDLK_y) return "y";
  if(key == SDLK_z) return "z";
  if(key == SDLK_DELETE) return "delete";
  // End of ASCII mapped keysyms

  // International keyboard syms
  if(key == SDLK_WORLD_0) return "world_0";
  if(key == SDLK_WORLD_1) return "world_1";
  if(key == SDLK_WORLD_2) return "world_2";
  if(key == SDLK_WORLD_3) return "world_3";
  if(key == SDLK_WORLD_4) return "world_4";
  if(key == SDLK_WORLD_5) return "world_5";
  if(key == SDLK_WORLD_6) return "world_6";
  if(key == SDLK_WORLD_7) return "world_7";
  if(key == SDLK_WORLD_8) return "world_8";
  if(key == SDLK_WORLD_9) return "world_9";
  if(key == SDLK_WORLD_10) return "world_10";
  if(key == SDLK_WORLD_11) return "world_11";
  if(key == SDLK_WORLD_12) return "world_12";
  if(key == SDLK_WORLD_13) return "world_13";
  if(key == SDLK_WORLD_14) return "world_14";
  if(key == SDLK_WORLD_15) return "world_15";
  if(key == SDLK_WORLD_16) return "world_16";
  if(key == SDLK_WORLD_17) return "world_17";
  if(key == SDLK_WORLD_18) return "world_18";
  if(key == SDLK_WORLD_19) return "world_19";
  if(key == SDLK_WORLD_20) return "world_20";
  if(key == SDLK_WORLD_21) return "world_21";
  if(key == SDLK_WORLD_22) return "world_22";
  if(key == SDLK_WORLD_23) return "world_23";
  if(key == SDLK_WORLD_24) return "world_24";
  if(key == SDLK_WORLD_25) return "world_25";
  if(key == SDLK_WORLD_26) return "world_26";
  if(key == SDLK_WORLD_27) return "world_27";
  if(key == SDLK_WORLD_28) return "world_28";
  if(key == SDLK_WORLD_29) return "world_29";
  if(key == SDLK_WORLD_30) return "world_30";
  if(key == SDLK_WORLD_31) return "world_31";
  if(key == SDLK_WORLD_32) return "world_32";
  if(key == SDLK_WORLD_33) return "world_33";
  if(key == SDLK_WORLD_34) return "world_34";
  if(key == SDLK_WORLD_35) return "world_35";
  if(key == SDLK_WORLD_36) return "world_36";
  if(key == SDLK_WORLD_37) return "world_37";
  if(key == SDLK_WORLD_38) return "world_38";
  if(key == SDLK_WORLD_39) return "world_39";
  if(key == SDLK_WORLD_40) return "world_40";
  if(key == SDLK_WORLD_41) return "world_41";
  if(key == SDLK_WORLD_42) return "world_42";
  if(key == SDLK_WORLD_43) return "world_43";
  if(key == SDLK_WORLD_44) return "world_44";
  if(key == SDLK_WORLD_45) return "world_45";
  if(key == SDLK_WORLD_46) return "world_46";
  if(key == SDLK_WORLD_47) return "world_47";
  if(key == SDLK_WORLD_48) return "world_48";
  if(key == SDLK_WORLD_49) return "world_49";
  if(key == SDLK_WORLD_50) return "world_50";
  if(key == SDLK_WORLD_51) return "world_51";
  if(key == SDLK_WORLD_52) return "world_52";
  if(key == SDLK_WORLD_53) return "world_53";
  if(key == SDLK_WORLD_54) return "world_54";
  if(key == SDLK_WORLD_55) return "world_55";
  if(key == SDLK_WORLD_56) return "world_56";
  if(key == SDLK_WORLD_57) return "world_57";
  if(key == SDLK_WORLD_58) return "world_58";
  if(key == SDLK_WORLD_59) return "world_59";
  if(key == SDLK_WORLD_60) return "world_60";
  if(key == SDLK_WORLD_61) return "world_61";
  if(key == SDLK_WORLD_62) return "world_62";
  if(key == SDLK_WORLD_63) return "world_63";
  if(key == SDLK_WORLD_64) return "world_64";
  if(key == SDLK_WORLD_65) return "world_65";
  if(key == SDLK_WORLD_66) return "world_66";
  if(key == SDLK_WORLD_67) return "world_67";
  if(key == SDLK_WORLD_68) return "world_68";
  if(key == SDLK_WORLD_69) return "world_69";
  if(key == SDLK_WORLD_70) return "world_70";
  if(key == SDLK_WORLD_71) return "world_71";
  if(key == SDLK_WORLD_72) return "world_72";
  if(key == SDLK_WORLD_73) return "world_73";
  if(key == SDLK_WORLD_74) return "world_74";
  if(key == SDLK_WORLD_75) return "world_75";
  if(key == SDLK_WORLD_76) return "world_76";
  if(key == SDLK_WORLD_77) return "world_77";
  if(key == SDLK_WORLD_78) return "world_78";
  if(key == SDLK_WORLD_79) return "world_79";
  if(key == SDLK_WORLD_80) return "world_80";
  if(key == SDLK_WORLD_81) return "world_81";
  if(key == SDLK_WORLD_82) return "world_82";
  if(key == SDLK_WORLD_83) return "world_83";
  if(key == SDLK_WORLD_84) return "world_84";
  if(key == SDLK_WORLD_85) return "world_85";
  if(key == SDLK_WORLD_86) return "world_86";
  if(key == SDLK_WORLD_87) return "world_87";
  if(key == SDLK_WORLD_88) return "world_88";
  if(key == SDLK_WORLD_89) return "world_89";
  if(key == SDLK_WORLD_90) return "world_90";
  if(key == SDLK_WORLD_91) return "world_91";
  if(key == SDLK_WORLD_92) return "world_92";
  if(key == SDLK_WORLD_93) return "world_93";
  if(key == SDLK_WORLD_94) return "world_94";
  if(key == SDLK_WORLD_95) return "world_95";

  // Numeric keypad
  if(key == SDLK_KP0) return "kp0";
  if(key == SDLK_KP1) return "kp1";
  if(key == SDLK_KP2) return "kp2";
  if(key == SDLK_KP3) return "kp3";
  if(key == SDLK_KP4) return "kp4";
  if(key == SDLK_KP5) return "kp5";
  if(key == SDLK_KP6) return "kp6";
  if(key == SDLK_KP7) return "kp7";
  if(key == SDLK_KP8) return "kp8";
  if(key == SDLK_KP9) return "kp9";
  if(key == SDLK_KP_PERIOD) return "kp_period";
  if(key == SDLK_KP_DIVIDE) return "kp_divide";
  if(key == SDLK_KP_MULTIPLY) return "kp_multiply";
  if(key == SDLK_KP_MINUS) return "kp_minus";
  if(key == SDLK_KP_PLUS) return "kp_plus";
  if(key == SDLK_KP_ENTER) return "kp_enter";
  if(key == SDLK_KP_EQUALS) return "kp_equals";

  // Arrows + Home/End pad
  if(key == SDLK_UP) return "up";
  if(key == SDLK_DOWN) return "down";
  if(key == SDLK_RIGHT) return "right";
  if(key == SDLK_LEFT) return "left";
  if(key == SDLK_INSERT) return "insert";
  if(key == SDLK_HOME) return "home";
  if(key == SDLK_END) return "end";
  if(key == SDLK_PAGEUP) return "pageup";
  if(key == SDLK_PAGEDOWN) return "pagedown";

  // Function keys
  if(key == SDLK_F1) return "f1";
  if(key == SDLK_F2) return "f2";
  if(key == SDLK_F3) return "f3";
  if(key == SDLK_F4) return "f4";
  if(key == SDLK_F5) return "f5";
  if(key == SDLK_F6) return "f6";
  if(key == SDLK_F7) return "f7";
  if(key == SDLK_F8) return "f8";
  if(key == SDLK_F9) return "f9";
  if(key == SDLK_F10) return "f10";
  if(key == SDLK_F11) return "f11";
  if(key == SDLK_F12) return "f12";
  if(key == SDLK_F13) return "f13";
  if(key == SDLK_F14) return "f14";
  if(key == SDLK_F15) return "f15";

  // Key state modifier keys
  if(key == SDLK_NUMLOCK) return "numlock";
  if(key == SDLK_CAPSLOCK) return "capslock";
  if(key == SDLK_SCROLLOCK) return "scrollock";
  if(key == SDLK_RSHIFT) return "rshift";
  if(key == SDLK_LSHIFT) return "lshift";
  if(key == SDLK_RCTRL) return "rctrl";
  if(key == SDLK_LCTRL) return "lctrl";
  if(key == SDLK_RALT) return "ralt";
  if(key == SDLK_LALT) return "lalt";
  if(key == SDLK_RMETA) return "rmeta";
  if(key == SDLK_LMETA) return "lmeta";
  if(key == SDLK_LSUPER) return "lsuper";
  if(key == SDLK_RSUPER) return "rsuper";
  if(key == SDLK_MODE) return "mode";
  if(key == SDLK_COMPOSE) return "compose";

  // Miscellaneous function keys
  if(key == SDLK_HELP) return "help";
  if(key == SDLK_PRINT) return "print";
  if(key == SDLK_SYSREQ) return "sysreq";
  if(key == SDLK_BREAK) return "break";
  if(key == SDLK_MENU) return "menu";
  if(key == SDLK_POWER) return "power";
  if(key == SDLK_EURO) return "euro";
  if(key == SDLK_UNDO) return "undo";

  return "unknown";
}

ManMachineInterface::Key_t ManMachineInterface::GetActionFromActionName(const std::string &name) const
{

  if(name == "quit") return KEY_QUIT;
  if(name == "weapons1") return KEY_WEAPONS1;
  if(name == "weapons2") return KEY_WEAPONS2;
  if(name == "weapons3") return KEY_WEAPONS3;
  if(name == "weapons4") return KEY_WEAPONS4;
  if(name == "weapons5") return KEY_WEAPONS5;
  if(name == "weapons6") return KEY_WEAPONS6;
  if(name == "weapons7") return KEY_WEAPONS7;
  if(name == "weapons8") return KEY_WEAPONS8;
  if(name == "PAUSE") return KEY_PAUSE;
  if(name == "fullscreen") return KEY_FULLSCREEN;
  if(name == "toggle_interface") return KEY_TOGGLE_INTERFACE;
  if(name == "center") return KEY_CENTER;
  if(name == "toggle_weapons_menus") return KEY_TOGGLE_WEAPONS_MENUS;
  if(name == "chat") return KEY_CHAT;
  if(name == "move_left") return KEY_MOVE_LEFT;
  if(name == "move_left_slowly") return KEY_MOVE_LEFT_SLOWLY;
  if(name == "move_right") return KEY_MOVE_RIGHT;
  if(name == "move_right_slowly") return KEY_MOVE_RIGHT_SLOWLY;
  if(name == "up") return KEY_UP;
  if(name == "up_slowly") return KEY_UP_SLOWLY;
  if(name == "down") return KEY_DOWN;
  if(name == "down_slowly") return KEY_DOWN_SLOWLY;
  if(name == "move_camera_left") return KEY_MOVE_CAMERA_LEFT;
  if(name == "move_camera_right") return KEY_MOVE_CAMERA_RIGHT;
  if(name == "move_camera_up") return KEY_MOVE_CAMERA_UP;
  if(name == "move_camera_down") return KEY_MOVE_CAMERA_DOWN;
  if(name == "jump") return KEY_JUMP;
  if(name == "high_jump") return KEY_HIGH_JUMP;
  if(name == "back_jump") return KEY_BACK_JUMP;
  if(name == "shoot") return KEY_SHOOT;
  if(name == "change_weapon") return KEY_CHANGE_WEAPON;
  if(name == "weapon_1") return KEY_WEAPON_1;
  if(name == "weapon_2") return KEY_WEAPON_2;
  if(name == "weapon_3") return KEY_WEAPON_3;
  if(name == "weapon_4") return KEY_WEAPON_4;
  if(name == "weapon_5") return KEY_WEAPON_5;
  if(name == "weapon_6") return KEY_WEAPON_6;
  if(name == "weapon_7") return KEY_WEAPON_7;
  if(name == "weapon_8") return KEY_WEAPON_8;
  if(name == "weapon_9") return KEY_WEAPON_9;
  if(name == "weapon_less") return KEY_WEAPON_LESS;
  if(name == "weapon_more") return KEY_WEAPON_MORE;
  if(name == "next_character") return KEY_NEXT_CHARACTER;
  if(name == "menu_options_from_game") return KEY_MENU_OPTIONS_FROM_GAME;
  if(name == "minimap_from_game") return KEY_MINIMAP_FROM_GAME;

  return KEY_NONE;

}

std::string ManMachineInterface::GetActionNameFromAction(ManMachineInterface::Key_t key) const
{
  if(key == KEY_QUIT) return "quit";
  if(key == KEY_WEAPONS1) return "weapons1";
  if(key == KEY_WEAPONS2) return "weapons2";
  if(key == KEY_WEAPONS3) return "weapons3";
  if(key == KEY_WEAPONS4) return "weapons4";
  if(key == KEY_WEAPONS5) return "weapons5";
  if(key == KEY_WEAPONS6) return "weapons6";
  if(key == KEY_WEAPONS7) return "weapons7";
  if(key == KEY_WEAPONS8) return "weapons8";
  if(key == KEY_PAUSE) return "pause";
  if(key == KEY_FULLSCREEN) return "fullscreen";
  if(key == KEY_TOGGLE_INTERFACE) return "toggle_interface";
  if(key == KEY_CENTER) return "center";
  if(key == KEY_TOGGLE_WEAPONS_MENUS) return "toggle_weapons_menus";
  if(key == KEY_CHAT) return "chat";
  if(key == KEY_MOVE_LEFT) return "move_left";
  if(key == KEY_MOVE_LEFT_SLOWLY) return "move_left_slowly";
  if(key == KEY_MOVE_RIGHT) return "move_right";
  if(key == KEY_MOVE_RIGHT_SLOWLY) return "move_right_slowly";
  if(key == KEY_UP) return "up";
  if(key == KEY_UP_SLOWLY) return "up_slowly";
  if(key == KEY_DOWN) return "down";
  if(key == KEY_DOWN_SLOWLY) return "down_slowly";
  if(key == KEY_MOVE_CAMERA_LEFT) return "move_camera_left";
  if(key == KEY_MOVE_CAMERA_RIGHT) return "move_camera_right";
  if(key == KEY_MOVE_CAMERA_UP) return "move_camera_up";
  if(key == KEY_MOVE_CAMERA_DOWN) return "move_camera_down";
  if(key == KEY_JUMP) return "jump";
  if(key == KEY_HIGH_JUMP) return "high_jump";
  if(key == KEY_BACK_JUMP) return "back_jump";
  if(key == KEY_SHOOT) return "shoot";
  if(key == KEY_CHANGE_WEAPON) return "change_weapon";
  if(key == KEY_WEAPON_1) return "weapon_1";
  if(key == KEY_WEAPON_2) return "weapon_2";
  if(key == KEY_WEAPON_3) return "weapon_3";
  if(key == KEY_WEAPON_4) return "weapon_4";
  if(key == KEY_WEAPON_5) return "weapon_5";
  if(key == KEY_WEAPON_6) return "weapon_6";
  if(key == KEY_WEAPON_7) return "weapon_7";
  if(key == KEY_WEAPON_8) return "weapon_8";
  if(key == KEY_WEAPON_9) return "weapon_9";
  if(key == KEY_WEAPON_LESS) return "weapon_less";
  if(key == KEY_WEAPON_MORE) return "weapon_more";
  if(key == KEY_NEXT_CHARACTER) return "next_character";
  if(key == KEY_MENU_OPTIONS_FROM_GAME) return "menu_options_from_game";
  if(key == KEY_MINIMAP_FROM_GAME) return "minimap_from_game";

  return "none";
}
