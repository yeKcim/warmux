/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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
#include "character/character.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game_loop.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/point.h"
#include "tool/resource_manager.h"
#include "weapon/weapon.h"

// XXX Unused mouse scroll speed
// const uint SCROLL_MOUSE = 20;

// size of the sensitive area use to scroll the map with mouse
const uint SENSIT_SCROLL_MOUSE = 40;

Mouse * Mouse::singleton = NULL;

Mouse * Mouse::GetInstance() {
  if (singleton == NULL) {
    singleton = new Mouse();
  }
  return singleton;
}

Mouse::Mouse(){
  scroll_actif = false;
  visible = MOUSE_VISIBLE;
  // Load the different pointers
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  pointer_select = resource_manager.LoadImage(res, "mouse/pointer_select");
  pointer_move = resource_manager.LoadImage(res, "mouse/pointer_move");
  pointer_arrow_up = resource_manager.LoadImage(res, "mouse/pointer_arrow_up");
  pointer_arrow_up_right = resource_manager.LoadImage(res, "mouse/pointer_arrow_up_right");
  pointer_arrow_up_left = resource_manager.LoadImage(res, "mouse/pointer_arrow_up_left");
  pointer_arrow_down = resource_manager.LoadImage(res, "mouse/pointer_arrow_down");
  pointer_arrow_down_right = resource_manager.LoadImage(res, "mouse/pointer_arrow_down_right");
  pointer_arrow_down_left = resource_manager.LoadImage(res, "mouse/pointer_arrow_down_left");
  pointer_arrow_right = resource_manager.LoadImage(res, "mouse/pointer_arrow_right");
  pointer_arrow_left = resource_manager.LoadImage(res, "mouse/pointer_arrow_left");
  pointer_aim = resource_manager.LoadImage(res, "mouse/pointer_aim");
  pointer_fire_right = resource_manager.LoadImage(res, "mouse/pointer_fire_right");
  pointer_fire_left = resource_manager.LoadImage(res, "mouse/pointer_fire_left");
  current_pointer = POINTER_STANDARD;
  resource_manager.UnLoadXMLProfile(res);
}

void Mouse::ActionLeftClic(bool) const
{
  const Point2i pos_monde = GetWorldPosition();

  // Action in weapon menu ?
  if( Interface::GetInstance()->weapons_menu.ActionClic( GetPosition() ) )
    return;

  //Change character by mouse click only if the choosen weapon allows it
  if( GameMode::GetInstance()->AllowCharacterSelection() &&
      ActiveTeam().GetWeapon().mouse_character_selection) {

    // Choose a character of our own team
    bool character_found=false;
    Team::iterator it=ActiveTeam().begin(),
                         fin=ActiveTeam().end();

    for( ; it != fin; ++it) {
      if( &(*it) != &ActiveCharacter()
        && !it -> IsDead()
        && it->GetRect().Contains( pos_monde ) ){

        character_found = true;
        break;
      }
    }

    if (character_found) {
      Action * next_character = new Action(Action::ACTION_PLAYER_NEXT_CHARACTER);
      next_character->StoreActiveCharacter();

      while ( &(*it) != &ActiveCharacter() )
        ActiveTeam().NextCharacter ();

      next_character->StoreActiveCharacter();
      ActionHandler::GetInstance()->NewAction(next_character);

      return;
    }

    if( ActiveCharacter().GetRect().Contains( pos_monde ) ){
      CharacterCursor::GetInstance()->FollowActiveCharacter();
      return;
    }
  }

  // Choosing target for a weapon, many posibilities :
  // - Do nothing
  // - Choose a target but don't fire
  // - Choose a target and fire it !
  Action* a = new Action(Action::ACTION_WEAPON_SET_TARGET);
  a->Push(GetWorldPosition());
  ActionHandler::GetInstance()->NewAction (a);
}


void Mouse::ActionRightClic(bool) const
{
  Interface::GetInstance()->weapons_menu.SwitchDisplay();
}

void Mouse::ActionWheelUp(bool shift) const
{
  ActiveTeam().AccessWeapon().HandleMouseWheelUp(shift);
}

