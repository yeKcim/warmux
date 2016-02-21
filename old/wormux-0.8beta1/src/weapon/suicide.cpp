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
#include <iostream>
#include "explosion.h"
#include "character/body.h"
#include "game/game_loop.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "include/action_handler.h"

Suicide::Suicide() : Weapon(WEAPON_SUICIDE, "suicide", new ExplosiveWeaponConfig())
{
  m_name = _("Commit Suicide");
  m_category = DUEL;
  sound_channel = -1;
}

void Suicide::p_Select()
{
  is_dying = false;
}

bool Suicide::p_Shoot()
{
  sound_channel = jukebox.Play ("share", "weapon/suicide");

  // GameLoop::GetInstance()->interaction_enabled=false;
  is_dying = true;

  return true;
}

void Suicide::Refresh()
{
  if (!is_dying) return;

  m_is_active = sound_channel != -1 && Mix_Playing(sound_channel);

  if(!m_is_active && !ActiveCharacter().IsDead())
  {
    ActiveCharacter().DisableDeathExplosion();
    ActiveCharacter().body->MakeParticles(ActiveCharacter().GetPosition());
    ActiveCharacter().SetEnergy(0); // Die!
    SendActiveCharacterInfo();
    ApplyExplosion(ActiveCharacter().GetCenter(),cfg());
  }
}

ExplosiveWeaponConfig& Suicide::cfg()
{ return static_cast<ExplosiveWeaponConfig&>(*extra_params); }

std::string Suicide::GetWeaponWinString(const char *TeamName, uint items_count )
{
  return Format(ngettext(
            "%s team has won %u suicide!",
            "%s team has won %u suicides!",
            items_count), TeamName, items_count);
}

