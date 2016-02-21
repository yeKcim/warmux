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
 * Define singleton base class members
 *****************************************************************************/

#include <SDL_mutex.h>
#include <WORMUX_singleton.h>

SingletonList singletons;

SDL_mutex* BaseSingleton::mutex = NULL;

BaseSingleton::BaseSingleton()
{
  // Usually the game loading makes the first calls to this constructor serial.
  if (!mutex) mutex = SDL_CreateMutex();

  SDL_LockMutex(mutex);
  singletons.push_back(this);
  SDL_UnlockMutex(mutex);

  MSG_DEBUG("singleton", "Added singleton %p", this);
}

BaseSingleton::~BaseSingleton()
{
  SDL_LockMutex(mutex);
  singletons.remove(this);
  SDL_UnlockMutex(mutex);

  MSG_DEBUG("singleton", "Removed singleton %p", this);
}

void BaseSingleton::ReleaseSingletons()
{
  SingletonList::iterator it = singletons.begin();

  while (it != singletons.end()) {
    MSG_DEBUG("singleton", "Releasing singleton %p of type %s", *it, typeid(*it).name());
    delete (*it);
    it = singletons.begin();
  }

  if (mutex)
    SDL_DestroyMutex(mutex);
  mutex = NULL;
}

