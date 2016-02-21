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
 * Weapon gnu : a gnu jump in (more or less) random directions and explodes
 *****************************************************************************/

#include "gnu.h"
#include <sstream>
#include "weapon_tools.h"
#include "../game/config.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../sound/jukebox.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"
#include "../tool/random.h"

Gnu::Gnu(ExplosiveWeaponConfig& cfg) : 
  WeaponProjectile("gnu", cfg)
{
}

void Gnu::Shoot (double strength)
{
  WeaponProjectile::Shoot(strength);

  //Dummy value, we only need save_x!=x and save_y!=y
  //To avoid a comparison in Refresh()
  save_x=(double)GetX()-1.0;
  save_y=(double)GetY()-1.0;

  double angle = ActiveTeam().crosshair.GetAngleRad();

  if(angle<M_PI/2 && angle>-M_PI/2)
    m_sens = 1;
  else
    m_sens = -1;
}

void Gnu::Refresh()
{
  WeaponProjectile::Refresh();

  double angle, norm;
  GetSpeed(norm, angle);
  if((!IsMoving() || norm<1.0)&& !FootsInVacuum())
  {
    norm = randomObj.GetDouble(0.5, 1) * cfg.rebound_factor;
    //angle = -(M_PI/2)-(m_sens*(RandomLong(0,90)*M_PI/45.0)); ??
	angle = randomObj.GetDouble(-M_PI_2, M_PI_2);
    SetSpeed(norm, angle);
  }

  angle *= 180.0/M_PI;

  if(angle<-90 || angle>90)
  {
    m_sens=-1;
    angle=-angle + 180;
  }
  else
    m_sens=1;

  //Due to a bug in the physic engine
  //sometimes, angle==infinite (according to gdb) ??
  if(angle>720)
    angle = 0;

  image->SetRotation_deg(angle);
  image->Update();
  // Fixe le rectangle de test  ??
  SetTestRect ( image->GetWidth()/2-1,
                image->GetWidth()/2-1,
                image->GetHeight()/2-1,
                image->GetHeight()/2-1);
}

void Gnu::SignalCollision()
{   
  if (IsGhost())
  {
    GameMessages::GetInstance()->Add ("The Gnu left the battlefield before exploding");
    is_active = false ;
  }
}
//-----------------------------------------------------------------------------

GnuLauncher::GnuLauncher() : 
  WeaponLauncher(WEAPON_GNU, "gnulauncher", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("GnuLauncher");

  projectile = new Gnu(cfg());
}

