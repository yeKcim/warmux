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
 ******************************************************************************/

#ifndef FONT_H
#define FONT_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include "include/base.h"
#include "colors.h"
#include "surface.h"

// Forward declarations
class Game;

class Font
{
private:

  /* If you need this, implement it */
  Font(const Font&);
  Font operator=(const Font&);
  /**********************************/

  typedef std::map<std::string, Surface>::value_type
    txt_sample;
  typedef std::map<std::string, Surface>::iterator
    txt_iterator;
  static const int FONT_SIZE[];
  static Font* FONT_ARRAY[6];
  static Font* FONT_ARRAY_BOLD[6];
  static Font* FONT_ARRAY_ITALIC[6];
  static bool  LIB_INIT;

  std::map<std::string, Surface> surface_text_table;
  TTF_Font *m_font;
  void Write(const Point2i &pos, const Surface &surface) const;

  Font(int size);

public:
  // Size
  typedef enum {
    FONT_HUGE = 0,
    FONT_LARGE = 1,
    FONT_BIG = 2,
    FONT_MEDIUM = 3,
    FONT_SMALL = 4,
    FONT_TINY = 5
  } font_size_t;

  // Style
  typedef enum {
    FONT_NORMAL = 0,
    FONT_BOLD = 1,
    FONT_ITALIC = 2,
  } font_style_t;

  // type: defined as static consts above
  static Font* GetInstance(font_size_t size, font_style_t style = FONT_NORMAL);
  static void ReleaseInstances(void);

  ~Font();

  static Surface GenerateSurface(const std::string &txt, const Color &color,
                                 font_size_t size = FONT_MEDIUM, font_style_t style = FONT_NORMAL);

  void WriteLeft(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteLeftBottom(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteRight(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteCenterTop(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteCenter(const Point2i &pos, const std::string &txt, const Color &color);

  static int GetPointSize(font_size_t size) { return FONT_SIZE[size]; };
  int GetWidth(const std::string &txt) const;
  int GetHeight() const;
  int GetHeight(const std::string &txt) const;
  Point2i GetSize(const std::string &txt) const;

  Surface Render(const std::string &txt, const Color &color, bool cache=false);
  Surface CreateSurface(const std::string &txt, const Color &color);
  void SetBold();
  void SetItalic();
};

#endif
