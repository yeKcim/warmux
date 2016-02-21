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
#include <SDL.h>
#include "../include/app.h"
#include "../map/map.h"
#include "../tool/math_tools.h"

BarreProg::BarreProg(){   
   border_color.SetColor(0, 0, 0, 255);
   value_color.SetColor(255, 255, 255, 255);
   background_color.SetColor(100, 100 ,100, 255);
   x = y = larg = haut = 0;
   val = min = max = 0;
   m_use_ref_val = false;
}

void BarreProg::SetBorderColor(Color color){
   border_color = color;
}

void BarreProg::SetBackgroundColor(Color color){
   background_color = color;
}

void BarreProg::SetValueColor(Color color){
   value_color = color;
}

void BarreProg::InitPos (uint px, uint py, uint plarg, uint phaut){
  assert (3 <= plarg);
  assert (3 <= phaut);
  x = px;
  y = py;
  larg = plarg;
  haut = phaut;

  image.NewSurface(Point2i(larg, haut), SDL_SWSURFACE|SDL_SRCALPHA, true);
}

void BarreProg::InitVal (long pval, long pmin, long pmax){
  assert (pmin != pmax);
  assert (pmin < pmax);
  val = pval;
  min = pmin;
  max = pmax;
  val_barre = CalculeValBarre(val);
}

void BarreProg::Actu (long pval){
  val = CalculeVal(pval);
  val_barre = CalculeValBarre(val);
}

uint BarreProg::CalculeVal (long val) const{ 
  return BorneLong(val, min, max); 
}

uint BarreProg::CalculeValBarre (long val) const{
  return ( CalculeVal(val) -min)*(larg-2)/(max-min);
}

void BarreProg::Draw(){
  DrawXY( Point2i(x, y) );
}

// TODO pass a Surface as parameter
void BarreProg::DrawXY(const Point2i &pos){ 
  int left, right;
   
  // Bordure
  image.Fill(border_color);
   
  // Fond
  Rectanglei r_back(1, 1, larg - 2, haut - 2);
  image.FillRect(r_back, background_color);
   
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

  Rectanglei r_value (left, 1, right - left, haut - 2);
  image.FillRect(r_value, value_color);
   
  if (m_use_ref_val) {
    int ref = CalculeValBarre (m_ref_val);
    Rectanglei r_ref(1 + ref, 1, 1, haut - 2);
	image.FillRect(r_ref, border_color);
  }

  // Marqueurs
  marqueur_it_const it=marqueur.begin(), fin=marqueur.end();
  for (; it != fin; ++it)
  {
    Rectanglei r_marq(1 + it->val, 1, 1, haut - 2);
	image.FillRect( r_marq, border_color);
  }
  Rectanglei dst(pos.x, pos.y, larg, haut); 
  AppWormux::GetInstance()->video.window.Blit(image, pos);

  world.ToRedrawOnScreen(dst);
}

// Ajoute/supprime un marqueur
BarreProg::marqueur_it BarreProg::AjouteMarqueur (long val, const Color& color){
  marqueur_t m;
  
  m.val = CalculeValBarre (val);
  m.color = color;
  marqueur.push_back (m);
  
  return --marqueur.end();
}

void BarreProg::SupprimeMarqueur (marqueur_it it){
  marqueur.erase (it);
}

void BarreProg::Reset_Marqueur(){
  marqueur.clear();
}

void BarreProg::SetReferenceValue (bool use, long value){
  m_use_ref_val = use;
  m_ref_val = CalculeVal(value);
}

