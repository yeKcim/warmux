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
 * Skip his turn
 *****************************************************************************/

#include "../weapon/skip_turn.h"
//-----------------------------------------------------------------------------
#include "../tool/i18n.h"
#include "../team/teams_list.h"
#include "../game/game_loop.h"
#include "../interface/game_msg.h"
//-----------------------------------------------------------------------------
namespace Wormux 
{

  SkipTurn skipturn;

// Espace entre l'espace en l'image
const uint ESPACE = 5;

//-----------------------------------------------------------------------------

SkipTurn::SkipTurn() : Weapon(WEAPON_SKIP_TURN, "skip_turn")
{
  m_name = _("Skip turn");
}

//-----------------------------------------------------------------------------

bool SkipTurn::p_Shoot()
{ 
  
  // Show message
  game_messages.Add (Format(_("%s team has skipped its turn."), 
			      ActiveTeam().GetName().c_str()));

#ifdef CL
  jukebox.PlayProfile(ActiveTeam().GetSoundProfile(), "skip_turn");
#else
  jukebox.Play(ActiveTeam().GetSoundProfile(), "skip_turn");
#endif

  // End turn
  m_is_active = false;
  return true;
}

//-----------------------------------------------------------------------------

void SkipTurn::Refresh()
{
  if (m_is_active)
    m_is_active = false;
}

//-----------------------------------------------------------------------------
} // namespace Wormux
