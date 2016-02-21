/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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

#include "weapon/weapon_cfg.h"
#include "weapon/anvil.h"
#include "weapon/explosion.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include "character/character.h"
#include "game/time.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "interface/game_msg.h"
#include "interface/mouse.h"
#include "map/camera.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
//-----------------------------------------------------------------------------

class Anvil : public WeaponProjectile
{
  private:
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
    virtual void SignalObjectCollision(PhysicalObj * obj);
    virtual void SignalGroundCollision(const Point2d& /* speed_before */);
    virtual void SignalOutOfMap();
};

Anvil::Anvil(ExplosiveWeaponConfig& cfg,
             WeaponLauncher * p_launcher) :
  WeaponProjectile ("anvil", cfg, p_launcher)
{
  explode_with_collision = false;
  explode_colliding_character = false;
  merge_time = 0;
  SetTestRect(0, 0, 0, 0);
}

Anvil::~Anvil()
{
  falling_sound.Stop(); // paranoiac sound stop
}

void Anvil::SignalObjectCollision(PhysicalObj * obj)
{
  merge_time = Time::GetInstance()->Read() + 5000;
  obj->SetEnergyDelta(-200);
  PlayCollisionSound();
}

void Anvil::SignalGroundCollision(const Point2d& /* speed_before */)
{
  merge_time = Time::GetInstance()->Read() + 5000;
  PlayCollisionSound();
}

void Anvil::SignalOutOfMap()
{
  falling_sound.Stop();
}

void Anvil::Refresh()
{
  if(merge_time != 0 && merge_time < Time::GetInstance()->Read()) {
    world.MergeSprite(GetPosition(), image);
    Ghost();
  } else {
    WeaponProjectile::Refresh();
  }
}

void Anvil::PlayFallSound()
{
  falling_sound.Play("share", "weapon/anvil_fall", -1);
}

void Anvil::PlayCollisionSound()
{
  falling_sound.Stop();
  JukeBox::GetInstance()->Play("share", "weapon/anvil_collision");
}

//-----------------------------------------------------------------------------

AnvilLauncher::AnvilLauncher() :
    WeaponLauncher(WEAPON_ANVIL, "anvil_launcher", new ExplosiveWeaponConfig(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  UpdateTranslationStrings();

  m_category = DUEL;
  mouse_character_selection = false;
  can_be_used_on_closed_map = false;
  ReloadLauncher();
  target_chosen = false;
}

void AnvilLauncher::UpdateTranslationStrings()
{
  m_name = _("Anvil");
  m_help = _("Howto use it : left click on target\nan ammo per turn");
}

void AnvilLauncher::ChooseTarget(Point2i mouse_pos)
{
  target.x = mouse_pos.x - (projectile->GetWidth() / 2);
  target.y = 0 - projectile->GetHeight();

  if (!world.ParanoiacRectIsInVacuum(Rectanglei(target, projectile->GetSize())) ||
     !projectile->IsInVacuumXY(target))
    return;

  target_chosen = true;
  Shoot();
}

bool AnvilLauncher::p_Shoot ()
{
  if (!target_chosen)
    return false;

  projectile->SetXY(target);
  ((Anvil*)projectile)->PlayFallSound();
  lst_objects.AddObject(projectile);
  Camera::GetInstance()->FollowObject(projectile, true);
  projectile = NULL;
  ReloadLauncher();

  // Go back to default cursor
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  target_chosen = false; // ensure next shoot cannot be done pressing key space
  return true;
}

void AnvilLauncher::p_Select()
{
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_FIRE_LEFT);
}

void AnvilLauncher::p_Deselect()
{
  // Go back to default cursor
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);
}

WeaponProjectile * AnvilLauncher::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new Anvil(cfg(),dynamic_cast<WeaponLauncher *>(this)));
}

std::string AnvilLauncher::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u anvil! Splat them all!",
            "%s team has won %u anvils! Splat them all!",
            items_count), TeamName, items_count);
}


