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
 * Teleportation : move a charecter anywhere on the map
 *****************************************************************************/

#include "teleportation.h"
#include "character/body.h"
#include "game/game_loop.h"
#include "game/game_mode.h"
#include "game/time.h"
#include "graphic/effects.h"
#include "include/action_handler.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "particles/teleport_member.h"
#include "team/teams_list.h"
#include "tool/i18n.h"

Teleportation::Teleportation() : Weapon(WEAPON_TELEPORTATION, "teleportation",
					new WeaponConfig(),
					VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Teleportation");
  m_category = MOVE;
  target_chosen = false;
}

bool Teleportation::p_Shoot ()
{
  if(!target_chosen)
    return false;

  // Check we are not going outside of the world !
  if( ActiveCharacter().IsOutsideWorldXY(dst) )
    return false;

  Rectanglei rect = ActiveCharacter().GetTestRect();
  rect.SetPosition(dst);

  // Go back to default cursor
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  //  GameLoop::GetInstance()->interaction_enabled = false;

  jukebox.Play("share", "weapon/teleport_start");

  time = Time::GetInstance()->Read();
  ActiveCharacter().Hide();
  ActiveCharacter().body->MakeTeleportParticles(ActiveCharacter().GetPosition(), dst);

  target_chosen = false; // ensure next teleportation cannot be done pressing key space
  return true;
}

void Teleportation::Refresh()
{
  if (!m_is_active) return;

  double dt = Time::GetInstance()->Read() - time;

  if(dt > teleportation_anim_duration)
  {
    camera.SetXYabs(dst - camera.GetSize()/2);
    ActiveCharacter().SetXY(dst);
    m_is_active = false;
    ActiveCharacter().SetSpeed(0.0,0.0);
    ActiveCharacter().Show();
    jukebox.Play("share","weapon/teleport_end");
    //    GameLoop::GetInstance()->interaction_enabled = true;
    return;
  }
}

void Teleportation::Draw()
{
  if (!m_is_active)
    Weapon::Draw();
}

void Teleportation::p_Select()
{
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_FIRE_LEFT);
}

void Teleportation::p_Deselect()
{
  // Go back to default cursor
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
}

void Teleportation::ChooseTarget(Point2i mouse_pos)
{
  dst = mouse_pos - ActiveCharacter().GetSize()/2;
  if(!world.ParanoiacRectIsInVacuum(Rectanglei(dst,ActiveCharacter().GetSize())) ||
     !ActiveCharacter().IsInVacuumXY(dst))
    return;
  target_chosen = true;
  Shoot();
}

std::string Teleportation::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u teleportation!",
            "%s team has won %u teleportations!",
            items_count), TeamName, items_count);
}

WeaponConfig& Teleportation::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

