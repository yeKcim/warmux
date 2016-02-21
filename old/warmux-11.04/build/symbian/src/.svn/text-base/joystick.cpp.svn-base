/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Warmux Team.
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

#include "interface/joystick.h"
#include <SDL.h>

Joystick::Joystick()
  : ManMachineInterface()
  , init(false)
  , previous_x_value(0)
  , previous_y_value(0)
  , previous_x_axis(KEY_NONE)
  , previous_y_axis(KEY_NONE)
{
  return;
}

Joystick::~Joystick()
{
  if (init)
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
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
  return 0;
}

bool Joystick::HandleKeyEvent(const SDL_Event& evnt)
{
  return false;
}

void Joystick::Reset()
{
  ManMachineInterface::Reset();
}
