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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Handle a SDL Surface
 *****************************************************************************/

#include "surface.h"
#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>
#include <SDL_rotozoom.h>
#include "../tool/debug.h"
#include "../tool/error.h"
#include "../tool/i18n.h"
#include "../include/base.h"

/**
 * Default constructor.
 *
 * Build a null surface with autoFree at true.
 */
Surface::Surface(){
	surface = NULL;
	autoFree = true;
}

/**
 * Constructor building a surface object using an existing SDL_Surface pointer.
 *
 * @param sdl_surface The existing sdl_surface.
 */
Surface::Surface(SDL_Surface *sdl_surface){
	surface = sdl_surface;
	autoFree = true;
}

/**
 * Constructor building a surface object using the NewSurface function.
 *
 * @param size
 * @param flags
 * @param useAlpha
 * @see NewSurface 
 */
Surface::Surface(const Point2i &size, Uint32 flags, bool useAlpha){
	surface = NULL;
	autoFree = true;
	NewSurface(size, flags, useAlpha);
}

/**
 * Constructor building a surface by reading the image from a file.
 *
 * @param filename_str A string containing the path to the graphic file.
 */
Surface::Surface(const std::string &filename){
	surface = NULL;
	autoFree = true;
	if( !ImgLoad(filename) )
		Error( Format("Unable to open image file : %s", filename.c_str() ) );
}

/** 
 * Copy constructor: build a surface from an other surface.
 *
 * The two surfaces share the same graphic data.
 */
Surface::Surface(const Surface &src){
	surface = src.surface;
	autoFree = true;
	if( !IsNull() )
		surface->refcount++;
}

/**
 * Destructor of the surface.
 *
 * Will free the memory used by the surface if autoFree is set to true and if the counter of reference reach 0
 */
Surface::~Surface(){
	AutoFree();
}

Surface &Surface::operator=(const Surface & src){
	AutoFree();
	surface = src.surface;
	if( !IsNull() )
		surface->refcount++;

	return *this;
}

/**
 * Free the memory occupied by the surface.
 *
 * The memory is really freed if the reference counter reach 0.
 */
void Surface::Free(){
	if( !IsNull() ){
		SDL_FreeSurface( surface );
		surface = NULL;
	}
}

void Surface::AutoFree(){
	if( autoFree )
		Free();
}

/** 
 * Set the auto free status of a surface.
 *
 * In general it should always be true for non-system surface.
 * @param newAutoFree the new autoFree status.
 */
void Surface::SetAutoFree( bool newAutoFree ){
	autoFree = newAutoFree;
}

/**
 * Return the pointer of the SDL_Surface.
 *
 * Should be used carefully.
 */
SDL_Surface *Surface::GetSurface(){
	return surface;
}

/**
 * Change the surface pointer.
 *
 * @param newSurface The new surface to use.
 * @param freePrevius Indicate if the old surface should be freed.
 */
void Surface::SetSurface(SDL_Surface *newSurface, bool freePrevious){
	if( freePrevious )
		Free();

	surface = newSurface;
}

/**
 * Create a new surface.
 *
 * @param size
 * @param flags
 * @param useAlpha
 */
void Surface::NewSurface(const Point2i &size, Uint32 flags, bool useAlpha){
	Uint32 alphaMask;
	Uint32 redMask;
	Uint32 greenMask;
	Uint32 blueMask;

	if( autoFree )
		Free();

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	redMask = 0xff000000;
	greenMask = 0x00ff0000;
	blueMask = 0x0000ff00;
	alphaMask =	0x000000ff;
#else
	redMask = 0x000000ff;
	greenMask = 0x0000ff00;
	blueMask = 0x00ff0000;
	alphaMask = 0xff000000;
#endif

	if( !useAlpha )
		alphaMask = 0;

	surface = SDL_CreateRGBSurface(flags, size.x, size.y, 32,
			redMask, greenMask, blueMask, alphaMask );

	if( surface == NULL )
		Error( std::string("Can't create SDL RGBA surface: ") + SDL_GetError() );
}

/**
 * Set the alpha value of a surface.
 *
 */
