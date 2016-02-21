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

#include <SDL_gfxPrimitives.h>
#include "box.h"
#include "../include/app.h"

//-----------------------------------------------------------------------------

Box::Box(uint x, uint y, uint w, uint h, 
	 bool _visible) :
  Widget(x,y,w,h)
{
  last_widget = NULL;
  visible = _visible;
  margin = 5;
  w_border = h_border = 5;
}

//-----------------------------------------------------------------------------

Box::~Box()
{
  
  widgets.clear();
}

//-----------------------------------------------------------------------------

void Box::Draw (uint mouse_x, uint mouse_y)
{
  if (visible) {
    boxRGBA(app.sdlwindow, x, y, x+w, y+h,
	    80,80,159,206);

    rectangleRGBA(app.sdlwindow, x, y, x+w, y+h,
		  49, 32, 122, 255);  
  }

  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){
    (*it)->Draw(mouse_x, mouse_y);
  }
}

//-----------------------------------------------------------------------------

bool Box::Clic (uint mouse_x, uint mouse_y, uint button)
{
  bool r=false;

  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){
    assert(it != NULL);
    r = (*it)->Clic(mouse_x, mouse_y, button);
    if (r) return true;
  }

  return false;
}

//-----------------------------------------------------------------------------

void Box::SetMargin (uint _margin)
{
  margin = _margin;
}

//-----------------------------------------------------------------------------

void Box::SetBorder (uint _w_border, uint _h_border)
{
  w_border = _w_border;
  h_border = _h_border;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

VBox::VBox(uint x, uint y, uint w, bool _visible) :
  Box(x, y, w, 1, _visible)
{
}

//-----------------------------------------------------------------------------

void VBox::AddWidget(Widget * a_widget)
{
  assert(a_widget != NULL);

  uint _y;

  if (last_widget != NULL) {
    _y = last_widget->GetY()+last_widget->GetH();
  } else {
    _y = y+h_border-margin;
  }

  a_widget->SetSizePosition(x+w_border, 
			    _y+margin, 
			    w-2*w_border, 
			    a_widget->GetH());

  last_widget = a_widget;

  widgets.push_back(a_widget);

  h = a_widget->GetY() + a_widget->GetH() - y + h_border;
}

//-----------------------------------------------------------------------------

void VBox::SetSizePosition(uint _x, uint _y, uint _w, uint _h)
{
  x = _x;
  y = _y;

  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){

    assert(it != NULL);

    if (it == widgets.begin()) {
      _y += h_border-margin;
    }

    (*it)->SetSizePosition(x+w_border,
			   _y+margin,
			   (*it)->GetW(),
			   (*it)->GetH());
    _y = (*it)->GetY() + (*it)->GetH();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

HBox::HBox(uint x, uint y, uint h, bool _visible) :
  Box(x, y, 1, h, _visible)
{
}

//-----------------------------------------------------------------------------

void HBox::AddWidget(Widget * a_widget)
{
  assert(a_widget != NULL);

  uint _x;

  if (last_widget != NULL) {
    _x = last_widget->GetX()+last_widget->GetW();
  }
  else {
    _x = x+w_border-margin;
  }

  a_widget->SetSizePosition(_x+margin, 
			    y+h_border, 
			    a_widget->GetW(), 
			    h-2*h_border);

  last_widget = a_widget;

  widgets.push_back(a_widget);

  w = a_widget->GetX() + a_widget->GetW() - x + w_border;
}

//-----------------------------------------------------------------------------

void HBox::SetSizePosition(uint _x, uint _y, uint _w, uint _h)
{
  x = _x;
  y = _y;

  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){
    assert(it != NULL);   

    if (it == widgets.begin()) {
      _x += w_border-margin;
    }

    (*it)->SetSizePosition(_x+margin,
			   y+h_border,
			   (*it)->GetW(),
			   (*it)->GetH());
    _x = (*it)->GetX()+ (*it)->GetW();
  }
}

//-----------------------------------------------------------------------------
