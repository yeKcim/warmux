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
 ******************************************************************************/

#ifndef WARMUX_TYPES_H
#define WARMUX_TYPES_H
//-----------------------------------------------------------------------------
#include <fixed_class.h>

typedef unsigned int     uint;

#ifdef _MSC_VER
typedef int ssize_t;
#endif

#ifdef _WIN32
#  define SIZET_FORMAT  "%I"
#else
#  define SIZET_FORMAT  "%z"
#endif

typedef struct fixedpoint::fixed_point<16> Double;

static const Double QUARTER_PI = 0.25*M_PI;
static const Double HALF_PI    = 0.5*M_PI;
static const Double PI         = M_PI;
static const Double TWO_PI     = 2*M_PI;

// Limit under which, real numbers are considered as NULL
static const Double EPSILON = 0.001f;

static const Double ZERO = 0;
static const Double ONE = 1;
static const Double TWO = 2;
static const Double THREE = 3;
static const Double FOUR = 4;
static const Double FIVE = 5;
static const Double SIX = 6;
static const Double ONE_HALF = 0.5;

enum LRDirection
{
  DIRECTION_LEFT = -1,
  DIRECTION_RIGHT = 1
};

enum UDDirection
{
  DIRECTION_UP,
  DIRECTION_DOWN
};
#endif
