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

#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include <list>
#include "../include/base.h"
//-----------------------------------------------------------------------------

class BarreProg
{
public:
  SDL_Color border_color, value_color, background_color;
  SDL_Surface *image; // in order to pemit alpha blended progressbar
private:
  uint x, y, larg, haut; // Position
  long val, min, max; // Valeur
  bool m_use_ref_val; // Valeur de référence
  long m_ref_val; // Valeur de référence
  uint val_barre; // Valeur dans la barre

  uint CalculeVal (long val) const;
  uint CalculeValBarre (long val) const;

  typedef struct s_marqueur_t{ SDL_Color color; uint val; } marqueur_t;
 public:
  void SetBorderColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
  void SetBackgroundColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
  void SetValueColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
 private:
  typedef std::list<marqueur_t>::iterator marqueur_it;
  typedef std::list<marqueur_t>::const_iterator marqueur_it_const;
  std::list<marqueur_t> marqueur;

public:
  BarreProg();

  // Actualisation de la valeur
  void Actu (long val);

  // Initialise la position
  void InitPos (uint x, uint y, uint larg, uint haut);

  // Initialise les valeurs
  void InitVal (long val, long min, long max);

  // Set reference value
  // Use it after InitVal !
  void SetReferenceValue (bool use, long value=0);

  // Draw la barre de progresssion
  void Draw () const;

  // Change les coordonnées, puis dessine la barre de progression
  void DrawXY (uint x, uint y) const;

  // Lit sa taille
  uint GetWidth() const { return larg; }
  uint GetHeight() const { return haut; }

  // Ajoute/supprime un marqueur
  marqueur_it AjouteMarqueur (long val, const SDL_Color& coul);
  marqueur_it AjouteMarqueur (long val, unsigned char r, unsigned char g, unsigned char b, unsigned char a=255);
  void SupprimeMarqueur (marqueur_it it);
  void Reset_Marqueur();
};

//-----------------------------------------------------------------------------
#endif
