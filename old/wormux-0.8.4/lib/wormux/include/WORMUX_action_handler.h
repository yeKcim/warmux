/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef WORMUX_ACTION_HANDLER_H
#define WORMUX_ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include <map>
#include <list>
#include <WORMUX_action.h>
#include <WORMUX_singleton.h>
#include <WORMUX_types.h>
//-----------------------------------------------------------------------------

// Forward declarations
struct SDL_mutex;

class WActionHandler
{
private:
  /* If you need this, you probably made an error in your code... */
  WActionHandler(const WActionHandler&);
  const WActionHandler& operator=(const WActionHandler&);
  /****************************************************************/

  // Mutex needed to be thread safe for the network
  SDL_mutex* mutex;

  // Handler for each action
  typedef void (*callback_t) (Action *a);
  std::map<Action::Action_t, callback_t> handler;
  typedef std::map<Action::Action_t, callback_t>::const_iterator handler_it;

  // Action strings
  std::map<Action::Action_t, std::string> action_name;
  typedef std::map<Action::Action_t, std::string>::const_iterator name_it;

protected:
  WActionHandler();
  ~WActionHandler();

  // Action queue
  std::list<Action*> queue;

  void Exec(Action *a);
  void NewAction(Action* a);

public:
  void Flush();
  void ExecActions();

  void Lock();
  void UnLock();

  // To call when locked
  void Register(Action::Action_t action, const std::string &name, callback_t fct);

  const std::string &GetActionName(Action::Action_t action) const;
};

#endif
