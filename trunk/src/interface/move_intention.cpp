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
 * Describes which left or right movement the user wants to do with a character.
 *****************************************************************************/

#include "interface/move_intention.h"
#include <WARMUX_error.h>

static const LRMoveIntention LEFT_SLOWLY_OBJECT(DIRECTION_LEFT, true);
static const LRMoveIntention LEFT_OBJECT(DIRECTION_LEFT, false);
static const LRMoveIntention RIGHT_SLOWLY_OBJECT(DIRECTION_RIGHT, true);
static const LRMoveIntention RIGHT_OBJECT(DIRECTION_RIGHT, false);

static const UDMoveIntention UP_SLOWLY_OBJECT(DIRECTION_UP, true);
static const UDMoveIntention UP_OBJECT(DIRECTION_UP, false);
static const UDMoveIntention DOWN_SLOWLY_OBJECT(DIRECTION_DOWN, true);
static const UDMoveIntention DOWN_OBJECT(DIRECTION_DOWN, false);


const LRMoveIntention * const INTENTION_MOVE_LEFT_SLOWLY = &LEFT_SLOWLY_OBJECT;
const LRMoveIntention * const INTENTION_MOVE_LEFT = &LEFT_OBJECT;
const LRMoveIntention * const INTENTION_MOVE_RIGHT_SLOWLY = &RIGHT_SLOWLY_OBJECT;
const LRMoveIntention * const INTENTION_MOVE_RIGHT = &RIGHT_OBJECT;

const UDMoveIntention * const INTENTION_MOVE_UP_SLOWLY = &UP_SLOWLY_OBJECT;
const UDMoveIntention * const INTENTION_MOVE_UP = &UP_OBJECT;
const UDMoveIntention * const INTENTION_MOVE_DOWN_SLOWLY = &DOWN_SLOWLY_OBJECT;
const UDMoveIntention * const INTENTION_MOVE_DOWN = &DOWN_OBJECT;
