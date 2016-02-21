/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
 * Mouvement droite/gauche pour un character.
 *****************************************************************************/

#include "move.h"
#include <math.h>
#include "body.h"
#include "../team/teams_list.h"
#include "../game/config.h"
#include "../game/game_loop.h"
#include "../include/action_handler.h"
#include "../map/map.h"
#include "../map/camera.h"
#include "../network/network.h"
#include "../sound/jukebox.h"
#include "../tool/debug.h"

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

// Bouge un character characters la droite ou la gauche (selon le signe de direction)
void MoveCharacter(Character &character)
{
  int height;
  bool fantome;

  // On est bien dans le monde ? (sinon, pas besoin de tester !)
  if (character.GetDirection() == -1)
    fantome = character.IsOutsideWorld ( Point2i(-1, 0) );
  else
    fantome = character.IsOutsideWorld ( Point2i(1, 0) );
  if (fantome){
    MSG_DEBUG("ghost", "%s will be a ghost.", character.GetName().c_str());
    character.Ghost();
    return;
  }

  // Calcule la hauteur a descendre
  if (!ComputeHeightMovement (character, height, true)) return;

  do
  {
    // Bouge !
    GameLoop::GetInstance()->character_already_chosen = true;
    // Deplace enfin le character

    character.SetXY( Point2i(character.GetX() +character.GetDirection(),
                             character.GetY() +height) );

    // Gravite (s'il n'y a pas eu de collision
    character.UpdatePosition();

  }while(character.CanStillMoveDG(PAUSE_MOVEMENT) && ComputeHeightMovement (character, height, true));
}
// Move a character to the left
void MoveCharacterLeft(Character &character){
  // Le character est pret a bouger ?
  if (!character.MouvementDG_Autorise()) return;

  bool bouge = (character.GetDirection() == -1);
  if (bouge)
  {
    MoveCharacter(character);
  }
  else{
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_SET_CHARACTER_DIRECTION,-1));
    character.InitMouvementDG (PAUSE_CHG_DIRECTION);
  }

  //Refresh skin position across network
  if( !network.IsLocal() && (ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI()))
    SendCharacterPosition();
}

// Move a character to the right
void MoveCharacterRight (Character &character)
{
  // Le character est pret a bouger ?
  if (!character.MouvementDG_Autorise()) return;

  bool bouge = (character.GetDirection() == 1);
  if (bouge)
  {
    MoveCharacter(character);
  }
  else
  {
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_SET_CHARACTER_DIRECTION,1));
    character.InitMouvementDG (PAUSE_CHG_DIRECTION);
  }


  //Refresh skin position across network
  if( !network.IsLocal() && ActiveTeam().IsLocal())
    SendCharacterPosition();
}

void SendCharacterPosition()
{
  assert(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());
  Action* a = BuildActionSendCharacterPhysics(ActiveCharacter().GetTeamIndex(), ActiveCharacter().GetCharacterIndex());
  network.SendAction(a);
  delete a;

  Action a_set_skin(Action::ACTION_SET_SKIN,ActiveCharacter().body->GetClothe());
  a_set_skin.Push(ActiveCharacter().body->GetMovement());
  a_set_skin.Push((int)ActiveCharacter().body->GetFrame());
  network.SendAction(&a_set_skin);
}
