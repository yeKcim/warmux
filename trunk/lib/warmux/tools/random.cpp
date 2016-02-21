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

#include <time.h>
#include <WARMUX_debug.h>
#include <WARMUX_random.h>

RandomGenerator::RandomGenerator() :
  next(0), initialized(false), unrandom(false)
{
}

RandomGenerator::~RandomGenerator()
{
}

void RandomGenerator::UnRandom()
{
  next = 0x12345678;
  initialized = true;
  unrandom = true;
}

void RandomGenerator::InitRandom()
{
  if (unrandom) {
    MSG_DEBUG("random.set", "no, unrandom set");
    return;
  }
  SetRand(uint(time(NULL)));
}

void RandomGenerator::SetRand(uint seed)
{
  if (unrandom) {
    MSG_DEBUG("random.set", "no, unrandom set");
    return;
  }
  MSG_DEBUG("random.set", "srand: %d", seed);
  next = seed;
  initialized = true;
}

void RandomGenerator::SetSeed(uint seed)
{
  if (unrandom) {
    MSG_DEBUG("random.set", "no, unrandom set");
    return;
  }
  SetRand(seed);
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
  return (next>>16) & 0x7FFF;
}

#define WARMUX_RAND_MAX 32767

bool RandomGenerator::GetBool()
{
  uint middle = WARMUX_RAND_MAX/2;
  uint random = GetRand();
  return (random <= middle);
}

//==========================================================================
Double RandomGenerator::GetDouble()
{
  return GetRand()/(WARMUX_RAND_MAX + ONE);
}

//==========================================================================
float RandomGenerator::Getfloat()
{
  return GetRand()/(WARMUX_RAND_MAX + 1.0f);
}

float RandomGenerator::GetGaussianfloat(float mean, float stddev)
{
  static const float FACTOR = 1.0f/16384;
  static const uint NUM_INPUTS = 6;
  uint sum = 0;
  // Do we really need a Box-Muller transform?
  for (uint i=0; i<NUM_INPUTS; i++)
    sum += GetRand();
  // To do as if values were randomly between -16384 and 16383
  sum -= 16384*NUM_INPUTS;

  return sum * stddev * FACTOR + mean;
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
  int32_t x = GetInt(topPoint.x, bottomPoint.x);
  int32_t y = GetInt(topPoint.y, bottomPoint.y);
  return Point2i( x, y );
}

Point2i RandomGenerator::GetPoint(const Point2i &pt)
{
  int32_t x = GetInt(0, pt.x - 1);
  int32_t y = GetInt(0, pt.y - 1);
  return Point2i(x, y);
}
