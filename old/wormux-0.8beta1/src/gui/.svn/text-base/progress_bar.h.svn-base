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
 ******************************************************************************
 * Progress bar for GUI.
 *****************************************************************************/

#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <SDL.h>
#include <list>
#include "include/base.h"
#include "graphic/color.h"
#include "graphic/surface.h"

class ProgressBar
{
public:
  Color border_color, value_color, background_color;
  Surface image; // in order to pemit alpha blended progressbar
  enum orientation {
    PROG_BAR_VERTICAL,
    PROG_BAR_HORIZONTAL
  };

private:
  uint x, y, larg, haut; // Position
  long val, min, max; // current, min and max values
  bool m_use_ref_val; // use reference value ?
  long m_ref_val; // reference value
  uint val_barre; // current value in the progress bar
  enum orientation orientation;

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
  ProgressBar();
  virtual ~ProgressBar() {};

  // Actualisation de la valeur
  void UpdateValue (long val);

  // Initialise la position
  void InitPos (uint x, uint y, uint larg, uint haut);

  // Initialise les valeurs
  void InitVal (long val, long min, long max, enum orientation orientation = PROG_BAR_HORIZONTAL);

  // Set reference value
  // Use it after InitVal !
  void SetReferenceValue (bool use, long value=0);

  // Draw la barre de progresssion
  void Draw() const;

  // Draw the progress bar
  void DrawXY(const Point2i &pos) const;

  inline const long & GetMaxVal() const { return max; }
  inline const long & GetVal() const { return val; }

  int GetWidth() const { return larg; }
  int GetHeight() const { return haut; }
  Point2i GetSize() const { return Point2i(larg, haut); }

  // add/remove value tag
  marqueur_it AddTag (long val, const Color& coul);
  void ResetTag();
};

#endif
