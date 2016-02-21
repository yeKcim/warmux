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
 * Mouse management
 *****************************************************************************/

#include "mouse.h"

#include "cursor.h"
#include "interface.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../map/camera.h"
#include "../team/macro.h"
#include "../tool/point.h"
#include "../weapon/weapon.h"

// Vitesse du definalement à la souris
const uint SCROLL_MOUSE = 20;

// Largeur de la zone de sensibilite au camera a la souris
const uint SENSIT_SCROLL_MOUSE = 40; // pixels

Mouse * Mouse::singleton = NULL;

Mouse * Mouse::GetInstance() {
  if (singleton == NULL) {
    singleton = new Mouse();
  }
  return singleton;
}

Mouse::Mouse(){
  scroll_actif = false;
}

void Mouse::Reset(){
}

bool Mouse::ActionClicD(){ 
  if( ActiveTeam().GetWeapon().CanChangeWeapon() )
    Interface::GetInstance()->weapons_menu.SwitchDisplay();
  
  return true;
}

bool Mouse::ActionClicG()
{
  const Point2i pos_monde = GetWorldPosition();   
	
  // Action dans le menu des armes ?
  if( Interface::GetInstance()->weapons_menu.ActionClic( GetPosition() ) )
    return true;

  // On peut changer de ver ?
  if( GameMode::GetInstance()->AllowCharacterSelection() ){
    // Sélection d'un ver se son équipe ?
    bool ver_choisi=false;
    Team::iterator it=ActiveTeam().begin(),
	                 fin=ActiveTeam().end();
    uint index=0;
    for( ; it != fin; ++it, ++index ){
      if( &(*it) != &ActiveCharacter()
        && !it -> IsDead() 
        && it->GetRect().Contains( pos_monde ) ){
        ver_choisi = true;
        break;
      }
    }

    if( ver_choisi ){
      ActiveTeam().SelectCharacterIndex (index);
      return true;
    }

    if( ActiveCharacter().GetRect().Contains( pos_monde ) ){
      CurseurVer::GetInstance()->SuitVerActif();
      return true;
    }
  }
  
  // Action dans le menu des armes ?
  if( Interface::GetInstance()->weapons_menu.ActionClic(GetPosition()) ) 
    return true;
  
  // Choosing target for a weapon, many posibilities :
  // - Do nothing
  // - Choose a target but don't fire
  // - Choose a target and fire it !
  if (GameLoop::GetInstance()->ReadState() == GameLoop::PLAYING) {
    ActiveTeam().AccessWeapon().ChooseTarget();
    return true ;
  }
  
  return false;
}

void Mouse::ChoixVerPointe(){
  if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING)
    return;

  const Point2i pos_monde = GetWorldPosition();
   
  // Quel ver est pointé par la souris ? (en dehors du ver actif)
  Interface::GetInstance()->character_under_cursor = NULL;
  FOR_ALL_LIVING_CHARACTERS(equipe,ver){
    if ((&(*ver) != &ActiveCharacter())
       && ver->GetRect().Contains(pos_monde) ){
      Interface::GetInstance()->character_under_cursor = &(*ver);
    }
  }
  
  // Aucun ver n'est pointé ... et le ver actif alors ?
  if ((Interface::GetInstance()->character_under_cursor == NULL)
      && ActiveCharacter().GetRect().Contains( pos_monde)){
      Interface::GetInstance()->character_under_cursor = &ActiveCharacter();
  }
  
  // Dessine le curseur autour du ver pointé s'il y en a un
//  if (interface.character_under_cursor != NULL) {
//    curseur_ver.PointeObj (interface.character_under_cursor);
//  } else {
//    curseur_ver.PointeAucunObj();
//  }
}

void Mouse::ScrollCamera() const{
	Point2i mousePos = GetPosition();
	Point2i sensitZone(SENSIT_SCROLL_MOUSE, SENSIT_SCROLL_MOUSE);
	Point2i winSize = AppWormux::GetInstance()->video.window.GetSize();
	Point2i tstVector;
	
	tstVector = mousePos.inf(sensitZone);
	if( !tstVector.IsNull() ){
		camera.SetXY( tstVector * (mousePos - sensitZone)/2 );
		camera.autorecadre = false;
	}

	tstVector = winSize.inf(mousePos + sensitZone);
	if( !tstVector.IsNull() ){
		camera.SetXY( tstVector * (mousePos + sensitZone - winSize)/2 );
		camera.autorecadre = false;
	}
}

void Mouse::TestCamera(){
  Point2i mousePos = GetPosition();
  
  //Move camera with mouse holding Ctrl key down
  const bool demande_scroll = SDL_GetModState() & KMOD_CTRL;
   
  if( demande_scroll ){
    if( scroll_actif ){
	  Point2i offset = savedPos - mousePos;
      camera.SetXY(offset);
      camera.autorecadre = false;
    }else{
      scroll_actif = true;
    }
	savedPos = mousePos;
    return;
  }else{
    scroll_actif = false;
  }

  if(!Interface::GetInstance()->weapons_menu.IsDisplayed())
    ScrollCamera();
}

void Mouse::Refresh(){
  if (!scroll_actif)
    ChoixVerPointe();
}

Point2i Mouse::GetPosition() const{
	int x, y;

	SDL_GetMouseState( &x, &y);
	return Point2i(x, y);
}

Point2i Mouse::GetWorldPosition() const{ 
   return GetPosition() + camera.GetPosition();
}

void Mouse::TraiteClic (const SDL_Event *event){
  if( event->type == SDL_MOUSEBUTTONDOWN ){
    if( event->button.button == SDL_BUTTON_RIGHT ){
      ActionClicD();
      return;
    }
	
    // Clic gauche de la souris ?
    if( event->button.button == SDL_BUTTON_LEFT ){
      ActionClicG();
      return;
    }
  }
}

