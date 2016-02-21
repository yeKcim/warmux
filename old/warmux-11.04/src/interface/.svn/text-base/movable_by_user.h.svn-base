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
 * Base class for objects which can be moved by the user.
 *****************************************************************************/

#ifndef MOVABLE_BY_USER_H
#define MOVABLE_BY_USER_H

#include <list>
#include "interface/move_intention.h"

class MovableByUser
{
protected:
  std::list<const LRMoveIntention *> lr_move_intentions;
  std::list<const UDMoveIntention *> ud_move_intentions;
public:
  const LRMoveIntention * GetLastLRMoveIntention() const
  {
    return (lr_move_intentions.empty()) ? NULL : lr_move_intentions.back();
  }
  void AddLRMoveIntention(const LRMoveIntention * intention)
  {
    lr_move_intentions.push_back(intention);
  }
  void RemoveLRMoveIntention(const LRMoveIntention * intention)
  {
    lr_move_intentions.remove(intention);
  }

  const UDMoveIntention * GetLastUDMoveIntention() const
  {
    return (ud_move_intentions.empty()) ? NULL : ud_move_intentions.back();
  }
  void AddUDMoveIntention(const UDMoveIntention * intention)
  {
    ud_move_intentions.push_back(intention);
  }
  void RemoveUDMoveIntention(const UDMoveIntention * intention)
  {
    ud_move_intentions.remove(intention);
  }
};

#endif
