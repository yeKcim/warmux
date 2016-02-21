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
 * Suicide.
 *****************************************************************************/

#include "../weapon/suicide.h"
//-----------------------------------------------------------------------------
#include "../tool/i18n.h"
#include "../team/teams_list.h"
#include "../game/game_loop.h"
#include "weapon_tools.h"
#include <iostream>
//-----------------------------------------------------------------------------
namespace Wormux 
{
Suicide suicide;

// Espace entre l'espace en l'image
const uint ESPACE = 5;

//-----------------------------------------------------------------------------

Suicide::Suicide() : Weapon(WEAPON_SUICIDE, "suicide")
{  
  m_name = _("Commit suicide");
  extra_params = new ExplosiveWeaponConfig();
  sound_channel = -1;
}

//-----------------------------------------------------------------------------

void Suicide::p_Select()
{
  is_dying = false;
}

//-----------------------------------------------------------------------------

void Suicide::p_Init()
{
#ifdef CL
  hole_image = CL_Surface("suicide_hole", &graphisme.weapons);
#else
  hole_image = resource_manager.LoadImage( weapons_res_profile, "suicide_hole"); 
#endif
}

//-----------------------------------------------------------------------------

bool Suicide::p_Shoot()
{ 
#ifdef CL
  jukebox.Play ("weapon/suicide", false, &son);
#else
  sound_channel = jukebox.Play ("share", "weapon/suicide");
#endif

  game_loop.interaction_enabled=false;
  is_dying = true;

  return true;
}

//-----------------------------------------------------------------------------

void Suicide::Refresh()
{
  if (!is_dying) return;

#ifdef CL
  m_is_active = son -> is_playing();
#else
  if ( sound_channel != -1 && Mix_Playing(sound_channel) ) {
    m_is_active = true;
  } else {
    m_is_active = false;
  }
#endif

  if (!m_is_active) {
    if ( !ActiveCharacter().IsDead() ) ActiveCharacter().Die();
  }
}

//-----------------------------------------------------------------------------

ExplosiveWeaponConfig& Suicide::cfg()
{ return static_cast<ExplosiveWeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
