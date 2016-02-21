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
 * Text box widget
 *****************************************************************************/

#include "include/app.h"
#include "text_box.h"
#include "label.h"

TextBox::TextBox (const std::string &label, const Rectanglei &rect, 
		  Font::font_size_t fsize, Font::font_style_t fstyle) :
  Label(label, rect, fsize, fstyle),
  cursor_pos(label.size())
{
}

void TextBox::SetText(std::string const &new_txt)
{
  Label::SetText(new_txt);
  cursor_pos = new_txt.size();
}

void TextBox::SetCursor(std::string::size_type pos)
{
  if(pos > GetText().size())
  {
    cursor_pos = GetText().size();
  }
  else
  {
    cursor_pos = pos;
  }
}

TextBox::~TextBox(){
}

void TextBox::SendKey(SDL_keysym key)
{
  need_redrawing = true;

  std::string new_txt = GetText();

  if (strcmp(SDL_GetKeyName(key.sym),"backspace")==0)
  {
    if(cursor_pos != 0)
    {
      while((new_txt[--cursor_pos] & 0xc0) == 0x80)
      {
        new_txt.erase(cursor_pos, 1);
      }
      new_txt.erase(cursor_pos, 1);
      Label::SetText(new_txt);
    }
  }
  else if(strcmp(SDL_GetKeyName(key.sym),"left")==0)
  {
    if(cursor_pos != 0)
    {
      while((new_txt[--cursor_pos] & 0xc0) == 0x80);
    }
  }
  else if(strcmp(SDL_GetKeyName(key.sym),"right")==0)
  {
    if(cursor_pos < new_txt.size())
    {
      while((new_txt[++cursor_pos] & 0xc0) == 0x80);
    }
  }
  else
  {
    if(key.unicode > 0)
    {
      if(key.unicode < 0x80) { // 1 byte char
          new_txt.insert(cursor_pos++, 1, (char)key.unicode);
      }
      else if (key.unicode < 0x800) // 2 byte char
      {
        new_txt.insert(cursor_pos++, 1, (char)(((key.unicode & 0x7c0) >> 6) | 0xc0));
        new_txt.insert(cursor_pos++, 1, (char)((key.unicode & 0x3f) | 0x80));
      }
      else // if (key.unicode < 0x10000) // 3 byte char
      {
        new_txt.insert(cursor_pos++, 1, (char)(((key.unicode & 0xf000) >> 12) | 0xe0));
        new_txt.insert(cursor_pos++, 1, (char)(((key.unicode & 0xfc0) >> 6) | 0x80));
        new_txt.insert(cursor_pos++, 1, (char)((key.unicode & 0x3f) | 0x80));
      }
    }
    Label::SetText(new_txt);
  }
}

void TextBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  if (!hidden) 
    {
      if(have_focus)
        surf.BoxColor(*this, highlightOptionColorBox);

      surf.RectangleColor(*this, defaultOptionColorRect);

      Label::Draw(mousePosition, surf);

      //sort of a hacky way to get the cursor pos, but I couldn't find anything better...
      Text txt_before_cursor(*txt_label);
      txt_before_cursor.Set(GetText().substr(0, cursor_pos));

      surf.VlineColor(GetPositionX()+txt_before_cursor.GetWidth(),
              GetPositionY()+2,
              GetPositionY()+GetSizeY()-4, c_white);
    }
}
