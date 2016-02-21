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
 * Keyboard management.
 *****************************************************************************/

#include "keyboard.h"
#include "game/game_loop.h"
#include "network/chat.h"
#include <SDL_events.h>

Keyboard * Keyboard::singleton = NULL;

Keyboard * Keyboard::GetInstance() {
  if (singleton == NULL) {
    singleton = new Keyboard();
  }
  return singleton;
}

Keyboard::Keyboard() : ManMachineInterface()
{
  //Disable repeated events when a key is kept down
  SDL_EnableKeyRepeat(0,0);
  SetDefaultConfig();

  // Registring SDL event
  RegisterEvent(SDL_KEYDOWN);
  RegisterEvent(SDL_KEYUP);
}

void Keyboard::SetDefaultConfig()
{
  SetKeyAction(SDLK_LEFT,      ManMachineInterface::KEY_MOVE_LEFT);
  SetKeyAction(SDLK_RIGHT,     ManMachineInterface::KEY_MOVE_RIGHT);
  SetKeyAction(SDLK_UP,        ManMachineInterface::KEY_UP);
  SetKeyAction(SDLK_DOWN,      ManMachineInterface::KEY_DOWN);
  SetKeyAction(SDLK_RETURN,    ManMachineInterface::KEY_JUMP);
  SetKeyAction(SDLK_BACKSPACE, ManMachineInterface::KEY_HIGH_JUMP);
  SetKeyAction(SDLK_b,         ManMachineInterface::KEY_BACK_JUMP);
  SetKeyAction(SDLK_SPACE,     ManMachineInterface::KEY_SHOOT);
  SetKeyAction(SDLK_TAB,       ManMachineInterface::KEY_NEXT_CHARACTER);
  SetKeyAction(SDLK_ESCAPE,    ManMachineInterface::KEY_QUIT);
  SetKeyAction(SDLK_p,         ManMachineInterface::KEY_PAUSE);
  SetKeyAction(SDLK_F10,       ManMachineInterface::KEY_FULLSCREEN);
  SetKeyAction(SDLK_F9,        ManMachineInterface::KEY_TOGGLE_INTERFACE);
  SetKeyAction(SDLK_F1,        ManMachineInterface::KEY_WEAPONS1);
  SetKeyAction(SDLK_F2,        ManMachineInterface::KEY_WEAPONS2);
  SetKeyAction(SDLK_F3,        ManMachineInterface::KEY_WEAPONS3);
  SetKeyAction(SDLK_F4,        ManMachineInterface::KEY_WEAPONS4);
  SetKeyAction(SDLK_F5,        ManMachineInterface::KEY_WEAPONS5);
  SetKeyAction(SDLK_F6,        ManMachineInterface::KEY_WEAPONS6);
  SetKeyAction(SDLK_F7,        ManMachineInterface::KEY_WEAPONS7);
  SetKeyAction(SDLK_F8,        ManMachineInterface::KEY_WEAPONS8);
  SetKeyAction(SDLK_c,         ManMachineInterface::KEY_CENTER);
  SetKeyAction(SDLK_1,         ManMachineInterface::KEY_WEAPON_1);
  SetKeyAction(SDLK_2,         ManMachineInterface::KEY_WEAPON_2);
  SetKeyAction(SDLK_3,         ManMachineInterface::KEY_WEAPON_3);
  SetKeyAction(SDLK_4,         ManMachineInterface::KEY_WEAPON_4);
  SetKeyAction(SDLK_5,         ManMachineInterface::KEY_WEAPON_5);
  SetKeyAction(SDLK_6,         ManMachineInterface::KEY_WEAPON_6);
  SetKeyAction(SDLK_7,         ManMachineInterface::KEY_WEAPON_7);
  SetKeyAction(SDLK_8,         ManMachineInterface::KEY_WEAPON_8);
  SetKeyAction(SDLK_9,         ManMachineInterface::KEY_WEAPON_9);
  SetKeyAction(SDLK_PAGEUP,    ManMachineInterface::KEY_WEAPON_MORE);
  SetKeyAction(SDLK_PAGEDOWN,  ManMachineInterface::KEY_WEAPON_LESS);
  SetKeyAction(SDLK_s,         ManMachineInterface::KEY_CHAT);
  SetKeyAction(SDLK_F11,         ManMachineInterface::KEY_MENU_OPTIONS_FROM_GAME);
}

void Keyboard::HandleKeyEvent(const SDL_Event& event)
{
  // Not a registred event
  if(!IsRegistredEvent(event.type))
    return;

  //Handle input text for Chat session in Network game
  //While player writes, it cannot control the game.
  if(GameLoop::GetInstance()->chatsession.CheckInput()){
    GameLoop::GetInstance()->chatsession.HandleKey(event);
    return;
  }

  Key_Event_t event_type;
  switch(event.type)
    {
    case SDL_KEYDOWN:
      event_type = KEY_PRESSED;
      break;
    case SDL_KEYUP:
      event_type = KEY_RELEASED;
      break;
    default:
      return;
    }

  std::map<int, Key_t>::iterator it = layout.find(event.key.keysym.sym);

  if(it == layout.end())
    return;

  Key_t key = it->second;

  if(event_type == KEY_PRESSED) {
    HandleKeyPressed(key);
    return;
  }

  if(event_type == KEY_RELEASED) {
    HandleKeyReleased(key);
    return;
  }
}
