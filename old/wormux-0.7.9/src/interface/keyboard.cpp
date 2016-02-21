/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Keyboard managment.
 *****************************************************************************/

#include "keyboard.h"
#include <sstream>
#include <iostream>
#include "cursor.h"
#include "game_msg.h"
#include "interface.h"
#include "../include/action.h"
#include "../include/app.h"
#include "../game/config.h"
#include "../game/game.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../include/action_handler.h"
#include "../include/constant.h"
#include "../map/camera.h"
#include "../team/macro.h"
#include "../character/move.h"
#include "../tool/i18n.h"
#include "../tool/math_tools.h"
#include "../sound/jukebox.h"
#include "../map/camera.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"
#include "../network/network.h"

// Vitesse du definalement au clavier
#define SCROLL_CLAVIER 20 // ms

Keyboard::Keyboard()
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);
}

void Keyboard::Reset()
{
  for (int i = Action::ACTION_FIRST; i != Action::ACTION_LAST; i++)
    PressedKeys[i] = false ;
}

void Keyboard::SetKeyAction(int key, Action::Action_t at)
{
  layout[key] = at;
}

// Get the key associated to an action.
int Keyboard::GetKeyAssociatedToAction(Action::Action_t at)
{
  std::map<int, Action::Action_t>::iterator it;
  for (it= layout.begin(); it != layout.end(); it++) {
    if (it->second == at) {
      return it->first;
    }
  }
  return 0;
}


void Keyboard::HandleKeyEvent( const SDL_Event *event)
{
  //Handle input text for Chat session in Network game
  //While player writes, it cannot control the game.
  if(GameLoop::GetInstance()->chatsession.CheckInput()){
    GameLoop::GetInstance()->chatsession.HandleKey(event);
    return;
  }

  std::map<int, Action::Action_t>::iterator it = layout.find(event->key.keysym.sym);

  if ( it == layout.end() )
    return;

  Action::Action_t action = it->second;

  //We can perform the next actions, only if the player is played localy:
  if(ActiveTeam().IsLocal())
  {

    if(action <= Action::ACTION_NEXT_CHARACTER)
      {
        switch (action) {
//           case Action::ACTION_ADD:
//   	  if (lance_grenade.time < 15)
//   	    lance_grenade.time ++;
//   	    break ;

//           case Action::ACTION_SUBSTRACT:
//      if (lance_grenade.time > 1)
//   	    lance_grenade.time --;
//   	  break ;
          default:
	    break ;
        }
      }

     Key_Event_t event_type;
     switch( event->type)
       {
        case SDL_KEYDOWN:
          event_type = KEY_PRESSED;break;
        case SDL_KEYUP:
          event_type = KEY_RELEASED;break;
        default:
          return;
       }
    if(event_type == KEY_PRESSED)
      HandleKeyPressed(action);
    if(event_type == KEY_RELEASED)
      HandleKeyReleased(action);

    if ((ActiveTeam().GetWeapon().override_keys &&
        ActiveTeam().GetWeapon().IsActive()) || ActiveTeam().GetWeapon().force_override_keys)
      {
        ActiveTeam().AccessWeapon().HandleKeyEvent(action, event_type);
        return ;
      }
    ActiveCharacter().HandleKeyEvent( action, event_type);
  }
  else
  {
    Key_Event_t event_type;
    switch( event->type)
    {
      case SDL_KEYDOWN: event_type = KEY_PRESSED;break;
      case SDL_KEYUP: event_type = KEY_RELEASED;break;
      default: return;
    }
    //Current player is on the network
    if(event_type == KEY_RELEASED)
      HandleKeyReleased(action);
  }
}

// Handle a pressed key
void Keyboard::HandleKeyPressed (const Action::Action_t &action)
{
  PressedKeys[action] = true ;

  if (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING &&
      ActiveTeam().GetWeapon().CanChangeWeapon())
    {
      int weapon_sort = -1;

      switch(action) {
        case Action::ACTION_WEAPONS1:
          weapon_sort = 1;
          break;

        case Action::ACTION_WEAPONS2:
          weapon_sort = 2;
          break;

        case Action::ACTION_WEAPONS3:
          weapon_sort = 3;
          break;

        case Action::ACTION_WEAPONS4:
          weapon_sort = 4;
          break;

        case Action::ACTION_WEAPONS5:
          weapon_sort = 5;
          break;

        case Action::ACTION_WEAPONS6:
          weapon_sort = 6;
          break;

        case Action::ACTION_WEAPONS7:
          weapon_sort = 7;
          break;

        case Action::ACTION_WEAPONS8:
          weapon_sort = 8;
          break;

        case Action::ACTION_NEXT_CHARACTER:
          if (GameMode::GetInstance()->AllowCharacterSelection()) {
            Action * next_character = new Action(Action::ACTION_NEXT_CHARACTER);
            next_character->StoreActiveCharacter();
            ActiveTeam().NextCharacter();
            next_character->StoreActiveCharacter();
            ActionHandler::GetInstance()->NewAction(next_character);
          }
          return ;

        default:
          break ;
      }

      if ( weapon_sort > 0 )
        {
          Weapon::Weapon_type weapon;
          if (Config::GetInstance()->GetWeaponsList()->GetWeaponBySort(weapon_sort, weapon))
            ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHANGE_WEAPON, weapon));

          return;
        }
    }
}

// Handle a released key
void Keyboard::HandleKeyReleased (const Action::Action_t &action)
{
  PressedKeys[action] = false ;

  // We manage here only actions which are active on KEY_RELEASED event.
  Interface * interface = Interface::GetInstance();
  BonusBox * current_box;

  switch(action) // Convert to int to avoid a warning
  {
    case Action::ACTION_QUIT:
      Game::GetInstance()->SetEndOfGameStatus( true );
      return;
    case Action::ACTION_PAUSE:
      ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_PAUSE));
      return;
    case Action::ACTION_SHOOT:
      current_box = GameLoop::GetInstance()->GetCurrentBonusBox();
      if(current_box != NULL)
        current_box->DropBonusBox();
      return;
    case Action::ACTION_FULLSCREEN:
      AppWormux::GetInstance()->video.ToggleFullscreen();
      return;
    case Action::ACTION_CHAT:
      if(network.IsConnected())
        GameLoop::GetInstance()->chatsession.ShowInput();
      return;
    case Action::ACTION_CENTER:
      CharacterCursor::GetInstance()->FollowActiveCharacter();
      camera.FollowObject (&ActiveCharacter(), true, true, true);
      return;
    case Action::ACTION_TOGGLE_INTERFACE:
      interface->EnableDisplay (!interface->IsDisplayed());
      return;
    default:
      return;
  }

  if( ! ActiveTeam().IsLocal())
    return;

  switch(action) {
    case Action::ACTION_TOGGLE_WEAPONS_MENUS:
      interface->weapons_menu.SwitchDisplay();
      return;

    default:
      break ;
  }
}

// Refresh keys which are still pressed.
void Keyboard::Refresh()
{
  //Treat KEY_REFRESH events:
  for (int i = Action::ACTION_FIRST; i < Action::ACTION_LAST; i++)
    if(PressedKeys[i])
      {
        if (ActiveTeam().GetWeapon().override_keys &&
            ActiveTeam().GetWeapon().IsActive())
          {
            ActiveTeam().AccessWeapon().HandleKeyEvent(static_cast<Action::Action_t>(i), KEY_REFRESH);
          }
        else
          {
            ActiveCharacter().HandleKeyEvent(static_cast<Action::Action_t>(i),KEY_REFRESH);
          }
      }
}

void Keyboard::TestCamera()
{
}

