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

#include <time.h>
#include "tool/debug.h"
#include "tool/random.h"

RandomGenerator::RandomGenerator() :
  next(0), initialized(false)
{
}

RandomGenerator::~RandomGenerator()
{}

void RandomGenerator::InitRandom()
{
  SetRand(time(NULL));
}

void RandomGenerator::SetRand(uint seed)
{
  MSG_DEBUG("random", "srand: %d", seed);
  next = seed;
  initialized = true;
}


/******************************************************************************
 * From "man 3 rand"
 * POSIX.1-2001  gives the following example of an implementation of rand() and
 * srand(), possibly useful when one needs the same sequence on two different
 * machines.
 ******************************************************************************/

/* RAND_MAX assumed to be 32767 */
uint RandomGenerator::GetRand()
{
  ASSERT(initialized == true);

  next = next * 1103515245 + 12345;
  return((uint)(next/65536) % 32768);
}

#define WORMUX_RAND_MAX 32767

bool RandomGenerator::GetBool()
{
  double middle = WORMUX_RAND_MAX/2;
  return (GetRand() <= middle);
}

/**
 * Get a random number between 0.0 and 1.0
 *
 * @return A number between 0.0 and 1.0
 */
double RandomGenerator::GetDouble()
{
  return 1.0*GetRand()/(WORMUX_RAND_MAX + 1.0);
}

/**
 *  Get a random number between min and max
 */
long RandomGenerator::GetLong(long min, long max)
{
  return min + (long)GetDouble(max - min + 1);
}

int RandomGenerator::GetInt(int min, int max)
{
  return GetLong(min, max);
}

uint RandomGenerator::GetUint(uint min, uint max)
{
  return (uint)GetLong(min, max);
}

double RandomGenerator::GetDouble(double min, double max)
{
  return min + GetDouble(max - min);
}

double RandomGenerator::GetDouble(double max)
{
  return max * GetDouble();
}

/**
 * Return a random point in the given rectangle.
 *
 * @param rect The rectangle in which the returned point will be.
 * @return a random point.
 */
Point2i RandomGenerator::GetPoint(const Rectanglei &rect)
{
  Point2i topPoint = rect.GetPosition();
  Point2i bottomPoint = rect.GetBottomRightPoint();
  long x = GetLong(topPoint.x, bottomPoint.x);
  long y = GetLong(topPoint.y, bottomPoint.y);
  return Point2i( x, y );
}

Point2i RandomGenerator::GetPoint(const Point2i &pt)
{
  long x = GetLong(0, pt.x - 1);
  long y = GetLong(0, pt.y - 1);
  return Point2i( x, y );
}

int RandomGenerator::GetSign()
{
  if (GetBool()) return 1;
  else return -1;
}

// =============================================================

RandomLocalGen& RandomLocal()
{
  return RandomLocalGen::GetRef();
}
