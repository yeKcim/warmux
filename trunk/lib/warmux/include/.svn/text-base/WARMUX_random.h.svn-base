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
 * Functions to generate random datas (number, boolean, etc.).
 *****************************************************************************/

#ifndef RANDOM_H
#define RANDOM_H

#include <WARMUX_singleton.h>
#include <WARMUX_point.h>
#include <WARMUX_rectangle.h>

class RandomGenerator
{
  uint32_t next;
  bool initialized;

protected:
  bool unrandom;
  virtual void SetRand(uint seed);
  virtual uint GetRand();

public:
  RandomGenerator();
  virtual ~RandomGenerator();
  virtual void InitRandom();

  void UnRandom();
  uint GetSeed() const { return next; }
  void SetSeed(uint seed);

  bool GetBool();
  Double GetDouble();
  Double GetDouble(Double max) { return max * GetDouble(); }
  Double GetDouble(Double min, Double max) { return min + GetDouble(max - min); }

  float  Getfloat();
  float  Getfloat(float max) { return max * Getfloat(); }
  float  Getfloat(float min, float max) { return min + Getfloat(max - min); }
  float  GetGaussianfloat(float mean, float stddev);

  int32_t GetInt(int32_t min, int32_t max) { return min + (int32_t)GetDouble(max - min + 1); }
  uint GetUint(uint32_t min, uint32_t max) { return (uint)GetInt(min, max); }

  Point2i GetPoint(const Rectanglei &rect);
  Point2i GetPoint(const Point2i &pt);

  int GetSign() { return GetBool() ? 1 : -1; }
};


class RandomLocalGen : public Singleton<RandomLocalGen>, public RandomGenerator
{
};

inline RandomLocalGen& RandomLocal() { return RandomLocalGen::GetRef(); }

#endif
