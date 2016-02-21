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

#include <SDL.h>
#include <list>
#include "../include/base.h"
#include "../graphic/color.h"
#include "../graphic/surface.h"

class BarreProg
{
public:
  Color border_color, value_color, background_color;
  Surface image; // in order to pemit alpha blended progressbar
private:
  uint x, y, larg, haut; // Position
  long val, min, max; // Valeur
  bool m_use_ref_val; // Valeur de référence
  long m_ref_val; // Valeur de référence
  uint val_barre; // Valeur dans la barre

  uint CalculeVal (long val) const;
  uint CalculeValBarre (long val) const;

  typedef struct s_marqueur_t{
	  Color color;
	  uint val;
  } marqueur_t;

 public:
  void SetBorderColor(Color color);
  void SetBackgroundColor(Color color);
  void SetValueColor(Color color);
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
  void Draw();

  // Change les coordonnées, puis dessine la barre de progression
  void DrawXY(const Point2i &pos);

  // Lit sa taille
  int GetWidth() const { return larg; }
  int GetHeight() const { return haut; }
  Point2i GetSize() const { return Point2i(larg, haut); }

  // Ajoute/supprime un marqueur
  marqueur_it AjouteMarqueur (long val, const Color& coul);
  void SupprimeMarqueur (marqueur_it it);
  void Reset_Marqueur();
};

#endif
