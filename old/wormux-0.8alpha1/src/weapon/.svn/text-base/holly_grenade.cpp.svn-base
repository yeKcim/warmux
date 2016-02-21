/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Holly Grenade
 *****************************************************************************/

#include "holly_grenade.h"
#include <sstream>
#include "explosion.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"

HollyGrenade::HollyGrenade(ExplosiveWeaponConfig& cfg,
                           WeaponLauncher * p_launcher) :
  WeaponProjectile ("holly_grenade", cfg, p_launcher),
  smoke_engine(40)
{
  m_rebound_sound = "weapon/holly_grenade_bounce";
  sing_alleluia = false;
  explode_with_collision = false;
}

void HollyGrenade::Explosion()
{
  const uint star_nbr = 9;
  const float cos_angle[] = {1.000000, 0.766044, 0.173648, -0.500000, -0.939693, -0.939693, -0.500000, 0.173648, 0.766044};
  const float sin_angle[] = {0.000000, 0.642788, 0.984808, 0.866025, 0.342020, -0.342020, -0.866025, -0.984808, -0.642788};

  for(uint i=0;i < star_nbr;i++)
  {
    double angle = 2.0*(double)i*M_PI/(double)star_nbr;
    //  cos_angle[i] = cos(angle);
    //  sin_angle[i] = sin(angle);
    smoke_engine.AddNow(Point2i(GetX()+(int)(cos_angle[i]*(float)cfg.explosion_range),
                                GetY()+(int)(sin_angle[i]*(float)cfg.explosion_range)),
                                1,particle_MAGIC_STAR,false,angle,2.5);
  }
  WeaponProjectile::Explosion();
}

void HollyGrenade::Refresh()
{
  WeaponProjectile::Refresh();

#ifdef HAVE_A_REALLY_BIG_CPU
  if(IsMoving())
  {
    double norme,angle;
    GetSpeed(norme,angle);
    for(int i = -3; i<4 ; i++)
      smoke_engine.AddNow(GetPosition(), 1,particle_MAGIC_STAR, false,angle+(i*M_PI_4/3.0)+M_PI_2,2.0);
  }
  else
  {
      smoke_engine.AddNow(GetPosition(), 1,particle_MAGIC_STAR, false,((float)(Time::GetInstance()->Read()%500)-250.0) * M_PI / 250.0,3.0);
  }
#else //  :-P
  smoke_engine.AddPeriodic(GetPosition(), particle_MAGIC_STAR, false);
#endif //HAVE_A_REALLY_BIG_CPU

  double tmp = Time::GetInstance()->Read() - begin_time;
  // Sing Alleluia ;-)
  if (tmp > (1000 * cfg.timeout - 2000) && !sing_alleluia) {
    jukebox.Play("share","weapon/alleluia") ;
    sing_alleluia = true;
  }
  
  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180.0/M_PI ;
  image->SetRotation_deg(angle);
}

void HollyGrenade::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add ("The grenade has left the battlefield before exploding");
  WeaponProjectile::SignalOutOfMap();
}

//-----------------------------------------------------------------------------

HollyGrenadeLauncher::HollyGrenadeLauncher() : 
  WeaponLauncher(WEAPON_HOLLY_GRENADE, "holly_grenade", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{  
  m_name = _("Holy Grenade");
  m_allow_change_timeout = true;
  ignore_collision_signal = true;
  ReloadLauncher();
}

WeaponProjectile * HollyGrenadeLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new HollyGrenade(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}
