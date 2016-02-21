/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

Label::Label(const std::string & text,
             uint maxWidth,
             Font::font_size_t fontSize,
             Font::font_style_t fontStyle,
             const Color & fontColor,
             Text::Alignment align,
             bool shadowed,
             const Color & shadowColor)
  : Text(text, fontColor, fontSize, fontStyle, shadowed, shadowColor, false, align)
{
  size.x = maxWidth;
  SetMaxWidth(size.x);
  size.y = GetHeight();
  Widget::clickable = false;
}

Label::Label(const Point2i & size)
  : Widget(size, false)
{
  align = Text::ALIGN_LEFT_TOP;
}

Label::Label(Profile * profile,
             const xmlNode * labelNode)
  : Widget(profile, labelNode)
{
  align = Text::ALIGN_LEFT_TOP;
}

Label::~Label()
{
}

bool Label::LoadXMLConfiguration()
{
  if (NULL == profile || NULL == widgetNode) {
    return false;
  }

  ParseXMLGeometry();
  ParseXMLBorder();
  ParseXMLBackground();

  Text::LoadXMLConfiguration(profile->GetXMLDocument(),
                             widgetNode);

  return true;
}

void Label::Draw(const Point2i& /*mousePosition*/)
{
  switch (align) {
  case Text::ALIGN_CENTER:
    DrawCenter(position + size/2);
    break;
  case Text::ALIGN_CENTER_TOP:
    DrawCenterTop(Point2i(position.x + size.x/2, position.y));
    break;
  case Text::ALIGN_CENTER_BOTTOM:
    DrawCenterBottom(Point2i(position.x + size.x/2, position.y + size.y));
    break;
  case Text::ALIGN_LEFT_CENTER:
    DrawLeftCenter(Point2i(position.x, position.y + size.y/2));
    break;
  case Text::ALIGN_RIGHT_CENTER:
    DrawRightCenter(Point2i(position.x + size.x, position.y + size.y/2));
    break;
   case Text::ALIGN_RIGHT_TOP:
    DrawRightTop(Point2i(position.x + size.x, position.y));
    break;
  case Text::ALIGN_LEFT_TOP:
  default:
    DrawLeftTop(position);
    break;
  }
}

void Label::Pack()
{
  if (max_width) SetMaxWidth(size.x);
  size.y = GetHeight();
}

void Label::SetText(const std::string & new_txt)
{
  NeedRedrawing();

  Text::SetText(new_txt);

  if (max_width) SetMaxWidth(size.x);
  size.y = GetHeight();
}
