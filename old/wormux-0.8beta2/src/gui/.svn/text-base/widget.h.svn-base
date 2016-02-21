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
#include "tool/rectangle.h"
#include "tool/point.h"

class Container;
class Surface;
struct SDL_keysym;

class Widget : public Rectanglei
{
  Widget(const Widget&);
  const Widget& operator=(const Widget&);

 protected:
  Container * ct;
  bool need_redrawing;
  void StdSetSizePosition(const Rectanglei &rect);

 public:
  bool have_focus;
  bool is_selected;

  Widget();
  Widget(const Rectanglei &rect);
  virtual ~Widget() { };

  virtual void Update(const Point2i &mousePosition,
                      const Point2i &lastMousePosition,
                      Surface& surf); // virtual only for Box and ListBox
  virtual void Draw(const Point2i &mousePosition,
                    Surface& surf) const = 0;
  virtual void ForceRedraw() { need_redrawing = true; }; // set need_redrawing to true; -- virtual for widget_list

  virtual void SendKey(const SDL_keysym&) { };
  virtual Widget* Click(const Point2i &, uint)
  {
    need_redrawing = true;
    return this;
  };
  virtual Widget* ClickUp(const Point2i &, uint)
  {
    need_redrawing = true;
    return this;
  };
  bool IsSelected() const { return is_selected; };
  virtual void Select() { is_selected = true; need_redrawing = true; };
  virtual void Unselect() { is_selected = false; need_redrawing = true; };

  void SetContainer(Container * _ct) { ct = _ct; };

  virtual void SetSizePosition(const Rectanglei &rect) = 0;
  void SetXY(int _x, int _y){
    SetSizePosition( Rectanglei(Point2i(_x, _y), size) );
  };
};

#endif

