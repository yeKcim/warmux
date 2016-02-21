/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 * Widget
 *****************************************************************************/

#include "gui/widget.h"

#include "gui/container.h"
#include "graphic/colors.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include <WORMUX_debug.h>

Widget::Widget():
  Rectanglei(),
  has_focus(false),
  visible(true),
  is_highlighted(false),
  border_color(white_color),
  border_size(0),
  background_color(transparent_color),
  highlight_bg_color(transparent_color),
  ct(NULL),
  need_redrawing(true),
  profile(NULL),
  widgetNode(NULL),
  actionName("NoAction")
{
}

Widget::Widget(const Point2i &size):
  Rectanglei(0, 0, size.x, size.y),
  has_focus(false),
  visible(true),
  is_highlighted(false),
  border_color(white_color),
  border_size(0),
  background_color(transparent_color),
  highlight_bg_color(transparent_color),
  ct(NULL),
  need_redrawing(true),
  profile(NULL),
  widgetNode(NULL),
  actionName("NoAction")
{
}

Widget::Widget(Profile * _profile,
               const xmlNode * _widgetNode):
  Rectanglei(0, 0, size.x, size.y),
  has_focus(false),
  visible(true),
  is_highlighted(false),
  border_color(white_color),
  border_size(0),
  background_color(transparent_color),
  highlight_bg_color(transparent_color),
  ct(NULL),
  need_redrawing(true),
  profile(_profile),
  widgetNode(_widgetNode),
  actionName("NoAction")
{
}

// From Container: it redraws the border and the background
void Widget::RedrawBackground(const Rectanglei& rect)
{
  Surface& surf = GetMainWindow();

  if (ct != NULL)
    ct->RedrawBackground(rect);

  if (!visible)
    return;

  if (IsHighlighted() && highlight_bg_color != transparent_color) {
    surf.BoxColor(*this, highlight_bg_color);
  } else if (background_color != transparent_color) {
    surf.BoxColor(rect, background_color);
  }

  if (border_size != 0 && border_color != transparent_color) {
    if (rect == *this)
      surf.RectangleColor(*this, border_color, border_size);
    else {
      // TODO: partial redraw of the border...
      ASSERT(border_color.GetAlpha() == SDL_ALPHA_OPAQUE);
      surf.RectangleColor(*this, border_color, border_size);
    }
  }

  if (IsLOGGING("widget.border"))
    surf.RectangleColor(*this, c_red, border_size);
}

void Widget::ParseXMLMisc(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  xmlFile->ReadStringAttr(widgetNode, "action", actionName);
}

void Widget::ParseXMLBorder(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  int borderSize = 0;
  xmlFile->ReadPixelAttr(widgetNode, "borderSize", borderSize);
  Color borderColor = defaultOptionColorRect;
  xmlFile->ReadHexColorAttr(widgetNode, "borderColor", borderColor);
  SetBorder(borderColor, borderSize);
}

void Widget::ParseXMLBackground(void)
{
  if (NULL == profile || NULL == widgetNode) {
    return;
  }
  XmlReader * xmlFile = profile->GetXMLDocument();

  Color backgroundColor = defaultOptionColorBox;
  xmlFile->ReadHexColorAttr(widgetNode, "backgroundColor", backgroundColor);
  SetBackgroundColor(backgroundColor);  
}

void Widget::ParseXMLPosition(void)
{
  int x = ParseHorizontalTypeAttribut("x", 0);
  int y = ParseVerticalTypeAttribut("y", 0);
  SetPosition(x, y);
}

void Widget::ParseXMLSize(void)
{
  int width = ParseHorizontalTypeAttribut("width", 100);
  int height = ParseVerticalTypeAttribut("height", 100);
  SetSize(width, height);
}

int Widget::ParseHorizontalTypeAttribut(const std::string & attributName,
                                        int defaultValue)
{
  int finalValue = defaultValue;

  if (NULL == profile || NULL == widgetNode) {
    return finalValue;
  }

  XmlReader * xmlFile = profile->GetXMLDocument();
  Double tmpValue;
  
  if (xmlFile->ReadPercentageAttr(widgetNode, attributName, tmpValue)) {
    finalValue = GetMainWindow().GetWidth() * tmpValue / 100;
  } else {
    xmlFile->ReadPixelAttr(widgetNode, attributName, finalValue);
  }
  return finalValue;
}

int Widget::ParseVerticalTypeAttribut(const std::string & attributName, 
                                      int defaultValue)
{
  int finalValue = defaultValue;
  
  if (NULL == profile || NULL == widgetNode) {
    return finalValue;
  }

  XmlReader * xmlFile = profile->GetXMLDocument();
  Double tmpValue;

  if (xmlFile->ReadPercentageAttr(widgetNode, attributName, tmpValue)) {
    finalValue = GetMainWindow().GetHeight() * tmpValue / 100;
  } else {
    xmlFile->ReadPixelAttr(widgetNode, attributName, finalValue);
  }
  return finalValue;
}

void Widget::Update(const Point2i &mousePosition,
                    const Point2i &lastMousePosition)
{
  if (need_redrawing ||
      (Contains(mousePosition) && mousePosition != lastMousePosition) ||
      (Contains(lastMousePosition) && !Contains(mousePosition))) {

    // Redraw the border and the background
    RedrawBackground(*this);

    __Update(mousePosition, lastMousePosition);

    if (visible) {
      Draw(mousePosition);
    }
  }
  need_redrawing = false;
}

void Widget::SetFocus(bool focus)
{
  if (has_focus != focus
      || is_highlighted != focus) {
    has_focus = focus;
    is_highlighted = focus;
    NeedRedrawing();
  }
}

Widget* Widget::Click(const Point2i &mousePosition, uint /* button */)
{
  if (Contains(mousePosition)) {
    NeedRedrawing();
    return this;
  }
  return NULL;
}

Widget* Widget::ClickUp(const Point2i &mousePosition, uint /* button */)
{
  if (Contains(mousePosition)) {
    NeedRedrawing();
    return this;
  }
  return NULL;
}

void Widget::SetVisible(bool _visible)
{
  if (visible != _visible) {
    visible = _visible;
    NeedRedrawing();
  }
}

bool Widget::Contains(const Point2i& point) const
{
  return (Rectanglei::Contains(point) && visible);
}

void Widget::SetBorder(const Color &_border_color, uint _border_size)
{
  if (border_color != _border_color ||
      border_size != _border_size) {
    border_color = _border_color;
    border_size = _border_size;
    NeedRedrawing();
  }
}

void Widget::SetBackgroundColor(const Color &bg_color)
{
  if (background_color != bg_color) {
    background_color = bg_color;
    NeedRedrawing();
  }
}

bool Widget::IsHighlighted() const
{
  return (is_highlighted || HasFocus());
}

void Widget::SetHighlighted(bool focus)
{
  if (is_highlighted != focus) {
    is_highlighted = focus;
    NeedRedrawing();
  }
}

void Widget::SetHighlightBgColor(const Color &_highlight_bg_color)
{
  if (highlight_bg_color != _highlight_bg_color) {
    highlight_bg_color = _highlight_bg_color;
    NeedRedrawing();
  }
}