int Surface::SetAlpha(Uint32 flags, Uint8 alpha){
	return SDL_SetAlpha( surface, flags, alpha );
}

/**
 * Lock the surface to permit direct access.
 *
 */
int Surface::Lock(){
	return SDL_LockSurface( surface );
}

/**
 * Unlock the surface.
 *
 */
void Surface::Unlock(){
	SDL_UnlockSurface( surface );
}

int Surface::Blit(const Surface& src, SDL_Rect *srcRect, SDL_Rect *dstRect){
	return SDL_BlitSurface( src.surface, srcRect, surface, dstRect );
}

/**
 * Blit the whole surface src on the current surface.
 *
 * @param src The source surface.
 */
int Surface::Blit(const Surface& src){
	return Blit(src, NULL, NULL);
}

/**
 * Blit a surface (src) on the current surface at a certain position (dst)
 *
 * @src The source surface.
 * @dst A point defining the destination coordinate on the current surface.
 */
int Surface::Blit(const Surface& src, const Point2i &dst){
	SDL_Rect dstRect = GetSDLRect( dst );;
	
	return Blit(src, NULL, &dstRect);
}

/**
 * Blit a part (srcRect) of surface (src) at a certaint position (dst) of the current surface/
 *
 * @param src
 * @param srcRect
 * @param dstPoint
 */
int Surface::Blit(const Surface& src, const Rectanglei &srcRect, const Point2i &dstPoint){
	SDL_Rect sdlSrcRect = GetSDLRect( srcRect );
	SDL_Rect sdlDstRect = GetSDLRect( dstPoint );

	return Blit(src, &sdlSrcRect, &sdlDstRect);
}

/**
 *
 * @param flag
 * @param key
 */
int Surface::SetColorKey(Uint32 flag, Uint32 key){
	return SDL_SetColorKey( surface, flag, key );
}

/**
 *
 *
 * @param flag
 * @param r
 * @param g
 * @param b
 * @param a
 */
int Surface::SetColorKey(Uint32 flag, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
	return SetColorKey( flag, MapRGBA(r, g, b, a) );
}

/**
 * @param color
 * @param r
 * @param g
 * @param b
 * @param a
 */
void Surface::GetRGBA(Uint32 color, Uint8 &r, Uint8 &g, Uint8 &b, Uint8 &a){
	SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);
}

/** 
 * @param r
 * @param g
 * @param b
 * @param a
 */
Uint32 Surface::MapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    return SDL_MapRGBA(surface->format, r, g, b, a);
}

/**
 *
 * @param color
 */
Color Surface::GetColor(Uint32 color){
	Uint8 r, g, b, a;
	GetRGBA(color, r, g, b, a);
	return Color(r, g, b, a);
}

/**
 *
 * @param color
 */
