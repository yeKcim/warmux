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
 ******************************************************************************/

#ifndef WORMUX_TYPES_H
#define WORMUX_TYPES_H
//-----------------------------------------------------------------------------
#include <fixed_class.h>

#ifndef _MSC_VER
#include <stdint.h>
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
typedef unsigned char  uint8;
//typedef unsigned long uint32;
typedef signed char    sint8;
typedef signed long    sint32;
#else
typedef unsigned __int8  uchar;
typedef unsigned __int16 ushort;
typedef unsigned int     uint;
typedef unsigned long    ulong;
typedef unsigned __int8  uint8;
typedef signed __int8    sint8;
typedef signed __int32   sint32;
typedef long int         ssize_t;

typedef signed   __int16  int16_t;
typedef unsigned __int32  uint32_t;
#endif

typedef struct fixedpoint::fixed_point<16> Double;
typedef struct fixedpoint::fixed_point<16> Float;

const Double QUARTER_PI = 0.78539816339744828;
const Double HALF_PI = 1.5707963267948966;
const Double PI = 3.1415926535897931f;

// Limit under which, real numbers are considered as NULL
const Double EPSILON = 0.001f;

const Double ZERO = 0;
const Double ONE = 1;
const Double TWO = 2;
const Double THREE = 3;
const Double FOUR = 4;
const Double FIVE = 5;
const Double SIX = 6;
const Double ONE_HALF = 0.5;

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
