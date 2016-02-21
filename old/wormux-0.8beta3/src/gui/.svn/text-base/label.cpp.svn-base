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
 * Label in GUI.
 *****************************************************************************/

#include "gui/label.h"
#include "graphic/text.h"

Label::Label (const std::string &label,
              const Point2i &_size,
              Font::font_size_t fsize,
              Font::font_style_t fstyle,
              const Color& color,
              bool _center,
              bool _shadowed):
  hidden(false),
  font_size(fsize),
  font_style(fstyle),
  font_color(color),
  center(_center),
  shadowed(_shadowed)
{
  position = Point2i(-1, -1);
  size = _size;

  txt_label = new Text(label, color, fsize, fstyle, _shadowed, label.empty());
  size.y = txt_label->GetHeight();
  txt_label->SetMaxWidth(GetSizeX());
}

Label::~Label()
{
  delete txt_label;
}

void Label::Draw(const Point2i &/*mousePosition*/, Surface& /*surf*/) const
{
  if (!hidden)
    {
      if (!center)
        txt_label->DrawTopLeft(position);
      else
        txt_label->DrawCenterTop(Point2i(position.x + size.x/2, position.y));
    }
}

void Label::SetSizePosition(const Rectanglei &rect)
{
  StdSetSizePosition(rect);
  txt_label->SetMaxWidth(GetSizeX());
  size.y = txt_label->GetHeight();
}

void Label::SetText(const std::string &new_txt)
{
  need_redrawing = true;
  delete txt_label;
  txt_label = new Text(new_txt, font_color, font_size, font_style, shadowed, new_txt.empty());
  txt_label->SetMaxWidth(GetSizeX());
}

const std::string& Label::GetText() const
{
  return txt_label->GetText();
}

void Label::SetVisible(bool visible)
{
  if (hidden == visible) {
    hidden = !visible;
    need_redrawing = true;
  }
}
