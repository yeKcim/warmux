/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Text box widget
 *****************************************************************************/

#include "../include/app.h"
#include "text_box.h"
#include "label.h"

TextBox::TextBox (const std::string &label, const Rectanglei &rect, Font& _font) :
  Label(label, rect, _font)
{
}

TextBox::~TextBox(){
}

void TextBox::SendKey(SDL_keysym key)
{
  need_redrawing = true;

  std::string new_txt = GetText();
  if (strcmp(SDL_GetKeyName(key.sym),"backspace")==0)
  {
    if(new_txt != "")
      new_txt = new_txt.substr(0, new_txt.size()-1);
  }
  else
  {
    if(key.unicode < 0x80 && key.unicode > 0)
      new_txt = new_txt + (char)key.unicode;
  }

  SetText(new_txt);
}

void TextBox::Draw(const Point2i &mousePosition, Surface& surf)
{
  if(have_focus)
    surf.BoxColor(*this, highlightOptionColorBox);

  surf.RectangleColor(*this, defaultOptionColorRect);

  Label::Draw(mousePosition, surf);
  surf.VlineColor(GetPositionX()+txt_label->GetWidth(), 
		  GetPositionY()+2, 
		  GetPositionY()+GetSizeY()-4, c_white);
}
