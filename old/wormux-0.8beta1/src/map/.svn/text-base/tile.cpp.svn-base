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

#include "tile.h"
#include "include/app.h"
#include "map/camera.h"

Tile::Tile(){
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

void Tile::InitTile(const Point2i &pSize){
    nbCells = pSize / CELL_SIZE;

    if( (pSize.x % CELL_SIZE.x) != 0 )
        nbCells.x++;

    if( (pSize.y % CELL_SIZE.y) != 0 )
        nbCells.y++;

    size = pSize;

    nbr_cell = nbCells.x * nbCells.y;
}

Point2i Tile::Clamp(const Point2i &v) const{
	return v.clamp(Point2i(0, 0), nbCells - 1);
}

void Tile::Dig(const Point2i &position, const Surface& dig){
   Rectanglei rect = Rectanglei(position, dig.GetSize());
	Point2i firstCell = Clamp(position/CELL_SIZE);
	Point2i lastCell = Clamp((position + dig.GetSize())/CELL_SIZE);
	Point2i c;

    for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
        for( c.x = firstCell.x; c.x <= lastCell.x; c.x++){
            Point2i offset = position - c * CELL_SIZE;

            item[c.y*nbCells.x + c.x]->Dig(offset, dig);
        }
}

void Tile::Dig(const Point2i &center, const uint radius){  
   Point2i size = Point2i(2 * (radius + EXPLOSION_BORDER_SIZE),
                          2 * (radius + EXPLOSION_BORDER_SIZE));
   Point2i position = center - Point2i(radius + EXPLOSION_BORDER_SIZE,
                                       radius + EXPLOSION_BORDER_SIZE);

   Rectanglei rect = Rectanglei(position, size);
	Point2i firstCell = Clamp(position/CELL_SIZE);
	Point2i lastCell = Clamp((position+size)/CELL_SIZE);
	Point2i c;

    for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++){
            Point2i offset = center - c * CELL_SIZE;
            item[c.y*nbCells.x + c.x]->Dig(offset, radius);
        }
}

void Tile::PutSprite(const Point2i pos, Sprite* spr)
{
  Rectanglei rec = Rectanglei(pos, spr->GetSizeMax());
  Point2i firstCell = Clamp(pos/CELL_SIZE);
  Point2i lastCell = Clamp((pos + spr->GetSizeMax())/CELL_SIZE);
  Point2i c;
  Surface s = spr->GetSurface();
  s.SetAlpha(0, 0);

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
  for( c.x = firstCell.x; c.x <= lastCell.x; c.x++)
  {
    if(item[c.y*nbCells.x + c.x]->IsTotallyEmpty())
    {
      delete item[c.y*nbCells.x + c.x];
      item[c.y*nbCells.x + c.x] = new TileItem_AlphaSoftware(CELL_SIZE);
      item[c.y*nbCells.x + c.x]->GetSurface().SetAlpha(0,0);
      item[c.y*nbCells.x + c.x]->GetSurface().Fill(0x00000000);
      item[c.y*nbCells.x + c.x]->GetSurface().SetAlpha(SDL_SRCALPHA,0);
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

    item[c.y*nbCells.x + c.x]->GetSurface().Blit(s, dst, src.GetPosition());
    static_cast<TileItem_AlphaSoftware*>(item[c.y*nbCells.x + c.x])->ResetEmptyCheck();
  }
  s.SetAlpha(SDL_SRCALPHA, 0);
}

void Tile::MergeSprite(const Point2i &position, Surface& surf){
  Rectanglei rect = Rectanglei(position, surf.GetSize());
  Point2i firstCell = Clamp(position/CELL_SIZE);
  Point2i lastCell = Clamp((position + surf.GetSize())/CELL_SIZE);
  Point2i c;

  for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
    for( c.x = firstCell.x; c.x <= lastCell.x; c.x++){
      Point2i offset = position - c * CELL_SIZE;
      if(item[c.y*nbCells.x + c.x]->IsTotallyEmpty()) {
        delete item[c.y*nbCells.x + c.x];
        item[c.y*nbCells.x + c.x] = new TileItem_AlphaSoftware(CELL_SIZE);
        item[c.y*nbCells.x + c.x]->GetSurface().SetAlpha(0,0);
        item[c.y*nbCells.x + c.x]->GetSurface().Fill(0x00000000);
        item[c.y*nbCells.x + c.x]->GetSurface().SetAlpha(SDL_SRCALPHA,0);
      }
      item[c.y*nbCells.x + c.x]->MergeSprite(offset, surf);
    }
}
void Tile::LoadImage (Surface& terrain){
    FreeMem();

    InitTile(terrain.GetSize());
    assert(nbr_cell != 0);

    // Create the TileItem objects
    for (uint i=0; i<nbr_cell; ++i)
       item.push_back ( new TileItem_AlphaSoftware(CELL_SIZE) );

    // Fill the TileItem objects
	Point2i i;
    for( i.y = 0; i.y < nbCells.y; i.y++ )
        for( i.x = 0; i.x < nbCells.x; i.x++ ){
            int piece = i.y * nbCells.x + i.x;
            Rectanglei sr( i * CELL_SIZE, CELL_SIZE);

            terrain.SetAlpha(0, 0);
            item[piece]->GetSurface().Blit(terrain, sr, Point2i(0, 0));
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
    Point2i firstCell = Clamp(camera.GetPosition() / CELL_SIZE);
    Point2i lastCell = Clamp((camera.GetPosition() + camera.GetSize()) / CELL_SIZE);
	Point2i i;
    for( i.y = firstCell.y; i.y <= lastCell.y; i.y++ )
        for( i.x = firstCell.x; i.x <= lastCell.x; i.x++)
	  item[i.y*nbCells.x + i.x]->Draw( i );
}

void Tile::DrawTile_Clipped(Rectanglei worldClip) const
{
	worldClip.SetSize( worldClip.GetSize() + 1); // mmm, y aurait t-il quelque chose qui
	// donne des zones trops petites à redessiner ?
    Point2i firstCell = Clamp(worldClip.GetPosition() / CELL_SIZE);
    Point2i lastCell  = Clamp((worldClip.GetBottomRightPoint()) / CELL_SIZE);
	Point2i c;

    for( c.y = firstCell.y; c.y <= lastCell.y; c.y++ )
        for( c.x = firstCell.x; c.x <= lastCell.x; c.x++){
		// For all selected items, clip source and destination blitting rectangles 
			Rectanglei destRect(c * CELL_SIZE, CELL_SIZE);

			destRect.Clip(worldClip);
			if( destRect.Intersect( camera ) ){
				Point2i ptDest = destRect.GetPosition() - camera.GetPosition();
				Point2i ptSrc = destRect.GetPosition() - c * CELL_SIZE;
			
                AppWormux::GetInstance()->video.window.Blit( item[c.y*nbCells.x + c.x]->GetSurface(), Rectanglei(ptSrc, destRect.GetSize()) , ptDest); 
			}
        }
}

Surface Tile::GetPart(Rectanglei& rec)
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
