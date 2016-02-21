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

Clavier * Clavier::singleton = NULL;

Clavier * Clavier::GetInstance() {
  if (singleton == NULL) {
    singleton = new Clavier();
  }
  return singleton;
}

Clavier::Clavier()
{}

void Clavier::Reset()
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);
  for (uint i = 0; i < ACTION_MAX; i++)
    PressedKeys[i] = false ;
}

void Clavier::SetKeyAction(int key, Action_t at)
{
  layout[key] = at;
}

void Clavier::HandleKeyEvent( const SDL_Event *event)
{
  //Handle input text for Chat session in Network game
  //While player writes, it cannot control the game.
  if(GameLoop::GetInstance()->chatsession.CheckInput()){
    GameLoop::GetInstance()->chatsession.HandleKey(event);
    return;
  }

  std::map<int, Action_t>::iterator it = layout.find(event->key.keysym.sym);

  if ( it == layout.end() )
    return;

  Action_t action = it->second;

  //We can perform the next actions, only if the player is played localy:
  if(ActiveTeam().is_local)
  {

    if(action <= ACTION_NEXT_CHARACTER)
      {
        switch (action) {
//           case ACTION_ADD:
//   	  if (lance_grenade.time < 15)
//   	    lance_grenade.time ++;
//   	    break ;
	  
//           case ACTION_SUBSTRACT:
//      if (lance_grenade.time > 1)
//   	    lance_grenade.time --;
//   	  break ;
          default:
	    break ;
        }
      }

     int event_type=0;
     switch( event->type)
       {
        case SDL_KEYDOWN: event_type = KEY_PRESSED;break;
        case SDL_KEYUP: event_type = KEY_RELEASED;break;
       }
    if(event_type==KEY_PRESSED)
      HandleKeyPressed(action);
    if(event_type==KEY_RELEASED)
      HandleKeyReleased(action);

    if ((ActiveTeam().GetWeapon().override_keys &&
        ActiveTeam().GetWeapon().IsActive()) || ActiveTeam().GetWeapon().force_override_keys)
      {
        ActiveTeam().AccessWeapon().HandleKeyEvent((int)action, event_type);
        return ;
      }
    ActiveCharacter().HandleKeyEvent( action, event_type);
  }
  else
  {
    int event_type=0;
    switch( event->type)
    {
      case SDL_KEYDOWN: event_type = KEY_PRESSED;break;
      case SDL_KEYUP: event_type = KEY_RELEASED;break;
    }
    //Current player is on the network
    if(event_type==KEY_RELEASED)
      HandleKeyReleased(action);
  }
}

// Handle a pressed key
void Clavier::HandleKeyPressed (const Action_t &action)
{
  PressedKeys[action] = true ;

  if (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING &&
      ActiveTeam().GetWeapon().CanChangeWeapon())
    {
      int weapon_sort = -1;

      switch(action) {
        case ACTION_WEAPONS1:
	  weapon_sort = 1;
	  break;

        case ACTION_WEAPONS2:
	  weapon_sort = 2;
	  break;

        case ACTION_WEAPONS3:
	  weapon_sort = 3;
	  break;

        case ACTION_WEAPONS4:
	  weapon_sort = 4;
	  break;

        case ACTION_WEAPONS5:
	  weapon_sort = 5;
	  break;

        case ACTION_WEAPONS6:
	  weapon_sort = 6;
	  break;

        case ACTION_WEAPONS7:
	  weapon_sort = 7;
	  break;

        case ACTION_WEAPONS8:
	  weapon_sort = 8;
	  break;

        case ACTION_NEXT_CHARACTER:
	  if (GameMode::GetInstance()->AllowCharacterSelection())
	    ActionHandler::GetInstance()->NewAction(new Action(action));
	  return ;

        default:
	  break ;
      }

      if ( weapon_sort > 0 )
        {
          Weapon_type weapon;
          if (weapons_list.GetWeaponBySort(weapon_sort, weapon))
            ActionHandler::GetInstance()->NewAction(new Action(ACTION_CHANGE_WEAPON, weapon));

          return;
        }
    }
}

// Handle a released key
void Clavier::HandleKeyReleased (const Action_t &action)
{
  PressedKeys[action] = false ;

  // We manage here only actions which are active on KEY_RELEASED event.
  Interface * interface = Interface::GetInstance();

  switch((int)action) // Convert to int to avoid a warning
  {
    case ACTION_QUIT:
      Game::GetInstance()->SetEndOfGameStatus( true );
      return;

    case ACTION_PAUSE:
      ActionHandler::GetInstance()->NewAction(new Action(ACTION_PAUSE));
      return;

    case ACTION_FULLSCREEN:
#ifdef TODO 
      video.SetFullScreen( !video.IsFullScreen() );
#endif
      return;
  case ACTION_CHAT:
    if(network.IsConnected())
      GameLoop::GetInstance()->chatsession.ShowInput();
    return;
  case ACTION_CENTER:

      CharacterCursor::GetInstance()->FollowActiveCharacter();
      camera.ChangeObjSuivi (&ActiveCharacter(), true, true, true);
      return;

    case ACTION_TOGGLE_INTERFACE:
      interface->EnableDisplay (!interface->IsDisplayed());
      return;
  }

  if( ! ActiveTeam().is_local)
    return;

  switch(action) {
    case ACTION_TOGGLE_WEAPONS_MENUS:
      interface->weapons_menu.SwitchDisplay();
      return;

    default:
      break ;
  }
}

// Refresh keys which are still pressed.
void Clavier::Refresh()
{
  //Treat KEY_REFRESH events:
  for (uint i = 0; i < ACTION_MAX; i++)
  if(PressedKeys[i])
  {
    if (ActiveTeam().GetWeapon().override_keys &&
        ActiveTeam().GetWeapon().IsActive())
    {
      ActiveTeam().AccessWeapon().HandleKeyEvent(i, KEY_REFRESH);
    }
    else
    {
      ActiveCharacter().HandleKeyEvent(i,KEY_REFRESH);
    }
  }
}

void Clavier::TestCamera()
{
}

