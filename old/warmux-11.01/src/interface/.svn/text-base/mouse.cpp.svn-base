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
 * Mouse management
 *****************************************************************************/

#include "interface/mouse.h"

#include "interface/cursor.h"
#include "interface/interface.h"
#include "interface/mouse_cursor.h"
#include "character/character.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "game/game.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "map/map.h"
#include "team/macro.h"
#include "team/team.h"
#include <WARMUX_point.h>
#include "tool/resource_manager.h"
#include "weapon/weapon.h"
#include "game/game_time.h"

#define MOUSE_CLICK_SQUARE_DISTANCE 5*5
#define LONG_CLICK_DURATION 600

std::string __pointers[] = {
  "mouse/pointer_standard",
  "mouse/pointer_select",
  "mouse/pointer_move",
  "mouse/pointer_arrow_up",
  "mouse/pointer_arrow_up_right",
  "mouse/pointer_arrow_up_left",
  "mouse/pointer_arrow_down",
  "mouse/pointer_arrow_down_right",
  "mouse/pointer_arrow_down_left",
  "mouse/pointer_arrow_right",
  "mouse/pointer_arrow_left",
  "mouse/pointer_aim",
  "mouse/pointer_attack_from_left",
  "mouse/pointer_attack_from_right",
};
std::map<Mouse::pointer_t, MouseCursor> Mouse::cursors;

Mouse::Mouse()
  : lastpos(-1,-1)
  , last_hide_time(0)
  , long_click_timer(0)
  , click_pos(-1,-1)
  , is_long_click(false)
  , was_long_click(false)
{
  visible = MOUSE_VISIBLE;

  // Load the different pointers
  Profile *res = GetResourceManager().LoadXMLProfile("cursors.xml", false);

  for (int i=POINTER_SELECT; i < POINTER_ATTACK; i++) {
    cursors.insert(std::make_pair(Mouse::pointer_t(i),
                                  GetResourceManager().LoadMouseCursor(res, __pointers[i],
                                                                       Mouse::pointer_t(i))));
  }

  current_pointer = POINTER_STANDARD;
  GetResourceManager().UnLoadXMLProfile(res);

#ifdef HAVE_TOUCHSCREEN
  SDL_ShowCursor(false);
#endif
}

void Mouse::EndLongClickTimer()
{
  if (long_click_timer) {
    SDL_RemoveTimer(long_click_timer);
    long_click_timer = 0;
  }
  is_long_click = false;
  was_long_click = false;
}

bool Mouse::HasFocus() const
{
  Uint8 state = SDL_GetAppState();

  if ((state & SDL_APPMOUSEFOCUS) &&
      (state & SDL_APPINPUTFOCUS) &&
      (state & SDL_APPACTIVE)) {
    return true;
  }

  return false;
}

