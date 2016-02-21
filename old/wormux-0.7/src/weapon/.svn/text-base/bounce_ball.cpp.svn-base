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
 * Bounce ball:
 * bounce since it has not collide a character
 *****************************************************************************/

#include "bounce_ball.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "../tool/debug.h"
#include "../game/time.h"
#include "../team/teams_list.h"
#include "../graphic/video.h"
#include "../tool/math_tools.h"
#include "../map/camera.h"
#include "../weapon/weapon_tools.h"
#include "../interface/game_msg.h"
#include "../tool/i18n.h"
#include "../object/objects_list.h"
//-----------------------------------------------------------------------------

BounceBall::BounceBall(ExplosiveWeaponConfig& cfg) :
  WeaponProjectile ("bounce_ball", cfg)
{
  m_rebound_sound = "weapon/grenade_bounce";
  m_rebounding = true;
  touche_ver_objet = true;
  explode_colliding_character = true;
}

//-----------------------------------------------------------------------------

void BounceBall::Refresh()
{
  WeaponProjectile::Refresh();

  // rotation de l'image de la grenade...
  double angle = GetSpeedAngle() * 180/M_PI ;
  image->SetRotation_deg( angle);
}


//-----------------------------------------------------------------------------

void BounceBall::SignalCollision()
{   
  MSG_DEBUG("bounce_ball", "rebound factor :%f", m_rebound_factor);

  if (IsGhost())
  {
    GameMessages::GetInstance()->Add ("The ball left the battlefield before exploding");
    is_active = false ;
  }
}

//-----------------------------------------------------------------------------

BounceBallLauncher::BounceBallLauncher() : 
  WeaponLauncher(WEAPON_BOUNCE_BALL, "bounce_ball", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{  
  m_name = _("BounceBall");
  projectile = new BounceBall(cfg());
}
