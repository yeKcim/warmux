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
#include "network/randomsync.h"
#include "network/network.h"
#include "include/action_handler.h"
#include "tool/debug.h"
#include "tool/random.h"

void RandomSyncGen::InitRandom()
{
  MSG_DEBUG("random", "Initialization...");

#ifdef DEBUG
  nb_get = 0;
#endif

  if (Network::GetInstance()->IsLocal()) {
    int seed = time(NULL);
    SetRand(seed);
  }

  if (Network::GetInstance()->IsServer()) {
    int seed = time(NULL);
    SetRand(seed);

    MSG_DEBUG("random", "Server sending seed %d", seed);

    Action a(Action::ACTION_NETWORK_RANDOM_INIT, seed);
    Network::GetInstance()->SendAction(a);
  }
}

uint RandomSyncGen::GetRand()
{
  uint nbr = RandomGenerator::GetRand();
#ifdef DEBUG
  nb_get++;
  MSG_DEBUG("random.get", "Get %04d: %u", nb_get, nbr);

  if (Network::IsConnected())
    ASSERT(Network::GetInstance()->GetState() == Network::NETWORK_LOADING_DATA
	   || Network::GetInstance()->GetState() == Network::NETWORK_PLAYING);
#endif
  return nbr;
}

void RandomSyncGen::SetRand(uint seed)
{
  RandomGenerator::SetRand(seed);
}

RandomSyncGen& RandomSync()
{
  return (*RandomSyncGen::GetInstance());
}
