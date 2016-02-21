/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Keyboard managment. Use ClanLIB event.
 *****************************************************************************/

#include "keyboard.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "../team/macro.h"
#include "../game/time.h"
#include "../include/action_handler.h"
#include "../weapon/weapon.h"
#include "../weapon/weapons_list.h"
#include "game_msg.h"
#include "../map/camera.h"
#include "interface.h"
#include "../team/move.h"
#include "../tool/i18n.h"
#include "../tool/math_tools.h"
#include "../game/game.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../sound/jukebox.h"
#include "../game/config.h"
#include "../graphic/video.h"
#include "cursor.h"
#include "../include/constant.h"
#include "../map/camera.h"
#include <iostream>
using namespace Wormux;
//-----------------------------------------------------------------------------

// Active le mode tricheur ?
#ifdef DEBUG
#  define MODE_TRICHEUR
//#  define USE_HAND_POSITION_MODIFIER
#endif

// Vitesse du definalement au clavier
#define SCROLL_CLAVIER 20 // ms

//-----------------------------------------------------------------------------
Clavier clavier;
//-----------------------------------------------------------------------------

Clavier::Clavier()
{}

//-----------------------------------------------------------------------------


void Clavier::Reset()
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);
  for (uint i = 0; i < ACTION_MAX; i++)
    PressedKeys[i] = false ;
}

//-----------------------------------------------------------------------------

void Clavier::SetKeyAction(int key, Action_t at)
{
  layout[key] = at;
}

//-----------------------------------------------------------------------------

void Clavier::HandleKeyEvent( const SDL_Event *event)
{
  std::map<int, Action_t>::iterator it = layout.find(event->key.keysym.sym);

  if ( it == layout.end() )
    return;

  Action_t action = it->second;

  //We can perform the next actions, only if the player is played localy:
  //if(!ActiveTeam().is_local)
  //  return;

  if(action <= ACTION_CHANGE_CHARACTER)
    {
      switch (action) {
//         case ACTION_ADD:
// 	  if (lance_grenade.time < 15)
// 	    lance_grenade.time ++;
// 	  break ;
	  
//         case ACTION_SUBSTRACT:
// 	  if (lance_grenade.time > 1)
// 	    lance_grenade.time --;
// 	  break ;
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

  if (ActiveTeam().GetWeapon().override_keys &&
      ActiveTeam().GetWeapon().IsActive())
    {
      ActiveTeam().AccessWeapon().HandleKeyEvent((int)action, event_type);
      return ;
    }
   
  ActiveCharacter().HandleKeyEvent( action, event_type);
}

//-----------------------------------------------------------------------------

// Handle a pressed key
void Clavier::HandleKeyPressed (const Action_t &action)
{
  PressedKeys[action] = true ;

  //We can perform the next actions, only if the player is played localy:
  if(!ActiveTeam().is_local)
    return;

  if (game_loop.ReadState() == gamePLAYING &&
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

        case ACTION_CHANGE_CHARACTER:
	  if (game_mode.AllowCharacterSelection())
	    action_handler.NewAction(ActionInt(action,
					ActiveTeam().NextCharacterIndex()));
	  return ;

        default:
	  break ;
      }

      if ( weapon_sort > 0 )
        {
          Weapon_type weapon;
          if (weapons_list.GetWeaponBySort(weapon_sort, weapon))
            action_handler.NewAction(ActionInt(ACTION_CHANGE_WEAPON, weapon));

          return;
        }
    }
}

//-----------------------------------------------------------------------------

// Handle a released key
void Clavier::HandleKeyReleased (const Action_t &action)
{
  PressedKeys[action] = false ;

  // We manage here only actions which are active on KEY_RELEASED event.

  switch(action) {
    case ACTION_QUIT:
      game.SetEndOfGameStatus( true );
      return;

    case ACTION_PAUSE:
      game.Pause();
      return;

    case ACTION_FULLSCREEN:
#ifdef TODO 
      video.SetFullScreen( !video.IsFullScreen() );
#endif
      return;

    case ACTION_TOGGLE_INTERFACE:
      interface.EnableDisplay (!interface.IsDisplayed());
      return;

    case ACTION_CENTER:
      curseur_ver.SuitVerActif();
      camera.ChangeObjSuivi (&ActiveCharacter(), true, true, true);
      return;

    case ACTION_TOGGLE_WEAPONS_MENUS:
      interface.weapons_menu.SwitchDisplay();
      return;

    default:
      break ;
  }
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
                                                                                    
void Clavier::TestCamera()
{
}

//-----------------------------------------------------------------------------
