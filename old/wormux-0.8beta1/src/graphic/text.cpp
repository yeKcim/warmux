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

#include <SDL_video.h>
#include <SDL_gfxPrimitives.h>
#include <string>
#include <iostream> //cerr
#include "text.h"
#include "color.h"
#include "colors.h"
#include "font.h"
#include "video.h"
#include "include/app.h"
#include "tool/error.h"
#include "interface/interface.h"
#include "map/map.h"

Text::Text(const std::string &new_txt, 
	   const Color& new_color,
	   Font::font_size_t fsize, 
	   Font::font_style_t fstyle, 
           bool _shadowed)
{
  font_size = fsize;
  font_style = fstyle;

  txt = new_txt;
  color = new_color;
  shadowed = _shadowed;

  if( shadowed ){
    int width = Font::GetInstance(font_size, font_style)->GetWidth("x");
    bg_offset = (unsigned int)width/8; // shadow offset = 0.125ex
    if (bg_offset < 1) bg_offset = 1;
  }
  else {
    bg_offset = 0;
  }
  max_width = 0;

  Render();
}

Text::~Text()
{
}

void Text::Render()
{
  if (txt=="") return;

  if (max_width != 0) {
    RenderMultiLines();
    return;
  }
  
  Font* font = Font::GetInstance(font_size, font_style);

  surf = font->CreateSurface(txt, color);
  if ( shadowed ) {
    background = font->CreateSurface(txt, black_color);
  }
}

void Text::RenderMultiLines()
{
  if (txt=="") return;

  Font* font = Font::GetInstance(font_size, font_style);

  // Make a first try
  if (font->GetWidth(txt) < int(max_width)) {
    surf = font->CreateSurface(txt, color);
    if ( shadowed ) {
      background = font->CreateSurface(txt, black_color);
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
      if (tmp != " ") {
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
  surf.NewSurface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);
  surf = surf.DisplayFormatAlpha();

  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    Surface tmp=(font->CreateSurface(lines.at(i), color)).DisplayFormatAlpha();
    surf.MergeSurface(tmp, Point2i(0, (font->GetHeight() + 2) * i));
  }

  // Render the shadow !
  if (!shadowed) return;

  background.NewSurface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);
  background = background.DisplayFormatAlpha();

  // Putting pixels of each image in destination surface
  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    Surface tmp=(font->CreateSurface(lines.at(i), black_color)).DisplayFormatAlpha();
    background.MergeSurface(tmp, Point2i(0, (font->GetHeight() + 2) * i));
  }
}

void Text::Set(const std::string &new_txt)
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

void Text::DrawCenter (int x, int y) const
{ 
  DrawTopLeft(x - surf.GetWidth() / 2, y - surf.GetHeight() / 2);
}

void Text::DrawCenter (const Point2i &position) const
{
  DrawCenter(position.GetX(), position.GetY());
}

void Text::DrawTopRight (int x, int y) const
{ 
  DrawTopLeft( x - surf.GetWidth(), y);
}

void Text::DrawCenterTop (int x, int y) const
{ 
  DrawTopLeft( x - surf.GetWidth()/2, y);
}

void Text::DrawTopLeft(const Point2i &position) const
{
  if(txt == "") return;

  Rectanglei dst_rect(position, surf.GetSize());
  AppWormux * app = AppWormux::GetInstance();

  if(shadowed){
    Rectanglei shad_rect;
    
    shad_rect.SetPosition(dst_rect.GetPosition() + bg_offset);
    shad_rect.SetSize(background.GetWidth(), background.GetHeight() );
    
    app->video.window.Blit(background, shad_rect.GetPosition());
    app->video.window.Blit(surf, dst_rect.GetPosition());
		
    world.ToRedrawOnScreen(Rectanglei(dst_rect.GetPosition(),
                                      shad_rect.GetSize() + bg_offset));
  }else{
    app->video.window.Blit(surf, dst_rect.GetPosition());
    world.ToRedrawOnScreen(dst_rect);
  }		
}

void Text::DrawTopLeft (int x, int y) const
{ 
  DrawTopLeft( Point2i(x, y) );
}

void Text::DrawCenterOnMap (int x, int y) const
{
  DrawTopLeftOnMap(x - surf.GetWidth()/2, y - surf.GetHeight()/2 );
}

void Text::DrawCenterTopOnMap (int x, int y) const
{
  DrawTopLeftOnMap(x - surf.GetWidth()/2, y);
}

void Text::DrawTopLeftOnMap (int x, int y) const
{
  if(shadowed)
    AbsoluteDraw(background, Point2i(bg_offset + x, bg_offset + y) );
  AbsoluteDraw(surf, Point2i(x, y) );
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
  if(txt=="") return 0;
  return surf.GetWidth();
}

int Text::GetHeight() const
{
  if(txt=="") return 0;
  return surf.GetHeight();
}

void DrawTmpBoxText(Font& font, Point2i pos, 
		    const std::string& txt, uint space,
                    const Color& boxColor, const Color& rectColor)
{
  Point2i size = font.GetSize(txt) + Point2i(space, space)*2;
  
  Rectanglei rect( pos - size/2, size);
  
  AppWormux * app = AppWormux::GetInstance();

  app->video.window.BoxColor(rect, boxColor);
  app->video.window.RectangleColor(rect, rectColor);  

  world.ToRedrawOnScreen( rect );
  
  pos.y += font.GetHeight(txt)/2;
  font.WriteCenter( pos, txt, white_color);
}