void Mouse::ActionWheelDown(bool shift) const
{
  ActiveTeam().AccessWeapon().HandleMouseWheelDown(shift);
}

bool Mouse::HandleClic (const SDL_Event& event) const
{
  if ( event.type != SDL_MOUSEBUTTONDOWN &&
       event.type != SDL_MOUSEBUTTONUP ) {
    return false ;
  }

  if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING)
    return true;

  if(!ActiveTeam().IsLocal())
    return true;

  if( event.type == SDL_MOUSEBUTTONDOWN ){
    bool shift = !!(SDL_GetModState() & KMOD_SHIFT);
    switch (event.button.button) {
    case SDL_BUTTON_RIGHT:
      ActionRightClic(shift);
      break;
    case SDL_BUTTON_LEFT:
      ActionLeftClic(shift);
      break;
    case SDL_BUTTON_WHEELDOWN:
      ActionWheelDown(shift);
      break;
    case SDL_BUTTON_WHEELUP:
      ActionWheelUp(shift);
      break;
    default:
      break;
    }
  }
  return true;
}

void Mouse::ChoixVerPointe() const
{
  if (GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING)
    return;

  const Point2i pos_monde = GetWorldPosition();

  // Which character is pointed by the mouse ? (appart from the active one)
  Interface::GetInstance()->character_under_cursor = NULL;
  FOR_ALL_LIVING_CHARACTERS(equipe,ver){
    if ((&(*ver) != &ActiveCharacter())
       && ver->GetRect().Contains(pos_monde) ){
      Interface::GetInstance()->character_under_cursor = &(*ver);
    }
  }

  // No character is pointed... what about the active one ?
  if ((Interface::GetInstance()->character_under_cursor == NULL)
      && ActiveCharacter().GetRect().Contains( pos_monde)){
      Interface::GetInstance()->character_under_cursor = &ActiveCharacter();
  }

  // Draws the cursor arround the pointed character il any
//  if (interface.character_under_cursor != NULL) {
//    curseur_ver.PointeObj (interface.character_under_cursor);
//  } else {
//    curseur_ver.PointeAucunObj();
//  }
}

void Mouse::ScrollCamera() const
{
  // XXX Not used
  // bool scroll = false;

  Point2i mousePos = GetPosition();
  Point2i winSize = AppWormux::GetInstance()->video->window.GetSize();
  Point2i tstVector;
  // If application is fullscreen, mouse is only sensitive when touching the border screen
  int coef = (AppWormux::GetInstance()->video->IsFullScreen() ? 10 : 1);
  Point2i sensitZone(SENSIT_SCROLL_MOUSE / coef, SENSIT_SCROLL_MOUSE / coef);

  tstVector = mousePos.inf(sensitZone);
  if( !tstVector.IsNull() ){
    Camera::GetInstance()->GetInstance()->SetXY( tstVector * (mousePos - (sensitZone * coef))/2 );
    Camera::GetInstance()->GetInstance()->SetAutoCrop(false);
    // XXX Not used
    // scroll = true;
  }

  tstVector = winSize.inf(mousePos + sensitZone);
  if( !tstVector.IsNull() ){
    Camera::GetInstance()->GetInstance()->SetXY( tstVector * (mousePos + (sensitZone * coef) - winSize)/2 );
    Camera::GetInstance()->GetInstance()->SetAutoCrop(false);
    // XXX Not used
    //scroll = true;
  }

}

