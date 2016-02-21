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
 * Arme gun : la balle part tout droit dans la direction donnée par
 * le viseur. Si la balle ne touche pas un ver, elle va faire un trou dans
 * le terrain. La balle peut également toucher les objets du plateau du jeu.
 *****************************************************************************/

#include "../weapon/gun.h"
#include <sstream>
#include "../map/map.h"
#include "../game/time.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../interface/game_msg.h"
#include "../interface/game_msg.h"
#include "../weapon/gun.h"
#include "../weapon/weapon_tools.h"

const uint BULLET_SPEED = 20;

GunBullet::GunBullet(ExplosiveWeaponConfig& cfg) :
  WeaponBullet("gun_bullet", cfg)
{
  cfg.explosion_range = 5;
}

void GunBullet::ShootSound()
{
  jukebox.Play("share","weapon/gun");
}

//-----------------------------------------------------------------------------

Gun::Gun() : WeaponLauncher(WEAPON_GUN, "gun", new ExplosiveWeaponConfig())
{
  m_name = _("Gun");

  projectile = new GunBullet(cfg());
}

bool Gun::p_Shoot ()
{  
  if (m_is_active)
    return false;  

  m_is_active = true;
  projectile->Shoot (20);

  return true;
}



