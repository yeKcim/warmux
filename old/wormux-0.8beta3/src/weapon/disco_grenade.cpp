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
 * Disco Grenade
 *****************************************************************************/

#include "weapon/disco_grenade.h"
#include "weapon/weapon_cfg.h"

#include <sstream>
#include "weapon/explosion.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/math_tools.h"
#include "tool/i18n.h"

// The Disco Grenade
class DiscoGrenade : public WeaponProjectile
{
private:
  bool have_played_music;
  ParticleEngine smoke_engine;
  SoundSample disco_sound;
public:
  DiscoGrenade(ExplosiveWeaponConfig& cfg,
	       WeaponLauncher * p_launcher);
  void Refresh();
  void Shoot(double strength);
protected:
  void Explosion();
  void SignalOutOfMap();
  void SignalDrowning();
};


DiscoGrenade::DiscoGrenade(ExplosiveWeaponConfig& cfg,
                           WeaponLauncher * p_launcher) :
  WeaponProjectile ("disco_grenade", cfg, p_launcher),
  smoke_engine(40)
{
  m_rebound_sound = "weapon/disco_grenade_bounce";
  have_played_music = false;
  explode_with_collision = false;
}

void DiscoGrenade::Shoot(double strength)
{   
  // Sound must be launched before WeaponProjectile::Shoot
  // in case that the projectile leave the battlefield
  // during WeaponProjectile::Shoot (#bug 10241)
  disco_sound.Play("share","weapon/disco_grenade_music", -1);
  
  WeaponProjectile::Shoot(strength);
}

void DiscoGrenade::Explosion()
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
  disco_sound.Stop();
  
  WeaponProjectile::Explosion();
}

void DiscoGrenade::Refresh()
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
  // Ah ! Ah ! Ah ! Staying Alive, staying alive ...
  if (GetTotalTimeout() >= 2 && tmp > (1000 * GetTotalTimeout() - 2000) && !have_played_music) {
    jukebox.Play("share","weapon/alleluia") ;
    have_played_music = true;
  }
  image->SetRotation_rad(GetSpeedAngle());
}

void DiscoGrenade::SignalOutOfMap()
{
  GameMessages::GetInstance()->Add (_("The disco grenade has left the dance floor before exploding"));
  WeaponProjectile::SignalOutOfMap();  

  disco_sound.Stop();
}

void DiscoGrenade::SignalDrowning()
{
  WeaponProjectile::SignalDrowning();

  disco_sound.Stop();
}



//-----------------------------------------------------------------------------

DiscoGrenadeLauncher::DiscoGrenadeLauncher() :
  WeaponLauncher(WEAPON_DISCO_GRENADE, "disco_grenade", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Disco Grenade");
  m_category = THROW;
  m_allow_change_timeout = true;
  ignore_collision_signal = true;
  ReloadLauncher();
}

WeaponProjectile * DiscoGrenadeLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new DiscoGrenade(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}
std::string DiscoGrenadeLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u disco grenade! Shake your body when throwing it!",
            "%s team has won %u disco grenades! Shake your body when throwing them!",
            items_count), TeamName, items_count);
}


