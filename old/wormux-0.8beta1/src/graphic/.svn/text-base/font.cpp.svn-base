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

#include <exception>
#include <SDL_image.h>
#include <SDL_video.h>
#include <iostream>
#include <string>
#include "font.h"
#include "colors.h"
#include "game/config.h"
#include "include/app.h"
#include "map/map.h"
#include "tool/error.h"
#include "tool/file_tools.h"

Font* Font::FONT_ARRAY[] = {NULL, NULL, NULL, NULL, NULL, NULL};
Font* Font::FONT_ARRAY_BOLD[] = {NULL, NULL, NULL, NULL, NULL, NULL};
Font* Font::FONT_ARRAY_ITALIC[] = {NULL, NULL, NULL, NULL, NULL, NULL};

/*
 * Constants
 */
// Size
const int Font::FONT_SIZE[] = {40, 32, 24, 16, 12, 8};

Font* Font::GetInstance(font_size_t ftype, font_style_t fstyle) {
  Font * font = NULL;
  int type = (int)ftype;

  if (FONT_ARRAY[ftype] == NULL) {
    // Load the font in the different styles
    FONT_ARRAY_BOLD[type] = new Font(FONT_SIZE[type]);
    FONT_ARRAY_BOLD[type]->SetBold();

    FONT_ARRAY_ITALIC[type] = new Font(FONT_SIZE[type]);
    FONT_ARRAY_ITALIC[type]->SetItalic();

    FONT_ARRAY[type] = new Font(FONT_SIZE[type]);
  }

  switch(fstyle) {
  case FONT_BOLD:
    font = FONT_ARRAY_BOLD[type];
    break;
  case FONT_ITALIC:
    font = FONT_ARRAY_ITALIC[type];
    break;
  case FONT_NORMAL:
    font = FONT_ARRAY[type];
    break;
  }
  return font;
}

Font::Font(int size):
  surface_text_table(),
  m_font(NULL)
{
  bool ok = Load(Config::GetInstance()->GetTtfFilename(), size);
  if( !ok )
    Error("Error during initialisation of a font!");
}

Font::~Font(){
  if( m_font != NULL ){
    TTF_CloseFont(m_font);
    m_font = NULL;
  }

  txt_iterator it;

  for( it = surface_text_table.begin();
       it != surface_text_table.end();
       ++it ){
    //SDL_FreeSurface(it->second);
    surface_text_table.erase(it->first);
  }
}

bool Font::Load (const std::string& filename, int size) {
  bool ok = false;

  if( IsFileExist(filename) ){
      m_font = TTF_OpenFont(filename.c_str(), size);
      ok = (m_font != NULL);
  }

  if( !ok ){
      std::cout << "Error: Font " << filename << " can't be found!" << std::endl;
      return false;
  }

  TTF_SetFontStyle(m_font, TTF_STYLE_NORMAL);

  return true;
}

void Font::SetBold()
{
  TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);
}

void Font::SetItalic()
{
  TTF_SetFontStyle(m_font, TTF_STYLE_ITALIC);
}

void Font::Write(const Point2i &pos, Surface &surface){
  AppWormux::GetInstance()->video.window.Blit(surface, pos);

  // TODO: Remove this line! (and use GameFont instead of Font)
  world.ToRedrawOnScreen( Rectanglei(pos, surface.GetSize()) );
}

void Font::WriteLeft(const Point2i &pos, const std::string &txt,  const Color &color){
  Surface surface(Render(txt, color, true));
  Write(pos, surface);
}

void Font::WriteLeftBottom(const Point2i &pos, const std::string &txt,
			     const Color &color){
  Surface surface(Render(txt, color, true));
  Write(pos - Point2i(0, surface.GetHeight()), surface);
}

void Font::WriteRight(const Point2i &pos, const std::string &txt,
		        const Color &color){
  Surface surface(Render(txt, color, true));
  Write(pos - Point2i(surface.GetWidth(), 0), surface);
}

void Font::WriteCenter (const Point2i &pos, const std::string &txt,
			 const Color &color){
  Surface surface(Render(txt, color, true));
  Write(pos - Point2i(surface.GetWidth()/2, surface.GetHeight()), surface);
}

void Font::WriteCenterTop(const Point2i &pos, const std::string &txt,
		const Color &color){
	Surface surface(Render(txt, color, true));
	Write(pos - Point2i(surface.GetWidth()/2, 0), surface);
}

Surface Font::CreateSurface(const std::string &txt, const Color &color){
  return Surface( TTF_RenderUTF8_Blended(m_font, txt.c_str(), color.GetSDLColor()) );
}

Surface Font::Render(const std::string &txt, const Color &color, bool cache){
  Surface surface;

  if( cache ){
    txt_iterator p = surface_text_table.find(txt);
    if( p == surface_text_table.end() ){
      if( surface_text_table.size() > 5 ){
        //SDL_FreeSurface( surface_text_table.begin()->second );
        surface_text_table.erase( surface_text_table.begin() );
      }
      surface = CreateSurface(txt, color);
      surface_text_table.insert( txt_sample(txt, surface) );
    } else {
      txt_iterator p = surface_text_table.find( txt );
      surface = p->second;
    }
  } else
    surface = CreateSurface(txt, color);

  assert( !surface.IsNull() );
  return surface;
}

int Font::GetWidth (const std::string &txt){
  int width=-1;

  TTF_SizeUTF8(m_font, txt.c_str(), &width, NULL);

  return width;
}

int Font::GetHeight (){
  return TTF_FontHeight(m_font);
}

int Font::GetHeight (const std::string &str){
  int height=-1;

  TTF_SizeUTF8(m_font, str.c_str(), NULL, &height);

  return height;
}

Point2i Font::GetSize(const std::string &txt){
	return Point2i(GetWidth(txt), GetHeight(txt));
}

Surface Font::GenerateSurface(const std::string &txt, const Color &color, font_size_t font_size, font_style_t font_style)
{
  return Surface(Font::GetInstance(font_size, font_style)->CreateSurface(txt, color));
}
