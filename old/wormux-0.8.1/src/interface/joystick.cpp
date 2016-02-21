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
 * Joystick management.
 *****************************************************************************/

#include <SDL.h>
#include "interface/joystick.h"

Joystick::Joystick() :
  ManMachineInterface(),
  previous_x_value(0),
  previous_y_value(0),
  previous_x_axis(KEY_NONE),
  previous_y_axis(KEY_NONE)
{
  SetDefaultConfig();

  // Registring SDL event
  RegisterEvent(SDL_JOYAXISMOTION);
  RegisterEvent(SDL_JOYBALLMOTION);
  RegisterEvent(SDL_JOYHATMOTION);
  RegisterEvent(SDL_JOYBUTTONDOWN);
  RegisterEvent(SDL_JOYBUTTONUP);
}

void Joystick::SetDefaultConfig()
{
  int i = 0;
  SetKeyAction(i++, ManMachineInterface::KEY_SHOOT);
  SetKeyAction(i++, ManMachineInterface::KEY_JUMP);
  SetKeyAction(i++, ManMachineInterface::KEY_HIGH_JUMP);
  SetKeyAction(i++, ManMachineInterface::KEY_BACK_JUMP);
  SetKeyAction(i++, ManMachineInterface::KEY_NEXT_CHARACTER);
  SetKeyAction(i++, ManMachineInterface::KEY_CHANGE_WEAPON);
  SetKeyAction(i++, ManMachineInterface::KEY_WEAPON_LESS);
  SetKeyAction(i++, ManMachineInterface::KEY_WEAPON_MORE);
}

int Joystick::GetNumberOfJoystick() const
{
  return SDL_NumJoysticks();
}

void Joystick::HandleKeyEvent(const SDL_Event& event)
{
  // Not a registred event
  if(!IsRegistredEvent(event.type))
    return;

  Key_Event_t event_type;
  switch(event.type)
    {
    case SDL_JOYAXISMOTION:
      if(event.jaxis.axis == 0) {
        event_type = X_AXIS_MOTION;
      } else {
        event_type = Y_AXIS_MOTION;
      }
      break;
    case SDL_JOYBUTTONDOWN:
      event_type = KEY_PRESSED;
      break;
    case SDL_JOYBUTTONUP:
      event_type = KEY_RELEASED;
      break;

    default:
      return;
    }

  if(event_type == X_AXIS_MOTION) {
    if(event.jaxis.value > -100 && event.jaxis.value < 100) {
      if(previous_x_axis != KEY_NONE)
        HandleKeyReleased(previous_x_axis);
    } else {
      if(event.jaxis.value > 0)
        previous_x_axis = KEY_MOVE_RIGHT;
      else
        previous_x_axis = KEY_MOVE_LEFT;
      HandleKeyPressed(previous_x_axis);
    }
    return;
  } else if(event_type == Y_AXIS_MOTION) {
    if(event.jaxis.value > -100 && event.jaxis.value < 100) {
      if(previous_y_axis != KEY_NONE)
        HandleKeyReleased(previous_y_axis);
    } else {
      if(event.jaxis.value > 0)
        previous_y_axis = KEY_DOWN;
      else
        previous_y_axis = KEY_UP;
      HandleKeyPressed(previous_y_axis);
    }
    return;
  }

  std::map<int, Key_t>::iterator it = layout.find(event.jbutton.button);

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

void Joystick::Reset()
{
  ManMachineInterface::Reset();
  SDL_Init(SDL_INIT_JOYSTICK);
  SDL_JoystickEventState(SDL_ENABLE);
  SDL_JoystickOpen(0);
}
