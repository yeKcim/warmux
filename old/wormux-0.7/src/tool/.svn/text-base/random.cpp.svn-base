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

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "random.h"

Random randomObj;

Random::Random(){
	Init();
}

void Random::Init(){
	srand( time(NULL) );
}

bool Random::GetBool(){
  int moitie = RAND_MAX/2;
  return (rand() <= moitie);
}

/**
 *  Génère un nombre entier aléatoire compris dans [min;max]
 */
long Random::GetLong(long min, long max){
	return min + (long)GetDouble(max - min + 1);
/*  double r = rand();
  
  r *= (max - min);
  r /= RAND_MAX;
  if( r >= 0 )
	  r = floor(r + 0.5);
  else
	  r = ceil(r - 0.5);
  
  return min + (long)r; */
}

double Random::GetDouble(double min, double max){
	return min + GetDouble(max - min);
}

double Random::GetDouble(double max){
	return max * GetDouble();
}

/**
 * Get a random number between 0.0 and 1.0
 * 
 * @return A number between 0.0 and 1.0
 */
double Random::GetDouble(){
	return 1.0*rand()/(RAND_MAX + 1.0);
}

/**
 * Return a random point in the given rectangle.
 *
 * @param rect The rectangle in which the returned point will be.
 * @return a random point.
 */
Point2i Random::GetPoint(const Rectanglei &rect){
	Point2i topPoint = rect.GetPosition();
	Point2i bottomPoint = rect.GetBottomRightPoint();
	
	return Point2i( GetLong(topPoint.x, bottomPoint.x), 
			GetLong(topPoint.y, bottomPoint.y) );
}

Point2i Random::GetPoint(const Point2i &pt){
	return Point2i( GetLong(0, pt.x - 1), GetLong(0, pt.y - 1) );
}
