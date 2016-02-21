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
 ******************************************************************************/

#include <iostream> //cerr
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/interface.h"
#include "map/map.h"

Text::Text(const std::string & text,
           const Color & fontColor,
           uint fontSize,
           Font::font_style_t fontStyle,
           bool isShadowed,
           const Color & _shadowColor,
           bool _dummy) :
  surf(),
  background(),
  txt(text),
  color(fontColor),
  shadowed(isShadowed),
  dummy(_dummy),
  bg_offset(0),
  max_width(0),
  shadowColor(_shadowColor),
  font_size((Font::font_size_t)fontSize),
  font_style(fontStyle)
{
  Init();
}

Text::Text() :
  surf(),
  background(),
  txt("No text"),
  color(black_color),
  shadowed(true),
  dummy(false),
  bg_offset(0),
  max_width(0),
  shadowColor(),
  font_size(Font::FONT_SMALL),
  font_style(Font::FONT_NORMAL)
{
}

Text::~Text()
{
}

void Text::Init()
{
  if (shadowed) {
    int width = Font::GetInstance(font_size, font_style)->GetWidth("x");
    bg_offset = (unsigned int)width/8; // shadow offset = 0.125ex
    if (bg_offset < 1) {
      bg_offset = 1;
    }
  }
  Render();
}

void Text::LoadXMLConfiguration(XmlReader * xmlFile,
                                const xmlNode * textNode)
{
  std::string xmlText("Text not found");
  xmlFile->ReadStringAttr(textNode, "text", xmlText);

  Color textColor(0, 0, 0, 255);
  xmlFile->ReadHexColorAttr(textNode, "textColor", textColor);

  // Load the font size ... based on 72 DPI
  int fontSize = 12;
  Double tmpValue;

  if (xmlFile->ReadPercentageAttr(textNode, "fontSize", tmpValue)) {
    fontSize = GetMainWindow().GetHeight() * tmpValue / 100;
  } else {
    xmlFile->ReadPixelAttr(textNode, "fontSize", fontSize);
  }

  std::string fontStyle;
  xmlFile->ReadStringAttr(textNode, "fontStyle", fontStyle);

  bool activeShadow = false;
  xmlFile->ReadBoolAttr(textNode, "shadow", activeShadow);
  Color shadowColor(255, 255, 255, 255);
  xmlFile->ReadHexColorAttr(textNode, "shadowColor", shadowColor);

  SetText(xmlText);
  SetFont(textColor,
          (Font::font_size_t)fontSize,
          DetectFontStyle(fontStyle),
          activeShadow,
          shadowColor);
  Init();
}

Font::font_style_t Text::DetectFontStyle(const std::string & fontStyle)
{
  if ("bold" == fontStyle) {
    return Font::FONT_BOLD;
  } else if ("italic" == fontStyle) {
    return Font::FONT_ITALIC;
  }
  return Font::FONT_NORMAL;
}

void Text::Render()
{
  if (!dummy) {
    if ("" == txt) {
      return;
    }

    if (max_width != 0) {
      RenderMultiLines();
      return;
    }

    Font* font = Font::GetInstance(font_size, font_style);

    surf = font->CreateSurface(txt, color);
    if (shadowed) {
      background = font->CreateSurface(txt, shadowColor);
    }
  } else {
    surf = Surface(Point2i(font_size, font_size), 0);
    if (shadowed) {
      background = Surface(Point2i(font_size, font_size), 0);
    }
  }
}

void Text::RenderMultiLines()
{
  if ("" == txt) {
    return;
  }

  Font* font = Font::GetInstance(font_size, font_style);

  // Make a first try
  if (font->GetWidth(txt) < int(max_width)) {
    surf = font->CreateSurface(txt, color);
    if (shadowed) {
      background = font->CreateSurface(txt, shadowColor);
    }
    return;
  }

  // Cut the text on space
  std::vector<std::string> tokens;
  std::string::size_type old_pos = 0, current_pos = 0;

  while ( old_pos < txt.size() &&
          (current_pos = txt.find_first_of(" ", old_pos)) != std::string::npos )
    {
      std::string tmp = txt.substr(old_pos, current_pos-old_pos);
      if (tmp != " " && tmp != "") {
        tokens.push_back(tmp);
      }
      old_pos = current_pos+1;
    }
  tokens.push_back(txt.substr(old_pos));

  // Compute size
  std::vector<std::string> lines;
  uint index_lines = 0;
  uint index_word = 0;
  uint line_width = 0;
  uint max_line_width = 0;

  while (index_word < tokens.size())
    {
      if ( lines.size() == index_lines ) {
        // first word of a line
        lines.push_back(tokens.at(index_word));

        // compute current line size
        line_width = font->GetWidth(tokens.at(index_word));
        if (line_width > max_line_width) {
            max_line_width = line_width;
        }

      } else {
        line_width = font->GetWidth(lines.at(index_lines)+" "+tokens.at(index_word));

        if ( line_width > max_width ) {

          // line will be too long : prepare next line!
          index_lines++;
          index_word--;

        } else {
          lines.at(index_lines) += " " + tokens.at(index_word);

          // this is the longest line
          if (line_width > max_line_width) {
            max_line_width = line_width;
          }
        }

      }

      index_word++;
    }

  // really Render !

  // First, creating a destination surface
  if (max_line_width == 0) {
    max_line_width = max_width;
  }
  Point2i size(max_line_width, (font->GetHeight()+2) * lines.size());
  Surface tmp = Surface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);
  surf = tmp.DisplayFormatAlpha();

  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    tmp=(font->CreateSurface(lines.at(i), color)).DisplayFormatAlpha();
    surf.MergeSurface(tmp, Point2i(0, (font->GetHeight() + 2) * i));
  }

  // Render the shadow !
  if (!shadowed) return;

  tmp = Surface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);
  background = tmp.DisplayFormatAlpha();

  // Putting pixels of each image in destination surface
  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    tmp=(font->CreateSurface(lines.at(i), black_color)).DisplayFormatAlpha();
    background.MergeSurface(tmp, Point2i(0, (font->GetHeight() + 2) * i));
  }
}

