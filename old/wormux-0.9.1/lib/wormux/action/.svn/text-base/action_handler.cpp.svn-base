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
 * Wormux action handler.
 *****************************************************************************/

#include <iostream>
#include <SDL_mutex.h>
#include <WORMUX_action_handler.h>
#include <WORMUX_debug.h>
#include <WORMUX_error.h>

WActionHandler::WActionHandler():
  handler(),
  action_name(),
  queue()
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

void WActionHandler::Lock()
{
  SDL_LockMutex(mutex);
}

void WActionHandler::UnLock()
{
  SDL_UnlockMutex(mutex);
}

void WActionHandler::Flush()
{
  std::list<Action*>::iterator it;
  Lock();
  MSG_DEBUG("action_handler","remove all actions");

  for (it = queue.begin(); it != queue.end() ;)
  {
    MSG_DEBUG("action_handler","remove action %s", GetActionName((*it)->GetType()).c_str());
    delete *it;
    it = queue.erase(it);
  }
  UnLock();
}

void WActionHandler::NewAction(Action* a)
{
  ASSERT(mutex!=NULL);
  Lock();
  MSG_DEBUG("action_handler","New action : %s", GetActionName(a->GetType()).c_str());
  //  std::cout << "New action " << a->GetType() << std::endl ;
  queue.push_back(a);
  //  std::cout << "  queue_size " << queue.size() << std::endl;
  UnLock();
}

// To call when locked
void WActionHandler::Register (Action::Action_t action,
                              const std::string &name,
                              callback_t fct)
{
  handler[action] = fct;
  action_name[action] = name;
}

void WActionHandler::Exec(Action *a)
{
#ifdef WMX_LOG
  int id=rand();
#endif

  MSG_DEBUG("action_handler", "-> (%d) Executing action %s", id, GetActionName(a->GetType()).c_str());
  handler_it it=handler.find(a->GetType());
  NET_ASSERT(it != handler.end())
  {
    if(a->GetCreator()) a->GetCreator()->ForceDisconnection();
    return;
  }
  (*it->second) (a);
  MSG_DEBUG("action_handler", "<- (%d) Executing action %s", id, GetActionName(a->GetType()).c_str());
}

const std::string &WActionHandler::GetActionName (Action::Action_t action) const
{
  ASSERT(mutex!=NULL);
  SDL_LockMutex(mutex);
  name_it it=action_name.find(action);
  ASSERT(it != action_name.end());
  SDL_UnlockMutex(mutex);
  return it->second;
}


