/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
#include <SDL_image.h>
#include <SDL_video.h>
#include <iostream>
#include <string>
#include "../game/config.h"
#include "../include/app.h"
#include "font.h"
#include "colors.h"
#include "../tool/file_tools.h"
#include <exception>
#include "../map/map.h"
#include "../tool/error.h"
//-----------------------------------------------------------------------------

bool Font::InitAllFonts()
{ 
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Font::Font()
{ 
  m_font = NULL; 
}

//-----------------------------------------------------------------------------

Font::Font(int size) 
{ 
  m_font = NULL; 
  bool ok = Load(Wormux::config.ttf_filename, size);
  if (!ok)
    Error("Error during initialisation of a font!");
}

//-----------------------------------------------------------------------------

Font::~Font()
{
  if (m_font != NULL) {
    TTF_CloseFont(m_font);
    m_font = NULL;
  }

  
  txt_iterator it;
  for (it = surface_text_table.begin(); 
       it != surface_text_table.end(); 
       ++it){
    SDL_FreeSurface(it->second);
    surface_text_table.erase(it->first);
  }
}

//-----------------------------------------------------------------------------

bool Font::Load (const std::string& filename, int size) 
{
  bool ok = false;
  if (FichierExiste(filename))
  {
      m_font = TTF_OpenFont(filename.c_str(), size);
      ok = (m_font != NULL);
  }
  if (!ok)
  {
      std::cout << "Error: Font " << filename << " can't be found!" << std::endl;
      return false;
  }
  TTF_SetFontStyle(m_font,TTF_STYLE_NORMAL);
  return true;
}

//-----------------------------------------------------------------------------

void Font::WriteLeft (int x, int y, const std::string &txt, 
		      SDL_Color color)
{ 
  //assert (m_font != NULL);
  SDL_Surface * text_surface = Render(txt.c_str(), color, true);
  SDL_Rect dst_rect;
  dst_rect.x = x;
  dst_rect.y = y;
  dst_rect.h = text_surface->h;
  dst_rect.w = text_surface->w;

  SDL_BlitSurface(text_surface,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x,dst_rect.y, dst_rect.w, dst_rect.h));
}

// //-----------------------------------------------------------------------------

void Font::WriteLeftBottom (int x, int y, const std::string &txt,
			    SDL_Color color)
{ 
  //assert (m_font != NULL);
  SDL_Surface * text_surface = Render(txt.c_str(), color, true);
  SDL_Rect dst_rect;
  dst_rect.x = x;
  dst_rect.y = y - GetHeight();
  dst_rect.h = text_surface->h;
  dst_rect.w = text_surface->w;

  SDL_BlitSurface(text_surface,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x,dst_rect.y, dst_rect.w, dst_rect.h));
}

// //-----------------------------------------------------------------------------

void Font::WriteRight (int x, int y, const std::string &txt,
		       SDL_Color color)
{ 
  //assert (m_font != NULL);
  SDL_Surface * text_surface = Render(txt.c_str(), color, true);
  SDL_Rect dst_rect;
  dst_rect.x = x - GetWidth(txt);
  dst_rect.y = y;
  dst_rect.h = text_surface->h;
  dst_rect.w = text_surface->w;

  SDL_BlitSurface(text_surface, NULL, app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x,dst_rect.y, dst_rect.w, dst_rect.h));
}

// //-----------------------------------------------------------------------------

void Font::WriteCenter (int x, int y, const std::string &txt,
			SDL_Color color)
{ 
  //assert (m_font != NULL);
  SDL_Surface * text_surface = Render(txt.c_str(), color, true);
  SDL_Rect dst_rect;
  dst_rect.x = x - GetWidth(txt)/2;
  dst_rect.y = y - GetHeight()/2;
  dst_rect.h = text_surface->h;
  dst_rect.w = text_surface->w;

  SDL_BlitSurface(text_surface, NULL, app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x,dst_rect.y, dst_rect.w, dst_rect.h));
}

// //-----------------------------------------------------------------------------

void Font::WriteCenterTop (int x, int y, const std::string &txt,
			   SDL_Color color)
{ 
  //assert (m_font != NULL);
  SDL_Surface * text_surface = Render(txt.c_str(), color, true);
  SDL_Rect dst_rect;
  dst_rect.x = x - GetWidth(txt)/2;
  dst_rect.y = y;
  dst_rect.h = text_surface->h;
  dst_rect.w = text_surface->w;

  SDL_BlitSurface(text_surface, NULL, app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x,dst_rect.y, dst_rect.w, dst_rect.h));
}

//-----------------------------------------------------------------------------


SDL_Surface * Font::Render(const std::string &txt, SDL_Color color, bool cache)
{
  SDL_Surface * surface = NULL;
  
  if (cache) {
    txt_iterator p = surface_text_table.find(txt);
    if (p == surface_text_table.end() ) {
      
      if (surface_text_table.size() > 5) {
        SDL_FreeSurface(surface_text_table.begin()->second);
        surface_text_table.erase(surface_text_table.begin());
      }
      surface = TTF_RenderUTF8_Blended(m_font, txt.c_str(), 
				       color); //, black_color);

      surface_text_table.insert(txt_sample(txt, surface));
    } else {
      txt_iterator p = surface_text_table.find(txt);
      surface = p->second;
    }
  } else {
    surface = TTF_RenderUTF8_Blended(m_font, txt.c_str(), 
				     color); //, black_color);
  }
  assert (surface != NULL);
  return surface;
}

//-----------------------------------------------------------------------------

int Font::GetWidth (const std::string &txt)
{ 
  //assert (m_font != NULL);
  int width=-1;
  TTF_SizeUTF8(m_font, txt.c_str(), &width, NULL);
  return width;
}

//-----------------------------------------------------------------------------

int Font::GetHeight ()
{ 
  //assert (m_font != NULL);
  return TTF_FontHeight(m_font);
}

//-----------------------------------------------------------------------------

int Font::GetHeight (const std::string &str)
{ 
  int height=-1;
  TTF_SizeUTF8(m_font, str.c_str(), NULL, &height);
  return height;
}

//-----------------------------------------------------------------------------

