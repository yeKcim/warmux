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
 * Functions to generate random datas (number, boolean, etc.).
 *****************************************************************************/

#ifndef RANDOM_H
#define RANDOM_H

#include "rectangle.h"
#include "point.h"

class Random{
 public:
  Random();

  static bool GetBool();
  static long GetLong(long min, long max);
  static int GetInt(int min, int max);
  static double GetDouble();
  static double GetDouble(double max);
  static double GetDouble(double min, double max);
  static Point2i GetPoint(const Rectanglei &rect);
  static Point2i GetPoint(const Point2i &pt);
  static int GetSign();
};

extern Random randomObj;

#endif
