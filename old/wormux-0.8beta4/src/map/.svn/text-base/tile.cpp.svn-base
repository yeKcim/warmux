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
 *****************************************************************************/

#include "map/tile.h"
#include "map/tileitem.h"
#if TILE_HAS_PREVIEW
#  include "game/game.h"
#endif
#include "graphic/surface.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "map/camera.h"

Tile::Tile()
#if TILE_HAS_PREVIEW
: m_preview(NULL)
#endif
{
}

void Tile::FreeMem(){
  for (uint i=0; i<nbr_cell; ++i)
    delete item[i];
  nbr_cell = 0;
  item.clear();
}

Tile::~Tile(){
  FreeMem();
}

void Tile::InitTile(const Point2i &pSize, const Point2i & upper_left_offset, const Point2i & lower_right_offset){
  Point2i offset = upper_left_offset + lower_right_offset;
  size = pSize + offset;
  nbCells = size / CELL_SIZE;

  if((size.x % CELL_SIZE.x) != 0)
    nbCells.x++;

  if((size.y % CELL_SIZE.y) != 0)
    nbCells.y++;

  nbr_cell = nbCells.x * nbCells.y;
}

#define CORRECTION  0

void Tile::Dig(const Point2i &position, const Surface& dig){
  Point2i firstCell = Clamp(position / CELL_SIZE);
  Point2i lastCell = Clamp((position + dig.GetSize()) / CELL_SIZE);
  Point2i c;
  uint    index = firstCell.y*nbCells.x;
#if TILE_HAS_PREVIEW
  uint8_t *dst  = m_preview->GetPixels();
  uint    pitch = m_preview->GetPitch();
  dst += firstCell.y*(CELL_SIZE.y>>m_shift)*pitch;
#endif

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
  {
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++)
    {
      Point2i offset = position - c * CELL_SIZE;

      item[index + c.x]->Dig(offset, dig);

#if TILE_HAS_PREVIEW
      item[index + c.x]->ScalePreview(dst+4*c.x*(CELL_SIZE.x>>m_shift), pitch, m_shift);
#endif
    }
#if TILE_HAS_PREVIEW
    dst += (CELL_SIZE.y>>m_shift)*pitch;
#endif
    index += nbCells.x;
  }
}

void Tile::Dig(const Point2i &center, const uint radius){
  Point2i size = Point2i(2 * (radius + EXPLOSION_BORDER_SIZE),
                         2 * (radius + EXPLOSION_BORDER_SIZE));
  Point2i position = center - Point2i(radius + EXPLOSION_BORDER_SIZE,
                                       radius + EXPLOSION_BORDER_SIZE);

  Point2i firstCell = Clamp(position/CELL_SIZE);
  Point2i lastCell = Clamp((position+size)/CELL_SIZE);
  Point2i c;
  uint    index = firstCell.y*nbCells.x;
#if TILE_HAS_PREVIEW
  uint8_t *dst  = m_preview->GetPixels();
  uint    pitch = m_preview->GetPitch();
  dst += firstCell.y*(CELL_SIZE.y>>m_shift)*pitch;
#endif

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
  {
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++)
    {
      Point2i offset = center - c * CELL_SIZE;
      item[index + c.x]->Dig(offset, radius);
#if TILE_HAS_PREVIEW
      item[index + c.x]->ScalePreview(dst+4*c.x*(CELL_SIZE.x>>m_shift), pitch, m_shift);
#endif
    }
#if TILE_HAS_PREVIEW
    dst += (CELL_SIZE.y>>m_shift)*pitch;
#endif
    index += nbCells.x;
  }
}

