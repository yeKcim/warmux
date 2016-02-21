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
 * Widget
 *****************************************************************************/

#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "include/base.h"
#include "graphic/color.h"
#include "graphic/font.h"
#include "gui/container.h"
#include "tool/rectangle.h"
#include "tool/point.h"

class Surface;
struct SDL_keysym;

#define W_UNDEF 0

class Widget : public Rectanglei, public Container
{
  bool has_focus;
  bool visible;
  bool is_highlighted;

  Color border_color;
  uint border_size;
  Color background_color;
  Color highlight_bg_color;

  Color font_color;
  bool font_shadowed;
  Font::font_size_t font_size;
  Font::font_style_t font_style;

  Widget(const Widget&);
  const Widget& operator=(const Widget&);

 protected:
  Container * ct;
  bool need_redrawing;

  virtual void __Update(const Point2i &/* mousePosition */,
			const Point2i &/* lastMousePosition */) {};

  void RedrawBackground(const Rectanglei& rect);

  // Handle font
  virtual void OnFontChange() {};
  const Color& GetFontColor() const { return font_color; };
  Font::font_size_t GetFontSize() const { return font_size; };
  Font::font_style_t GetFontStyle() const { return font_style; };
  bool IsFontShadowed() const { return font_shadowed; };

 public:
  Widget();
  Widget(const Point2i &size);
  virtual ~Widget() { };

  virtual void Update(const Point2i &mousePosition,
		      const Point2i &lastMousePosition); // Virtual for widget_list: to remove!


  virtual void Draw(const Point2i &mousePosition) const = 0;
  virtual void NeedRedrawing() { need_redrawing = true; }; // set need_redrawing to true; -- virtual for widget_list

  virtual bool SendKey(const SDL_keysym&) { return false; };
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  // widget may be hidden
  void SetVisible(bool _visible);
  bool Contains(const Point2i& point) const; // always false if !visible

  // manage mouse/keyboard focus
  bool HasFocus() const { return has_focus; };
  void SetFocus(bool focus);

  bool IsHighlighted() const;
  void SetHighlighted(bool focus);

  // border, background color
  void SetBorder(const Color &border_color, uint boder_size);
  const Color& GetBorderColor() const { return border_color; };

  void SetBackgroundColor(const Color &background_color);
  const Color& GetBackgroundColor() const { return background_color; };

  void SetHighlightBgColor(const Color &highlight_bg_color);
  const Color& GetHighlightBgColor() const { return highlight_bg_color; };

  // font color
  // If (update_now == true), we call OnFontChange()
  void SetFont(const Color &font_color,
	       const Font::font_size_t font_size,
	       const Font::font_style_t font_style,
	       bool font_shadowed,
	       bool update_now = true);

  void SetContainer(Container * _ct) { ct = _ct; };

  // to manage browsing between the widgets with keyboard
  virtual Widget* GetFirstWidget() const { return NULL; };
  virtual Widget* GetLastWidget() const { return NULL; };
  virtual Widget* GetNextWidget(const Widget */*w*/, bool /*loop*/) const { return NULL; };
  virtual Widget* GetPreviousWidget(const Widget */*w*/, bool /*loop*/) const { return NULL; };
  virtual bool IsWidgetBrowser() const { return false; };

  virtual void Pack() = 0;
};

#endif

