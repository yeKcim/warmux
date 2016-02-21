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
 * Arme dynamite : lorqu'on "tire", un baton de dynamite est lâché. Celui
 * explos après un laps de temps. La baton fait alors un gros trou dans la
 * carte, souffle les vers qui sont autour en leur faisant perdre de l'énergie.
 *****************************************************************************/

#include "dynamite.h"
#include "weapon_tools.h"
#include "../game/config.h"
#include "../include/app.h"
#include "../object/objects_list.h"
#include "../team/teams_list.h"
#include "../tool/i18n.h"
#include "../tool/resource_manager.h"

#ifdef __MINGW32__
#undef LoadImage
#endif

BatonDynamite::BatonDynamite(ExplosiveWeaponConfig& cfg) :
  WeaponProjectile("dynamite_bullet", cfg)
{
  channel = -1;

  image->animation.SetLoopMode(false);
  SetSize(image->GetSize());

  SetTestRect (0, 0, 2, 3);
}

void BatonDynamite::Reset()
{
  Ready();
  is_active = false;

  unsigned int delay = (1000 * cfg.timeout)/image->GetFrameCount();
  image->SetFrameSpeed(delay);

  image->Scale(ActiveCharacter().GetDirection(), 1);
  image->SetCurrentFrame(0);
  image->Start(); 
}

void BatonDynamite::Refresh()
{
  if (!is_active) return;
  assert (!IsGhost());
  image->Update(); 
  is_active = !image->IsFinished();
}

void BatonDynamite::Draw()
{
  if (!is_active) return;
  assert (!IsGhost());
  image->Draw(GetPosition());
}

void BatonDynamite::ShootSound()
{
  channel = jukebox.Play("share","weapon/dynamite_fuze", -1);
}

void BatonDynamite::Explosion()
{
  jukebox.Stop(channel);
  channel = -1;
  WeaponProjectile::Explosion();
}

void BatonDynamite::SignalCollision() 
{
  if (IsGhost()) is_active = false;
}

//-----------------------------------------------------------------------------

Dynamite::Dynamite() :
  WeaponLauncher(WEAPON_DYNAMITE, "dynamite", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Dynamite");
  
  projectile = new BatonDynamite(cfg());
}

void Dynamite::p_Select()
{
  dynamic_cast<BatonDynamite *>(projectile)->Reset();
}

// Pose une dynamite
bool Dynamite::p_Shoot ()
{
  Point2d speed_vector;

  dynamic_cast<BatonDynamite *>(projectile)->Reset();
  projectile->Shoot(0);

  // Ajoute la vitesse actuelle du ver
  ActiveCharacter().GetSpeedXY (speed_vector);
  projectile->SetSpeedXY (speed_vector);

  return true;
}
