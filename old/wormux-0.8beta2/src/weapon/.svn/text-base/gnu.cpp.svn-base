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
 * Weapon gnu : a gnu jump in (more or less) random directions and explodes
 *****************************************************************************/

#include "explosion.h"
#include "gnu.h"
#include "weapon_cfg.h"

#include <sstream>
#include "character/character.h"
#include "game/config.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

class Gnu : public WeaponProjectile
{
 private:
  int m_sens;
  int save_x, save_y;
 protected:
  void SignalOutOfMap();
public:
  Gnu(ExplosiveWeaponConfig& cfg,
      WeaponLauncher * p_launcher);
  void Shoot(double strength);
  void Refresh();
};


Gnu::Gnu(ExplosiveWeaponConfig& cfg,
         WeaponLauncher * p_launcher) :
  WeaponProjectile("gnu", cfg, p_launcher)
{
  explode_with_collision = false;
}

void Gnu::Shoot (double strength)
{
  WeaponProjectile::Shoot(strength);

  save_x=GetX();
  save_y=GetY();

  double angle = ActiveCharacter().GetFiringAngle();

  if(angle<M_PI/2 && angle>-M_PI/2)
    m_sens = 1;
  else
    m_sens = -1;
}

void Gnu::Refresh()
{
  WeaponProjectile::Refresh();

  double norme, angle;
  //When we hit the ground, jump !
  if(!IsMoving()&& !FootsInVacuum())
  {
    //If the GNU is stuck in ground -> change direction
    int x = GetX();
    int y = GetY();
    if(x==save_x && y==save_y)
      m_sens = - m_sens;
    save_x = x;
    save_y = y;

    //Do the jump
    norme = randomSync.GetDouble(2.0, 5.0);
    PutOutOfGround();
    SetSpeedXY(Point2d(m_sens * norme , - norme * 3.0));
    jukebox.Play("share", "weapon/gnu_bounce");
  }

  //Due to a bug in the physic engine
  //sometimes, angle==infinite (according to gdb) ??
  GetSpeed(norme, angle);

  while(angle < -M_PI)
    angle += M_PI;
  while(angle > M_PI)
    angle -= M_PI;

  angle /= 2.0;
  if(m_sens == -1)
  {
    if(angle > 0)
      angle -= M_PI_2;
    else
      angle += M_PI_2;
  }

  if(angle > 4 * M_PI)
    angle = 0;

  image->SetRotation_rad(angle);
  image->Scale((double)m_sens,1.0);
  image->Update();
  // Fixes test rectangle ??
  SetTestRect ( image->GetWidth()/2-1,
                image->GetWidth()/2-1,
                image->GetHeight()/2-1,
                image->GetHeight()/2-1);
}

void Gnu::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The Gnu left the battlefield before exploding"));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

GnuLauncher::GnuLauncher() :
  WeaponLauncher(WEAPON_GNU, "gnulauncher", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Gnu Launcher");
  m_category = SPECIAL;
  ReloadLauncher();
}

WeaponProjectile * GnuLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new Gnu(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

std::string GnuLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u Gnu! Blow them all, cowboy!",
            "%s team has won %u Gnus! Blow them all, cowboy!",
            items_count), TeamName, items_count);
}


