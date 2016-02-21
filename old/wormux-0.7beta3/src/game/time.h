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
 *  Handle the game time. The game can be paused.
 *****************************************************************************/

#ifndef TIME_H
#define TIME_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include <string>

namespace Wormux
{

class Time
{
private:
  uint pause_start;
  uint pause_offset;// Offset between get_time() and LitTemps()
  bool is_game_paused;
   
public:
  Time();
  void Reset();
  bool IsGamePaused() const;

  // Read the time of the game, excluding paused time
  uint Read() const;
  uint ReadSec() const;
  uint ReadMin() const;

  // Read the clock time
  uint ClockSec();  // ReadSec() % 60
  uint ClockMin();  // ReadMin() % 60
  std::string GetString();

  void Pause();
  void Continue();
};

 extern Time global_time;
}
#endif
