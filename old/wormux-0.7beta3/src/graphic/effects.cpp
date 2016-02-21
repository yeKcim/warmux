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
 ******************************************************************************
 *  Graphic effects on sprite / pixel acces to SDL_Surface
 *****************************************************************************/

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <math.h>
#include "sprite.h"
#include "video.h"
#include "../game/time.h"

// From SDL's wiki
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
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
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
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

//Make the sdl_surface 'a', do a wave of 'nbr_frames', and last 'duration' milliseconds.
//wave_amp is the amplitude of the wave on the left and the right side of the sprite
//wave_per is the number of periode of the wave when it is waved at the maximum
//used on the skin during teleportation
Sprite* WaveSurface(SDL_Surface* a,unsigned int nbr_frames, unsigned int duration, float wave_amp, float wave_per)
{
	assert(a!=NULL);
	Sprite* sprite = new Sprite;
   sprite->SetSize(a->w + 2 * (unsigned int) wave_amp, a->h);
	for(unsigned int f=0;f<nbr_frames;f++)
	{
		SDL_Surface* b = CreateRGBASurface(a->w+2*(int)wave_amp, a->h, SDL_SWSURFACE|SDL_SRCALPHA);
		SDL_FillRect(b,NULL,0x00000000);
		SDL_SetAlpha(b,SDL_SRCALPHA,0);
		SDL_LockSurface(a);
		SDL_LockSurface(b);
		for(int x=0;x< a->w;x++)
		{
			for(int y=0;y< a->h; y++)
			{
				Uint32 col = getpixel(a,x,y);
				Uint8 r,g,bl,al;
				SDL_GetRGBA(col,a->format,&r,&g,&bl,&al);
				col=SDL_MapRGBA(b->format,r,g,bl,al);

				float t = (float)nbr_frames * sin(M_PI*(float)f/(float)nbr_frames);
				unsigned int wave_x = (unsigned int)(x+(wave_amp*(1+sin(((float)t*wave_per*2.0*M_PI/(float)nbr_frames)*(float)y*2.0*M_PI/(float)a->h))));
				putpixel(b, wave_x, y, col);
			}
		}
		SDL_UnlockSurface(a);
		SDL_UnlockSurface(b);	
		sprite->AddFrame(b, duration / nbr_frames);
	}
	return sprite;
}

//Modify the scale of 'spr' to make it deform as if it was rebounding
// dy return the offset that should be used to display the sprite at the good position
// t0 time when we began to rebound
// per time to do one full rebound
// dy_max offset max of the rebound ( 0 <= dy <= dy_max )
void Rebound(Sprite* spr, int &dy, uint t0, uint per, int dy_max)
{
  float scale_x, scale_y;
  int spr_w, spr_h;
  uint dt = (Wormux::global_time.Read() - t0) % per;

  spr->Scale(1.0,1.0);
  spr_w = spr->GetWidth();
  spr_h = spr->GetHeight();
  dy = 0;

  //sprite at bottom:
  if( dt < per / 4 )
  {
    float dt2 = ((per / 4) - dt) / ((float)per / 4.0);    
    scale_y =        2.0*dt2*dt2 - 2.0*dt2 + 1.0;
    scale_x = 2.0 - (2.0*dt2*dt2 - 2.0*dt2 + 1.0);
    dy = 0;
    spr->Scale(scale_x,scale_y);
    return;
  }

  dt -= per/4;
  float dt2 = ((3*per/4)-dt)/(3.0*per/4.0);
  dy += (int)((-4.0*dt2*dt2 + 4.0*dt2) * dy_max);
}
