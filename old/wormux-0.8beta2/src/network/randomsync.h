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
 * Functions to generate random datas (number, boolean, etc.).
 *****************************************************************************/

#ifndef RANDOMSYNC_H
#define RANDOMSYNC_H
#include <list>
#include "tool/rectangle.h"
#include "tool/point.h"

class RandomSync{
  //Pregenerated table of random number (mainly usefull for network)
  std::list<double> rnd_table;

  double GetRand();
  void GenerateTable();
public:
  RandomSync();
  void Init();

  bool GetBool();
  double GetDouble();
  double GetDouble(double max);
  double GetDouble(double min, double max);
  long GetLong(long min, long max);
  Point2i GetPoint(const Rectanglei &rect);
  Point2i GetPoint(const Point2i &pt);

  //to fill the pregenerated tables
  void SetRandMax(double rand_max);
  void AddToTable(double nbr);
};

extern RandomSync randomSync;

#endif