void Mouse::TestCamera()
{
  Point2i mousePos = GetPosition();
  int x,y;
  //Move camera with mouse holding Ctrl key down or with middle button of mouse
  const bool demande_scroll = SDL_GetModState() & KMOD_CTRL ||
                              SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_MIDDLE);

  // Show cursor and information interface when moving mouse
  if (lastPos != mousePos) {
    MSG_DEBUG("mouse", "Before: %d, %d -> now: %d, %d",
            lastPos.GetX(), lastPos.GetY(),
            mousePos.GetX(), mousePos.GetY());

    Show();
    Interface::GetInstance()->Show();
    lastPos = mousePos;
  }

  if( demande_scroll ){
    if( scroll_actif ){
      Point2i offset = savedPos - mousePos;
      Camera::GetInstance()->GetInstance()->SetXY(offset);
      Camera::GetInstance()->GetInstance()->SetAutoCrop(false);
    }else{
      scroll_actif = true;
    }
    savedPos = mousePos;
    return;
  }else{
    scroll_actif = false;
  }

  if(!Interface::GetInstance()->weapons_menu.IsDisplayed() &&
     Config::GetInstance()->GetScrollOnBorder())
    ScrollCamera();
}

void Mouse::Refresh()
{
  if (!scroll_actif)
    ChoixVerPointe();

  Point2i pos = GetPosition();
  if (visible == MOUSE_HIDDEN_UNTIL_NEXT_MOVE
      && lastPos != pos) {
    MSG_DEBUG("mouse", "Show : %d, %d - %d, %d", lastPos.GetX(),
              lastPos.GetY(), pos.GetX(), pos.GetY() );
    Show();
  }
}

Point2i Mouse::GetPosition() const
{
  int x, y;

  SDL_GetMouseState( &x, &y);
  return Point2i(x, y);
}

Point2i Mouse::GetWorldPosition() const
{
  return GetPosition() + Camera::GetInstance()->GetPosition();
}

// set the new pointer type and return the previous one
Mouse::pointer_t Mouse::SetPointer(pointer_t pointer)
{
  if (Config::GetInstance()->GetDefaultMouseCursor()) {
    Show(); // be sure cursor is visible
    return current_pointer;
  }

  if (current_pointer == pointer) return current_pointer;

  if (pointer == POINTER_STANDARD) SDL_ShowCursor(true);
  else SDL_ShowCursor(false);

  pointer_t old_pointer = current_pointer;
  current_pointer = pointer;

  return old_pointer;
}

const Surface& Mouse::GetSurfaceFromPointer(pointer_t pointer) const
{
  switch (pointer) {
  case POINTER_STANDARD:
    return pointer_select;
  case POINTER_SELECT:
    return pointer_select;
  case POINTER_MOVE:
    return pointer_move;
  case POINTER_ARROW_UP:
    return pointer_arrow_up;
  case POINTER_ARROW_UP_RIGHT:
    return pointer_arrow_up_right;
  case POINTER_ARROW_UP_LEFT:
    return pointer_arrow_up_left;
  case POINTER_ARROW_DOWN:
    return pointer_arrow_down;
  case POINTER_ARROW_DOWN_RIGHT:
    return pointer_arrow_down_right;
  case POINTER_ARROW_DOWN_LEFT:
    return pointer_arrow_down_left;
  case POINTER_ARROW_RIGHT:
    return pointer_arrow_right;
  case POINTER_ARROW_LEFT:
    return pointer_arrow_left;
  case POINTER_AIM:
    return pointer_aim;
  case POINTER_FIRE:
    if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
      return pointer_fire_left; // left hand to shoot on the right
    else
      return pointer_fire_right;
  case POINTER_FIRE_RIGHT:
    return pointer_fire_left;
  case POINTER_FIRE_LEFT:
    return pointer_fire_right;
  }

  // to make g++ happy
  return pointer_select;
}


