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
 *  Handle the game time. The game can be paused.
 *****************************************************************************/

#ifndef TIME_H
#define TIME_H

#include <string>
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "game/stopwatch.h"

// XXX uint wrap-around (or at least system timer) not handled
class GameTime : public Singleton<GameTime>
{
private:
  Stopwatch   stopwatch;
  bool        waiting_for_network;
  bool        waiting_for_user;
  uint        current_time;
  uint        delta_t;
  Stopwatch   network_wait_time_stopwatch;

protected:
  friend class Singleton<GameTime>;
  GameTime();

public:

  void Reset();
  uint Read() const { return current_time; }
  uint ReadSec() const { return Read() / 1000; }
  uint ReadMin() const { return ReadSec() / 60; }

  void SetPause(bool pause) { stopwatch.SetPause(pause); }
  bool IsPaused() const { return stopwatch.IsPaused(); }
  void SetSpeed(const Double& speed);
  const Double& GetSpeed() const { return stopwatch.GetSpeed(); }

  void Increase();
  bool CanBeIncreased() const { return stopwatch.GetValue() >= current_time; }
  bool CanDraw() const { return stopwatch.GetValue()<current_time+20 || IsWaiting(); }
  void LetRealTimePassUntilFrameEnd();
  void Resynch() { stopwatch.Resynch(current_time); }

  bool IsWaiting() const { return waiting_for_user || waiting_for_network; }
  bool IsWaitingForUser() const { return waiting_for_user; }
  void SetWaitingForUser(bool value);

  bool IsWaitingForNetwork() const { return waiting_for_network; }
  void SetWaitingForNetwork(bool value);
  uint GetMSWaitingForNetwork();

  uint GetDelta() const { return delta_t; };

  // Read the clock time
  uint ClockSec() const { return ReadSec() % 60; };
  uint ClockMin() const { return ReadMin() % 60; };
  std::string GetString() const;
};

#endif
