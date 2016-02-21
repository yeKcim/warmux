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
 * Téléportation : déplacement d'un ver n'importe où sur le terrain.
 *****************************************************************************/

#include "teleportation.h"
//-----------------------------------------------------------------------------
#include "../interface/mouse.h"
#include "../include/action_handler.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../game/game_loop.h"
#include "../game/game_mode.h"
#include "../map/map.h"
#include "../game/time.h"
#include "../graphic/effects.h"
//-----------------------------------------------------------------------------
namespace Wormux {
Teleportation teleportation;
//-----------------------------------------------------------------------------

double ZOOM_MAX = 10; // zoom maximum durant le petit effet graphique
uint ESPACE = 4;

//-----------------------------------------------------------------------------

Teleportation::Teleportation() : Weapon(WEAPON_TELEPORTATION, "teleportation")
{  
  m_name = _("Teleportation");

  m_visibility = VISIBLE_ONLY_WHEN_INACTIVE;
  extra_params = new WeaponConfig();  
}

//-----------------------------------------------------------------------------

bool Teleportation::p_Shoot ()
{
  // Vérifie qu'on se téléporte dans le vide !
  dst = mouse.GetPosMonde();
  dst.x -= ActiveCharacter().GetWidth()/2;
  dst.y -= ActiveCharacter().GetHeight()/2;
  if (ActiveCharacter().IsOutsideWorldXY(dst.x, dst.y)) return false;
  if (!ActiveCharacter().IsInVacuumXY (dst.x, dst.y)) return false; 

  game_loop.interaction_enabled = false;

  jukebox.Play("share", "weapon/teleport_start");
  
  // Initialise les variables
  temps = Wormux::global_time.Read();
  retour = false;
  m_direction = ActiveCharacter().GetDirection();

  // Compute skins animation
  SDL_Surface* current_skin=NULL;
  if(m_direction == 1)
    current_skin = ActiveCharacter().image->GetCurrentFrameObject().surface;
  else
    current_skin = ActiveCharacter().image->GetCurrentFrameObject().flipped_surface;

  ActiveCharacter().Hide();
  skin = WaveSurface(current_skin, 100, game_mode.duration_move_player * 1000, 5.0, 1.5);
  return true;
}

//-----------------------------------------------------------------------------

void Teleportation::Refresh()
{
  if (!m_is_active) return;

  double dt = Wormux::global_time.Read() - temps;

  // On a fait le chemin retour ?
  if (retour) {
    // Oui, c'est la fin de la téléportation
    m_is_active = false;
    ActiveCharacter().image->Scale (m_direction, 1);
    ActiveCharacter().SetSpeed(0.0,0.0);
    ActiveCharacter().Show();
    jukebox.Play("share","weapon/teleport_end");
    game_loop.interaction_enabled = true;
    delete skin;
    return;
  }

  // Fin du chronometre ?
  if (game_mode.duration_move_player * 1000 < dt)
  {
    // Non, on fait le chemin retour en 
    // commençant par déplacer le ver
    retour = true;
    ActiveCharacter().SetXY (dst.x, dst.y);
    temps = Wormux::global_time.Read();
    dt = 0.0;
    return;
  }

  if (game_mode.duration_move_player * 1000 / 2 < dt)
  {
    m_x = dst.x;
    m_y = dst.y - skin->GetHeight()/2;
    return;
  }

  uint larg=ActiveCharacter().GetWidth();

  m_x = ActiveCharacter().GetX() - (skin->GetWidth()-larg)/2;
  m_y = ActiveCharacter().GetY();

//  if(ActiveCharacter().GetDirection() == -1)
//    m_x += nv_larg;
}

//-----------------------------------------------------------------------------

void Teleportation::Draw()
{
  if (m_is_active) {
    skin->Update();
    skin->Draw(m_x, m_y);
  } else {
    Weapon::Draw();
  }
}

//-----------------------------------------------------------------------------

void Teleportation::ChooseTarget()
{
  ActiveTeam().GetWeapon().NewActionShoot();
}

//-----------------------------------------------------------------------------

WeaponConfig& Teleportation::cfg()
{ return static_cast<WeaponConfig&>(*extra_params); }

//-----------------------------------------------------------------------------
} // namespace Wormux
