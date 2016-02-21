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
 * Vertical Scroll Box
 *****************************************************************************/

#include "graphic/video.h"
#include "graphic/colors.h"
#include <SDL.h>
#include "gui/button.h"
#include "gui/horizontal_box.h"
#include "gui/vertical_box.h"
#include "gui/scroll_box.h"
#include "include/app.h"
#include "tool/eventtimer.h"

static const float MAX_CAMERA_SPEED = 200;
static const float MAX_CAMERA_ACCELERATION = 1.5;

#define REACTIVITY                1.2f
#define SPEED_REACTIVITY          0.05f
#define ANTICIPATION              9
#define SPEED_REACTIVITY_CEIL     4

#define SCROLL_UPDATE_INTERVAL    33
#define SCROLL_SPEED  6
#define NO_DRAG      -1

static const Color c_even(0x80, 0x80, 0x80, 0x40);
static const Color  c_odd(0x80, 0x80, 0x80, 0x20);

ScrollBox::ScrollBox(const Point2i & _size, bool force, bool alt, bool v)
  : WidgetList(_size)
  , alternate_colors(alt)
  , m_dec(NULL)
  , m_inc(NULL)
  , scroll_target(0)
  , scroll_counter(0)
  , start_drag(NO_DRAG)
  , start_drag_offset(NO_DRAG)
  , offset(0)
  , nonselectable_width(0)
  , scroll_speed(0.0,0.0)
  , first_mouse_position(0,0)
  , scroll_mode(SCROLL_MODE_NONE)
  , vertical(v)

{
  // Load buttons
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  if (v) {
    m_dec = new Button(res, "menu/up");
    m_inc = new Button(res, "menu/down");
  } else {
    m_dec = new Button(res, "menu/big_minus");
    m_inc = new Button(res, "menu/big_plus");
  }

  Widget::SetBorder(white_color, 1);
  Widget::SetBackgroundColor(transparent_color);

  scrollbar_dim = (v) ? m_dec->GetSizeX() : m_dec->GetSizeY();
  // Let's consider the scrollbar is not displayed for now.
  if (v)
    box = new VBox(size.x - 2*border_size - scrollbar_dim, false, false, force);
  else
    box = new HBox(size.y - 2*border_size - scrollbar_dim, false, false, force);
  box->SetNoBorder();
  box->SetMargin(0);
  box->SetBackgroundColor(transparent_color);

  WidgetList::AddWidget(box);
  WidgetList::AddWidget(m_dec);
  WidgetList::AddWidget(m_inc);
}

ScrollBox::~ScrollBox() {
  EventTimer::GetInstance()->Stop();
}