// Return POINTER_STANDARD if it does not need a special
// arrow cursor
Mouse::pointer_t Mouse::ScrollPointer() const
{
  if (!Config::GetInstance()->GetScrollOnBorder() ||
      Interface::GetInstance()->weapons_menu.IsDisplayed())
    return POINTER_STANDARD;

  Point2i mousePos = GetPosition();
  Point2i winSize = AppWormux::GetInstance()->video->window.GetSize();
  Point2i cameraPos = Camera::GetInstance()->GetPosition();

  // tries to go up
  if ( (mousePos.y > 0 && mousePos.y < (int)SENSIT_SCROLL_MOUSE)
       && (cameraPos.y > 0) )
    {
      // and to the right
      if ( (mousePos.x > winSize.x - (int)SENSIT_SCROLL_MOUSE)
           && ( cameraPos.x + winSize.x < world.GetWidth() ))
        return POINTER_ARROW_UP_RIGHT;

      // or to the left
      if ( (mousePos.x > 0 && mousePos.x < (int)SENSIT_SCROLL_MOUSE)
       && (cameraPos.x > 0) )
        return POINTER_ARROW_UP_LEFT;

      return POINTER_ARROW_UP;
    }

  // tries to go down
  if ( (mousePos.y > winSize.y - (int)SENSIT_SCROLL_MOUSE)
       && (cameraPos.y + winSize.y < world.GetHeight()) )
    {
      // and to the right
      if ( (mousePos.x > winSize.x - (int)SENSIT_SCROLL_MOUSE)
           && ( cameraPos.x + winSize.x < world.GetWidth() ))
        return POINTER_ARROW_DOWN_RIGHT;

      // or to the left
      if ( (mousePos.x > 0 && mousePos.x < (int)SENSIT_SCROLL_MOUSE)
       && (cameraPos.x > 0) )
        return POINTER_ARROW_DOWN_LEFT;

      return POINTER_ARROW_DOWN;
    }


  // tries to go on the left
  if ( (mousePos.x > 0 && mousePos.x < (int)SENSIT_SCROLL_MOUSE)
       && (cameraPos.x > 0) )
      return POINTER_ARROW_LEFT;

  // tries to go on the right
  if ( (mousePos.x > winSize.x - (int)SENSIT_SCROLL_MOUSE)
       && ( cameraPos.x + winSize.x < world.GetWidth() ))
      return POINTER_ARROW_RIGHT;

  return POINTER_STANDARD;
}

bool Mouse::DrawMovePointer() const
{
  if (scroll_actif) {
    AppWormux::GetInstance()->video->window.Blit( pointer_move, GetPosition() );
    world.ToRedrawOnScreen(Rectanglei(GetPosition().x, GetPosition().y ,
                                      pointer_move.GetWidth(), pointer_move.GetHeight()));
    return true;
  }

  pointer_t scroll_pointer = ScrollPointer();
  if (scroll_pointer != POINTER_STANDARD) {
    const Surface& cursor = GetSurfaceFromPointer(scroll_pointer);
    AppWormux::GetInstance()->video->window.Blit( cursor, GetPosition() );
    world.ToRedrawOnScreen(Rectanglei(GetPosition().x, GetPosition().y ,
                                      cursor.GetWidth(), cursor.GetHeight()));
    return true;
  }

  return false;
}

void Mouse::DrawSelectPointer() const
{
  AppWormux::GetInstance()->video->window.Blit( pointer_select,
                                               Point2i(GetPosition().x-3, GetPosition().y-2) );
  world.ToRedrawOnScreen(Rectanglei(GetPosition().x-3, GetPosition().y-2,
                                    pointer_select.GetWidth(), pointer_select.GetHeight()));
}