void Tile::PutSprite(const Point2i& pos, const Sprite* spr)
{
  Rectanglei rec = Rectanglei(pos, spr->GetSizeMax());
  Point2i firstCell = Clamp(pos/CELL_SIZE);
  Point2i lastCell = Clamp((pos + spr->GetSizeMax())/CELL_SIZE);
  Point2i c;
  Surface s = spr->GetSurface();
  s.SetAlpha(0, 0);
#if TILE_HAS_PREVIEW
  uint8_t *pdst  = m_preview->GetPixels();
  uint    pitch = m_preview->GetPitch();
  pdst += firstCell.y*(CELL_SIZE.y>>m_shift)*pitch;
#endif

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++)
  {
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++)
    {
      TileItem *ti = item[c.y*nbCells.x + c.x];
      if(ti->IsTotallyEmpty())
      {
        delete item[c.y*nbCells.x + c.x];
        ti = item[c.y*nbCells.x + c.x] = new TileItem_AlphaSoftware(CELL_SIZE);
        ti->GetSurface().SetAlpha(0,0);
        ti->GetSurface().Fill(0x00000000);
        ti->GetSurface().SetAlpha(SDL_SRCALPHA,0);
      }

      Point2i cell_pos = c * CELL_SIZE;
      Rectanglei src;
      Rectanglei dst;
      src.SetPosition( rec.GetPosition() - cell_pos );
      if(src.GetPositionX() < 0) src.SetPositionX(0);
      if(src.GetPositionY() < 0) src.SetPositionY(0);

      src.SetSize( rec.GetPosition() + rec.GetSize() - cell_pos - src.GetPosition());
      if(src.GetSizeX() + src.GetPositionX() > CELL_SIZE.x) src.SetSizeX(CELL_SIZE.x - src.GetPositionX());
      if(src.GetSizeY() + src.GetPositionY() > CELL_SIZE.y) src.SetSizeY(CELL_SIZE.y - src.GetPositionY());

      dst.SetPosition( cell_pos - rec.GetPosition() );
      if(dst.GetPositionX() < 0) dst.SetPositionX(0);
      if(dst.GetPositionY() < 0) dst.SetPositionY(0);
      dst.SetSize(src.GetSize());

      ti->GetSurface().Blit(s, dst, src.GetPosition());
      static_cast<TileItem_AlphaSoftware*>(ti)->ResetEmptyCheck();
#if TILE_HAS_PREVIEW
      ti->ScalePreview(pdst+4*c.x*(CELL_SIZE.x>>m_shift), pitch, m_shift);
#endif
    }
#if TILE_HAS_PREVIEW
    pdst += (CELL_SIZE.y>>m_shift)*pitch;
#endif
  }

  s.SetAlpha(SDL_SRCALPHA, 0);
}

void Tile::MergeSprite(const Point2i &position, Surface& surf){
  Point2i firstCell = Clamp(position/CELL_SIZE);
  Point2i lastCell = Clamp((position + surf.GetSize())/CELL_SIZE);
  Point2i c;
#if TILE_HAS_PREVIEW
  uint8_t *dst = m_preview->GetPixels();
  uint    pitch = m_preview->GetPitch();
  dst += firstCell.y*(CELL_SIZE.y>>m_shift)*pitch;
#endif

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
  {
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++)
    {
      TileItem *ti = item[c.y*nbCells.x + c.x];
      Point2i offset = position - c * CELL_SIZE;
      if(ti->IsTotallyEmpty()) {
        delete item[c.y*nbCells.x + c.x];
        ti = item[c.y*nbCells.x + c.x] = new TileItem_AlphaSoftware(CELL_SIZE);
        ti->GetSurface().SetAlpha(0,0);
        ti->GetSurface().Fill(0x00000000);
        ti->GetSurface().SetAlpha(SDL_SRCALPHA,0);
      }
      ti->MergeSprite(offset, surf);
#if TILE_HAS_PREVIEW
      ti->ScalePreview(dst+4*c.x*(CELL_SIZE.x>>m_shift), pitch, m_shift);
#endif
    }
#if TILE_HAS_PREVIEW
    dst += (CELL_SIZE.y>>m_shift)*pitch;
#endif
  }
}