Widget * ScrollBox::ClickUp(const Point2i & mousePosition, uint button)
{
  ScrollMode old_mode = scroll_mode;
  start_drag_offset = NO_DRAG;
  if (scroll_mode == SCROLL_MODE_DRAG) {
    EventTimer::GetInstance()->Start(SCROLL_UPDATE_INTERVAL);
    scroll_mode = SCROLL_MODE_KINETIC;
  } else if (scroll_mode != SCROLL_MODE_TOTARGET) {
    scroll_mode = SCROLL_MODE_NONE;
    EventTimer::GetInstance()->Stop();
  }

  if (!box->GetFirstWidget()) {
    return NULL;
  }

  // Handle the click up as a widget click only if we weren't dragging
  // If we click up close to where we clicked down, it will however register
  // as a click and not a scrolling
  if (box->Contains(mousePosition) && (start_drag==NO_DRAG || LargeDrag(mousePosition))) {
    Widget *w = box->ClickUp(mousePosition, button);

    if (w) {
      SetFocusOn(w);
      return w;
    }
    // The click was not handled, let's try using it for scrolling
  }

  if (HasScrollBar()) {
    bool is_click   = Mouse::IS_CLICK_BUTTON(button);
    int  max_offset = GetMaxOffset();
    int  min_offset = GetMinOffset();
    int  new_offset = offset;

    // The event involves the scrollbar or its buttons
    if ((button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
        (is_click && m_inc->Contains(mousePosition))) {

      // bottom button
      new_offset = offset+SCROLL_SPEED;
    } else if ((button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
               (is_click && m_dec->Contains(mousePosition))) {

      // top button
      new_offset = offset-SCROLL_SPEED;
    } else if (is_click) {
      // Was it released after a drag operation?
      if (old_mode!=SCROLL_MODE_NONE /*&& start_drag != mousePosition.y*/)
        return this;
      const Rectanglei& scroll_track = GetScrollTrack();
      if (scroll_track.Contains(mousePosition)) {
        // Set this as new scroll thumb position
        if (vertical) {
          int height = scroll_track.GetSizeY();
          new_offset = ((mousePosition.y - scroll_track.GetPositionY()) * (size.y+max_offset)
                        + (height/2)) / scroll_track.GetSizeY();
        } else {
          int width = scroll_track.GetSizeX();
          new_offset = GetMinOffset() + ((mousePosition.x - scroll_track.GetPositionX()) * (size.x+max_offset)
                        + (width/2)) / scroll_track.GetSizeX();
        }
      }
    }

    // Clip new offset to correct value
    if (new_offset > max_offset)
      new_offset = max_offset;
    if (new_offset < min_offset)
      new_offset = min_offset;

    if (new_offset != offset) {
      offset = new_offset;
      Pack();
      NeedRedrawing();
    }

    return this;
  }

  return NULL;
}

Widget * ScrollBox::Click(const Point2i & mousePosition, uint button)
{
  first_mouse_position = mousePosition;
  scroll_counter = 0;
  if (!Contains(mousePosition)) {
    return NULL;
  }

  if (HasScrollBar() && Mouse::IS_CLICK_BUTTON(button)) {
    start_drag_offset = NO_DRAG;
    scroll_mode = SCROLL_MODE_NONE;
    start_drag = (vertical) ? mousePosition.y : mousePosition.x;
    if (GetScrollThumb().Contains(mousePosition)) {
      if (!offset) {
        // Not yet set, derive from mouse position
        const Rectanglei& scroll_track = GetScrollTrack();
        if (vertical) {
          int height = scroll_track.GetSizeY();
          offset = ( (mousePosition.y - scroll_track.GetPositionY()) * (size.y+GetMaxOffset())
                     + (height/2) ) / height;
        } else {
          int width = scroll_track.GetSizeX();
          offset = GetMinOffset() +  ((mousePosition.x - scroll_track.GetPositionX()) * (size.x+GetMaxOffset())
                     + (width/2) ) / width;
        }
      }

      start_drag_offset = offset;
      scroll_mode = SCROLL_MODE_THUMB;
    } else if (box->Contains(mousePosition)) {
      // The click occurred inside the box, this means scrolling
      scroll_mode = SCROLL_MODE_DRAG;
      start_drag_offset = offset;
    }
  }

  return WidgetList::Click(mousePosition, button);
}

void ScrollBox::__Update(const Point2i & mousePosition,
                         const Point2i & /*lastMousePosition*/)
{
  if (scroll_mode == SCROLL_MODE_TOTARGET)
    __ScrollToPos(scroll_target);

  if (scroll_mode == SCROLL_MODE_DRAG) {
    scroll_counter++;
    scroll_speed = (mousePosition - first_mouse_position) / scroll_counter;
  }

  // update position of items because of dragging
  if (HasScrollBar() && scroll_mode!=SCROLL_MODE_NONE) {
    int max_offset = GetMaxOffset();
    int min_offset = GetMinOffset();
    int new_offset = offset;

    if (scroll_mode == SCROLL_MODE_THUMB) {
      //Point2i track_pos  = GetScrollTrackPos();

      if (vertical) {
        int   height     = GetTrackDimension();

        new_offset = start_drag_offset +
                     ((mousePosition.y - start_drag) * (size.y+max_offset))/height;
      } else {
        int   width      = GetTrackDimension();

        new_offset = start_drag_offset +
                     ((mousePosition.x - start_drag) * (size.x+max_offset))/width;
      }
    } else if (scroll_mode == SCROLL_MODE_DRAG) {
      // Act as if the scroll corresponds to bringing the starting point to the
      // current point
      new_offset = start_drag_offset + start_drag;
      if (vertical)
        new_offset -= mousePosition.y;
      else
        new_offset -= mousePosition.x;
    } else if (scroll_mode == SCROLL_MODE_KINETIC) {
      if (vertical)
        new_offset -= scroll_speed.y;
      else
        new_offset -= scroll_speed.x;
      scroll_speed = scroll_speed - scroll_speed*0.04;
      if (scroll_speed.Norm() < 3) {
        if (vertical) {
          scroll_mode = SCROLL_MODE_NONE;
          EventTimer::GetInstance()->Stop();
        }
        else
          scroll_mode = SCROLL_MODE_KINETIC_DONE;
      }
    }

    if (new_offset < min_offset)
      new_offset = min_offset;
    if (new_offset > max_offset)
      new_offset = max_offset;

    if (new_offset != offset) {
      offset = new_offset;
      Pack();
    }
  }
}

void ScrollBox::ScrollToPos(int new_offset)
{
  if (vertical)
    return;

  scroll_target = new_offset;
  scroll_mode = SCROLL_MODE_TOTARGET;
  NeedRedrawing();
  EventTimer::GetInstance()->Start(SCROLL_UPDATE_INTERVAL);;
}

void ScrollBox::__ScrollToPos(int new_offset)
{
  if (scroll_mode != SCROLL_MODE_TOTARGET)
    return;

  float target = new_offset;
  float prev_position;
  float position = offset;
  float speed;
  //float prev_speed;

  if (vertical)
    speed = scroll_speed.x;
  else
    speed = scroll_speed.y;

  //Compute new speed to reach target
  float acceleration = (target - speed*ANTICIPATION - position)*REACTIVITY;
  // Limit acceleration
  if (acceleration > MAX_CAMERA_ACCELERATION) acceleration = MAX_CAMERA_ACCELERATION;
  if (acceleration < -MAX_CAMERA_ACCELERATION) acceleration = -MAX_CAMERA_ACCELERATION;

  if ((int)abs(speed) > SPEED_REACTIVITY_CEIL) {
    acceleration *= (1 + SPEED_REACTIVITY * ((int)abs(speed) - SPEED_REACTIVITY_CEIL));
  }

  //Apply acceleration
  //prev_speed = speed;
  speed += acceleration;

  //Limit
  if (speed > MAX_CAMERA_SPEED) speed = MAX_CAMERA_SPEED;
  if (speed < -MAX_CAMERA_SPEED) speed = -MAX_CAMERA_SPEED;

  //Update position
  prev_position = position;
  position = position + speed;
  //printf("pos=%i target=%f speed=%.2f acceleration=%.2f\n", offset, scroll_target, speed, acceleration);
  offset = position;
  scroll_speed = Point2f(speed, speed);
  Pack();

  if (abs(position - target) < 20 && abs(position - prev_position) < 2) {
    scroll_mode = SCROLL_MODE_NONE;
    EventTimer::GetInstance()->Stop();
  }
}

void ScrollBox::AddWidget(Widget* widget)
{
  box->AddWidget(widget);
  if (alternate_colors) {
    widget->SetBackgroundColor((box->WidgetCount()&1) ? c_odd : c_even);
  }
}

Point2i ScrollBox::GetScrollTrackPos() const
{
  if (vertical)
    return Point2i(position.x + size.x - border_size - scrollbar_dim,
                   position.y + border_size + m_dec->GetSizeY());
  return Point2i(position.x + border_size + m_dec->GetSizeX(),
                 position.y + size.y - border_size - scrollbar_dim);
}

Rectanglei ScrollBox::GetScrollThumb() const
{
  // Height: (part of the box that is displayed / box size) * scrollbar height
  const Rectanglei& scroll_track = GetScrollTrack();

  if (vertical) {
    uint tmp_h = ((size.y - 2*border_size) * scroll_track.GetSizeY())
               / ((size.y - 2*border_size) + GetMaxOffset());
    // Start position: from the offset
    uint h     = size.y + GetMaxOffset();
    uint tmp_y = scroll_track.GetPositionY()
               + (offset * scroll_track.GetSizeY() + h/2) / h;
    if (tmp_h < 6)
      tmp_h = 6;
    return Rectanglei(scroll_track.GetPositionX(), tmp_y,
                      scrollbar_dim, tmp_h);
  } else {
    uint tmp_w = ((size.x - 2*border_size) * scroll_track.GetSizeX())
               / ((size.x - 2*border_size) + GetMaxOffset() - GetMinOffset());
    // Start position: from the offset
    uint w     = size.x + GetMaxOffset() - GetMinOffset();
    uint tmp_x = scroll_track.GetPositionX()
               + ((offset - GetMinOffset()) * scroll_track.GetSizeX() + w/2) / w;
    if (tmp_w < 6)
      tmp_w = 6;
    return Rectanglei(tmp_x, scroll_track.GetPositionY(),
                      tmp_w, scrollbar_dim);
  }
}

int ScrollBox::GetTrackDimension() const
{
  if (vertical)
    return size.y - 2*(m_dec->GetSizeY()+border_size);
  return size.x - 2*(m_dec->GetSizeX()+border_size);
}

bool ScrollBox::Update(const Point2i &mousePosition,
                       const Point2i &lastMousePosition)
{
  bool redraw = need_redrawing;
  bool updated = Widget::Update(mousePosition, lastMousePosition);
  need_redrawing = redraw;

  bool has_scrollbar = HasScrollBar();
  m_dec->SetVisible(has_scrollbar);
  m_inc->SetVisible(has_scrollbar);

  updated |= WidgetList::Update(mousePosition, lastMousePosition);

  if (has_scrollbar) {
    GetMainWindow().BoxColor(GetScrollTrack(), dark_gray_color);

    const Rectanglei& thumb = GetScrollThumb();
    bool over = scroll_mode==SCROLL_MODE_THUMB || thumb.Contains(mousePosition);
    GetMainWindow().BoxColor(thumb, over ? white_color : gray_color);
  }
  return updated;
}

void ScrollBox::Pack()
{
  // Make a first guess about the box properties
  if (vertical) {
    box->SetPosition(position.x + border_size,
                     position.y + border_size - offset);
    box->SetSizeX(size.x -2*border_size - scrollbar_dim);
    box->Pack();

    //printf("Pack: size=%ix%i max=%i\n", size.x, size.y, GetMaxOffset());

    m_dec->SetPosition(position.x + size.x - m_dec->GetSizeX() - border_size,
                      position.y + border_size);
    m_inc->SetPosition(position + size - m_inc->GetSize() - border_size);
  } else {
    box->SetPosition(position.x + border_size - offset,
                     position.y + border_size);
    box->SetSizeY(size.y -2*border_size - scrollbar_dim);
    box->Pack();

    //printf("Pack: size=%ix%i max=%i\n", size.x, size.y, GetMaxOffset());

    m_dec->SetPosition(position.x + border_size,
                      position.y + size.y - m_dec->GetSizeY() - border_size);
    m_inc->SetPosition(position + size - m_inc->GetSize() - border_size);
  }
}

bool ScrollBox::SendKey(const SDL_keysym & key)
{
  if (!WidgetList::SendKey(key)) {
    int new_offset = offset;
    switch(key.sym)
    {
    case SDLK_PAGEUP:
      new_offset -= (vertical) ? size.y : size.x;
      break;
    case SDLK_PAGEDOWN:
      new_offset += (vertical) ? size.y : size.x;
      break;
    default:
      return false;
    }

    if (new_offset < 0)
      new_offset = 0;
    if (new_offset > GetMaxOffset())
      new_offset = GetMaxOffset();

    if (new_offset != offset) {
      offset = new_offset;
      Pack();
      NeedRedrawing();
    }
  }
  return true;
}

bool ScrollBox::IsScrolling()
{
  return scroll_mode != SCROLL_MODE_NONE;
}
