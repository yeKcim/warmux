/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
#include "graphic/surface.h"
#include "include/base.h"
#include "include/singleton.h"
#include "tool/point.h"

// Forward declarations
struct SDL_event;
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
    POINTER_FIRE_LEFT,
    POINTER_FIRE_RIGHT,
    POINTER_FIRE // Must always be the last one
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

  void GetDesignatedCharacter() const;

  void ActionLeftClic(bool shift = false) const;
  void ActionRightClic(bool shift = false) const;
  void ActionWheelDown(bool shift = false) const;
  void ActionWheelUp(bool shift = false) const;
  MouseCursor& GetCursor(pointer_t pointer) const;

protected:
  friend class Singleton<Mouse>;
  Mouse();

public:
  bool HandleClic (const SDL_Event& event) const;
  void Refresh();

  Point2i GetPosition() const;
  Point2i GetWorldPosition() const;
  void CenterPointer();

  // Choose the pointer
  pointer_t GetPointer() const;
  pointer_t SetPointer(pointer_t pointer);
  void Draw() const;

  // Hide/show mouse pointer
  void Show();
  void Hide();

  visibility_t GetVisibility() const { return visible; };
};
#endif
