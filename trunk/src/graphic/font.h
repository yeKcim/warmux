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

#ifndef FONT_H
#define FONT_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <WARMUX_base.h>
#include "graphic/colors.h"
#include "graphic/surface.h"

class Font
{
  typedef std::map<std::string, Surface>::value_type txt_sample;
  typedef std::map<std::string, Surface>::iterator txt_iterator;

  static bool  LIB_INIT;

  static std::map<int, Font *> fontMapNormal;
  static std::map<int, Font *> fontMapBold;
  static std::map<int, Font *> fontMapItalic;

  std::map<std::string, Surface> surface_text_table;
  TTF_Font *m_font;
  void Write(const Point2i &pos, const Surface &surface) const;

  Font(int size);

public:
  // Size
  typedef enum {
    FONT_HUGE = 40,
    FONT_LARGE = 32,
    FONT_BIG = 24,
    FONT_MEDIUM = 16,
    FONT_SMALL = 12,
    FONT_TINY = 10
  } font_size_t;

  // Style
  typedef enum {
    FONT_NORMAL = 0,
    FONT_BOLD = 1,
    FONT_ITALIC = 2,
  } font_style_t;

  // type: defined as static consts above
  static Font * GetInstance(font_size_t size, 
                            font_style_t style = FONT_BOLD);
  static void ReleaseInstances(void);
  static font_size_t GetFixedSize(uint s)
  {
    if (s <= (FONT_TINY+FONT_SMALL)/2) return FONT_TINY;
    if (s <= (FONT_SMALL+FONT_MEDIUM)/2) return FONT_SMALL;
    if (s <= (FONT_MEDIUM+FONT_BIG)/2) return FONT_MEDIUM;
    if (s <= (FONT_BIG+FONT_LARGE)/2) return FONT_LARGE;
    return FONT_HUGE;
  }

  ~Font();

  static Surface GenerateSurface(const std::string &txt, const Color &color,
                                 font_size_t size = FONT_MEDIUM, font_style_t style = FONT_BOLD);

  void WriteLeft(const Point2i &pos, const std::string &txt, const Color &color)
  {
    Surface surface(Render(txt, color, true));
    Write(pos, surface);
  }
  void WriteLeftBottom(const Point2i &pos, const std::string &txt, const Color &color)
  {
    Surface surface(Render(txt, color, true));
    Write(pos - Point2i(0, surface.GetHeight()), surface);
  }
  void WriteRight(const Point2i &pos, const std::string &txt, const Color &color)
  {
    Surface surface(Render(txt, color, true));
    Write(pos - Point2i(surface.GetWidth(), 0), surface);
  }
  void WriteCenterTop(const Point2i &pos, const std::string &txt, const Color &color)
  {
    Surface surface(Render(txt, color, true));
    Write(pos - Point2i(surface.GetWidth()>>1, 0), surface);
  }
  void WriteCenter(const Point2i &pos, const std::string &txt, const Color &color)
  {
    Surface surface(Render(txt, color, true));
    Write(pos - Point2i(surface.GetWidth()>>1, surface.GetHeight()), surface);
  }

  int GetWidth(const std::string &txt) const;
  int GetHeight() const;
  int GetHeight(const std::string &txt) const;
  Point2i GetSize(const std::string &txt) const { return Point2i(GetWidth(txt), GetHeight(txt)); }

  Surface Render(const std::string &txt, const Color &color, bool cache=false);
  Surface CreateSurface(const std::string &txt, const Color &color);
  void SetBold();
  void SetItalic();
};

#endif
