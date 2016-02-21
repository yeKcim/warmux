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

#include <SDL_video.h>
#include <SDL_gfxPrimitives.h>
#include <string>
#include "text.h"
#include "colors.h"
#include "font.h"
#include "video.h"
#include "../include/app.h"
#include "../tool/error.h"
#include "../interface/interface.h"
#include "../map/map.h"
#include "../include/global.h"
//-----------------------------------------------------------------------------

Text::Text(const std::string &new_txt, SDL_Color new_color,
           Font* new_font, bool shadowed)
{
  if (new_font == NULL)
      new_font = &global().small_font();
  txt = new_txt;
  color = new_color;
  font = new_font;
  surf = NULL;
  background = NULL;
  this->shadowed = shadowed;

  if(shadowed)
  {
    int width=-1;
    TTF_SizeUTF8(font->m_font, "x", &width, NULL);
    bg_offset = (unsigned int)width/8; // shadow offset = 0.125ex
    if (bg_offset < 1) bg_offset = 1;
  }
  else
    bg_offset = 0;

  Render();
}

//-----------------------------------------------------------------------------
Text::~Text()
{
  if (surf!=NULL) SDL_FreeSurface(surf);
  if(shadowed)
  {
    if (background!=NULL) SDL_FreeSurface(background);
  }
}

//-----------------------------------------------------------------------------
void Text::Render()
{
  if (surf != NULL)
  {
    SDL_FreeSurface(surf);
    if(shadowed)
      SDL_FreeSurface(background);
  }
  surf = TTF_RenderUTF8_Blended(font->m_font, txt.c_str(),color);
  if(shadowed)
    background = TTF_RenderUTF8_Blended(font->m_font, txt.c_str(),black_color);
}

//-----------------------------------------------------------------------------
void Text::Set(const std::string &new_txt)
{
  if(txt == new_txt)
    return;

  txt = new_txt;
  Render();
}

//-----------------------------------------------------------------------------
void Text::SetColor(SDL_Color new_color)
{
  if(color.r == new_color.r
  && color.g == new_color.g
  && color.b == new_color.b)
    return;

  color = new_color;
  Render();
}

//-----------------------------------------------------------------------------
void Text::DrawCenter (int x, int y)
{ 
  SDL_Rect dst_rect;
  dst_rect.x = x - surf->w / 2;
  dst_rect.y = y - surf->h / 2;
  dst_rect.w = surf->w;
  dst_rect.h = surf->h;

  if(shadowed)
  {
    SDL_Rect shad_rect;
    shad_rect.x = dst_rect.x + bg_offset;
    shad_rect.y = dst_rect.y + bg_offset;
    shad_rect.w = background->w;
    shad_rect.h = background->h;
    SDL_BlitSurface(background,NULL,app.sdlwindow, &shad_rect);
    SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
    world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y,
                                      shad_rect.w+bg_offset, shad_rect.h+bg_offset));
    return;
  }
  SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h));
}

//-----------------------------------------------------------------------------
void Text::DrawTopLeft (int x, int y)
{ 
  SDL_Rect dst_rect;
  dst_rect.x = x;
  dst_rect.y = y;
  dst_rect.w = surf->w;
  dst_rect.h = surf->h;

  if(shadowed)
  {
    SDL_Rect shad_rect;
    shad_rect.x = dst_rect.x + bg_offset;
    shad_rect.y = dst_rect.y + bg_offset;
    shad_rect.w = background->w;
    shad_rect.h = background->h;
    SDL_BlitSurface(background,NULL,app.sdlwindow, &shad_rect);
    SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
    world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y,
                                      shad_rect.w+bg_offset, shad_rect.h+bg_offset));
    return;
  }
  SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h));
}

//-----------------------------------------------------------------------------
void Text::DrawTopRight (int x, int y)
{ 
  SDL_Rect dst_rect;
  dst_rect.x = x - surf->w;
  dst_rect.y = y;
  dst_rect.w = surf->w;
  dst_rect.h = surf->h;

  if(shadowed)
  {
    SDL_Rect shad_rect;
    shad_rect.x = dst_rect.x + bg_offset;
    shad_rect.y = dst_rect.y + bg_offset;
    shad_rect.w = background->w;
    shad_rect.h = background->h;
    SDL_BlitSurface(background,NULL,app.sdlwindow, &shad_rect);
    SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
    world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y,
                                      shad_rect.w+bg_offset, shad_rect.h+bg_offset));
    return;
  }
  SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h));
}

//-----------------------------------------------------------------------------
void Text::DrawCenterTop (int x, int y)
{ 
  SDL_Rect dst_rect;
  dst_rect.x = x - surf->w / 2;
  dst_rect.y = y;
  dst_rect.w = surf->w;
  dst_rect.h = surf->h;

  if(shadowed)
  {
    SDL_Rect shad_rect;
    shad_rect.x = dst_rect.x + bg_offset;
    shad_rect.y = dst_rect.y + bg_offset;
    shad_rect.w = background->w;
    shad_rect.h = background->h;
    SDL_BlitSurface(background,NULL,app.sdlwindow, &shad_rect);
    SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
    world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y,
                                      shad_rect.w+bg_offset, shad_rect.h+bg_offset));
    return;
  }
  SDL_BlitSurface(surf,NULL,app.sdlwindow, &dst_rect);
  world.ToRedrawOnScreen(Rectanglei(dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h));
}

//-----------------------------------------------------------------------------
void Text::DrawCenterOnMap (int x, int y)
{
  if(shadowed)
    AbsoluteDraw(background, bg_offset + x - surf->w / 2, bg_offset + y - surf->h / 2);
  AbsoluteDraw(surf, x - surf->w / 2, y - surf->h / 2);
}

//-----------------------------------------------------------------------------
void Text::DrawTopLeftOnMap (int x, int y)
{
  if(shadowed)
    AbsoluteDraw(background, bg_offset + x, bg_offset + y);
  AbsoluteDraw(surf, x, y);
}

//-----------------------------------------------------------------------------
void Text::DrawCenterTopOnMap (int x, int y)
{
  if(shadowed)
    AbsoluteDraw(background, bg_offset + x - surf->w / 2, bg_offset + y);
  AbsoluteDraw(surf, x - surf->w / 2, y);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// 
void DrawTmpBoxText(Font &font, 
		    int _x, int _y, 
		    const std::string &txt, uint space)
{
  int x,y,w,h;
  w = font.GetWidth(txt)+space*2;
  x = _x - w / 2;

  h = font.GetHeight(txt)+space*2;
  y = _y - h / 2;
  _y -= font.GetHeight(txt)/2;

  boxRGBA(app.sdlwindow, x, y, x+w, y+h,
	  80,80,159,206);

  rectangleRGBA(app.sdlwindow, x, y, x+w, y+h,
		49, 32, 122, 255);  

  world.ToRedrawOnScreen(Rectanglei(x, y, w, h));
  font.WriteCenterTop (_x, _y, txt, white_color);
}
