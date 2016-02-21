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
 *  Handle the game time. The game can be paused.
 *****************************************************************************/

#ifndef TIME_H
#define TIME_H

#include <string>
#include "include/base.h"

// XXX uint wrap-around (or at least system timer) not handled
class Time
{
private:
  uint        current_time;
  //uint        max_time;
  uint        delta_t;
  bool        is_game_paused;
  static Time *singleton;

  uint        real_time_game_start;
  uint        real_time_pause_dt;
  uint        real_time_pause_begin;

private:
  Time();
public:
  static Time *GetInstance();
  ~Time() { singleton = NULL; };

  void Reset();
  bool IsGamePaused() const;

  // Read the time of the game, excluding paused time
  uint ReadRealTime() const;
  uint Read() const { return current_time; };
  uint ReadDuration() const
  {
    return current_time-real_time_game_start-real_time_pause_dt;
  };
  uint ReadSec() const { return Read() / 1000; };
  uint ReadMin() const { return ReadSec() / 60; };
  void Refresh();
  uint GetDelta() const { return delta_t; };
  //void RefreshMaxTime(uint updated_max_time);

  // Read the clock time
  uint ClockSec() const { return ReadSec() % 60; };
  uint ClockMin() const { return ReadMin() % 60; };
  std::string GetString() const;

  void Pause();
  void Continue();
  void TogglePause();
};

#endif
