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
 *  Refresh du vent
 *****************************************************************************/

#ifndef WIND_H
#define WIND_H

#include <list>
#include "include/base.h"
#include "object/physical_obj.h"

// Forward declarations
class Sprite;
namespace xmlpp
{
  class Element;
}

// Max wind strength in m/(sec*sec)
#define WIND_STRENGTH  5.0

class WindParticle : public PhysicalObj
{
  /* You should not need this */
  WindParticle(const WindParticle&);
  const WindParticle& operator=(const WindParticle&);

public:
  Sprite *sprite;
  Sprite *flipped;

public:
  WindParticle(const std::string& xml_file, float scale);
  ~WindParticle();
  void Draw();
  void Refresh();
};

class Wind
{
  long m_val, m_nv_val;
  uint m_last_move;
  uint m_last_part_mvt;

private:
  std::list<WindParticle *> particles;
  typedef std::list<WindParticle *>::iterator iterator;
  void RemoveAllParticles();

public:
  Wind() { m_val = m_nv_val = 0; };
  ~Wind() { RemoveAllParticles(); };
  double GetStrength() const { return m_nv_val * WIND_STRENGTH / 100.0; };
  void ChooseRandomVal() const;
  void SetVal (long val) { m_nv_val = val; };
  void Refresh();
  void Reset();
  void DrawParticles();
  void RandomizeParticlesPos(); // Put particles randomly on the screen
};

extern Wind wind;
#endif

