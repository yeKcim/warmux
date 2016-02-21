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
 * Base class for objects which can be moved by the user.
 *****************************************************************************/

#include "interface/movable_by_user.h"

template<typename T>
static void DeleteMatchingFromVector(const T * t, std::vector<const T *> & v)
{
  typename std::vector<const T*>::iterator it = v.begin();
  while (it != v.end()) {
    if (*it == t)
      v.erase(it);
    else
      it++;
  }
}

template<typename T>
static const T * GetLastOrNULL(std::vector<const T *> & v)
{
  if (v.size() == 0)
    return NULL;
  return v.back();
}

const LRMoveIntention * MovableByUser::GetLastLRMoveIntention()
{
  return GetLastOrNULL(lr_move_intentions);
}

void MovableByUser::AddLRMoveIntention(const LRMoveIntention * intention)
{
  lr_move_intentions.push_back(intention);
}

void MovableByUser::RemoveLRMoveIntention(const LRMoveIntention * intention)
{
  DeleteMatchingFromVector(intention, lr_move_intentions);
}

const UDMoveIntention * MovableByUser::GetLastUDMoveIntention()
{
  return GetLastOrNULL(ud_move_intentions);
}

void MovableByUser::AddUDMoveIntention(const UDMoveIntention * intention)
{
  ud_move_intentions.push_back(intention);
}

void MovableByUser::RemoveUDMoveIntention(const UDMoveIntention * intention)
{
  DeleteMatchingFromVector(intention, ud_move_intentions);
}
