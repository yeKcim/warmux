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
#include "gui/vertical_box.h"
#include "gui/scroll_box.h"
#include "include/app.h"

#define SCROLL_SPEED  6
#define NO_DRAG      -1

static const Color c_even(0x80, 0x80, 0x80, 0x40);
static const Color  c_odd(0x80, 0x80, 0x80, 0x20);

ScrollBox::ScrollBox(const Point2i & _size, bool force_widget_size, bool alternate)
  : WidgetList(_size)
  , alternate_colors(alternate)
  , m_up(NULL)
  , m_down(NULL)
  , start_drag_y(NO_DRAG)
  , start_drag_offset(NO_DRAG)
  , offset(0)
  , scroll_mode(SCROLL_MODE_NONE)
{
  // Load buttons
  Profile *res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  m_up = new Button(res, "menu/up");
  m_down = new Button(res, "menu/down");

  Widget::SetBorder(white_color, 1);
  Widget::SetBackgroundColor(transparent_color);

  scrollbar_width = m_up->GetSizeX();
  // Let's consider the scrollbar is not displayed for now.
  vbox = new VBox(_size.x - 2*border_size - scrollbar_width, false, false, force_widget_size);
  vbox->SetNoBorder();
  vbox->SetMargin(0);
  vbox->SetBackgroundColor(transparent_color);

  WidgetList::AddWidget(vbox);
  WidgetList::AddWidget(m_up);
  WidgetList::AddWidget(m_down);
}

