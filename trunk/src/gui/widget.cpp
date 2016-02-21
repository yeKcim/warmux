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
 * Widget
 *****************************************************************************/

#include "gui/widget.h"

#include "gui/container.h"
#include "graphic/colors.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/app.h"
#include <WARMUX_debug.h>

Widget::Widget()
  : Rectanglei()
  , has_focus(false)
  , visible(true)
  , is_highlighted(false)
  , border_color(transparent_color)
  , border_size(0)
  , background_color(transparent_color)
  , highlight_bg_color(transparent_color)
  , ct(NULL)
  , need_redrawing(true)
  , clickable(true)
  , profile(NULL)
  , widgetNode(NULL)
  , actionName("NoAction")
{
}

Widget::Widget(const Point2i &size, bool clickable)
  : Rectanglei(0, 0, size.x, size.y)
  , has_focus(false)
  , visible(true)
  , is_highlighted(false)
  , border_color(transparent_color)
  , border_size(0)
  , background_color(transparent_color)
  , highlight_bg_color(transparent_color)
  , ct(NULL)
  , need_redrawing(true)
  , clickable(clickable)
  , profile(NULL)
  , widgetNode(NULL)
  , actionName("NoAction")
{
}

Widget::Widget(Profile * _profile,
               const xmlNode * _widgetNode)
  : Rectanglei(0, 0, size.x, size.y)
  , has_focus(false)
  , visible(true)
  , is_highlighted(false)
  , border_color(transparent_color)
  , border_size(0)
  , background_color(transparent_color)
  , highlight_bg_color(transparent_color)
  , ct(NULL)
  , need_redrawing(true)
  , profile(_profile)
  , widgetNode(_widgetNode)
  , actionName("NoAction")
{
}

// From Container: it redraws the border and the background
void Widget::RedrawBackground(const Rectanglei& rect) const
{
  if (ct != NULL)
    ct->RedrawBackground(rect);

  if (!visible)
    return;

  Surface& surf = GetMainWindow();

  if (IsHighlighted() && highlight_bg_color != transparent_color) {
    surf.BoxColor(*this, highlight_bg_color);
  } else if (background_color != transparent_color) {
    surf.BoxColor(rect, background_color);
  }
}

void Widget::RedrawForeground() const
{
  if (!visible || !border_size)
    return;

  Surface& surf = GetMainWindow();

  if (IsLOGGING("widget.border"))
    surf.RectangleColor(*this, c_red, border_size);
  else if (border_size != 0 && border_color != transparent_color)
    surf.RectangleColor(*this, border_color, border_size);
}

void Widget::ParseXMLMisc(void)
{
  if (!profile || !widgetNode)
    return;

  XmlReader * xmlFile = profile->GetXMLDocument();

  xmlFile->ReadStringAttr(widgetNode, "action", actionName);
}

void Widget::ParseXMLBorder(void)
{
  if (!profile || !widgetNode)
    return;

  XmlReader * xmlFile = profile->GetXMLDocument();

  int borderSize = 0;
  xmlFile->ReadPixelAttr(widgetNode, "borderSize", borderSize);
  Color borderColor = defaultOptionColorRect;
  xmlFile->ReadHexColorAttr(widgetNode, "borderColor", borderColor);
  SetBorder(borderColor, borderSize);
}

void Widget::ParseXMLBackground(void)
{
  if (!profile || !widgetNode)
    return;

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

  if (!profile || !widgetNode)
    return finalValue;

  XmlReader * xmlFile = profile->GetXMLDocument();
  float tmpValue;

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

  if (!profile || !widgetNode)
    return finalValue;

  XmlReader * xmlFile = profile->GetXMLDocument();
  float tmpValue;

  if (xmlFile->ReadPercentageAttr(widgetNode, attributName, tmpValue)) {
    finalValue = GetMainWindow().GetHeight() * tmpValue / 100;
  } else {
    xmlFile->ReadPixelAttr(widgetNode, attributName, finalValue);
  }
  return finalValue;
}

void Widget::ParseXMLGeometry(void)
{
  if (!profile || !widgetNode) {
    return;
  }

  ParseXMLSize();

  XmlReader * xmlFile = profile->GetXMLDocument();

  std::string alignType;
  xmlFile->ReadStringAttr(widgetNode, "alignType", alignType);
  if ("manual" == alignType) {
    ParseXMLPosition();
  } else if ("centeredInX" == alignType) {
    SetPosition((GetMainWindow().GetWidth() - GetSizeX()) / 2,
                ParseVerticalTypeAttribut("y", 0));
  } else if ("centeredInY" == alignType) {
    SetPosition(ParseHorizontalTypeAttribut("x", 0),
                (GetMainWindow().GetHeight() - GetSizeY()) / 2);
  } else if ("centeredInXY" == alignType) {
    SetPosition((GetMainWindow().GetSize() - GetSize()) / 2);
  } else {
    ParseXMLPosition();
  }
}

bool Widget::Update(const Point2i &mousePosition,
                    const Point2i &lastMousePosition)
{
  bool updated = false;
  if (need_redrawing ||
      IsScrolling() ||
      (SDL_GetMouseState(NULL, NULL) && Rectanglei::Contains(mousePosition)) ||
      (Rectanglei::Contains(mousePosition) && !Rectanglei::Contains(lastMousePosition)) ||
      (Rectanglei::Contains(lastMousePosition) && !Rectanglei::Contains(mousePosition))) {

    // Redraw the border and the background
    RedrawBackground(*this);

    __Update(mousePosition, lastMousePosition);

    if (visible) {
      Draw(mousePosition);
    }
    RedrawForeground();
    updated = true;
  }
  need_redrawing = false;
  return updated;
}

void Widget::SetFocus(bool focus)
{
  if (has_focus != focus || is_highlighted != focus) {
    has_focus = focus;
    is_highlighted = focus;
    NeedRedrawing();
  }
}

Widget* Widget::Click(const Point2i &mousePosition, uint /* button */)
{
  if (clickable && visible && Contains(mousePosition)) {
    NeedRedrawing();
    return this;
  }

  return NULL;
}

Widget* Widget::ClickUp(const Point2i &mousePosition, uint /* button */)
{
  if (clickable && visible && Contains(mousePosition)) {
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

void Widget::SetBorder(const Color &_border_color, uint _border_size)
{
  if (border_color != _border_color || border_size != _border_size) {
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

// backup must be passed back to UnsetClip!
Rectanglei Widget::GetClip(Rectanglei& backup) const
{
  Rectanglei wlr = *this;
  Rectanglei wlr_tmp;

  // Get current clip rectangle in wlr_original, SDL clip rectangle is now garbage
  SwapWindowClip(backup);
  if (wlr.GetSizeX()>0 && wlr.GetSizeY()>0) {
    // Clip widget clip rectangle with current one
    wlr.Clip(backup);
    if (!wlr.GetSizeX() || !wlr.GetSizeY()) {
      SwapWindowClip(backup);
      return wlr;
    }
    // Back up final clip rectangle
    wlr_tmp = wlr;
  } else {
    // Clip rectangle was in fact garbage
    wlr_tmp = wlr = backup;
  }
  // Set final clip rectangle, wlr_tmp now has the previous garbage clip rectangle
  SwapWindowClip(wlr_tmp);

  return wlr;
}

void Widget::UnsetClip(Rectanglei& backup) const
{
  SwapWindowClip(backup);
}
