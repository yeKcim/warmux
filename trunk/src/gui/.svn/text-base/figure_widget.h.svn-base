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
 * PictureWidget having captions
 *****************************************************************************/

#ifndef GUI_FIGURE_WIDGET_H
#define GUI_FIGURE_WIDGET_H

#include <vector>
#include "picture_widget.h"

/** To help determine at compilation time the size of an array */
#define ARRAY_SIZE(array_) (sizeof(array_)/sizeof(*array_))

/** Why can't I initialize such a simple struct like an aggregate !? */
#define DEF_CAPTIONS_PARAMS  Font::FONT_BOLD, dark_gray_color

class FigureWidget : public PictureWidget
{
public:
  struct Caption
  {
    std::string        string; // gcc does not support correctly a char* here.
    int                x, y, w;
    Caption(const std::string& caption,
            int x, int y, int w)
      : string(caption), x(x), y(y), w(w) { }
  };
  typedef std::vector<Caption> Captions;

private:
  Captions           captions;
  Font::font_size_t  fsize;
  Font::font_style_t fstyle;
  Color              color;

public:
  FigureWidget(const Point2i & size,
               const std::string & resource_id,
               Font::font_size_t fsize = Font::FONT_BIG,
               Font::font_style_t fstyle = Font::FONT_NORMAL,
               const Color& color = dark_gray_color,
               ScalingType type = FIT_SCALING)
    : PictureWidget(size, resource_id, type)
    , fsize(fsize)
    , fstyle(fstyle)
    , color(color)
  { }

  virtual void Draw(const Point2i& mousePosition);

  void AddCaption(const std::string& caption,
                  int x, int y, int w)
  {
    captions.push_back(Caption(caption, x, y, w));
  }
};

#endif