Widget * ScrollBox::ClickUp(const Point2i & mousePosition, uint button)
{
  ScrollMode old_mode = scroll_mode;
  start_drag_offset = NO_DRAG;
  scroll_mode = SCROLL_MODE_NONE;

  if (!vbox->GetFirstWidget()) {
    return NULL;
  }

  // Handle the click up as a widget click only if we weren't dragging
  // If we click up close to where we clicked down, it will however register
  // as a click and not a scrolling
  if (vbox->Contains(mousePosition) &&
      (start_drag_y==NO_DRAG || abs(start_drag_y-mousePosition.y)<2)) {
    Widget *w = vbox->ClickUp(mousePosition, button);

    if (w) {
      SetFocusOn(w);
      return w;
    }
    // The click was not handled, let's try using it for scrolling
  }

  if (HasScrollBar()) {
    bool is_click   = Mouse::IS_CLICK_BUTTON(button);
    int  max_offset = GetMaxOffset();
    int  new_offset = offset;

    // The event involves the scrollbar or its buttons
    if ((button == SDL_BUTTON_WHEELDOWN && Contains(mousePosition)) ||
        (is_click && m_down->Contains(mousePosition))) {

      // bottom button
      new_offset = offset+SCROLL_SPEED;
    } else if ((button == SDL_BUTTON_WHEELUP && Contains(mousePosition)) ||
               (is_click && m_up->Contains(mousePosition))) {

      // top button
      new_offset = offset-SCROLL_SPEED;
    } else if (is_click) {
      // Was it released after a drag operation?
      if (old_mode!=SCROLL_MODE_NONE /*&& start_drag_y != mousePosition.y*/)
        return this;
      const Rectanglei& scroll_track = GetScrollTrack();
      if (scroll_track.Contains(mousePosition)) {
        // Set this as new scroll thumb position
        int height = scroll_track.GetSizeY();
        new_offset = ((mousePosition.y - scroll_track.GetPositionY()) * (size.y+max_offset)
                      + (height/2)) / scroll_track.GetSizeY();
      }
    }

    // Clip new offset to correct value
    if (new_offset > max_offset)
      new_offset = max_offset;
    if (new_offset < 0)
      new_offset = 0;

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
  if (!Contains(mousePosition)) {
    return NULL;
  }

  start_drag_offset = NO_DRAG;
  scroll_mode = SCROLL_MODE_NONE;

  if (HasScrollBar() && Mouse::IS_CLICK_BUTTON(button)) {
    start_drag_y = mousePosition.y;
    if (GetScrollThumb().Contains(mousePosition)) {
      if (!offset) {
        // Not yet set, derive from mouse position
        const Rectanglei& scroll_track = GetScrollTrack();
        int height = scroll_track.GetSizeY();
        offset = ( (mousePosition.y - scroll_track.GetPositionY()) * (size.y+GetMaxOffset())
                   + (height/2) ) / height;
      }

      start_drag_offset = offset;
      scroll_mode = SCROLL_MODE_THUMB;
    } else if (vbox->Contains(mousePosition)) {
      // The click occurred inside the vbox, this means scrolling
      scroll_mode = SCROLL_MODE_DRAG;
      start_drag_offset = offset;
    }
  }

  return WidgetList::Click(mousePosition, button);
}

void ScrollBox::__Update(const Point2i & mousePosition,
                         const Point2i & /*lastMousePosition*/)
{
  // update position of items because of dragging
  if (HasScrollBar() && scroll_mode!=SCROLL_MODE_NONE) {
    int max_offset = GetMaxOffset();
    int new_offset = offset;

    if (scroll_mode == SCROLL_MODE_THUMB) {
      Point2i track_pos  = GetScrollTrackPos();
      int     height     = GetTrackHeight();

      new_offset = start_drag_offset +
                   ((mousePosition.y - start_drag_y) * (size.y+max_offset))/height;
    } else if (scroll_mode == SCROLL_MODE_DRAG) {
      // Act as if the scroll corresponds to bringing the starting point to the
      // current point
      new_offset = start_drag_offset + start_drag_y - mousePosition.y;
    }

    if (new_offset < 0)
      new_offset = 0;
    if (new_offset > max_offset)
      new_offset = max_offset;

    if (new_offset != offset) {
      offset = new_offset;
      Pack();
    }
  }
}

void ScrollBox::AddWidget(Widget* widget)
{
  vbox->AddWidget(widget);
  if (alternate_colors) {
    widget->SetBackgroundColor((vbox->WidgetCount()&1) ? c_odd : c_even);
  }
}

Point2i ScrollBox::GetScrollTrackPos() const
{
  return Point2i(position.x + size.x - border_size - scrollbar_width,
                 position.y + border_size + m_up->GetSizeY());
}

Rectanglei ScrollBox::GetScrollThumb() const
{
  // Height: (part of the vbox that is displayed / vbox size) * scrollbar height
  const Rectanglei& scroll_track = GetScrollTrack();
  uint tmp_h = ((size.y - 2*border_size) * scroll_track.GetSizeY())
             / ((size.y - 2*border_size) + GetMaxOffset());
  // Start position: from the offset
  uint h     = size.y + GetMaxOffset();
  uint tmp_y = scroll_track.GetPositionY()
             + (offset * scroll_track.GetSizeY() + h/2) / h;
  if (tmp_h < 6)
    tmp_h = 6;
  return Rectanglei(scroll_track.GetPositionX(), tmp_y,
                    scrollbar_width, tmp_h);
}

int ScrollBox::GetTrackHeight() const
{
  return size.y - 2*(m_up->GetSizeY()+border_size);
}

bool ScrollBox::Update(const Point2i &mousePosition,
                       const Point2i &lastMousePosition)
{
  // Force redrawing if we are scrolling and the mouse has moved
  if (start_drag_offset!=NO_DRAG && mousePosition!=lastMousePosition) {
    //NeedRedrawing();
  }

  bool redraw = need_redrawing;
  bool updated = Widget::Update(mousePosition, lastMousePosition);
  need_redrawing = redraw;

  bool has_scrollbar = HasScrollBar();
  m_up->SetVisible(has_scrollbar);
  m_down->SetVisible(has_scrollbar);

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
  vbox->SetPosition(position.x + border_size,
                    position.y + border_size - offset);
  vbox->SetSizeX(size.x -2*border_size - scrollbar_width);
  vbox->Pack();

  //printf("Pack: size=%ix%i max=%i\n", size.x, size.y, GetMaxOffset());

  m_up->SetPosition(position.x + size.x - m_up->GetSizeX() - border_size,
                    position.y + border_size);
  m_down->SetPosition(position + size - m_down->GetSize() - border_size);
}

bool ScrollBox::SendKey(const SDL_keysym & key)
{
  if (!WidgetList::SendKey(key)) {
    int new_offset = offset;
    switch(key.sym)
    {
    case SDLK_PAGEUP:
      new_offset -= size.y;
      break;
    case SDLK_PAGEDOWN:
      new_offset += size.y;
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
