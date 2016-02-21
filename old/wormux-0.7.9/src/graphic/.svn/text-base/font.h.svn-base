/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
#include "../include/base.h"
#include "colors.h"
#include "surface.h"

class GameLoop;

class Font
{
private:
  typedef std::map<std::string, Surface>::value_type 
    txt_sample;
  typedef std::map<std::string, Surface>::iterator 
    txt_iterator;
  static const int FONT_SIZE[];
  static Font* FONT_ARRAY[6];
  static Font* FONT_ARRAY_BOLD[6];
  static Font* FONT_ARRAY_ITALIC[6];

  std::map<std::string, Surface> surface_text_table;
  TTF_Font *m_font;
  void Write(const Point2i &pos, Surface &surface);

  Font(int size);

public:
  // Size
  static const int FONT_HUGE;
  static const int FONT_LARGE;
  static const int FONT_BIG;
  static const int FONT_NORMAL;
  static const int FONT_SMALL;
  static const int FONT_TINY;
  // Style
  static const int NORMAL;
  static const int BOLD;
  static const int ITALIC;

  // type: defined as static consts above
  static Font* GetInstance(int type, int font_style = NORMAL);

  ~Font();

  bool Load (const std::string& filename, int size);

  void WriteLeft(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteLeftBottom(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteRight(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteCenterTop(const Point2i &pos, const std::string &txt, const Color &color);
  void WriteCenter(const Point2i &pos, const std::string &txt, const Color &color);
  
  int GetWidth(const std::string &txt);
  int GetHeight();
  int GetHeight(const std::string &txt);
  Point2i GetSize(const std::string &txt);

  Surface Render(const std::string &txt, const Color &color, bool cache=false);
  Surface CreateSurface(const std::string &txt, const Color &color);
  void SetBold();
  void SetItalic();
};

#endif
