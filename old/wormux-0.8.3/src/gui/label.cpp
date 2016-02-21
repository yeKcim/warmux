/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Label in GUI.
 *****************************************************************************/

#include "gui/label.h"
#include "graphic/text.h"

Label::Label (const std::string &label,
              uint max_width,
              Font::font_size_t fsize,
              Font::font_style_t fstyle,
              const Color& color,
              bool _center,
              bool shadowed):
  center(_center)
{
  Widget::SetFont(color, fsize, fstyle, shadowed, false);

  size.x = max_width;
  txt_label = new Text(label, color, fsize, fstyle, shadowed, label.empty());
  txt_label->SetMaxWidth(size.x);
  size.y = txt_label->GetHeight();
}

Label::~Label()
{
  delete txt_label;
}

void Label::Draw(const Point2i &/*mousePosition*/) const
{
  if (!center)
    txt_label->DrawTopLeft(position);
  else
    txt_label->DrawCenterTop(Point2i(position.x + size.x/2, position.y));
}

void Label::Pack()
{
  txt_label->SetMaxWidth(size.x);
  size.y = txt_label->GetHeight();
}

void Label::SetText(const std::string &new_txt)
{
  NeedRedrawing();

  if (txt_label)
    delete txt_label;

  txt_label = new Text(new_txt, GetFontColor(), GetFontSize(), GetFontStyle(), IsFontShadowed(), new_txt.empty());
  txt_label->SetMaxWidth(size.x);
  size.y = txt_label->GetHeight();
}

const std::string& Label::GetText() const
{
  return txt_label->GetText();
}

void Label::OnFontChange()
{
  SetText(GetText());
}
