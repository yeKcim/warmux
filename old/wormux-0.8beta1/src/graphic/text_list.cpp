/******************************************************************************
 *  Wormux is a convivial mass murder game.
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

#include "text_list.h"


TextList::TextList()
{}

TextList::~TextList()
{
  list.clear();
}

void TextList::AddText(const std::string &txt, uint maxlines){
  Text* new_txt = new Text(txt);
  list.push_back(new_txt);
  
  if(list.size() >= maxlines)
    list.pop_front();
}

int TextList::Size(){
  return list.size();
}

void TextList::DeleteLine(){
  if(list.size())
    list.pop_front();
}

void TextList::Clear(){
  list.clear();
}

void TextList::Draw(int x, int y, int height){
  iterator it = list.begin(), end = list.end();

  for(; it!=end; it++){
    //Draw each item in the list
    (*it)->DrawTopLeft(x, y);
    y+=height;
  }
}

void TextList::DrawLine(Text* newline, int x, int y, int height){
  int size = list.size();

  y += (size * height);
  newline->DrawTopLeft(x, y);
}
