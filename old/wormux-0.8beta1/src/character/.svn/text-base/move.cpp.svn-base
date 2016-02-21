/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either charactersion 2 of the License, or
 *  (at your option) any later charactersion.
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
 * Mouvement right/left for a character.
 *****************************************************************************/

#include "move.h"
#include <math.h>
#include "body.h"
#include "team/teams_list.h"
#include "game/config.h"
#include "game/game_loop.h"
#include "include/action_handler.h"
#include "map/map.h"
#include "map/camera.h"
#include "network/network.h"
#include "sound/jukebox.h"
#include "tool/debug.h"

// Max climbing height walking
const int MAX_CLIMBING_HEIGHT=30;

// Max height for which we do not need to call the Physical Engine with gravity features
const int MAX_FALLING_HEIGHT=20;

// Pause between changing direction
const uint PAUSE_CHG_DIRECTION=80; // ms

// Compute the height to fall or to walk on when moving horizontally
// Return a boolean which says if movement is possible
bool ComputeHeightMovement(Character &character, int &height,
			   bool falling)
{
  int y_floor=character.GetY();

  if( character.IsInVacuum( Point2i(character.GetDirection(), 0))
  && !character.IsInVacuum( Point2i(character.GetDirection(), +1)) ){
    //Land is flat, we can move!
    height = 0;
    return true;
  }

  //Compute height of the step:
  if( character.IsInVacuum( Point2i(character.GetDirection(), 0)) ){
    //Try to go down:
    for(height = 2; height <= MAX_FALLING_HEIGHT ; height++){
      if( !character.IsInVacuum(Point2i(character.GetDirection(), height))
      ||  character.FootsOnFloor(y_floor+height)){
        height--;
        return true;
      }
    }
    //We can go down, but the step is to big -> the character will fall.
    if (falling) {
      character.SetX (character.GetX() +character.GetDirection());
      character.UpdatePosition();
      character.SetMovement("fall");
    }
    return false;
  }
  else{
    //Try to go up:
    for(height = -1; height >= -MAX_CLIMBING_HEIGHT ; height--)
      if( character.IsInVacuum( Point2i(character.GetDirection(), height) ) )
        return true;
  }
  //We can't move!
  return false;
}

// Moves a character to the right/left depending the signess of direction
void MoveCharacter(Character &character)
{
  int height;
  bool ghost;

  // If character moves out of the world, no need to go further: it is dead
  if (character.GetDirection() == -1)
    ghost = character.IsOutsideWorld ( Point2i(-1, 0) );
  else
    ghost = character.IsOutsideWorld ( Point2i(1, 0) );
  if (ghost){
    MSG_DEBUG("ghost", "%s will be a ghost.", character.GetName().c_str());
    character.Ghost();
    return;
  }

  // Compute fall heigth
  if (!ComputeHeightMovement (character, height, true)) return;

  do
  {
    // Move !
    GameLoop::GetInstance()->character_already_chosen = true;
    // Eventually moves the character

    character.SetXY( Point2i(character.GetX() +character.GetDirection(),
                             character.GetY() +height) );

    // If no collision, let gravity do its job
    character.UpdatePosition();

  } while (character.CanStillMoveRL(PAUSE_MOVEMENT) && ComputeHeightMovement (character, height, true));
}

// Move the active character to the left
void MoveActiveCharacterLeft(){
  // character is ready to move ?
  if (!ActiveCharacter().CanMoveRL()) return;

  bool move = (ActiveCharacter().GetDirection() == Body::DIRECTION_LEFT);
  if (move) {
    MoveCharacter(ActiveCharacter());
  } else {
    ActiveCharacter().SetDirection(Body::DIRECTION_LEFT);
    ActiveCharacter().BeginMovementRL(PAUSE_CHG_DIRECTION);
  }

  //Refresh skin position across network
  if (!Network::GetInstance()->IsLocal() && (ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI()))
    SendActiveCharacterInfo();
}

// Move the active character to the right
void MoveActiveCharacterRight()
{
  // character is ready to move ?
  if (!ActiveCharacter().CanMoveRL()) return;

  bool move = (ActiveCharacter().GetDirection() == Body::DIRECTION_RIGHT);
  if (move) {
    MoveCharacter(ActiveCharacter());
  } else {
    ActiveCharacter().SetDirection(Body::DIRECTION_RIGHT);
    ActiveCharacter().BeginMovementRL(PAUSE_CHG_DIRECTION);
  }

  //Refresh skin position across network
  if (!Network::GetInstance()->IsLocal() && ActiveTeam().IsLocal())
    SendActiveCharacterInfo();
}

