/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2007 Jon de Andres
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
 * List of Text Clases.
 * TextBox is not valid while playing game
 * nefertum - Jon de Andres
 *****************************************************************************/

#include "graphic/text_list.h"
#include "graphic/text.h"

TextList::~TextList()
{
  for (std::list<Text*>::iterator t=list.begin(); t!=list.end(); t++)
     delete *t;

  list.clear();
}

void TextList::AddText(const std::string &txt, const Color& color, uint maxlines){
  Text* new_txt = new Text(txt, color);
  list.push_back(new_txt);

  if(list.size() >= maxlines)
    list.pop_front();
}

void TextList::Draw(int x, int y, int height){
  iterator it = list.begin(), end = list.end();

  for(; it!=end; it++){
    //Draw each item in the list
    (*it)->DrawLeftTop(Point2i(x, y));
    y+=height;
  }
}

void TextList::DrawLine(const Text* newline, int x, int y, int height) const {
  int size = list.size();

  y += (size * height);
  newline->DrawLeftTop(Point2i(x, y));
}
