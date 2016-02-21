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
 * Calculate frame per second.
 *****************************************************************************/

#ifndef FPS_H
#define FPS_H

#include <list>
#include "include/base.h"

class Text;

class FramePerSecond
{
private:
  /* If you need this, implement it (correctly) */
  FramePerSecond(const FramePerSecond&);
  FramePerSecond operator=(const FramePerSecond&);
  /**********************************************/

  // Minimum number of values needed to compute the average
  static const uint MIN_NB_VALUES;

  int nb_valid_values;
  double average;
  std::list<uint> nb_frames;
  uint time_in_second;
  Text* text;

public:
  bool display;

public:
  FramePerSecond();
  ~FramePerSecond();
  void Reset();
  void AddOneFrame();
  void Refresh();
  void Draw();
};

#endif
