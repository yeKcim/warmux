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

#include <SDL_video.h>
#include <SDL_gfxPrimitives.h>
#include <string>
#include <iostream> //cerr
#include "text.h"
#include "color.h"
#include "colors.h"
#include "font.h"
#include "video.h"
#include "../include/app.h"
#include "../tool/error.h"
#include "../interface/interface.h"
#include "../map/map.h"

Text::Text(const std::string &new_txt, const Color& new_color,
           Font* new_font, bool shadowed)
{
				
  if( new_font == NULL )
    new_font = Font::GetInstance(Font::FONT_SMALL);
	
  txt = new_txt;
  color = new_color;
  font = new_font;
  this->shadowed = shadowed;

  if( shadowed ){
    int width = font->GetWidth("x");
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

  surf = font->CreateSurface(txt, color);
  if ( shadowed ) {
    background = font->CreateSurface(txt, black_color);
  }
}

void Text::RenderMultiLines()
{
  if (txt=="") return;

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

  // Putting pixels of each image in destination surface
  surf.Lock();

  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    Surface tmp=(font->CreateSurface(lines.at(i), color)).DisplayFormatAlpha();
    tmp.Lock();

    // for each pixel lines of a source image
    for (int x=0; x < tmp.GetWidth() && x < int(max_width); x++) 
      { // for each pixel rows of a source image
	for (int y=0; y < tmp.GetHeight(); y++) 
	  { 
	    surf.PutPixel(x, ((font->GetHeight()+2)*i)+y, 
			  tmp.GetPixel(x, y));
	  }
      }
    tmp.Unlock();
  }
  surf.Unlock();

  // Render the shadow !
  if (!shadowed) return;

  background.NewSurface(size, SDL_SWSURFACE|SDL_SRCALPHA, true);
  background = background.DisplayFormatAlpha();

  // Putting pixels of each image in destination surface
  background.Lock();

  // for each lines
  for (uint i = 0; i < lines.size(); i++) {
    Surface tmp=(font->CreateSurface(lines.at(i), black_color)).DisplayFormatAlpha();
    tmp.Lock();

    // for each pixel lines of a source image
    for (int x=0; x < tmp.GetWidth() && x < int(max_width); x++) 
      { // for each pixel rows of a source image
	for (int y=0; y < tmp.GetHeight(); y++) 
	  { 
	    background.PutPixel(x, ((font->GetHeight()+2)*i)+y, 
				tmp.GetPixel(x, y));
	  }
      }
    tmp.Unlock();
  }
  background.Unlock();
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
  
  pos.y -= font.GetHeight(txt)/2;

  Rectanglei rect( pos - size/2, size);
  
  AppWormux * app = AppWormux::GetInstance();

  app->video.window.BoxColor(rect, boxColor);
  app->video.window.RectangleColor(rect, rectColor);  

  world.ToRedrawOnScreen( rect );
  font.WriteCenterTop( pos, txt, white_color);
}

// void DrawTmpBoxTextWithReturns(Font &font, const Point2i &position, 
//                                const std::string &txt, uint space,
//                                Color boxColor,
//                                Color rectColor)
// {
//   size_t pos          = 0;
//   size_t last_pos     = 0;
//   size_t max_width    = 0;
//   size_t total_height = 0;
//   int    x, y;
//   char  *lines        = strdup(txt.c_str());

//   std::vector< size_t > offsets;

//   // Store offsets
//   offsets.push_back(0);
//   while (lines[pos] != '\0')
//   {
//     if (lines[pos] == '\n')
//     {
//       lines[pos] = 0;
//       if (!lines[pos+1]) break;

//       offsets.push_back(pos+1);
//       int w = font.GetWidth(lines+last_pos) + space*2;
//       if ((int)max_width < w) max_width = w;
//       total_height += font.GetHeight(lines+last_pos);
// #if DEBUG
//       if (last_pos)
//       {
//         std::cerr << "(" << pos << "," << pos-last_pos
//                   << ") >>> " << lines+last_pos << " <<<\n";
//       }
// #endif
//       last_pos = pos+1;
//     }
//     pos++;
//   }
//   if (max_width == 0) {
//     max_width = font.GetWidth(lines) + space*2;
//   }

//   // Initial position
//   total_height += 5*space;
//   x = position.x - max_width / 2;
//   y = position.y - total_height / 2;

//   Rectanglei rect(x, y, max_width, total_height);
  
//   AppWormux * app = AppWormux::GetInstance();

//   app->video.window.BoxColor(rect, boxColor);
//   app->video.window.RectangleColor(rect, rectColor);

//   world.ToRedrawOnScreen(rect);

//   for( std::vector<size_t>::iterator it=offsets.begin();
//        it != offsets.end();
//        ++it)
//   {
//     font.WriteLeft( Point2i(x+space, y+space), lines+(*it), white_color);
//     y += font.GetHeight(lines+(*it));
//   }
//   offsets.clear();
//   free(lines);
// }
