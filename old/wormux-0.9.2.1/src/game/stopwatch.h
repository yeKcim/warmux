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

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <WORMUX_types.h>

class Stopwatch
{
private:
  uint start_time;
  uint paused_time;
  uint pause_begin;
  bool paused;
  Double speed;
private:
  void Stop();
  void Resume();
public:
  Stopwatch();
  void Reset(Double speed = 1);
  void SetPause(bool value);
  uint GetValue();
  bool IsPaused();
};

#endif
