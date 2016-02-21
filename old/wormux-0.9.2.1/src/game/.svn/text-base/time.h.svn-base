/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include <WORMUX_singleton.h>
#include "game/stopwatch.h"

// XXX uint wrap-around (or at least system timer) not handled
class Time : public Singleton<Time>
{
private:
  Stopwatch   stopwatch;
  bool        waiting_for_network;
  bool        waiting_for_user;
  uint        current_time;
  uint        delta_t;
  Stopwatch   network_wait_time_stopwatch;

protected:
  friend class Singleton<Time>;
  Time();

public:

  void Reset();
  uint Read() const { return current_time; };
  uint ReadSec() const { return Read() / 1000; };
  uint ReadMin() const { return ReadSec() / 60; };

  void Increase();
  bool CanBeIncreased();
  void LetRealTimePassUntilFrameEnd();

  bool IsWaiting();

  bool IsWaitingForUser();
  void SetWaitingForUser(bool value);

  bool IsWaitingForNetwork();
  void SetWaitingForNetwork(bool value);
  uint GetMSWaitingForNetwork();

  uint GetDelta() const { return delta_t; };

  // Read the clock time
  uint ClockSec() const { return ReadSec() % 60; };
  uint ClockMin() const { return ReadMin() % 60; };
  std::string GetString() const;
};

#endif
