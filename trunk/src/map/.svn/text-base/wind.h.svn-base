/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include <WARMUX_base.h>
#include <WARMUX_singleton.h>
#include "object/physical_obj.h"

// Forward declarations
class Sprite;
typedef struct _xmlNode xmlNode;

// Max wind strength in m/(sec*sec)
#define WIND_STRENGTH  4.0

class WindParticle : public PhysicalObj
{
  Sprite * sprite;

public:
  WindParticle(const std::string & xml_file, 
               Double scale);
  ~WindParticle();
  void Draw();
  void Refresh();
};

class Wind : public Singleton<Wind>
{
  int m_val, m_nv_val;
  uint m_last_move;
  uint m_last_part_mvt;

  std::list<WindParticle *> particles;
  typedef std::list<WindParticle *>::iterator iterator;
  void RemoveAllParticles();
  void RandomizeParticlesPos(); // Put particles randomly on the screen

  Wind();
  ~Wind() { RemoveAllParticles(); }
  friend class Singleton<Wind>;

public:
  Double GetStrength() const { return m_nv_val * WIND_STRENGTH / 100.0; }
  void ChooseRandomVal();
  void UpdateStrength();

  void SetVal(int val);
  void Refresh();
  void Reset();
  void DrawParticles();
};

#endif