void Tile::LoadImage(Surface& terrain, const Point2i & upper_left_offset, const Point2i & lower_right_offset){
  Point2i offset = upper_left_offset + lower_right_offset;
  FreeMem();
  InitTile(terrain.GetSize(), upper_left_offset, lower_right_offset);
  ASSERT(nbr_cell != 0);

#if TILE_HAS_PREVIEW
  Point2i world_size = terrain.GetSize();
  Point2i video_size = AppWormux::GetInstance()->video->window.GetSize()/3;
  m_shift = 0;
  while (world_size > video_size)
  {
    world_size >>= 1;
    m_shift++;
  }
  m_preview = new Surface();
  *m_preview = Surface(Point2i(nbCells.x*(CELL_SIZE.x>>m_shift), nbCells.y*(CELL_SIZE.y>>m_shift)),
                       SDL_SWSURFACE|SDL_SRCALPHA, true).DisplayFormatAlpha();
  m_preview->SetAlpha(SDL_SRCALPHA, 0);
  uint8_t *dst  = m_preview->GetPixels();
  uint    pitch = m_preview->GetPitch();

  m_preview_size = m_preview->GetSize() - (offset / (1<<m_shift));
  m_preview_rect = Rectanglei(upper_left_offset / (1<<m_shift), m_preview_size);
#endif

  // Create the TileItem objects
  for (uint i=0; i<nbr_cell; ++i)
    item.push_back ( new TileItem_AlphaSoftware(CELL_SIZE) );

  // Fill the TileItem objects
  Point2i i;
  for( i.y = 0; i.y < nbCells.y; i.y++ )
  {
    for( i.x = 0; i.x < nbCells.x; i.x++ )
    {
      int piece = i.y * nbCells.x + i.x;
      Rectanglei sr(i * CELL_SIZE - upper_left_offset, CELL_SIZE);

      terrain.SetAlpha(0, 0);
      item[piece]->GetSurface().Blit(terrain, sr, Point2i(0, 0));
#if TILE_HAS_PREVIEW
      item[piece]->ScalePreview(dst+4*i.x*(CELL_SIZE.x>>m_shift), pitch, m_shift);
#endif
    }
#if TILE_HAS_PREVIEW
    dst += (CELL_SIZE.y>>m_shift)*pitch;
#endif
  }

  // Replace transparent tiles by TileItem_Empty tiles
  for( uint i=0; i<nbr_cell; ++i )
  {
    TileItem_AlphaSoftware* t = static_cast<TileItem_AlphaSoftware*>(item[i]);
    while(t->need_check_empty)
      t->CheckEmpty();
    if(t->NeedDelete())
    {
#ifdef DBG_TILE
      printf("\nDeleting tile %i",i);
#endif
      delete item[i];
      item[i] = (TileItem*)new TileItem_Empty;
    }
#ifdef DBG_TILE
    else
    {
      if(i % nbCells.x % 2 == (i / nbCells.x) % 2)
        item[i]->FillWithRGB(0, 0, 255);
      else
        item[i]->FillWithRGB(0, 255, 0);
    }
#endif
  }
}

uchar Tile::GetAlpha(const Point2i &pos) const{
  int cell = pos.y / CELL_SIZE.y * nbCells.x + pos.x / CELL_SIZE.x;
  return item[cell]->GetAlpha(pos % CELL_SIZE);
}

void Tile::DrawTile() {
  Point2i firstCell = Clamp(Camera::GetInstance()->GetPosition() / CELL_SIZE);
  Point2i lastCell = Clamp((Camera::GetInstance()->GetPosition() + Camera::GetInstance()->GetSize()) / CELL_SIZE);
  Point2i i;
  for( i.y = firstCell.y; i.y <= lastCell.y; i.y++ )
    for( i.x = firstCell.x; i.x <= lastCell.x; i.x++)
      item[i.y*nbCells.x + i.x]->Draw( i );
}

