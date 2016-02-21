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
 * Weapon bazooka : projette une roquette avec un angle et une force donnée.
 *****************************************************************************/

#ifndef AUTO_BAZOOKA_H
#define AUTO_BAZOOKA_H
#include "launcher.h"
#include "../graphic/surface.h"
#include "../gui/progress_bar.h"
#include "../include/base.h"
#include "../object/physical_obj.h"

class AutomaticBazooka;

// Roquette du bazooka à tête chercheuse
class RoquetteTeteCherche : public WeaponProjectile
{
protected:
  double angle_local;
  Point2i m_cible;
  bool m_attire;
public:
  RoquetteTeteCherche(ExplosiveWeaponConfig& cfg);
  void Refresh();
  void Shoot(double strength);
  void SetTarget (int x,int y);
 protected:
  void SignalCollision();
};

class AutomaticBazooka : public WeaponLauncher
{
private:
  
  struct s_cible
  {
    Point2i pos;
    bool choisie;
    Surface image;
  } cible;
  
public:

  AutomaticBazooka();

  void Draw ();
  void Refresh();
  void p_Select(); 
  void p_Deselect();

  bool IsReady() const;
  virtual void ChooseTarget();
  void DrawTarget();
};

#endif
