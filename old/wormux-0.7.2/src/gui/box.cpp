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
#include "../graphic/colors.h"
#include "../include/app.h"

Box::Box(const Rectanglei &rect, bool _visible) : Widget( rect ){
  visible = _visible;
  margin = 5;
  border.SetValues(5, 5);
}

Box::~Box(){
  std::list<Widget *>::iterator it;
  for (it = widgets.begin();
      it != widgets.end();
      it++){
    delete *it;
    *it = NULL;
  }
  widgets.clear();
}

void Box::Draw(const Point2i &mousePosition){
  Rectanglei rect(position, size);
	
  if( visible ){
    AppWormux::GetInstance()->video.window.BoxColor(rect, defaultOptionColorBox);
    AppWormux::GetInstance()->video.window.RectangleColor(rect, defaultOptionColorRect);
  }

  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){
    (*it)->Draw(mousePosition);
  }
}

bool Box::Clic (const Point2i &mousePosition, uint button){
  std::list<Widget *>::iterator it;
  for (it = widgets.begin(); 
       it != widgets.end(); 
       ++it){
    if( (*it)->Clic(mousePosition, button) )
      return true;
  }

  return false;
}

void Box::SetMargin (uint _margin){
  margin = _margin;
}

void Box::SetBorder (const Point2i &newBorder){
	border = newBorder;
}

VBox::VBox(const Rectanglei &rect, bool _visible) : Box(rect, _visible){
	size.y = 1;
}

void VBox::AddWidget(Widget * a_widget){
  assert(a_widget != NULL);

  uint _y;

  if(!widgets.empty())
    _y = widgets.back()->GetPositionY() + widgets.back()->GetSizeY();
  else
    _y = position.y + border.y - margin;

  a_widget->SetSizePosition(Rectanglei(position.x + border.x, 
			    _y + margin, 
			    size.x - 2 * border.x,
			    a_widget->GetSizeY() ));

  widgets.push_back(a_widget);

  size.y = a_widget->GetPositionY() + a_widget->GetSizeY() - position.y + border.y;
}

void VBox::SetSizePosition(const Rectanglei &rect){
  position = rect.GetPosition();
  int _y = rect.GetPositionY();
  std::list<Widget *>::iterator it;
  for( it = widgets.begin(); 
       it != widgets.end(); 
       ++it ){

    if( it == widgets.begin() )
      _y += border.y - margin;

    (*it)->SetSizePosition( Rectanglei(position.x + border.x,
			   _y + margin,
			   (*it)->GetSizeX(),
			   (*it)->GetSizeY() ));
    _y = (*it)->GetPositionY() + (*it)->GetSizeY();
  }
}

HBox::HBox(const Rectanglei &rect, bool _visible) :
  Box(rect, _visible){
	  size.x = 1;
}

void HBox::AddWidget(Widget * a_widget){
  assert(a_widget != NULL);

  uint _x;

  if (!widgets.empty())
    _x = widgets.back()->GetPositionX() + widgets.back()->GetSizeX();
  else 
    _x = position.x + border.x - margin;

  a_widget->SetSizePosition( Rectanglei(_x + margin, 
			    position.y + border.y, 
			    a_widget->GetSizeX(), 
			    size.y - 2 * border.y) );

  widgets.push_back(a_widget);

  size.x = a_widget->GetPositionX() + a_widget->GetSizeX() - position.x + border.x;
}

void HBox::SetSizePosition(const Rectanglei &rect){
  position = rect.GetPosition();
  int _x = rect.GetPositionX();
	
  std::list<Widget *>::iterator it;
  for( it = widgets.begin(); 
       it != widgets.end(); 
       ++it ){

    if( it == widgets.begin() )
      _x += border.x - margin;

    (*it)->SetSizePosition( Rectanglei(_x + margin,
			   position.y + border.y,
			   (*it)->GetSizeX(),
			   (*it)->GetSizeY()) );
    _x = (*it)->GetPositionX()+ (*it)->GetSizeX();
  }
}

