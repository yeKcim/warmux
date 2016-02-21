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
 * Anvil : appear in top of an enemy and crush down his head
 *****************************************************************************/

#include "anvil.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "../game/time.h"
#include "../team/teams_list.h"
#include "../graphic/video.h"
#include "../tool/math_tools.h"
#include "../map/camera.h"
#include "../weapon/explosion.h"
#include "../interface/game_msg.h"
#include "../tool/i18n.h"
#include "../object/objects_list.h"
#include "../map/map.h"
//-----------------------------------------------------------------------------

Anvil::Anvil(ExplosiveWeaponConfig& cfg,
             WeaponLauncher * p_launcher) :
  WeaponProjectile ("anvil", cfg, p_launcher)
{
  explode_with_collision = false;
  explode_colliding_character = false;
  merge_time = 0;
}

void Anvil::SignalObjectCollision(PhysicalObj * obj)
{
  if(typeid(*obj) == typeid(Character)) {
    Character * tmp = (Character *)(obj);
    tmp -> SetEnergyDelta (-200);
  }
}

void Anvil::SignalGroundCollision()
{
  merge_time = Time::GetInstance()->Read() + 5000;
}

void Anvil::Refresh()
{
  if(merge_time != 0 && merge_time < Time::GetInstance()->Read()) {
    world.MergeSprite(GetPosition(),image);
    Ghost();
  } else {
    WeaponProjectile::Refresh();
  }
}

//-----------------------------------------------------------------------------

AnvilLauncher::AnvilLauncher() :
    WeaponLauncher(WEAPON_ANVIL, "anvil_launcher", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Anvil");
  mouse_character_selection = false;
  can_be_used_on_closed_map = false;
  ReloadLauncher();
  target_chosen = false;
}

void AnvilLauncher::ChooseTarget(Point2i mouse_pos)
{
  mouse_pos.y = 0;
  target = mouse_pos - (projectile->GetSize() / 2);
  target_chosen = true;
  Shoot();
}

bool AnvilLauncher::p_Shoot ()
{
  if(!target_chosen)
    return false;
  projectile->SetXY(target);
  lst_objects.AddObject(projectile);
  camera.FollowObject(projectile,true,true);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

WeaponProjectile * AnvilLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new Anvil(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}
