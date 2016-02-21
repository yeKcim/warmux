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
 * Joystick management
 *****************************************************************************/

#ifndef JOYSTICK_H
#define JOYSTICK_H
//-----------------------------------------------------------------------------
#include "interface/man_machine_interface.h"
#include <WARMUX_singleton.h>
//-----------------------------------------------------------------------------

class Joystick : public Singleton<Joystick>, public ManMachineInterface
{
  bool init;
  int previous_x_value;
  int previous_y_value;
  Key_t previous_x_axis;
  Key_t previous_y_axis;
  void SetDefaultConfig();

protected:
  friend class Singleton<Joystick>;
  Joystick();
  ~Joystick();

public:
  int GetNumberOfJoystick() const;
  bool HandleKeyEvent(const SDL_Event& event);
  void Reset();
};

//-----------------------------------------------------------------------------
#endif /* JOYSTICK_H */
