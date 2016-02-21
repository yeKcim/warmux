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
 * Vertical or Horizontal Box
 *****************************************************************************/

#ifndef GUI_BOX_H
#define GUI_BOX_H

//-----------------------------------------------------------------------------
#include "widget.h"
#include <list>
//-----------------------------------------------------------------------------

class Box : public Widget
{
 protected:
  std::list<Widget *> widgets;
  Widget * last_widget;
  bool visible;
  uint margin;
  uint w_border, h_border;

 public:
  Box(uint x, uint y, uint w, uint h, bool _visible=true);
  virtual ~Box();

  void Draw (uint mouse_x, uint mouse_y);
  bool Clic (uint mouse_x, uint mouse_y, uint button);
  void SetMargin(uint _margin);
  void SetBorder(uint _w_border, uint _h_border);

  virtual void AddWidget(Widget * a_widget) = 0;
};

//-----------------------------------------------------------------------------

class VBox : public Box
{
 public:
  VBox(uint x, uint y, uint w, bool _visible=true);
  void AddWidget(Widget * a_widget);
  void SetSizePosition(uint _x, uint _y, uint _w, uint _h);
};

//-----------------------------------------------------------------------------

class HBox : public Box
{
 public:
  HBox(uint x, uint y, uint h, bool _visible=true);
  void AddWidget(Widget * a_widget);  
  void SetSizePosition(uint _x, uint _y, uint _w, uint _h);
};

//-----------------------------------------------------------------------------
#endif

