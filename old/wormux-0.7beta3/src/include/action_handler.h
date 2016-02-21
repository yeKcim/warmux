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
 * Define all Wormux actions.
 *****************************************************************************/

#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H
//-----------------------------------------------------------------------------
#include "base.h"
#include "action.h"
#include <map>
#include <list>
//-----------------------------------------------------------------------------

class ActionHandler
{
private:
	// Handler for each action
	typedef void (*callback_t) (const Action *a);
	std::map<Action_t, callback_t> handler;
	typedef std::map<Action_t, callback_t>::const_iterator handler_it;

	// Action strings
	std::map<Action_t, std::string> action_name;
	typedef std::map<Action_t, std::string>::const_iterator name_it;

	// Action queue
	std::list<Action*> queue;
	
public:
	void NewAction (const Action &a, bool repeat_to_network=true);
	void ExecActions ();
	void Init();
	std::string GetActionName(Action_t action);
	
private:
	void Exec (const Action *a);
	void Register (Action_t action, const std::string &name, callback_t fct);
};

extern ActionHandler action_handler;

//-----------------------------------------------------------------------------
#endif
