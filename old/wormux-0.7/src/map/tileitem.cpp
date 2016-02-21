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
 *****************************************************************************/

#include "tileitem.h"
#include <iostream>
#include <SDL.h>
#include <SDL_endian.h>
#include "../game/config.h"
#include "../include/app.h"
#include "../map/camera.h"
#include "../tool/error.h"
#include "../tool/point.h"
#include "../tool/stats.h"

// === Common to all TileItem_* except TileItem_Emtpy ==============================
void TileItem::Draw(const Point2i &pos){
  AppWormux::GetInstance()->video.window.Blit(GetSurface(),
        pos * CELL_SIZE - camera.GetPosition());
}

bool TileItem::IsEmpty(){
  Point2i pos;

  for( pos.x = 0 ; pos.x < CELL_SIZE.x; pos.x++ )
     for( pos.y = 0 ; pos.y < CELL_SIZE.y; pos.y++ )
       if( GetAlpha(pos) == 255 )
         return false;

   return true;
}

// === Implemenation of TileItem_Software_ALpha ==============================
TileItem_AlphaSoftware::TileItem_AlphaSoftware(const Point2i &size){
    m_size = size;
    m_surface = Surface(size, SDL_SWSURFACE|SDL_SRCALPHA, true).DisplayFormatAlpha();

    _GetAlpha = &TileItem_AlphaSoftware::GetAlpha_Generic;
    if( m_surface.GetBytesPerPixel() == 4 ){
       if( m_surface.GetSurface()->format->Amask == 0x000000ff ){
           if( SDL_BYTEORDER == SDL_LIL_ENDIAN )
               _GetAlpha = &TileItem_AlphaSoftware::GetAlpha_Index0;
           else
               _GetAlpha = &TileItem_AlphaSoftware::GetAlpha_Index3;
       }else{
           if( m_surface.GetSurface()->format->Amask == 0xff000000 ){
                if (SDL_BYTEORDER == SDL_LIL_ENDIAN )
                    _GetAlpha = &TileItem_AlphaSoftware::GetAlpha_Index3;
                else
                    _GetAlpha = &TileItem_AlphaSoftware::GetAlpha_Index0;
            }
        }
    }
}

TileItem_AlphaSoftware::~TileItem_AlphaSoftware(){
}

unsigned char TileItem_AlphaSoftware::GetAlpha(const Point2i &pos){
    return (this->*_GetAlpha)(pos);
}

unsigned char TileItem_AlphaSoftware::GetAlpha_Index0 (const Point2i &pos){
    return *(m_surface.GetPixels() + pos.y*m_surface.GetPitch() + pos.x * 4 + 0);
}

unsigned char TileItem_AlphaSoftware::GetAlpha_Index3 (const Point2i &pos){
    return *(m_surface.GetPixels() + pos.y*m_surface.GetPitch() + pos.x * 4 + 3);
}

unsigned char TileItem_AlphaSoftware::GetAlpha_Generic (const Point2i &pos){
    unsigned char r, g, b, a;

    Uint32 pixel = *(Uint32 *)(m_surface.GetPixels() + pos.y*m_surface.GetPitch() + pos.x*m_surface.GetBytesPerPixel()); 
    m_surface.GetRGBA(pixel, r, g, b, a);

    return a;
}

void TileItem_AlphaSoftware::Dig(const Point2i &position, const Surface& dig){
    int starting_x = position.x >= 0 ? position.x : 0;
    int starting_y = position.y >= 0 ? position.y : 0;
    int ending_x = position.x+dig.GetWidth() <= m_surface.GetWidth() ? position.x+dig.GetWidth() : m_surface.GetWidth();
    int ending_y = position.y+dig.GetHeight() <= m_surface.GetHeight() ? position.y+dig.GetHeight() : m_surface.GetHeight();

    for( int py = starting_y ; py < ending_y ; py++) 
        for( int px = starting_x ; px < ending_x ; px++)
            if ( *(dig.GetPixels() + (py-position.y)*dig.GetPitch() + (px-position.x) * 4 + 3) != 0)
                *(m_surface.GetPixels() + py*m_surface.GetPitch() + px * 4 + 3) = 0;
}

void TileItem_AlphaSoftware::Dig(const Point2i &center, const uint radius){
  unsigned char* buf   = m_surface.GetPixels();
  const uint line_size = m_surface.GetPitch();
  const uint bpp       = m_surface.GetBytesPerPixel();

  int y = (center.y - (int)radius >= 0) ? (center.y - (int)radius) : 0;
  assert( (uint) y <= center.y + radius && y < CELL_SIZE.y );
  buf += y * line_size;

  //Empties each line of the tile horizontaly that are in the cirlce
  while ( (uint) y <= center.y + radius && y < CELL_SIZE.y )
  {
    //Abscisse distance from the center of the circle to the circle
    int dac = center.y - y;

    //Angle on the circle 
    float angle = asin( (float)dac / (float)radius);

    //Zone of the line which needs to be 
    int start_x, end_x, lenght;
    lenght = (int) ((float) radius * cos (angle));
    lenght = lenght > 0 ? lenght : - lenght;
    start_x = center.x - lenght;
    lenght *= 2;
    end_x = start_x + lenght;

    if( start_x < CELL_SIZE.x && end_x >= 0)
    {
      //Clamp the value to empty only the in this tile
      int tile_start_x = (start_x < 0) ? 0 : (start_x >= CELL_SIZE.x) ? CELL_SIZE.x - 1 : start_x;
      assert( tile_start_x >= 0 && tile_start_x < CELL_SIZE.x);
      int tile_lenght = (end_x >= CELL_SIZE.x) ? CELL_SIZE.x - tile_start_x : end_x - tile_start_x + 1;
      assert( tile_lenght > 0);
      assert( tile_start_x + tile_lenght <= CELL_SIZE.x);

      assert(buf + tile_start_x * bpp + bpp * (tile_lenght-1) < m_surface.GetPixels() + CELL_SIZE.x * CELL_SIZE.y * bpp); //Check for overflow
      memset(buf + tile_start_x * bpp, 0 , bpp * tile_lenght);

/*      unsigned int* tmpbuf = (unsigned int*)(buf + tile_start_x * bpp);

      while(tile_lenght--)
        *(tmpbuf++) = 0;*/
    }
    buf += line_size;
    y++;
  }
}

Surface TileItem_AlphaSoftware::GetSurface(){
    return m_surface;
}

void TileItem_AlphaSoftware::SyncBuffer(){
   // nothing to do
}
