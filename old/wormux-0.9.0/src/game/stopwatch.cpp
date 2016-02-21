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
 *  The stopwatch can be used to messure how much time elapses.
 *****************************************************************************/

#include <SDL.h>
#include "game/stopwatch.h"
#include <WORMUX_error.h>

Stopwatch::Stopwatch()
{
  Reset();
}

void Stopwatch::Reset(double speed_value)
{
  speed = speed_value;
  paused = false;
  start_time = SDL_GetTicks();
  paused_time = 0;
}

void Stopwatch::Stop()
{
  ASSERT(!paused);
  paused = true;
  pause_begin = SDL_GetTicks();
}

void Stopwatch::Resume()
{
  ASSERT(paused);
  paused = false;
  paused_time += SDL_GetTicks() - pause_begin;
}

void Stopwatch::SetPause(bool value)
{
  if (paused == value)
    return;
  if (paused) {
    Resume();
  } else {
    Stop();
  }
}

uint Stopwatch::GetValue()
{
  if (paused)
    return static_cast<uint>(speed * (pause_begin - start_time - paused_time));
  else
    return static_cast<uint>(speed * (SDL_GetTicks() - start_time - paused_time));
}

bool Stopwatch::IsPaused()
{
  return paused;
}
