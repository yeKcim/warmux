/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include "teams_list.h"
#include "../game/config.h"
#include "../game/game_loop.h"
#include "../include/action_handler.h"
#include "../map/map.h"
#include "../map/camera.h"
#include "../network/network.h"
#include "../sound/jukebox.h"
#include "../tool/debug.h"

// Hauteur qu'un character peut grimper au maximum
#define HAUTEUR_GRIMPE_MAX 30

// Hauteur qu'un character peut chute (sans appeler la fonction Gravite)
#define HAUTEUR_CHUTE_MAX 20

// Pause entre deux deplacement
#define PAUSE_CHG_SENS 80 // ms

// Calcule la hauteur a chuter ou grimper lors d'un déplacement horizontal
// Renvoie true si le mouvement est possible
bool CalculeHauteurBouge (Character &character, int &hauteur){
  int y_floor=character.GetY();

  if( character.IsInVacuum( Point2i(character.GetDirection(), 0))
  && !character.IsInVacuum( Point2i(character.GetDirection(), +1)) ){
    //Land is flat, we can move!
    hauteur = 0;
    return true;
  }

  //Compute height of the step:
  if( character.IsInVacuum( Point2i(character.GetDirection(), 0)) ){
    //Try to go down:
    for(hauteur = 2; hauteur <= HAUTEUR_CHUTE_MAX ; hauteur++){
      if( !character.IsInVacuum(Point2i(character.GetDirection(), hauteur))
      ||  character.FootsOnFloor(y_floor+hauteur)){
        hauteur--;
        return true;
      }
    }
    //We can go down, but the step is to big -> the character will fall.
    character.SetX (character.GetX() +character.GetDirection());
    character.UpdatePosition();
    character.SetSkin("fall");
    return false;
  }
  else{
    //Try to go up:
    for(hauteur = -1; hauteur >= -HAUTEUR_GRIMPE_MAX ; hauteur--)
      if( character.IsInVacuum( Point2i(character.GetDirection(), hauteur) ) )
        return true;
  }
  //We can't move!
  return false;
}

// Bouge un character characters la droite ou la gauche (selon le signe de direction)
void MoveCharacter(Character &character){
  int hauteur;
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
  if (!CalculeHauteurBouge (character, hauteur)) return;

  do
  {
    // Bouge !
    GameLoop::GetInstance()->character_already_chosen = true;
    // Deplace enfin le character

    character.SetXY( Point2i(character.GetX() +character.GetDirection(),
                             character.GetY() +hauteur) );

    // Gravite (s'il n'y a pas eu de collision
    character.UpdatePosition();

    // Passe a l'image suivante
    character.FrameImageSuivante();

  }while(character.CanStillMoveDG(PAUSE_BOUGE) && CalculeHauteurBouge (character, hauteur));

//    character.UpdatePosition();

}
// Move a character to the left
void MoveCharacterLeft(Character &character){
  // Le character est pret a bouger ?
  if (!character.MouvementDG_Autorise()) return;

  bool bouge = (character.GetDirection() == -1);
  if (bouge)
  {
//    ActionHandler::GetInstance()->NewAction(Action(ACTION_WALK));
    MoveCharacter(character);
  }
  else{
    ActionHandler::GetInstance()->NewAction(ActionInt(ACTION_SET_CHARACTER_DIRECTION,-1));
    character.InitMouvementDG (PAUSE_CHG_SENS);
  }

  //Refresh skin position across network
  if( !network.is_local() && ActiveTeam().is_local)
  {
    network.SendAction(ActionInt2(ACTION_MOVE_CHARACTER,character.GetX(),character.GetY()));
    network.SendAction(ActionString(ACTION_SET_SKIN,character.current_skin));
    network.SendAction(ActionInt(ACTION_SET_FRAME,character.image->GetCurrentFrame()));
  }
}

// Move a character to the right
void MoveCharacterRight (Character &character){ 
  // Le character est pret a bouger ?
  if (!character.MouvementDG_Autorise()) return;

  bool bouge = (character.GetDirection() == 1);
  if (bouge)
  {
//    ActionHandler::GetInstance()->NewAction(Action(ACTION_WALK));
    MoveCharacter(character);
  }
  else
  {
    ActionHandler::GetInstance()->NewAction(ActionInt(ACTION_SET_CHARACTER_DIRECTION,1));
    character.InitMouvementDG (PAUSE_CHG_SENS);
  }


  //Refresh skin position across network
  if( !network.is_local() && ActiveTeam().is_local)
  {
    network.SendAction(ActionInt2(ACTION_MOVE_CHARACTER,character.GetX(),character.GetY()));
    network.SendAction(ActionString(ACTION_SET_SKIN,character.current_skin));
    network.SendAction(ActionInt(ACTION_SET_FRAME,character.image->GetCurrentFrame()));
  }
}

