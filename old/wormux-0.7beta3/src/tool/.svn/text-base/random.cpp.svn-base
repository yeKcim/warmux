/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "../tool/random.h"
//-----------------------------------------------------------------------------
#include <time.h>
#include <math.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------

void InitRandom()
{ srand ( time(NULL) ); }

//---------------------------------------------------------------------------

// Génère un nombre entier aléatoire compris dans [min;max]
long RandomLong (long min, long max)
{
  double r = rand();
  r *= (max-min);
  r /= RAND_MAX;
  if (0 <= r) r = floor(r +0.5); else r = ceil(r -0.5);
  return min + (long)r;
}

//-----------------------------------------------------------------------------

bool RandomBool ()
{
  int moitie = RAND_MAX/2;
  return (rand() <= moitie);
}

//-----------------------------------------------------------------------------
