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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Textured polygon (from SDL_Gfx)
 *****************************************************************************/

#ifndef TEXTURED_POLYGON_H
#define TEXTURED_POLYGON_H

#include <SDL_gfxPrimitives.h>

static int gfxPrimitivesPolyAllocated = 0;
static int *gfxPrimitivesPolyInts = NULL;
int gfxPrimitivesCompareInt(const void *a, const void *b)
{
  return (*(const int *) a) - (*(const int *) b);
}

int _texturedHLine(SDL_Surface * dst, Sint16 x1, Sint16 x2, Sint16 y,SDL_Surface *texture,int texture_dx,int texture_dy)
{
  Sint16 left, right, top, bottom;
  Sint16 w;
  Sint16 xtmp;
  int result = 0;
  int texture_x_walker;
  int texture_y_start;
  SDL_Rect source_rect,dst_rect;
  int pixels_written,write_width;

  /*
   * Get clipping boundary
   */
  left = dst->clip_rect.x;
  right = dst->clip_rect.x + dst->clip_rect.w - 1;
  top = dst->clip_rect.y;
  bottom = dst->clip_rect.y + dst->clip_rect.h - 1;

  /*
   * Check visibility of hline
   */
  if ((x1<left) && (x2<left)) {
    return(0);
  }
  if ((x1>right) && (x2>right)) {
    return(0);
  }
  if ((y<top) || (y>bottom)) {
    return (0);
  }

  /*
   * Swap x1, x2 if required
   */
  if (x1 > x2) {
    xtmp = x1;
    x1 = x2;
    x2 = xtmp;
  }

  /*
   * Clip x
   */
  if (x1 < left) {
    x1 = left;
  }
  if (x2 > right) {
    x2 = right;
  }

  /*
   * Calculate width
   */
  w = x2 - x1;

  /*
   * Sanity check on width
   */
  if (w < 0) {
    return (0);
  }

  /*
   * Determint where in the texture we start drawing
   **/
  texture_x_walker =   (x1 - texture_dx)  % texture->w;
  if (texture_x_walker < 0){
    texture_x_walker = texture->w +texture_x_walker ;
  }

  texture_y_start = (y + texture_dy) % texture->h;
  if (texture_y_start < 0){
    texture_y_start = texture->h + texture_y_start;
  }

  //setup the source rectangle  we are only drawing one horizontal line
  source_rect.y = texture_y_start;
  source_rect.x =texture_x_walker;
  source_rect.h =1;
  //we will draw to the current y
  dst_rect.y = y;

  //if there are enough pixels left in the current row of the texture
  //draw it all at once
  if (w <= texture->w -texture_x_walker){
    source_rect.w = w;
    source_rect.x = texture_x_walker;
    dst_rect.x= x1;
    result |= !SDL_BlitSurface  (texture,&source_rect , dst, &dst_rect) ;
  } else {//we need to draw multiple times
    //draw the first segment
    pixels_written = texture->w  -texture_x_walker;
    source_rect.w = pixels_written;
    source_rect.x = texture_x_walker;
    dst_rect.x= x1;
    result |= !SDL_BlitSurface  (texture,&source_rect , dst, &dst_rect);
    write_width = texture->w;

    //now draw the rest
    //set the source x to 0
    source_rect.x = 0;
    while(pixels_written < w){
      if (write_width >= w - pixels_written){
        write_width=  w- pixels_written;
      }
      source_rect.w = write_width;
      dst_rect.x = x1 + pixels_written;
      result |= !SDL_BlitSurface  (texture,&source_rect , dst, &dst_rect) ;
      pixels_written += write_width;
    }
  }
  return result;
}

/**
 * Draws a polygon filled with the given texture. this operation use SDL_BlitSurface. It supports
 * alpha drawing.
 * to get the best performance of this operation you need to make sure the texture and the dst surface have the same format
 * see  http://docs.mandragor.org/files/Common_libs_documentation/SDL/SDL_Documentation_project_en/sdlblitsurface.html
 *
 * dest the destination surface,
 * vx array of x vector components
 * vy array of x vector components
 * n the amount of vectors in the vx and vy array
 * texture the sdl surface to use to fill the polygon
 * texture_dx the offset of the texture relative to the screeen. if you move the polygon 10 pixels
 * to the left and want the texture to apear the same you need to increase the texture_dx value
 * texture_dy see texture_dx
 **/
int texturedPolygon(SDL_Surface * dst, const Sint16 * vx, const Sint16 * vy, int n, SDL_Surface * texture,int texture_dx,int texture_dy)
{
  int result;
  int i;
  int y, xa, xb;
  int minx,maxx,miny, maxy;
  int x1, y1;
  int x2, y2;
  int ind1, ind2;
  int ints;
  /*
   * Sanity check
   */
  if (n < 3) {
    return -1;
  }
  /*
   * Allocate temp array, only grow array
   */
  if (!gfxPrimitivesPolyAllocated) {
    gfxPrimitivesPolyInts = (int *) malloc(sizeof(int) * n);
    gfxPrimitivesPolyAllocated = n;
  } else {
    if (gfxPrimitivesPolyAllocated < n) {
      gfxPrimitivesPolyInts = (int *) realloc(gfxPrimitivesPolyInts, sizeof(int) * n);
      gfxPrimitivesPolyAllocated = n;
    }
  }
  miny = vy[0];
  maxy = vy[0];
  minx = vx[0];
  maxx = vx[0];
  for (i = 1; (i < n); i++) {
    if (vy[i] < miny) {
      miny = vy[i];
    } else if (vy[i] > maxy) {
      maxy = vy[i];
    }
    if (vx[i] < minx) {
      minx = vx[i];
    } else if (vx[i] > maxx) {
      maxx = vx[i];
    }
  }
  if (maxx <0 || minx > dst->w){
    return 0;
  }
  if (maxy <0 || miny > dst->h){
    return 0;
  }
  /*
   * Draw, scanning y
   */
  result = 0;
  for (y = miny; (y <= maxy); y++) {
    ints = 0;
    for (i = 0; (i < n); i++) {
      if (!i) {
        ind1 = n - 1;
        ind2 = 0;
      } else {
        ind1 = i - 1;
        ind2 = i;
      }
      y1 = vy[ind1];
      y2 = vy[ind2];
      if (y1 < y2) {
        x1 = vx[ind1];
        x2 = vx[ind2];
      } else if (y1 > y2) {
        y2 = vy[ind1];
        y1 = vy[ind2];
        x2 = vx[ind1];
        x1 = vx[ind2];
      } else {
        continue;
      }
      if ( ((y >= y1) && (y < y2)) || ((y == maxy) && (y > y1) && (y <= y2)) ) {
        gfxPrimitivesPolyInts[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
      }
    }
    qsort(gfxPrimitivesPolyInts, ints, sizeof(int), gfxPrimitivesCompareInt);
    for (i = 0; (i < ints); i += 2) {
      xa = gfxPrimitivesPolyInts[i] + 1;
      xa = (xa >> 16) + ((xa & 32768) >> 15);
      xb = gfxPrimitivesPolyInts[i+1] - 1;
      xb = (xb >> 16) + ((xb & 32768) >> 15);
      result |= _texturedHLine(dst, xa, xb, y, texture,texture_dx,texture_dy);
    }
  }
  return (result);
}
#endif /* TEXTURED_POLYGON_H */
