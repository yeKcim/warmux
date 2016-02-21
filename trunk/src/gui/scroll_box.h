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

#ifndef SCROLL_BOX_H
#define SCROLL_BOX_H

#include "gui/widget_list.h"
#include "gui/box.h"

// Forward declaration
struct SDL_keysym;
class Box;
class Button;

class ScrollBox : public WidgetList
{
public:
  typedef enum {
    SCROLL_MODE_NONE,
    SCROLL_MODE_THUMB,
    SCROLL_MODE_DRAG,
    SCROLL_MODE_KINETIC,
    SCROLL_MODE_KINETIC_DONE,
    SCROLL_MODE_TOTARGET
  } ScrollMode;
private:
  void __ScrollToPos(int position);
protected:
  // Internal box
  Box *box;
  bool alternate_colors;

  // Buttons
  Button * m_dec;
  Button * m_inc;

  // Scroll information
  float        scroll_target;
  int          scroll_counter;
  int          start_drag;
  int          start_drag_offset;
  int          offset;
  int          scrollbar_dim;
  int          nonselectable_width;
  Point2f      scroll_speed;
  Point2i      first_mouse_position;
  ScrollMode   scroll_mode;
  bool         vertical;

  virtual void __Update(const Point2i & mousePosition,
                        const Point2i & lastMousePosition);
  Rectanglei GetScrollThumb() const;
  Rectanglei GetScrollTrack() const
  {
    Point2i s;
    if (vertical)
      s.SetValues(scrollbar_dim, GetTrackDimension());
    else
      s.SetValues(GetTrackDimension(), scrollbar_dim);
    return Rectanglei(GetScrollTrackPos(), s);
  }
  Point2i    GetScrollTrackPos() const;
  void ScrollToPos(int position);
  int GetMaxOffset() const
  {
    if (vertical)
      return box->GetSizeY() - size.y;
    return box->GetSizeX() - (size.x - nonselectable_width);
  }
  int GetMinOffset() const
  {
    if (vertical)
      return 0;
    return - nonselectable_width;
  }
  bool HasScrollBar() const { return GetMaxOffset() > 0; }
  bool LargeDrag(const Point2i& mousePos) const
  {
    int pos = (vertical) ? mousePos.y : mousePos.x;
    return abs(start_drag-pos) < 2;
  }

public:
  ScrollBox(const Point2i & size, bool force = true, bool alternate = false, bool vertical=true);
  virtual ~ScrollBox();

  // No need for a Draw method: the additional stuff drawn is made by Update
  virtual bool Update(const Point2i &mousePosition,
                      const Point2i &lastMousePosition);
  virtual Widget* Click(const Point2i & mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i & mousePosition, uint button);
  virtual bool SendKey(const SDL_keysym & key);
  virtual void Pack();

  // to add a widget
  virtual bool Contains(const Point2i & point) const
  {
    return scroll_mode != SCROLL_MODE_NONE || Widget::Contains(point);
  }
  virtual void AddWidget(Widget* widget);
  virtual void RemoveWidget(Widget* w) { box->RemoveWidget(w); }
  virtual void RemoveFirstWidget()
  {
    Widget *w = box->GetFirstWidget();
    if (w) {
      RemoveWidget(w);
    }
  }
  virtual size_t WidgetCount() const { return box->WidgetCount(); }
  virtual void Empty() { offset = 0; box->Empty(); }
  virtual void Clear() { offset = 0; box->Clear(); }
  void SetMargin(uint margin) { box->SetMargin(margin); }
  uint GetMargin() { return box->GetMargin(); }
  int GetTrackDimension() const;
  void SetExtraWidthMode() { nonselectable_width = size.x/2; }

  virtual bool IsScrolling();

};

#endif  //SCROLL_BOX_H
