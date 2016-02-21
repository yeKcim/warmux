/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#include "weapon/suicide.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include <iostream>
#include "character/character.h"
#include "character/body.h"
#include "team/teams_list.h"
#include "game/time.h"
#include "include/action_handler.h"

const uint SUICIDE_SOUND_DURATION_IN_MS = 3600;

Suicide::Suicide() : Weapon(WEAPON_SUICIDE, "suicide", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();
  // The m_time_between_each_shot gets used here
  // to prevent that the weapon gets deselected before the character explodes.
  m_time_between_each_shot = SUICIDE_SOUND_DURATION_IN_MS;
  m_category = DUEL;
}

void Suicide::UpdateTranslationStrings()
{
  m_name = _("Commit Suicide");
  /* TODO: FILL IT */
  /* m_help = _(""); */
}

bool Suicide::p_Shoot()
{
  suicide_sound.Play ("default", "weapon/suicide");
  return true;
}

void Suicide::Refresh()
{
  // The suicide sound may play at different speed for different players,
  // that's why the explosion should not depend on the fact if the sound has finished playing or not.
  uint time_since_last_fire = Time::GetInstance()->Read() - m_last_fire_time;
  if (m_last_fire_time > 0 && time_since_last_fire > SUICIDE_SOUND_DURATION_IN_MS && !ActiveCharacter().IsDead()) {
    ActiveCharacter().DisableDeathExplosion();
    ActiveCharacter().body->MakeParticles(ActiveCharacter().GetPosition());
    ActiveCharacter().SetEnergy(0); // Die!
    ApplyExplosion(ActiveCharacter().GetCenter(),cfg());
  }
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

