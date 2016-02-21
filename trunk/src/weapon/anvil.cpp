/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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

#include <sstream>

#include "game/game_time.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "network/network.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/team.h"
#include "team/teams_list.h"
#include "weapon/weapon_cfg.h"
#include "weapon/anvil.h"
//#include "weapon/explosion.h"

//-----------------------------------------------------------------------------

class Anvil : public WeaponProjectile
{
  uint merge_time;
  SoundSample falling_sound;
public:
  Anvil(ExplosiveWeaponConfig& cfg,
        WeaponLauncher * p_launcher);
  ~Anvil();
  void Refresh();

  void PlayFallSound();
  void PlayCollisionSound();
  void SetEnergyDelta(int /*delta*/, bool /*do_report = true*/) { };
protected:
  virtual void SignalGroundCollision(const Point2d& /* speed_before */, const Double& /*contactAngle*/);
  virtual void SignalObjectCollision(const Point2d& /* my_speed_before */,
              PhysicalObj * obj,
              const Point2d& /* obj_speed_before */);
  virtual void SignalOutOfMap();
};

Anvil::Anvil(ExplosiveWeaponConfig& cfg,
             WeaponLauncher * p_launcher) :
  WeaponProjectile ("anvil", cfg, p_launcher)
{
  explode_colliding_character = false;
  merge_time = 0;
  explode_with_timeout = false;
  SetTestRect(0, 0, 0, 0);
}

Anvil::~Anvil()
{
  falling_sound.Stop(); // paranoiac sound stop
}

void Anvil::SignalObjectCollision(const Point2d& /* my_speed_before */,
                                  PhysicalObj * obj,
                                  const Point2d& /* obj_speed_before */)
{
  merge_time = GameTime::GetInstance()->Read() + 5000;
  obj->SetEnergyDelta(-200, &ActiveCharacter());
  PlayCollisionSound();

  WeaponProjectile::Collision();
}

void Anvil::SignalGroundCollision(const Point2d& /* speed_before */, const Double& /*contactAngle*/)
{
  merge_time = GameTime::GetInstance()->Read() + 5000;
  PlayCollisionSound();

  WeaponProjectile::Collision();
}

void Anvil::SignalOutOfMap()
{
  falling_sound.Stop();
}

void Anvil::Refresh()
{
  if (merge_time && merge_time < GameTime::GetInstance()->Read()) {
    GetWorld().MergeSprite(GetPosition(), image);
    Ghost();
  } else {
    WeaponProjectile::Refresh();
  }
}

void Anvil::PlayFallSound()
{
  falling_sound.Play("default", "weapon/anvil_fall", -1);
}

void Anvil::PlayCollisionSound()
{
  falling_sound.Stop();
  JukeBox::GetInstance()->Play("default", "weapon/anvil_collision");
}

//-----------------------------------------------------------------------------

AnvilLauncher::AnvilLauncher() :
    TargetLauncher(WEAPON_ANVIL, "anvil_launcher", new ExplosiveWeaponConfig())
{
  UpdateTranslationStrings();

  m_category = DUEL;
  mouse_character_selection = false;
  can_be_used_on_closed_map = false;
  ReloadLauncher();
}

void AnvilLauncher::UpdateTranslationStrings()
{
  m_name = _("Anvil");
  m_help = _("Usage: left click on the target\nAnvil kill all characters beneath.\nOne ammo per turn");
}

void AnvilLauncher::ChooseTarget(Point2i mouse_pos)
{
  Point2i tmp(mouse_pos.x - (projectile->GetWidth()>>1),
              // using 1 allows to detect when the ground goes at the very top of the sky
              // in that case, the current target is rejected (fix bug #12369)
              1 - projectile->GetHeight());

  if (!projectile->IsInVacuumXY(tmp) ||
      !GetWorld().ParanoiacRectIsInVacuum(Rectanglei(tmp, projectile->GetSize())))
    return;

  TargetLauncher::ChooseTarget(mouse_pos);
}

bool AnvilLauncher::p_Shoot()
{
  if (!m_target.selected)
    return false;

  Point2i tmp(m_target.pos.x, 1 - projectile->GetHeight());
  projectile->SetXY(tmp);
  ((Anvil*)projectile)->PlayFallSound();
  ObjectsList::GetRef().AddObject(projectile);
  Camera::GetInstance()->FollowObject(projectile);
  projectile = NULL;
  ReloadLauncher();

  // Go back to default cursor
  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  m_target.selected = false; // ensure next shoot cannot be done pressing key space
  return true;
}

WeaponProjectile * AnvilLauncher::GetProjectileInstance()
{
  return new Anvil(cfg(), this);
}

std::string AnvilLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext("%s team has won %u anvil! Splat them all!",
                         "%s team has won %u anvils! Splat them all!",
                         items_count), TeamName, items_count);
}
