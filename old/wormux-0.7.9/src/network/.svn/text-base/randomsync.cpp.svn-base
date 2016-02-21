/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
#include "randomsync.h"
#include "network.h"
#include "../include/action_handler.h"

const uint table_size = 128; //Number of pregerated numbers

RandomSync randomSync;

RandomSync::RandomSync(){
}

void RandomSync::Init(){
  //If we are a client on the network, we don't generate any random number
  if(network.IsClient()) return;

  srand( time(NULL) );

  rnd_table.clear();

  //Fill the pregenerated tables:
  for(uint i=0; i < table_size; i++)
    GenerateTable();
}

void RandomSync::GenerateTable()
{
  //Add a random number to the table, send it over network if needed
  double nbr = rand();
  AddToTable(nbr);
  if(network.IsServer()) ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_SEND_RANDOM,nbr));
}

void RandomSync::AddToTable(double nbr)
{
  rnd_table.push_back(nbr);
}

double RandomSync::GetRand()
{
  if(network.IsServer() || network.IsLocal()) GenerateTable();

  // If the table is empty freeze until the server have sent something
  while(rnd_table.size() == 0)
  {
    SDL_Delay(100);
    ActionHandler::GetInstance()->ExecActions();
  }

  double nbr = rnd_table.front();
  rnd_table.pop_front();
  return nbr;
}

bool RandomSync::GetBool(){
  int moitie = RAND_MAX/2;
  return (GetRand() <= moitie);
}

/**
 *  Génère un nombre entier aléatoire compris dans [min;max]
 */
long RandomSync::GetLong(long min, long max){
	return min + (long)GetDouble(max - min + 1);
}

double RandomSync::GetDouble(double min, double max){
	return min + GetDouble(max - min);
}

double RandomSync::GetDouble(double max){
	return max * GetDouble();
}

/**
 * Get a random number between 0.0 and 1.0
 *
 * @return A number between 0.0 and 1.0
 */
double RandomSync::GetDouble(){
	return 1.0*GetRand()/(RAND_MAX + 1.0);
}

/**
 * Return a random point in the given rectangle.
 *
 * @param rect The rectangle in which the returned point will be.
 * @return a random point.
 */
Point2i RandomSync::GetPoint(const Rectanglei &rect){
	Point2i topPoint = rect.GetPosition();
	Point2i bottomPoint = rect.GetBottomRightPoint();

	return Point2i( GetLong(topPoint.x, bottomPoint.x),
			GetLong(topPoint.y, bottomPoint.y) );
}

Point2i RandomSync::GetPoint(const Point2i &pt){
	return Point2i( GetLong(0, pt.x - 1), GetLong(0, pt.y - 1) );
}
