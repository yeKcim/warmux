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

#include <SDL_keyboard.h>
#include "gui/text_box.h"
#include "graphic/text.h"
#include "tool/text_handling.h"
#include "tool/copynpaste.h"

TextBox::TextBox (const std::string &label, const Point2i &_size,
                  Font::font_size_t fsize, Font::font_style_t fstyle) :
  Label(label, _size, fsize, fstyle),
  cursor_pos(label.size())
{
  Widget::SetBorder(defaultOptionColorRect, 1);
  Widget::SetHighlightBgColor(highlightOptionColorBox);
}

void TextBox::BasicSetText(std::string const &new_txt)
{
  Font* font = Font::GetInstance(font_size, font_style);

  if (font->GetWidth(new_txt) < GetSizeX() - 5) {
    Label::SetText(new_txt);
  }
}

void TextBox::SetText(std::string const &new_txt)
{
  BasicSetText(new_txt);

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

bool TextBox::SendKey(const SDL_keysym& key)
{
  bool used = true;

  NeedRedrawing();

  std::string new_txt = GetText();

  used = TextHandle(new_txt, cursor_pos, key);

  if (new_txt != GetText())
    BasicSetText(new_txt);
  
  return used;
}

void TextBox::Draw(const Point2i &mousePosition, Surface& surf) const
{
  Label::Draw(mousePosition, surf);
  txt_label->DrawCursor(position, cursor_pos);
}

Widget* TextBox::ClickUp(const Point2i &, uint button)
{
  bool used = true;

  NeedRedrawing();
  if (button == SDL_BUTTON_MIDDLE)
  {
    std::string new_txt = GetText();
    used = RetrieveBuffer(new_txt, cursor_pos);

    if (new_txt != GetText())
      BasicSetText(new_txt);
  }
  return used ? this : NULL;
};