void Tile::DrawTile_Clipped(Rectanglei worldClip) const
{
  // Revision 514:
  // worldClip.SetSize( worldClip.GetSize() + 1); // mmm, does anything gives areas
  // too small to redraw ?
  //
  // Revision 3095:
  // Sorry, I don't understand that comment. Moreover the +1 produces a bug when the ground of
  // a map have an alpha value != 255 and != 0
  worldClip.SetSize( worldClip.GetSize());
  Point2i firstCell = Clamp(worldClip.GetPosition() / CELL_SIZE);
  Point2i lastCell  = Clamp((worldClip.GetBottomRightPoint()) / CELL_SIZE);
  Point2i c;

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++){
      // For all selected items, clip source and destination blitting rectangles
      Rectanglei destRect(c * CELL_SIZE, CELL_SIZE);

      destRect.Clip(worldClip);
      if( destRect.Intersect( *Camera::GetInstance() ) ){
        Point2i ptDest = destRect.GetPosition() - Camera::GetInstance()->GetPosition();
        Point2i ptSrc = destRect.GetPosition() - c * CELL_SIZE;

        AppWormux::GetInstance()->video->window.Blit( item[c.y*nbCells.x + c.x]->GetSurface(), Rectanglei(ptSrc, destRect.GetSize()) , ptDest);
      }
    }
}

Surface Tile::GetPart(const Rectanglei& rec)
{
  Surface part(rec.GetSize(), SDL_SWSURFACE|SDL_SRCALPHA, true);
  part.SetAlpha(0,0);
  part.Fill(0x00000000);
  part.SetAlpha(SDL_SRCALPHA,0);

  Point2i firstCell = Clamp(rec.GetPosition() / CELL_SIZE);
  Point2i lastCell = Clamp((rec.GetPosition() + rec.GetSize()) / CELL_SIZE);
  Point2i i = nbCells - 1;

  for( i.y = firstCell.y; i.y <= lastCell.y; i.y++ )
  for( i.x = firstCell.x; i.x <= lastCell.x; i.x++ )
  {
    if(item[i.y*nbCells.x + i.x]->IsTotallyEmpty()) continue;

    Point2i cell_pos = i * CELL_SIZE;
    Rectanglei src;
    Point2i dst;
    src.SetPosition( rec.GetPosition() - cell_pos );
    if(src.GetPositionX() < 0) src.SetPositionX(0);
    if(src.GetPositionY() < 0) src.SetPositionY(0);

    src.SetSize( rec.GetPosition() + rec.GetSize() - cell_pos - src.GetPosition());
    if(src.GetSizeX() + src.GetPositionX() > CELL_SIZE.x) src.SetSizeX(CELL_SIZE.x - src.GetPositionX());
    if(src.GetSizeY() + src.GetPositionY() > CELL_SIZE.y) src.SetSizeY(CELL_SIZE.y - src.GetPositionY());

    dst =  cell_pos - rec.GetPosition();
    if(dst.x < 0) dst.x = 0;
    if(dst.y < 0) dst.y = 0;

    item[i.y*nbCells.x + i.x]->GetSurface().SetAlpha(0, 0);
    part.Blit(item[i.y*nbCells.x + i.x]->GetSurface(), src, dst);
    item[i.y*nbCells.x + i.x]->GetSurface().SetAlpha(SDL_SRCALPHA, 0);
  }
  return part;
}

void Tile::CheckEmptyTiles()
{
  for( int i = 0; i < nbCells.x * nbCells.y; i++ )
  {
    if(item[i]->IsTotallyEmpty()) continue;

    TileItem_AlphaSoftware* t = static_cast<TileItem_AlphaSoftware*>(item[i]);
    if(t->need_check_empty)
      t->CheckEmpty();
    if(t->need_delete)
    {
      // no need to display this tile as it can be deleted!
#ifdef DBG_TILE
     printf("Deleting tile %i\n",i);
#endif
      delete item[i];
      item[i] = (TileItem*)new TileItem_Empty;
    }
  }
}

