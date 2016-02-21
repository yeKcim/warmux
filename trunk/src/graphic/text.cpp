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

#include <iostream> //cerr
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "interface/interface.h"
#include "map/map.h"
#include "tool/xml_document.h"

Text::Text(const std::string & text,
           const Color & fontColor,
           uint fontSize,
           Font::font_style_t fontStyle,
           bool isShadowed,
           const Color & _shadowColor,
           bool _dummy,
           Alignment align) :
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
  font_style(fontStyle),
  offset(0),
  align(align)
{
  center = (align==ALIGN_CENTER || align==ALIGN_CENTER_TOP || align==ALIGN_CENTER_BOTTOM);
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
  font_style(Font::FONT_NORMAL),
  offset(0),
  center(false),
  align(ALIGN_CENTER)
{
}

void Text::Init()
{
  if (shadowed) {
    int width = Font::GetInstance(font_size, font_style)->GetWidth("x");
    bg_offset = (uint)(width>>3); // shadow offset = 0.125ex
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
  if ("%VERSION%" == xmlText) {
    xmlText = Constants::WARMUX_VERSION;
  } else if ("%WEB_SITE%" == xmlText) {
    xmlText = Constants::WEB_SITE;
  }

  Color textColor(0, 255);
  xmlFile->ReadHexColorAttr(textNode, "textColor", textColor);

  // Load the font size ... based on 72 DPI
  int fontSize = 12;
  float tmpValue;

  if (xmlFile->ReadPercentageAttr(textNode, "fontSize", tmpValue)) {
    fontSize = GetMainWindow().GetHeight() * tmpValue / 100;
  } else {
    xmlFile->ReadPixelAttr(textNode, "fontSize", fontSize);
  }

  std::string fontStyle;
  xmlFile->ReadStringAttr(textNode, "fontStyle", fontStyle);

  bool activeShadow = false;
  xmlFile->ReadBoolAttr(textNode, "shadow", activeShadow);
  Color shadowColor(255, 255);
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

    if (max_width != 0 || txt.find_first_of('\n', 0)!=std::string::npos) {
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
  std::vector<std::string> ret_lines;
  uint line_width = 0;
  uint max_line_width = 0;
  std::string::size_type ret_old_pos = 0;
  while (ret_old_pos < txt.size()) {
    std::string::size_type ret_current_pos = txt.find_first_of('\n', ret_old_pos);
    std::string line = (ret_current_pos==std::string::npos)
                     ? txt.substr(ret_old_pos, std::string::npos)
                     : txt.substr(ret_old_pos, ret_current_pos-ret_old_pos);
    std::string::size_type old_pos = 0, current_pos = 0;
    std::vector<std::string> tokens;
    uint index_word = 0;
    uint index_lines = 0;

    while (old_pos < line.size() &&
           (current_pos = line.find_first_of(' ', old_pos)) != std::string::npos) {
      std::string tmp = line.substr(old_pos, current_pos-old_pos);
      if (tmp != " " && tmp != "") {
        tokens.push_back(tmp);
      }
      old_pos = current_pos+1;
    }
    tokens.push_back(line.substr(old_pos));

    // Compute size
    std::vector<std::string> lines;
    while (index_word < tokens.size()) {
      if (lines.size() == index_lines) {
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
    std::vector<std::string>::iterator it = lines.begin();
    while (it != lines.end()) {
      ret_lines.push_back(std::string(*it));
      ++it;
    }

    if (ret_current_pos == std::string::npos)
      break;
    ret_old_pos = ret_current_pos+1;
  }

  // really Render !

  // First, creating a destination surface
  if (max_line_width == 0) {
    max_line_width = max_width;
  }

  // We reduce interline space by using GetLineHeight,
  // but we still want the last line to be properly displayed
  Point2i size(max_line_width,
               GetLineHeight(font)*(ret_lines.size()-1)+font->GetHeight());
#ifdef HAVE_HANDHELD
  Surface tmp = Surface(size, SDL_SWSURFACE, false);
  surf = tmp.DisplayFormat();

  tmp = font->CreateSurface(ret_lines[0], color);
  Uint32 ckey = tmp.GetSurface()->format->colorkey;
  surf.Fill(ckey);
  surf.SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, ckey);
  surf.Blit(tmp);

  // for all remaining lines
  for (uint i = 1; i < ret_lines.size(); i++) {
    tmp = font->CreateSurface(ret_lines[i], color);
    int x = (center) ? (size.x-tmp.GetWidth())/2 : 0;
    surf.Blit(tmp, Point2i(x, GetLineHeight(font)*i));
  }

  // Render the shadow !
  if (!shadowed)
    return;

  tmp = Surface(size, SDL_SWSURFACE, false);
  background = tmp.DisplayFormat();

  tmp = font->CreateSurface(ret_lines[0], black_color);
  ckey = tmp.GetSurface()->format->colorkey;
  background.Fill(ckey);
  background.SetColorKey(SDL_SRCCOLORKEY|SDL_RLEACCEL, ckey);
  background.Blit(tmp);

  // Putting pixels of each image in destination surface
  // for each lines
  for (uint i = 1; i < ret_lines.size(); i++) {
    tmp = font->CreateSurface(ret_lines[i], black_color);
    int x = (center) ? (size.x-tmp.GetWidth())/2 : 0;
    background.Blit(tmp, Point2i(x, GetLineHeight(font)*i));
  }
#else
  surf = Surface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);

  // for each line
  for (uint i = 0; i < ret_lines.size(); i++) {
    Surface tmp = font->CreateSurface(ret_lines[i], color);
    int x = (center) ? (size.x-tmp.GetWidth())/2 : 0;
    surf.MergeSurface(tmp, Point2i(x, GetLineHeight(font)*i));
  }

  // Render the shadow !
  if (!shadowed)
    return;

  background = Surface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);

  // Putting pixels of each image in destination surface
  // for each lines
  for (uint i = 0; i < ret_lines.size(); i++) {
    Surface tmp = font->CreateSurface(ret_lines[i], black_color);
    int x = (center) ? (size.x-tmp.GetWidth())/2 : 0;
    background.MergeSurface(tmp, Point2i(x, GetLineHeight(font)*i));
  }
#endif
}

void Text::DrawLeftTop(const Point2i &position) const
{
  if(txt == "" && !dummy) return;

  Rectanglei dst_rect(position + Point2i(offset, 0), surf.GetSize());
  Surface& window = GetMainWindow();

  if(shadowed){
    Rectanglei shad_rect;

    shad_rect.SetPosition(dst_rect.GetPosition() + bg_offset);
    shad_rect.SetSize(background.GetWidth(), background.GetHeight() );

    window.Blit(background, shad_rect.GetPosition());
    window.Blit(surf, dst_rect.GetPosition());

    GetWorld().ToRedrawOnScreen(Rectanglei(dst_rect.GetPosition(),
                                           shad_rect.GetSize() + bg_offset));
  }else{
    window.Blit(surf, dst_rect.GetPosition());
    GetWorld().ToRedrawOnScreen(dst_rect);
  }
}


void Text::DrawCenterTopOnMap(const Point2i &pos) const
{
  if(shadowed)
    AbsoluteDraw(background, Point2i(bg_offset + pos.x - surf.GetWidth() / 2,
                                     bg_offset + pos.y) );
  AbsoluteDraw(surf, Point2i(pos.x - surf.GetWidth() / 2, pos.y) );
}

void Text::DrawCursor(const Point2i &text_pos, std::string::size_type cursor_pos, int real_width)
{
  // the cursor position is expressed in number of bytes, taking care of UTF8 character

  //sort of a hacky way to get the cursor pos, but I couldn't find anything better...
  int txt_width = 1;
  if (GetText() != "") {
    Text txt_before_cursor(*this);
    txt_before_cursor.SetText(GetText().substr(0, cursor_pos));
    txt_width = txt_before_cursor.GetWidth();
    if (txt_width > real_width)
      offset = real_width - 2 - txt_width;
    if (txt_width+offset < 0)
      offset = -txt_width;
  }
  GetMainWindow().VlineColor(text_pos.GetX()+txt_width+offset,
                             text_pos.GetY(),
                             text_pos.GetY()+GetHeight()-2, c_white);
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

  AppWarmux * app = AppWarmux::GetInstance();

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