Uint32 Surface::MapColor(Color color){
	return MapRGBA(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

/** 
 * @param rect
 */
void Surface::SetClipRect(const Rectanglei &rect){
	SDL_Rect sdlRect = GetSDLRect( rect );
	SDL_SetClipRect( surface, &sdlRect );
}

void Surface::Flip(){
	SDL_Flip( surface );
}

int Surface::BoxColor(const Rectanglei &rect, const Color &color){
	if( rect.IsSizeZero() )
		return 0;

	Point2i ptBR = rect.GetBottomRightPoint();

	return boxRGBA( surface, rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), ptBR.GetY(), color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

int Surface::RectangleColor(const Rectanglei &rect, const Color &color, const uint &border_size)
{
  if( rect.IsSizeZero() )
    return 0;
  
  Point2i ptBR = rect.GetBottomRightPoint();
  
  if (border_size == 1)
    return rectangleRGBA( surface, rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), ptBR.GetY(), color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

  // top border
  boxRGBA (surface, 
	   rect.GetPositionX(), rect.GetPositionY(), ptBR.GetX(), rect.GetPositionY()+border_size, 
	   color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
  
  // bottom border
  boxRGBA (surface, 
	   rect.GetPositionX(), ptBR.GetY() - border_size, ptBR.GetX(), ptBR.GetY(), 
	   color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

  // left border
  boxRGBA (surface, 
	   rect.GetPositionX(), rect.GetPositionY() + border_size, rect.GetPositionX()+border_size, ptBR.GetY()-border_size, 
	   color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

  // right border
  boxRGBA (surface, 
	   ptBR.GetX() - border_size, rect.GetPositionY() + border_size, ptBR.GetX(), ptBR.GetY()-border_size, 
	   color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );

  return 1;
}

int Surface::VlineColor(const uint &x1, const uint &y1, const uint &y2, const Color &color){
	return vlineRGBA( surface, x1, y1, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

int Surface::LineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color){
  return lineRGBA( surface, x1, y1, x2, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

int Surface::AALineColor(const uint &x1, const uint &x2, const uint &y1, const uint &y2, const Color &color){
  return aalineRGBA( surface, x1, y1, x2, y2, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

int Surface::CircleColor(const uint &x, const uint &y, const uint &rad, const Color &color){
    return circleRGBA( surface, x, y, rad, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha() );
}

/**
 *
 * @param color
 */
int Surface::Fill(Uint32 color){
    return SDL_FillRect( surface, NULL, color);
}

int Surface::Fill(const Color &color){
	return Fill( MapColor(color) );
}

/** 
 *
 * @param dstRect
 * @param color
 */
int Surface::FillRect(const Rectanglei &dstRect, Uint32 color){
	SDL_Rect sdlDstRect = GetSDLRect( dstRect );

	return SDL_FillRect( surface, &sdlDstRect, color);
}

/**
 *
 * @param dstRect
 * @param color
 */
int Surface::FillRect(const Rectanglei &dstRect, const Color &color){
	
	return FillRect( dstRect, MapColor(color) );
}

/**
 *
 * @param filename
 */
int Surface::ImgLoad(std::string filename){
	AutoFree();
	surface = IMG_Load( filename.c_str() );

	return !IsNull();
}

/**
 *
 * @param angle
 * @param zoomx
 * @param zoomy
 * @param smooth
 */
Surface Surface::RotoZoom(double angle, double zoomx, double zoomy, int smooth){
	Surface newSurf;

	newSurf.SetSurface( rotozoomSurfaceXY(surface, angle, zoomx, zoomy, smooth) );

	if( newSurf.IsNull() )
		Error( "Unable to make a rotozoom on the surface !" );

	return newSurf;	
}

/**
 *
 */
Surface Surface::DisplayFormatAlpha(){
	Surface newSurf;

	newSurf.SetSurface( SDL_DisplayFormatAlpha( surface ) );

	if( newSurf.IsNull() )
		Error( "Unable to convert the surface to a surface compatible with the display format with alpha." );

	return newSurf;
}

/**
 *
 */
Surface Surface::DisplayFormat(){
	Surface newSurf;

	newSurf.SetSurface( SDL_DisplayFormat( surface ) );

	if( newSurf.IsNull() )
		Error( "Unable to convert the surface to a surface compatible with the display format." );

	return newSurf;
}


/**
 * GetPixel.
 *
 * From the SDL wiki.
 * @param x
 * @param y
 */
Uint32 Surface::GetPixel(int x, int y){
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
		Error("Unknow bpp!");
        return 0;   // To make gcc happy
    }
}

/**
 *
 * @param x
 * @param y
 * @param pixel
 */
void Surface::PutPixel(int x, int y, Uint32 pixel){
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

SDL_Rect Surface::GetSDLRect(const Rectanglei &r){
	SDL_Rect sdlRect;

    sdlRect.x = r.GetPositionX();
    sdlRect.y = r.GetPositionY();
    sdlRect.w = r.GetSizeX();
    sdlRect.h = r.GetSizeY();

	return sdlRect;	
}

SDL_Rect Surface::GetSDLRect(const Point2i &pt){
	SDL_Rect sdlRect;

	sdlRect.x = pt.GetX();
	sdlRect.y = pt.GetY();
	sdlRect.w = 0;
	sdlRect.h = 0;

	return sdlRect;
}
