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

#ifndef BAZ_TETE_C_H
#define BAZ_TETE_C_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include "../weapon/weapon.h"
#include "../gui/progress_bar.h"
#include "../object/physical_obj.h"

//-----------------------------------------------------------------------------
namespace Wormux {
//-----------------------------------------------------------------------------

// Roquette du bazooka à tête chercheuse
class RoquetteTeteCherche : public WeaponProjectile
{
protected:
  double angle_local;
  double temps_debut_tir;
  Point2i m_cible;
  bool m_attire;
public:
  RoquetteTeteCherche();
  void Tire (double force, uint cible_x,uint cible_y);
  uint ChoixFrame(double angle);
  void Init();
  void Reset();
  void Refresh();
protected:
  void SetTarget (int x,int y);
  void SignalCollision();
};

//-----------------------------------------------------------------------------

class AutomaticBazooka : public Weapon
{
private:

  struct s_cible
  {
    Point2i pos;
    bool choisie;
    SDL_Surface *image;
  } cible;
  
  void p_Init();
  bool p_Shoot();

public:
  SDL_Surface *impact;
  RoquetteTeteCherche roquette;

  AutomaticBazooka();
  void Draw ();
  void Refresh();
  void p_Select(); 
  void p_Deselect();

  bool IsReady() const;

  // Le bazooka explose car il a été poussé à bout !
  void ExplosionDirecte();

  virtual void ChooseTarget();
  void DrawTarget();

  ExplosiveWeaponConfig& cfg();

protected:
  void Explosion();
};

extern AutomaticBazooka auto_bazooka;
//-----------------------------------------------------------------------------
} // namespace Wormux
#endif