void Text::SetText(const std::string &new_txt)
{
  if(txt == new_txt)
    return;

  txt = new_txt;

  Render();
}

const std::string& Text::GetText() const
{
  return txt;
}

void Text::SetColor(const Color &new_color)
{
  if(color == new_color)
    return;

  color = new_color;

  Render();
}

void Text::DrawCenter (const Point2i &position) const
{
  DrawTopLeft(position - surf.GetSize() / 2);
}

void Text::DrawTopRight (const Point2i &position) const
{
  DrawTopLeft(position - Point2i(surf.GetWidth(), 0));
}

void Text::DrawCenterTop (const Point2i &position) const
{
  DrawTopLeft(position - Point2i(surf.GetWidth()/2, 0));
}

void Text::DrawTopLeft(const Point2i &position) const
{
  if(txt == "" && !dummy) return;

  Rectanglei dst_rect(position, surf.GetSize());
  AppWormux * app = AppWormux::GetInstance();

  if(shadowed){
    Rectanglei shad_rect;

    shad_rect.SetPosition(dst_rect.GetPosition() + bg_offset);
    shad_rect.SetSize(background.GetWidth(), background.GetHeight() );

    app->video->window.Blit(background, shad_rect.GetPosition());
    app->video->window.Blit(surf, dst_rect.GetPosition());

    GetWorld().ToRedrawOnScreen(Rectanglei(dst_rect.GetPosition(),
                                      shad_rect.GetSize() + bg_offset));
  }else{
    app->video->window.Blit(surf, dst_rect.GetPosition());
    GetWorld().ToRedrawOnScreen(dst_rect);
  }
}


void Text::DrawCenterTopOnMap (const Point2i &pos) const
{
  if(shadowed)
    AbsoluteDraw(background, Point2i(bg_offset + pos.x - surf.GetWidth() / 2,
                                     bg_offset + pos.y) );
  AbsoluteDraw(surf, Point2i(pos.x - surf.GetWidth() / 2, pos.y) );
}

void Text::DrawCursor(const Point2i &text_pos, std::string::size_type cursor_pos) const
{
  // the cursor position is expressed in number of bytes, taking care of UTF8 character

  //sort of a hacky way to get the cursor pos, but I couldn't find anything better...
  uint txt_width = 1;
  if (GetText() != "") {
    Text txt_before_cursor(*this);
    txt_before_cursor.SetText(GetText().substr(0, cursor_pos));
    txt_width = txt_before_cursor.GetWidth();
  }
  GetMainWindow().VlineColor(text_pos.GetX()+txt_width,
						     text_pos.GetY()+2,
						     text_pos.GetY()+GetHeight()-4, c_white);
}

void Text::SetMaxWidth(uint max_w)
{
  if (max_width == max_w)
    return;

  max_width = max_w;

  Render();
}

int Text::GetWidth() const
{
  if (txt=="" && !dummy) return 0;
  return surf.GetWidth();
}

int Text::GetHeight() const
{
  Font* font = Font::GetInstance(font_size, font_style);
  if (txt=="" || dummy) {
    return font->GetHeight() + bg_offset;
  }
  return std::max(surf.GetHeight(), font->GetHeight()) + bg_offset;
}

void DrawTmpBoxText(Font& font, Point2i pos,
                    const std::string& txt, uint space,
                    const Color& boxColor, const Color& rectColor)
{
  Point2i size = font.GetSize(txt) + Point2i(space, space)*2;

  Rectanglei rect( pos - size/2, size);

  AppWormux * app = AppWormux::GetInstance();

  app->video->window.BoxColor(rect, boxColor);
  app->video->window.RectangleColor(rect, rectColor);

  GetWorld().ToRedrawOnScreen( rect );

  pos.y += font.GetHeight(txt)/2;
  font.WriteCenter( pos, txt, white_color);
}

void Text::SetFont(const Color &_font_color,
                   const Font::font_size_t _font_size,
                   const Font::font_style_t _font_style,
                   bool _font_shadowed,
                   const Color & _shadowColor)
{
  color = _font_color;
  font_size = _font_size;
  font_style = _font_style;
  shadowColor = _shadowColor;

  if (shadowed != _font_shadowed) {
    // recompute shadow offset
    shadowed = _font_shadowed;
    Init();
  }
}

