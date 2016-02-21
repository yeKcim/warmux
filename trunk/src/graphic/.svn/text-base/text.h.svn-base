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
 ******************************************************************************/

#ifndef TEXT_H
#define TEXT_H

#include <string>
#include "colors.h"
#include "font.h"
#include "surface.h"

// Forward declaration
class XmlReader;
typedef struct _xmlNode xmlNode;

class Text
{
public:
  typedef enum
  {
    ALIGN_CENTER,
    ALIGN_LEFT_TOP,
    ALIGN_RIGHT_TOP,
    ALIGN_LEFT_CENTER,
    ALIGN_RIGHT_CENTER,
    ALIGN_CENTER_TOP,
    ALIGN_CENTER_BOTTOM
  } Alignment;

protected:
  Surface surf;
  Surface background; //shadow or outline or nothing;
  std::string txt;
  Color color;
  bool shadowed;
  bool dummy;
  uint bg_offset;
  uint max_width;
  Color shadowColor;
  Font::font_size_t font_size;
  Font::font_style_t font_style;
  int offset;
  bool center;
  Alignment align;

  virtual void Render();
  static int GetLineHeight(const Font *f) { return (7*f->GetHeight())>>3; }
  void RenderMultiLines();

public:
  Text(const std::string & text,
       const Color & fontColor = white_color,
       uint fontSize = (uint)Font::FONT_SMALL,
       Font::font_style_t fontStyle = Font::FONT_BOLD,
       bool shadowed = false,
       const Color & shadowColor = black_color,
       bool dummy = false,
       Alignment align = ALIGN_CENTER);
  Text(void);

  virtual ~Text() { }

  void Init();
  void LoadXMLConfiguration(XmlReader * xmlFile,
                            const xmlNode * textNode);
  Font::font_style_t DetectFontStyle(const std::string & fontStyle);

  //Draw method using windows coordinates
  void DrawCenter(const Point2i & position) const { DrawLeftTop(position - surf.GetSize() / 2); }
  void DrawLeftTop(const Point2i & position) const;
  void DrawRightTop(const Point2i & position) const { DrawLeftTop(position - Point2i(surf.GetWidth(), 0)); }
  void DrawCenterTop(const Point2i & position) const { DrawLeftTop(position - Point2i(surf.GetWidth()/2, 0)); }
  void DrawLeftCenter(const Point2i & position) const { DrawLeftTop(position - Point2i(0, surf.GetHeight()/2)); }
  void DrawRightCenter(const Point2i & position) const { DrawLeftTop(position - Point2i(surf.GetWidth(), surf.GetHeight()/2)); }
  void DrawCenterBottom(const Point2i & position) const { DrawLeftTop(position - surf.GetSize()); }

  //Draw text cursor only (text_pos = position for DrawTopLeft)
  void DrawCursor(const Point2i & text_pos, std::string::size_type cursor_pos, int real_width);

  //Draw method using map coordinates
  void DrawCenterTopOnMap(const Point2i & position) const;

  void SetText(const std::string & new_txt)
  {
    if (txt == new_txt)
      return;

    txt = new_txt;

    Render();
  }
  const std::string & GetText() const { return txt; }
  void SetColor(const Color & new_color)
  {
    if (color == new_color)
      return;

    color = new_color;

    Render();
  }
  void SetMaxWidth(uint max_w)
  {
    if (max_width == max_w)
      return;

    max_width = max_w;

    Render();
  }
  int GetWidth() const { return (txt=="" && !dummy) ? 0 : surf.GetWidth(); }
  int GetHeight() const;

  const Color & GetFontColor() const { return color; };
  Font::font_size_t GetFontSize() const { return font_size; };
  Font::font_style_t GetFontStyle() const { return font_style; };
  bool IsFontShadowed() const { return shadowed; };
  const Color & GetShadowColor(void) const { return this->shadowColor; };

  void SetFont(const Color & font_color,
               const Font::font_size_t font_size,
               const Font::font_style_t font_style,
               bool font_shadowed,
               const Color & shadowColor = black_color);
};

void DrawTmpBoxText(Font& font,
                    Point2i pos,
                    const std::string& txt, uint space=3,
                    const Color& boxColor = defaultColorBox,
                    const Color& rectColor = defaultColorRect);

#endif
