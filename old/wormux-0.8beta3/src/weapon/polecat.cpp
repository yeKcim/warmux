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
 * Polecat : send a polecat to the enemy. Close character get sick with the mefitic odor.
 *****************************************************************************/

#include "weapon/polecat.h"
#include "weapon/grenade.h"
#include "weapon/weapon_cfg.h"

#include <sstream>
#include "weapon/explosion.h"
#include "character/character.h"
#include "game/config.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

const uint TIME_BETWEEN_FART = 500;

class Polecat : public WeaponProjectile
{
 private:
  int m_sens;
  int save_x, save_y;
  uint last_fart_time;
  double angle;
 protected:
  void SignalOutOfMap();
 public:
  Polecat(ExplosiveWeaponConfig& cfg,
          WeaponLauncher * p_launcher);
  void Shoot(double strength);
  void Refresh();
};


Polecat::Polecat(ExplosiveWeaponConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile("polecat", cfg, p_launcher)
{
  explode_with_collision = false;
  last_fart_time = 0;
}

void Polecat::Shoot(double strength)
{
  WeaponProjectile::Shoot(strength);
  last_fart_time = Time::GetInstance()->Read() + TIME_BETWEEN_FART;

  save_x=GetX();
  save_y=GetY();

  double angle = ActiveCharacter().GetFiringAngle();

  if(angle<M_PI/2 && angle>-M_PI/2)
    m_sens = 1;
  else
    m_sens = -1;
}

void Polecat::Refresh()
{
  WeaponProjectile::Refresh();

  double norme, angle;
  //When we hit the ground, jump !
  if(!IsMoving() && !FootsInVacuum())
  {
    //If the GNU is stuck in ground -> change direction
    int x = GetX();
    int y = GetY();
    if(x==save_x && y==save_y)
      m_sens = - m_sens;
    save_x = x;
    save_y = y;

    //Do the jump
    norme = randomSync.GetDouble(1.0, 2.0);
    PutOutOfGround();
    SetSpeedXY(Point2d(m_sens * norme , - norme * 3.0));
  }
  if (last_fart_time + TIME_BETWEEN_FART < Time::GetInstance()->Read()) {
    double norme = randomSync.GetLong(0, 5000)/100;
    double angle = randomSync.GetLong(0, 3000)/1000;
    ParticleEngine::AddNow(GetPosition(), 1, particle_POLECAT_FART, true, norme, angle);
    last_fart_time = Time::GetInstance()->Read();
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

  image->SetRotation_rad(angle);
  image->Scale((double)m_sens,1.0);
  image->Update();
  // Set the test area ?
  SetTestRect ( image->GetWidth()/2-1,
                image->GetWidth()/2-1,
                image->GetHeight()/2-1,
                image->GetHeight()/2-1);
}

void Polecat::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The Polecat left the battlefield before exploding"));
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

PolecatLauncher::PolecatLauncher() :
  WeaponLauncher(WEAPON_POLECAT, "polecatlauncher", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Polecat Launcher");
  m_category = SPECIAL;
  ReloadLauncher();
}

WeaponProjectile * PolecatLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
    (new Polecat(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

std::string PolecatLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u polecat! You have you're gas mask, right ?",
            "%s team has won %u polecats! You have you're gas mask, right ?",
            items_count), TeamName, items_count);
}


