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
 ******************************************************************************
 * Functions to generate random datas (number, boolean, etc.).
 *****************************************************************************/

#include <time.h>
#include "network/randomsync.h"
#include "network/network.h"
#include "include/action_handler.h"
#include <WORMUX_debug.h>
#include <WORMUX_random.h>

void RandomSyncGen::InitRandom()
{
  MSG_DEBUG("random", "Initialization...");

  if (Network::GetInstance()->IsLocal()) {
    int seed = time(NULL);
    SetRand(seed);
  } else if (Network::GetInstance()->IsGameMaster()) {
    int seed = time(NULL);
    SetRand(seed);

    MSG_DEBUG("random", "Server sending seed %d", seed);

    Action a(Action::ACTION_NETWORK_RANDOM_INIT, seed);
    Network::GetInstance()->SendActionToAll(a);
  }
}

uint RandomSyncGen::GetRand()
{
  uint nbr = RandomGenerator::GetRand();
#ifdef DEBUG
  nb_get++;
  MSG_DEBUG("random.get", "Get %04d: %u", nb_get, nbr);

  if (Network::IsConnected())
    ASSERT(Network::GetInstance()->GetState() == WNet::NETWORK_LOADING_DATA
	   || Network::GetInstance()->GetState() == WNet::NETWORK_PLAYING);
#endif
  return nbr;
}

void RandomSyncGen::SetRand(uint seed)
{
#ifdef DEBUG
  nb_get = 0;
#endif
  RandomGenerator::SetRand(seed);
}

void RandomSyncGen::Verify()
{
  MSG_DEBUG("random.verify","Verify seed (%d)", GetSeed());
  bool turn_master = Network::GetInstance()->IsTurnMaster();
  ASSERT(turn_master);
  if (!turn_master)
    return;
  uint seed = GetSeed();
  Action* action = new Action(Action::ACTION_NETWORK_VERIFY_RANDOM_SYNC);
  action->Push((int)seed);
  ActionHandler::GetInstance()->NewAction(action);
}

RandomSyncGen& RandomSync()
{
  return (*RandomSyncGen::GetInstance());
}
