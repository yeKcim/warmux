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
//-----------------------------------------------------------------------------
#include "../team/macro.h"
#include "../game/game_mode.h"
#include "../game/game_loop.h"
#include "../game/time.h"
#include "../map/camera.h"
#include "../graphic/video.h"
#include "../include/constant.h" // FOND_X, FOND_Y
#include "interface.h"
#include "cursor.h"
#include "../weapon/weapon.h"
#include "../tool/Point.h"

using namespace Wormux;
//-----------------------------------------------------------------------------
Mouse mouse;
//-----------------------------------------------------------------------------

// Vitesse du definalement à la souris
const uint SCROLL_MOUSE = 20;

// Largeur de la zone de sensibilite au camera a la souris
const uint SENSIT_SCROLL_MOUSE = 40; // pixels

// Active le mode tricheur ?
#ifdef DEBUG
#  define MODE_TRICHEUR
#endif

//-----------------------------------------------------------------------------

Mouse::Mouse()
{
  scroll_actif = false;
}


//-----------------------------------------------------------------------------

void Mouse::Reset()
{}

//-----------------------------------------------------------------------------

bool Mouse::ActionClicD()
{ 
  if ( ActiveTeam().GetWeapon().CanChangeWeapon() )
  {
    interface.weapons_menu.SwitchDisplay();
  }
  return true;
}

//-----------------------------------------------------------------------------

bool Mouse::ActionClicG()
{
  const Point2i pos_monde = GetPosMonde();   
	
  // Action dans le menu des armes ?
  if (interface.weapons_menu.ActionClic (GetX(),GetY())) 
  {
    return true;
  } 

  // On peut changer de ver ?
  if (game_mode.AllowCharacterSelection())
  {
    // Sélection d'un ver se son équipe ?
    bool ver_choisi=false;
    Team::iterator it=ActiveTeam().begin(),
	                 fin=ActiveTeam().end();
    uint index=0;
    for (; it != fin; ++it, ++index)
    {
      if (&(*it) != &ActiveCharacter()
        && !it -> IsDead() 
        && it->GetRect().Contains( pos_monde ))
      {
        ver_choisi = true;
        break;
      }
  }

  if (ver_choisi)
  {
    ActiveTeam().SelectCharacterIndex (index);
    return true;
  }

  if (ActiveCharacter().GetRect().Contains( pos_monde ))
  {
    curseur_ver.SuitVerActif();
    return true;
  }
    }
  
  // Action dans le menu des armes ?
  if (interface.weapons_menu.ActionClic (GetX(),GetY())) 
    {
      return true;
    }
  
  // Choosing target for a weapon, many posibilities :
  // - Do nothing
  // - Choose a target but don't fire
  // - Choose a target and fire it !
  if (game_loop.ReadState() == gamePLAYING) {
    ActiveTeam().AccessWeapon().ChooseTarget();
    return true ;
  }
  
  return false;
}

//-----------------------------------------------------------------------------

void Mouse::ChoixVerPointe()
{
  if (game_loop.ReadState() != gamePLAYING) return;

  const Point2i pos_monde = GetPosMonde();
   
  // Quel ver est pointé par la souris ? (en dehors du ver actif)
  interface.character_under_cursor = NULL;
  POUR_TOUS_VERS_VIVANTS(equipe,ver)
    {
      if ((&(*ver) != &ActiveCharacter())
	  && ver->GetRect().Contains(pos_monde) )
	{
	  interface.character_under_cursor = &(*ver);
	}
    }
  
  // Aucun ver n'est pointé ... et le ver actif alors ?
  if ((interface.character_under_cursor == NULL)
      && ActiveCharacter().GetRect().Contains( pos_monde))  
    {
      interface.character_under_cursor = &ActiveCharacter();
    }
  
  // Dessine le curseur autour du ver pointé s'il y en a un
//  if (interface.character_under_cursor != NULL) {
//    curseur_ver.PointeObj (interface.character_under_cursor);
//  } else {
//    curseur_ver.PointeAucunObj();
//  }
}

//-----------------------------------------------------------------------------
 
void Mouse::ScrollCamera() const
{
  int x = GetX();
  int y = GetY();
  //Move camera with mouse when cursor is on border of the screen
  int dx = x-SENSIT_SCROLL_MOUSE;
  if(dx<0) {
    camera.SetXY(dx/2,0);
    camera.autorecadre = false;
  }
  dx=video.GetWidth()-x-SENSIT_SCROLL_MOUSE;
  if(dx<0) {
    camera.SetXY(-dx/2,0);
    camera.autorecadre = false;
  }
  
  int dy = y-SENSIT_SCROLL_MOUSE;
  if(dy<0) {
    camera.SetXY(0,dy/2);
    camera.autorecadre = false;
  }
  dy = video.GetHeight()-y-SENSIT_SCROLL_MOUSE;
  if(dy<0) {
    camera.SetXY(0,-dy/2);
    camera.autorecadre = false;
  }
}

void Mouse::TestCamera()
{
   int _x, _y;       
   SDL_GetMouseState( &_x, &_y);
   
   //Move camera with mouse holding Ctrl key down
   const bool demande_scroll = SDL_GetModState() & KMOD_CTRL;
   
   if (demande_scroll)
  {
    if (scroll_actif) {
      int dx = sauve_x - _x;
      int dy = sauve_y - _y;
      camera.SetXY(dx,dy);
      camera.autorecadre = false;
    } else {
      scroll_actif = true;
    }
    sauve_x = _x;
    sauve_y = _y;
     return;
  } else {
    scroll_actif = false;
  }

  if(!interface.weapons_menu.IsDisplayed()) ScrollCamera();
}

//-----------------------------------------------------------------------------

void Mouse::Refresh()
{
  if (!scroll_actif) ChoixVerPointe();
}

//-----------------------------------------------------------------------------

int Mouse::GetX() const 
{
   int x;
   
   SDL_GetMouseState( &x, NULL);
   return x; 
}

int Mouse::GetY() const 
{ 
   int y;
   
   SDL_GetMouseState( NULL, &y);
   return y; 
}

int Mouse::GetXmonde() const 
{ 
   return GetX() -FOND_X +camera.GetX(); 
}

int Mouse::GetYmonde() const 
{ 
   return GetY() -FOND_Y +camera.GetY(); 
}


Point2i Mouse::GetPosMonde() const
{ 
   return Point2i (GetXmonde(), GetYmonde());
}



//-----------------------------------------------------------------------------

void Mouse::TraiteClic (const SDL_Event *event)
{
   if ( event->type == SDL_MOUSEBUTTONDOWN )
     {
	
	if ( event->button.button == SDL_BUTTON_RIGHT)
	  {
	     ActionClicD();
	     return;
	  }
	
	// Clic gauche de la souris ?
	if ( event->button.button == SDL_BUTTON_LEFT)
	  {
	     ActionClicG();
	     return;
	  }
     }
}

//-----------------------------------------------------------------------------
