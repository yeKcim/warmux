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

#ifndef MOUSE_H
#define MOUSE_H

#include <map>

#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include <WARMUX_point.h>

// Forward declarations
union SDL_Event;
typedef struct _SDL_TimerID *SDL_TimerID;
class MouseCursor;

class Mouse : public Singleton<Mouse>
{
public:

  typedef enum {
    POINTER_STANDARD,
    POINTER_SELECT,
    POINTER_MOVE,
    POINTER_ARROW_UP,
    POINTER_ARROW_UP_RIGHT,
    POINTER_ARROW_UP_LEFT,
    POINTER_ARROW_DOWN,
    POINTER_ARROW_DOWN_RIGHT,
    POINTER_ARROW_DOWN_LEFT,
    POINTER_ARROW_RIGHT,
    POINTER_ARROW_LEFT,
    POINTER_AIM,
    POINTER_ATTACK_FROM_LEFT,
    POINTER_ATTACK_FROM_RIGHT,
    POINTER_ATTACK // Must always be the last one
  } pointer_t;

  typedef enum {
    MOUSE_HIDDEN,
    MOUSE_VISIBLE,
  } visibility_t;

private:
  static std::map<pointer_t, MouseCursor> cursors;
  Point2i lastpos;

  visibility_t visible;
  pointer_t current_pointer;
  uint last_hide_time;

  // For timed operations
  SDL_TimerID long_click_timer;
  Point2i     click_pos;
  bool        is_long_click, was_long_click;
  uint clickup_time;

  void EndLongClickTimer();

  void GetDesignatedCharacter() const;

  void ActionLeftClick(bool shift = false) const;
  void ActionRightClick(bool shift = false) const;
  void ActionWheelDown(bool shift = false) const;
  void ActionWheelUp(bool shift = false) const;
  MouseCursor& GetCursor(pointer_t pointer) const;

protected:
  friend class Singleton<Mouse>;
  Mouse();
  ~Mouse() { EndLongClickTimer(); }

public:

  // to compare with SDL_Event.button.button
  // and manage left-handed mouse option "transparently"
  static uint8_t BUTTON_RIGHT();
  static uint8_t BUTTON_LEFT();
  static bool  IS_CLICK_BUTTON(uint button);

  bool HandleEvent(const SDL_Event& evnt);
  void Refresh();

  Point2i GetPosition() const;
  Point2i GetWorldPosition() const;
  void CenterPointer();
  void SetPosition(Point2i pos);

  // Choose the pointer
  pointer_t GetPointer() const { return current_pointer; }
  pointer_t SetPointer(pointer_t pointer);
  void Draw() const;

  // Hide/show mouse pointer
  void Show();
  void Hide();

  bool HasFocus() const;

  visibility_t GetVisibility() const { return visible; }

  void SetLongClick() { is_long_click = true; long_click_timer = 0; }
};
#endif
