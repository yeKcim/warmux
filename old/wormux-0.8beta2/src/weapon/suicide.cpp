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
 * Suicide.
 *****************************************************************************/

#include "suicide.h"
#include "explosion.h"
#include "weapon_cfg.h"

#include <iostream>
#include "character/character.h"
#include "character/body.h"
#include "game/game_loop.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "include/action_handler.h"

Suicide::Suicide() : Weapon(WEAPON_SUICIDE, "suicide", new ExplosiveWeaponConfig())
{
  m_name = _("Commit Suicide");
  m_category = DUEL;
}

bool Suicide::p_Shoot()
{
  suicide_sound.Play ("share", "weapon/suicide");
  return true;
}

void Suicide::Refresh()
{
  if(m_last_fire_time > 0 && !suicide_sound.IsPlaying() && !ActiveCharacter().IsDead()) {
    ActiveCharacter().DisableDeathExplosion();
    ActiveCharacter().body->MakeParticles(ActiveCharacter().GetPosition());
    ActiveCharacter().SetEnergy(0); // Die!
    SendActiveCharacterInfo();
    ApplyExplosion(ActiveCharacter().GetCenter(),cfg());
  }
}

bool Suicide::IsInUse() const
{
  return m_last_fire_time > 0 && !ActiveCharacter().IsDead();
}

ExplosiveWeaponConfig& Suicide::cfg()
{
  return static_cast<ExplosiveWeaponConfig&>(*extra_params);
}

std::string Suicide::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext(
            "%s team has won %u suicide!",
            "%s team has won %u suicides! Use them all for an extra bonus!",
            items_count), TeamName, items_count);
}