void Mouse::Draw() const
{
  if (visible != MOUSE_VISIBLE)
    return;

  if (current_pointer == POINTER_STANDARD)
    return; // use standard SDL cursor

  if ( DrawMovePointer() )
    return;

  if (Interface::GetInstance()->weapons_menu.IsDisplayed()) {
    DrawSelectPointer();
    return;
  }

  switch (current_pointer)
    {
      // The standard beautiful hand
    case POINTER_SELECT:
      DrawSelectPointer();
      break;

      // Move pointer (displayed when middle clic on the map)
    case POINTER_MOVE:
      AppWormux::GetInstance()->video->window.Blit( pointer_move, GetPosition() );
      world.ToRedrawOnScreen(Rectanglei(GetPosition().x, GetPosition().y ,
                                        pointer_move.GetWidth(), pointer_move.GetHeight()));
      break;

      // Target pointer (used at least by automatic bazooka)
    case POINTER_AIM:
      if(ActiveTeam().IsLocal()) {
        AppWormux::GetInstance()->video->window.Blit( pointer_aim,
                                                     Point2i(GetPosition().x-7, GetPosition().y-10 ));
        world.ToRedrawOnScreen(Rectanglei(GetPosition().x-7, GetPosition().y-10,
                                          pointer_aim.GetWidth(), pointer_aim.GetHeight()));
      } else {
        DrawSelectPointer();
      }
      break;

      // Fire pointer (used by air attack)
    case POINTER_FIRE:
      if(ActiveTeam().IsLocal()) {
        if (ActiveCharacter().GetDirection() == DIRECTION_LEFT) {
          // right hand to shoot on the left
          AppWormux::GetInstance()->video->window.Blit( pointer_fire_right,
                                                       Point2i(GetPosition().x-7, GetPosition().y-9 ));
          world.ToRedrawOnScreen(Rectanglei(GetPosition().x-7, GetPosition().y-9,
                                            pointer_fire_right.GetWidth(), pointer_fire_right.GetHeight()));
        } else {
          // left hand to shoot on the right
          AppWormux::GetInstance()->video->window.Blit( pointer_fire_left,
                                                       Point2i(GetPosition().x-17, GetPosition().y-9 ));
          world.ToRedrawOnScreen(Rectanglei(GetPosition().x-17, GetPosition().y-9,
                                            pointer_fire_left.GetWidth(), pointer_fire_left.GetHeight()));
        }
      } else {
        DrawSelectPointer();
      }
      break;

    case POINTER_FIRE_LEFT: // left hand to shoot on the right
      if(ActiveTeam().IsLocal()) {
        AppWormux::GetInstance()->video->window.Blit( pointer_fire_right,
                                                     Point2i(GetPosition().x-7, GetPosition().y-9 ));
        world.ToRedrawOnScreen(Rectanglei(GetPosition().x-7, GetPosition().y-9,
                                          pointer_fire_right.GetWidth(), pointer_fire_right.GetHeight()));
      } else {
        DrawSelectPointer();
      }
      break;

    case POINTER_FIRE_RIGHT: // right hand to shoot on the left
      if(ActiveTeam().IsLocal()) {
        AppWormux::GetInstance()->video->window.Blit( pointer_fire_left,
                                                     Point2i(GetPosition().x-17, GetPosition().y-9 ));
        world.ToRedrawOnScreen(Rectanglei(GetPosition().x-17, GetPosition().y-9,
                                          pointer_fire_left.GetWidth(), pointer_fire_left.GetHeight()));
      } else {
        DrawSelectPointer();
      }
      break;

    default:
      break;
    };
}

void Mouse::Show()
{
  visible = MOUSE_VISIBLE;

  if (Config::GetInstance()->GetDefaultMouseCursor()) {
    SDL_ShowCursor(true); // be sure cursor is visible
  }

  MSG_DEBUG("mouse", "%d, %d", GetPosition().GetX(),
            GetPosition().GetY() );
}

void Mouse::Hide()
{
  visible = MOUSE_HIDDEN;
  SDL_ShowCursor(false); // be sure cursor is invisible

  MSG_DEBUG("mouse", "%d, %d", GetPosition().GetX(),
            GetPosition().GetY() );
}

void Mouse::HideUntilNextMove()
{
  visible = MOUSE_HIDDEN_UNTIL_NEXT_MOVE;
  lastPos = GetPosition();
  SDL_ShowCursor(false); // be sure cursor is invisible

  MSG_DEBUG("mouse", "%d, %d", GetPosition().GetX(),
            GetPosition().GetY() );
}

// Center the pointer on the screen
void Mouse::CenterPointer() const
{
  MSG_DEBUG("mouse", "1) %d, %d\n", GetPosition().GetX(),
            GetPosition().GetY());

  SDL_WarpMouse(AppWormux::GetInstance()->video->window.GetWidth()/2,
                AppWormux::GetInstance()->video->window.GetHeight()/2);
  SDL_PumpEvents(); // force new position else GetPosition does not return new position

  MSG_DEBUG("mouse", "2) %d, %d\n", GetPosition().GetX(),
            GetPosition().GetY());
}
