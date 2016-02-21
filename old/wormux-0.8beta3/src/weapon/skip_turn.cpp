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
 * Skip his turn
 *****************************************************************************/

#include "weapon/skip_turn.h"
#include "weapon/weapon_cfg.h"

//-----------------------------------------------------------------------------
#include "character/character.h"
#include "game/time.h"
#include "interface/game_msg.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"
//-----------------------------------------------------------------------------

SkipTurn::SkipTurn() : Weapon(WEAPON_SKIP_TURN, "skip_turn", new WeaponConfig())
{
  m_name = _("Skip turn");
  m_category = TOOL;
  m_time_between_each_shot = 40;
}

//-----------------------------------------------------------------------------

bool SkipTurn::p_Shoot()
{
  // Show message
  GameMessages::GetInstance()->Add (Format(_("%s team has skipped its turn."),
                                           ActiveTeam().GetName().c_str()));
  jukebox.Play(ActiveTeam().GetSoundProfile(), "skip_turn");
  return true;
}

//-----------------------------------------------------------------------------

std::string SkipTurn::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u turn skip! I know you'll love it.",
            "%s team has won %u turn skips! I know you'll love them.",
            items_count), TeamName, items_count);
}

bool SkipTurn::IsInUse() const
{
  return m_last_fire_time + m_time_between_each_shot > Time::GetInstance()->Read();
}
