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
 * Widget
 *****************************************************************************/

#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "include/base.h"
#include "graphic/color.h"
#include "gui/container.h"
#include "tool/rectangle.h"
#include "tool/point.h"

class Surface;
struct SDL_keysym;

class Widget : public Rectanglei, public Container
{
  bool has_focus;
  bool visible;
  bool is_highlighted;

  Color border_color;
  uint border_size;
  Color background_color;
  Color highlight_bg_color;

  Widget(const Widget&);
  const Widget& operator=(const Widget&);

 protected:
  Container * ct;
  bool need_redrawing;

  void StdSetSizePosition(const Rectanglei &rect);
  virtual void __Update(const Point2i &/* mousePosition */,
			const Point2i &/* lastMousePosition */,
			Surface& /* surf */) {};

  void RedrawBackground(const Rectanglei& rect,
			Surface& surf);

 public:
  Widget();
  Widget(const Rectanglei &rect);
  virtual ~Widget() { };

  virtual void Update(const Point2i &mousePosition,
		      const Point2i &lastMousePosition,
		      Surface& surf); // Virtual for widget_list: to remove!


  virtual void Draw(const Point2i &mousePosition,
                    Surface& surf) const = 0;
  virtual void NeedRedrawing() { need_redrawing = true; }; // set need_redrawing to true; -- virtual for widget_list

  virtual bool SendKey(const SDL_keysym&) { return false; };
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // widget may be hidden
  void SetVisible(bool _visible);

  // manage mouse/keyboard focus
  bool HasFocus() const { return has_focus; };
  void SetFocus(bool focus);

  bool IsHighlighted() const;
  void SetHighlighted(bool focus);

  // border, background color
  void SetBorder(const Color &border_color, uint boder_size);
  void SetBackgroundColor(const Color &background_color);
  void SetHighlightBgColor(const Color &highlight_bg_color);

  void SetContainer(Container * _ct) { ct = _ct; };

  virtual void SetSizePosition(const Rectanglei &rect) = 0;
  void SetXY(int _x, int _y){
    SetSizePosition( Rectanglei(Point2i(_x, _y), size) );
  };
};

#endif