void Mouse::ActionLeftClick(bool /*shift*/) const
{
  const Point2i pos_monde = GetWorldPosition();

  //Change character by mouse click only if the choosen weapon allows it
  if (GameMode::GetConstInstance()->AllowCharacterSelection() &&
      ActiveTeam().GetWeapon().mouse_character_selection) {

    // Choose a character of our own team
    bool character_found = false;
    Team::iterator it    = ActiveTeam().begin(),
                   end   = ActiveTeam().end();

    for (; it != end; ++it) {
      if (&(*it) != &ActiveCharacter()
          && !it -> IsDead()
          && it->GetRect().Contains(pos_monde)) {

        character_found = true;
        break;
      }
    }

    if (character_found) {
      ActiveTeam().SelectCharacter(&(*it));
      ActionHandler::GetInstance()->NewActionActiveCharacter();

      return;
    }

    if (ActiveCharacter().GetRect().Contains(pos_monde)) {
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


void Mouse::ActionRightClick(bool /*shift*/) const
{
  Interface::GetInstance()->weapons_menu.SwitchDisplay(GetPosition());
}

void Mouse::ActionWheelUp(bool shift) const
{
  ActiveTeam().AccessWeapon().HandleMouseWheelUp(shift);
}

void Mouse::ActionWheelDown(bool shift) const
{
  ActiveTeam().AccessWeapon().HandleMouseWheelDown(shift);
}

bool Mouse::IS_CLICK_BUTTON(uint button)
{
  return (button==SDL_BUTTON_LEFT || button==SDL_BUTTON_RIGHT || button==SDL_BUTTON_MIDDLE);
}

Uint8 Mouse::BUTTON_RIGHT() // static method
{
  return Config::GetConstRef().GetLeftHandedMouse() ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
}

Uint8 Mouse::BUTTON_LEFT() // static method
{
  return Config::GetConstRef().GetLeftHandedMouse() ? SDL_BUTTON_RIGHT : SDL_BUTTON_LEFT;
}

static Uint32 HandleLongClick(Uint32 /*interval*/, void *param)
{
  ((Mouse*)param)->SetLongClick();
  return 0;
}

bool Mouse::HandleEvent(const SDL_Event& evnt)
{
  if (!HasFocus()) {
    return false;
  }

  Point2i pos = GetPosition();

  if (click_pos.SquareDistance(pos) > MOUSE_CLICK_SQUARE_DISTANCE) {
    // We have moved too much, consider we are not long-clicking
    EndLongClickTimer();
  }

  if (is_long_click) {
    is_long_click = false;
    was_long_click = true;
    if (Interface::GetInstance()->ActionLongClick(pos, click_pos))
      return true;
    return false;
  }

  if (evnt.type != SDL_MOUSEBUTTONDOWN && evnt.type != SDL_MOUSEBUTTONUP) {
    return false;
  }

  if (evnt.type==SDL_MOUSEBUTTONDOWN && evnt.button.button==BUTTON_LEFT()) {
    click_pos = pos;
    if (Interface::GetInstance()->ActionClickDown(pos))
      return true;

    // Either it's out of the menu, or we want to know how long the click was
    long_click_timer = SDL_AddTimer(LONG_CLICK_DURATION, HandleLongClick, this);
    return true;
  }

  if (evnt.type==SDL_MOUSEBUTTONUP && evnt.button.button==BUTTON_LEFT()) {
    // Ignore click-ups from long clicks
    if (was_long_click) {
      was_long_click = false;
      return true;
    } else {
      EndLongClickTimer();
      if (Interface::GetInstance()->ActionClickUp(pos, click_pos))
        return true;
      if (click_pos.SquareDistance(pos) > MOUSE_CLICK_SQUARE_DISTANCE)
        return true;
    }
  }

  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return true;

  if (!ActiveTeam().IsLocalHuman())
    return true;

  bool shift = !!(SDL_GetModState() & KMOD_SHIFT);
  if (evnt.type == SDL_MOUSEBUTTONUP) {
    if (evnt.button.button == Mouse::BUTTON_RIGHT())
      ActionRightClick(shift);
    else if (evnt.button.button == Mouse::BUTTON_LEFT())
      ActionLeftClick(shift);
    else if (evnt.button.button == SDL_BUTTON_WHEELDOWN)
      ActionWheelDown(shift);
    else if (evnt.button.button == SDL_BUTTON_WHEELUP)
      ActionWheelUp(shift);
  }

  return true;
}

void Mouse::GetDesignatedCharacter() const
{
  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return;

  const Point2i pos_monde = GetWorldPosition();

  // Which character is pointed by the mouse ? (appart from the active one)
  Interface::GetInstance()->character_under_cursor = NULL;
  FOR_ALL_LIVING_CHARACTERS(team, character) {
    if (&(*character) != &ActiveCharacter()
        && character->GetRect().Contains(pos_monde)) {
      Interface::GetInstance()->character_under_cursor = &(*character);
    }
  }

  // No character is pointed... what about the active one ?
  if (Interface::GetConstInstance()->character_under_cursor
      && ActiveCharacter().GetRect().Contains(pos_monde)) {
    Interface::GetInstance()->character_under_cursor = &ActiveCharacter();
  }

}

void Mouse::Refresh()
{
  /* FIXME the 200 is hardcoded because I cannot find where the main loop
   * refresh rate is set... */
#define NB_LOOP_BEFORE_HIDE 200
  static int counter = 0;
  GetDesignatedCharacter();

  Point2i pos = GetPosition();

  if (lastpos != pos) {
    Show();
    lastpos = pos;
    counter = NB_LOOP_BEFORE_HIDE;
    ShowGameInterface();
  } else if (visible == MOUSE_VISIBLE) {
    /* The mouse is hidden after a while when not moving */
    if (--counter <= 0)
      Hide();
  }
}

Point2i Mouse::GetPosition() const
{
  int x, y;

  SDL_GetMouseState(&x, &y);
  return Point2i(x, y);
}

Point2i Mouse::GetWorldPosition() const
{
  return GetPosition() + Camera::GetConstInstance()->GetPosition();
}

MouseCursor& Mouse::GetCursor(pointer_t pointer) const
{
  ASSERT(pointer != POINTER_STANDARD);

  if (pointer == POINTER_ATTACK) {
    if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
      return GetCursor(POINTER_ATTACK_FROM_LEFT);
    else
      return GetCursor(POINTER_ATTACK_FROM_RIGHT);
  }
  return (*cursors.find(pointer)).second;
}

// set the new pointer type and return the previous one
Mouse::pointer_t Mouse::SetPointer(pointer_t pointer)
{
  if (Config::GetConstInstance()->GetDefaultMouseCursor()) {
    Show(); // be sure cursor is visible
    return Mouse::POINTER_STANDARD;
  }

  if (current_pointer == pointer) return pointer;

  if (pointer == POINTER_STANDARD)
#ifndef HAVE_TOUCHSCREEN
    SDL_ShowCursor(true);
#else
    SDL_ShowCursor(false);
#endif
  else
    SDL_ShowCursor(false);

  pointer_t old_pointer = current_pointer;
  current_pointer = pointer;

  return old_pointer;
}

void Mouse::Draw() const
{
  if (visible != MOUSE_VISIBLE)
    return;

  if (current_pointer == POINTER_STANDARD)
    return; // use standard SDL cursor

  const MouseCursor& cursor = GetCursor(current_pointer);
  const Surface& surf = cursor.GetSurface();
  Point2i pos = GetPosition() - cursor.GetClicPos();
  GetMainWindow().Blit(surf, pos);
  GetWorld().ToRedrawOnScreen(Rectanglei(pos, surf.GetSize()));
}

void Mouse::Show()
{
#ifndef HAVE_TOUCHSCREEN
  if (Time::GetConstInstance()->Read()-last_hide_time > 10000 && visible == MOUSE_HIDDEN) {
    CenterPointer();
  }
#endif
  visible = MOUSE_VISIBLE;

  if (Config::GetConstInstance()->GetDefaultMouseCursor()) {
    SDL_ShowCursor(true); // be sure cursor is visible
  }

}

void Mouse::Hide()
{
  if (visible == MOUSE_VISIBLE)
  {
    last_hide_time = Time::GetConstInstance()->Read();
  }
  visible = MOUSE_HIDDEN;
  SDL_ShowCursor(false); // be sure cursor is invisible

}

// Center the pointer on the screen
void Mouse::CenterPointer()
{
  SetPosition(GetMainWindow().GetSize() / 2);
}

void Mouse::SetPosition(Point2i pos)
{
  if (!HasFocus()) // The application has not the focus, don't move the mouse cursor!
    return;

  MSG_DEBUG("mouse", "1) %d, %d\n", GetPosition().GetX(), GetPosition().GetY());

  SDL_WarpMouse(pos.x, pos.y);
  SDL_PumpEvents(); // force new position else GetPosition does not return new position

  lastpos = GetPosition();

  MSG_DEBUG("mouse", "2) %d, %d\n", GetPosition().GetX(), GetPosition().GetY());
}
