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
 * Holly Grenade
 *****************************************************************************/

#include "holly_grenade.h"
#include <sstream>
#include "weapon_tools.h"
#include "../game/time.h"
#include "../graphic/video.h"
#include "../interface/game_msg.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/math_tools.h"
#include "../tool/i18n.h"

HollyGrenade::HollyGrenade(ExplosiveWeaponConfig& cfg) :
  WeaponProjectile ("holly_grenade", cfg), 
  smoke_engine(40)
{
  m_rebound_sound = "weapon/holly_grenade_bounce";
  m_rebounding = true;
  touche_ver_objet = false;
  sing_alleluia = false;
}

void HollyGrenade::Refresh()
{
  WeaponProjectile::Refresh();

  smoke_engine.AddPeriodic(GetPosition(), particle_SMOKE, false);
  
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

void HollyGrenade::SignalCollision()
{   
  if (IsGhost())
  {
    GameMessages::GetInstance()->Add ("The grenade left the battlefield before exploding");
    is_active = false ;
  }
}

//-----------------------------------------------------------------------------

HollyGrenadeLauncher::HollyGrenadeLauncher() : 
  WeaponLauncher(WEAPON_HOLLY_GRENADE, "holly_grenade", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{  
  m_name = _("HollyGrenade");
  projectile = new HollyGrenade(cfg());
}

