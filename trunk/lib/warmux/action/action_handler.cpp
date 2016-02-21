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
 * Warmux action handler.
 *****************************************************************************/

#include <iostream>

#include <WARMUX_action_handler.h>
#include <WARMUX_distant_cpu.h>
#include <WARMUX_debug.h>
#include <WARMUX_error.h>

WActionHandler::callback_t WActionHandler::handlers[Action::NUM_ACTIONS];
std::string WActionHandler::action_names[Action::NUM_ACTIONS];

WActionHandler::WActionHandler()
{
  mutex = SDL_CreateMutex();
  SDL_LockMutex(mutex);
  SDL_UnlockMutex(mutex);
}

WActionHandler::~WActionHandler()
{
  if (mutex)
    SDL_DestroyMutex(mutex);
}

void WActionHandler::Flush()
{
  std::list<Action*>::iterator it;
  Lock();
  MSG_DEBUG("action_handler", "remove all actions");

  for (it = queue.begin(); it != queue.end() ;) {
    MSG_DEBUG("action_handler", "remove action %s", GetActionName((*it)->GetType()).c_str());
    delete *it;
    it = queue.erase(it);
  }
  UnLock();
}

void WActionHandler::NewAction(Action* a)
{
  ASSERT(mutex);
  Lock();
  MSG_DEBUG("action_handler", "New action : %s", GetActionName(a->GetType()).c_str());
  //  std::cout << "New action " << a->GetType() << std::endl ;
  queue.push_back(a);
  //  std::cout << "  queue_size " << queue.size() << std::endl;
  UnLock();
}
