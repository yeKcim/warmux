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

#include "tile.h"
#include "../include/app.h"
#include "../map/camera.h"

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
   Point2i size = Point2i(2 * radius, 2 * radius);
   Point2i position = center - Point2i(radius,radius);

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
            item[piece]->SyncBuffer();
        }

    // Replace transparent tiles by TileItem_Empty tiles
    for( uint i=0; i<nbr_cell; ++i )
        if( item[i]->IsEmpty() ){
            delete item[i];
            item[i] = (TileItem*)new TileItem_Empty;
        }
}

uchar Tile::GetAlpha(const Point2i &pos) const{
	int cell = pos.y / CELL_SIZE.y * nbCells.x + pos.x / CELL_SIZE.x;
    return item[cell]->GetAlpha(pos % CELL_SIZE);
}

void Tile::DrawTile() const{
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
