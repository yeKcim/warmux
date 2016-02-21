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

#include "game/game.h"
#include "game/time.h"
#include "network/network.h"
#include "team/team.h"
#include "team/teams_list.h"
#include <SDL.h>
#include <sstream>
#include <iomanip>

Time::Time()
{
  delta_t = 20;
  Reset();
}

void Time::Reset()
{
  current_time = 0;
  waiting_for_user = false;
  waiting_for_network = false;
  if (IsLOGGING("extra_slow")) {
    stopwatch.Reset(0.1);
  } else if (IsLOGGING("slow")) {
    stopwatch.Reset(0.25);
  } else {
    stopwatch.Reset();
  }
}

void Time::Increase()
{
  ASSERT(!IsWaiting());
  ASSERT(CanBeIncreased());
  current_time += delta_t;
  MSG_DEBUG("time.increase","Real time without pause: %d; Game time: %d", stopwatch.GetValue(), current_time);
}

bool Time::CanBeIncreased()
{
  return stopwatch.GetValue() >= current_time;
}

void Time::LetRealTimePassUntilFrameEnd()
{
  ASSERT(!IsWaiting());
  long delay;
  do {
    delay = static_cast<long>(current_time) - stopwatch.GetValue();
    if (delay > 0) {
      SDL_Delay((uint)delay);
      MSG_DEBUG("time.skip","Do nothing for: %d", delay);
    }
  } while(delay > 0);
}

bool Time::IsWaiting()
{
  return waiting_for_user || waiting_for_network;
}

bool Time::IsWaitingForUser()
{
  return waiting_for_user;
}

void Time::SetWaitingForUser(bool value)
{
  if (waiting_for_user == value)
    return;
  waiting_for_user = value;
  stopwatch.SetPause(IsWaiting());
}

bool Time::IsWaitingForNetwork()
{
  return waiting_for_network;
}

void Time::SetWaitingForNetwork(bool value)
{
  if (waiting_for_network == value)
    return;
  waiting_for_network = value;
  if (waiting_for_network) {
    network_wait_time_stopwatch.Reset();
    MSG_DEBUG("time.waiting","Start waiting for network.");
  } else {
    MSG_DEBUG("time.waiting","Waited %d ms for network.", network_wait_time_stopwatch.GetValue());
  }
  stopwatch.SetPause(IsWaiting());
}

uint Time::GetMSWaitingForNetwork()
{
  if (waiting_for_network)
    return network_wait_time_stopwatch.GetValue();
  else
    return 0;
}


std::string Time::GetString() const
{
  std::ostringstream ss;

  ss << ClockMin() << ":" << std::setfill('0') << std::setw(2) << ClockSec();
  return ss.str();
}
