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

#include <time.h>
#include <stdlib.h>
#include "network/randomsync.h"
#include "network/network.h"
#include "include/action_handler.h"

const uint table_size = 1024; //Number of pregerated numbers

static double NET_RAND_MAX = RAND_MAX; // client should use same RAND_MAX than server (bug network windows/linux)

RandomSync randomSync;

RandomSync::RandomSync()
{
}

void RandomSync::Init()
{
  //If we are a client on the network, we don't generate any random number
  if (Network::GetInstance()->IsClient()) return;

  srand( time(NULL) );

  rnd_table.clear();

  if  (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_NETWORK_RANDOM_INIT);
    a.Push(NET_RAND_MAX);
    Network::GetInstance()->SendAction(&a);
  }

  //Fill the pregenerated tables:
  for (uint i=0; i < table_size; i++)
    GenerateTable();
}

void RandomSync::GenerateTable()
{
  //Add a random number to the table
  double nbr = rand();
  AddToTable(nbr);

  // send it over network if needed
  if (Network::GetInstance()->IsServer()) {
    Action a(Action::ACTION_NETWORK_RANDOM_ADD,nbr);
    Network::GetInstance()->SendAction(&a);
  }
}

void RandomSync::AddToTable(double nbr)
{
  rnd_table.push_back(nbr);
}

void RandomSync::SetRandMax(double rand_max)
{
  NET_RAND_MAX = rand_max;
  rnd_table.clear();
}

double RandomSync::GetRand()
{
  if (Network::GetInstance()->IsServer() || Network::GetInstance()->IsLocal())
    GenerateTable();

  ASSERT(rnd_table.size()!=0);
  if (rnd_table.size() == 0) {
    Error("Random table is empty!\n");
    exit(1);
  }

  double nbr = rnd_table.front();
  rnd_table.pop_front();
  return nbr;
}

bool RandomSync::GetBool()
{
  double middle = NET_RAND_MAX/2;
  return (GetRand() <= middle);
}

/**
 *  Get a random number between min and max
 */
long RandomSync::GetLong(long min, long max)
{
        return min + (long)GetDouble(max - min + 1);
}

double RandomSync::GetDouble(double min, double max)
{
        return min + GetDouble(max - min);
}

double RandomSync::GetDouble(double max)
{
        return max * GetDouble();
}

/**
 * Get a random number between 0.0 and 1.0
 *
 * @return A number between 0.0 and 1.0
 */
double RandomSync::GetDouble()
{
        return 1.0*GetRand()/(NET_RAND_MAX + 1.0);
}

/**
 * Return a random point in the given rectangle.
 *
 * @param rect The rectangle in which the returned point will be.
 * @return a random point.
 */
Point2i RandomSync::GetPoint(const Rectanglei &rect)
{
        Point2i topPoint = rect.GetPosition();
        Point2i bottomPoint = rect.GetBottomRightPoint();
	long x = GetLong(topPoint.x, bottomPoint.x);
	long y = GetLong(topPoint.y, bottomPoint.y);
        return Point2i( x, y );
}

Point2i RandomSync::GetPoint(const Point2i &pt)
{
	long x = GetLong(0, pt.x - 1);
	long y = GetLong(0, pt.y - 1);
        return Point2i( x, y );
}
