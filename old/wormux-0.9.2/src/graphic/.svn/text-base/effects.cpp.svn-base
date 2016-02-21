/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
 *  Graphic effects on sprite
 *****************************************************************************/

#include "game/time.h"
#include "graphic/sprite.h"

//Make the sdl_surface 'a', do a wave of 'nbr_frames', and last 'duration' milliseconds.
//wave_amp is the amplitude of the wave on the left and the right side of the sprite
//wave_per is the number of periods of the wave when it is waved at the maximum
//used on the skin during teleportation
Sprite* WaveSurface(Surface &a, unsigned int nbr_frames,
                    unsigned int duration, Double wave_amp, Double wave_per){
  Sprite* sprite = new Sprite;
  Point2i newSize = a.GetSize() + Point2i(2 * (int)wave_amp, 0);

  sprite->SetSize(newSize);
  for(unsigned int f=0; f < nbr_frames; f++){
    Surface b(newSize, SDL_SWSURFACE|SDL_SRCALPHA );
    b.Fill(0x00000000);
    b.SetAlpha(SDL_SRCALPHA, 0);
    a.Lock();
    b.Lock();
    for(int x = 0; x < a.GetWidth(); x++){
      for(int y = 0; y < a.GetHeight(); y++){
        Uint32 col = a.GetPixel(x, y);
        Uint8 r, g, bl, al;

        a.GetRGBA(col, r, g, bl, al);
        col = b.MapRGBA(r, g, bl, al);

        Double t = (Double)nbr_frames * sin(PI*(Double)f/(Double)nbr_frames);
        unsigned int wave_x = (long)(x+(wave_amp*(1+sin(((Double)t*wave_per*TWO*PI/(Double)nbr_frames)*(Double)y*TWO*PI/(Double)a.GetHeight()))));
        b.PutPixel(wave_x, y, col);
      }
    }
    a.Unlock();
    b.Unlock();

    sprite->AddFrame(b, duration / nbr_frames);
  }
  return sprite;
}

// Modify the scale of 'spr' to make it deform as if it was rebounding
// dy returns the offset that should be used to display the sprite
// at the good position t0 time when we began to rebound
// per time to do one full rebound
// dy_max offset max of the rebound ( 0 <= dy <= dy_max )
void Rebound(Sprite* spr, int &dy, uint t0, uint per, int dy_max)
{
  Double scale_x, scale_y;
  uint dt = (Time::GetInstance()->Read() - t0) % per;

  spr->Scale(ONE,ONE);
  dy = 0;

  //sprite at bottom:
  if( dt < per / 4 )
  {
    Double dt2 = ((Double)((per / 4) - dt)) / ((Double)per / FOUR);
    scale_y =        TWO*dt2*dt2 - TWO*dt2 + ONE;
    scale_x = TWO - (TWO*dt2*dt2 - TWO*dt2 + ONE);
    dy = 0;
    spr->Scale(scale_x,scale_y);
    return;
  }

  dt -= per/4;
  Double dt2 = ((Double)((3*per/4)-dt))/(THREE*per/FOUR);
  dy += (int)((-FOUR*dt2*dt2 + FOUR*dt2) * dy_max);
}

//Returns value of y_stretch and y, to be used on an object, to make him do
// a gelatin-like shaking.
// stretch_y: number of pixel to stretch the img
// t0 : beginning time of the effect in milliseconds
// amp: amplitude of the oscillation in pixel
// dur: duration of the oscillation in milliseconds
// per: number of oscillations
void Gelatine (int &y, int &stretch_y, uint t0, uint amp, uint dur, uint per)
{
  uint dt = Time::GetInstance()->Read() - t0;
  if(dt >= dur)
  {
    y = 0;
    stretch_y = 0;
    return;
  }

  //Amplitude decreases linearly with time
  amp = amp * (dur - dt) / dur;

  //Scale
  stretch_y = (int)(sin((Double)per * (Double)dt * TWO * PI / (Double)dur) * (Double)amp);

  //Offset
  if(stretch_y < 0.0)
  {
    y = stretch_y;
    stretch_y = -stretch_y;
  }
  else
  {
    y = 0;
  }
}

