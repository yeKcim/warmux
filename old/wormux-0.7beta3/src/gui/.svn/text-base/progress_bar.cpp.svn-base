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
 * Progress bar for GUI.
 *****************************************************************************/

#include "progress_bar.h"
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../tool/math_tools.h"
#include "../include/app.h"
#include "../map/map.h"
#include "../graphic/video.h"
//-----------------------------------------------------------------------------

BarreProg::BarreProg()
{   
   border_color.r = 0;
   border_color.g = 0;
   border_color.b = 0;
   value_color.r = 255;
   value_color.g = 255;
   value_color.b = 255;
   background_color.r = 100;
   background_color.g = 100;
   background_color.b = 100;
   x = y = larg = haut = 0;
   val = min = max = 0;
   m_use_ref_val = false;
   image = NULL;
}

//-----------------------------------------------------------------------------

void BarreProg::SetBorderColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
   border_color.r = r;
   border_color.g = g;
   border_color.b = b;
   border_color.unused = a;   
}

//-----------------------------------------------------------------------------

void BarreProg::SetBackgroundColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
   background_color.r = r;
   background_color.g = g;
   background_color.b = b;   
   background_color.unused = a;   
}

//-----------------------------------------------------------------------------

void BarreProg::SetValueColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
   value_color.r = r;
   value_color.g = g;
   value_color.b = b;
   value_color.unused = a;   
}

//-----------------------------------------------------------------------------

void BarreProg::InitPos (uint px, uint py, uint plarg, uint phaut)
{
  assert (3 <= plarg);
  assert (3 <= phaut);
  x = px;
  y = py;
  larg = plarg;
  haut = phaut;

  if ( image != NULL)
  {
    SDL_FreeSurface( image);
  }
   
  image = CreateRGBASurface(larg, haut, SDL_SWSURFACE|SDL_SRCALPHA);
}

//-----------------------------------------------------------------------------

void BarreProg::InitVal (long pval, long pmin, long pmax)
{
  assert (pmin != pmax);
  assert (pmin < pmax);
  val = pval;
  min = pmin;
  max = pmax;
  val_barre = CalculeValBarre(val);
}

//-----------------------------------------------------------------------------

void BarreProg::Actu (long pval)
{
  val = CalculeVal(pval);
  val_barre = CalculeValBarre(val);
}

//-----------------------------------------------------------------------------

uint BarreProg::CalculeVal (long val) const
{ 
  return BorneLong(val, min, max); 
}

//-----------------------------------------------------------------------------

uint BarreProg::CalculeValBarre (long val) const
{ 
  return ( CalculeVal(val) -min)*(larg-2)/(max-min);
}

//-----------------------------------------------------------------------------

void BarreProg::Draw() const
{
  DrawXY (x,y);
}

//-----------------------------------------------------------------------------
 
// TODO pass SDL_Surface as parameter
 
void BarreProg::DrawXY (uint px, uint py) const
{ 
  int left, right;
   
  // Bordure
  SDL_FillRect( image, NULL, SDL_MapRGBA( image->format, border_color.r, border_color.g, border_color.b, border_color.unused));
   
  // Fond
  SDL_Rect r_back = {1, 1, larg-2, haut-2};
  SDL_FillRect( image, &r_back, SDL_MapRGBA( image->format, background_color.r, background_color.g, background_color.b,background_color.unused));   
   
  // Valeur
  if (m_use_ref_val) {
    int ref = CalculeValBarre (m_ref_val);
    if (val < m_ref_val) {
      left = 1+val_barre;
      right = 1+ref;
    } else {
      left = 1+ref;
      right = 1+val_barre;
    }
  } else {
    left = 1;
    right = 1+val_barre;
  }  

  SDL_Rect r_value = {left, 1, right-left, haut-2};
  SDL_FillRect( image, &r_value, SDL_MapRGBA( image->format, value_color.r, value_color.g, value_color.b, value_color.unused));
   
  if (m_use_ref_val) {
    int ref = CalculeValBarre (m_ref_val);
    SDL_Rect r_ref = {1+ref, 1, 1, haut-2};
    SDL_FillRect( image, &r_ref, 
      SDL_MapRGBA( image->format, border_color.r, border_color.g, border_color.b, border_color.unused));           
  }

  // Marqueurs
  marqueur_it_const it=marqueur.begin(), fin=marqueur.end();
  for (; it != fin; ++it)
  {
    SDL_Rect r_marq = {1+it->val, 1, 1, haut-2};
    SDL_FillRect( image, &r_marq,
      SDL_MapRGBA( image->format, border_color.r, border_color.g, border_color.b, border_color.unused));           
  }
 
  // Blit internal surface to destination
  SDL_Rect d = {px, py, larg, haut};
  SDL_BlitSurface( image, NULL, app.sdlwindow, &d);

  world.ToRedrawOnScreen(Rectanglei(d.x, d.y, d.w, d.h));
}

//-----------------------------------------------------------------------------

// Ajoute/supprime un marqueur
BarreProg::marqueur_it BarreProg::AjouteMarqueur (long val, const SDL_Color& color)
{
  marqueur_t m;
  m.val = CalculeValBarre (val);
  m.color = color;
  marqueur.push_back (m);
  return --marqueur.end();
}

BarreProg::marqueur_it BarreProg::AjouteMarqueur (long val, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  marqueur_t m;
  m.val = CalculeValBarre (val);
  m.color.r = r;
  m.color.g = g;
  m.color.b = b;
  marqueur.push_back (m);
  return --marqueur.end();
}


//-----------------------------------------------------------------------------

void BarreProg::SupprimeMarqueur (marqueur_it it)
{ marqueur.erase (it); }

//-----------------------------------------------------------------------------

void BarreProg::Reset_Marqueur() { marqueur.clear(); }

//-----------------------------------------------------------------------------

void BarreProg::SetReferenceValue (bool use, long value)
{
  m_use_ref_val = use;
  m_ref_val = CalculeVal(value);
}

//-----------------------------------------------------------------------------
