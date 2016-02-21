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
 * Parachute !
 *****************************************************************************/

#include "weapon/parachute.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/physical_obj.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

class ParachuteConfig : public WeaponConfig
{
public:
  Double wind_factor;
  Double air_resist_factor;
  Double force_side_displacement;
  ParachuteConfig();
};

ParachuteConfig::ParachuteConfig()
{
  push_back(new DoubleConfigElement("wind_factor", &wind_factor, 10));
  push_back(new DoubleConfigElement("air_resist_factor", &air_resist_factor, 140));
  push_back(new DoubleConfigElement("force_side_displacement", &wind_factor, 2000));
}

//------------------------------------------------------------------------------

Parachute::Parachute() : Weapon(WEAPON_PARACHUTE, "parachute", new ParachuteConfig(), false)
{
  UpdateTranslationStrings();

  m_category = MOVE;
  m_initial_nb_ammo = 2;
  use_unit_on_first_shoot = false;
  m_used_this_turn = false;
  img = GetResourceManager().LoadSprite(weapons_res_profile, "parachute_sprite");
}

void Parachute::UpdateTranslationStrings()
{
  m_name = _("Parachute");
  m_help = _("Parachute avoids hitting the ground too hard\nOpens automatically\nUse left/right to change direction");
}

Parachute::~Parachute()
{
  if (img)
    delete img;
}

void Parachute::p_Select()
{
  open = false;
  closing = false;
  img->animation.SetShowOnFinish(SpriteAnimation::show_last_frame);
}

void Parachute::p_Deselect()
{
  Character &a = ActiveCharacter();
  // Fix for excessive damage being applied, as done for jetpack
  a.SetExternForceXY(Point2d());
  a.ResetConstants();
  a.SetMovement("breathe");
}

bool Parachute::p_Shoot()
{
  Weapon::Message(_("The parachute is activated automatically."));
  return false;
}

void Parachute::Draw()
{
  if(open) {
    img->Update();
    Point2i position;
    ActiveCharacter().GetHandPosition(position);
    position.x -= img->GetWidth()/2;
    position.y -= img->GetHeight();
    img->Draw(position);
  }
}

void Parachute::Refresh()
{
  if (Game::GetInstance()->GetRemainingTime() == 0)
    return;
  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return;

  Character& active = ActiveCharacter();
  Double speed = active.GetSpeedXY().Norm();

  if (active.FootsInVacuum() && speed.IsNotZero()) { // We are falling
    if (!open && (speed > GameMode::GetInstance()->safe_fall)) { // with a sufficient speed
      if (EnoughAmmo() && !m_used_this_turn) { // We have enough ammo => start opening the parachute
        if (!m_used_this_turn) {
          UseAmmo();
          m_used_this_turn = true;
        }

        active.SetAirResistFactor(cfg().air_resist_factor);
        active.SetWindFactor(cfg().wind_factor);
        open = true;
        img->animation.SetPlayBackward(false);
        img->Start();
        active.SetSpeedXY(Point2d(0,0));
        active.SetMovement("parachute");
        Camera::GetInstance()->FollowObject(&active);
      }
    }
  } else { // We are on the ground
    if (open) { // The parachute is opened
      active.SetMovement("walk");
      if (!closing) { // We have just hit the ground. Start closing animation
        img->animation.SetPlayBackward(true);
        img->animation.SetShowOnFinish(SpriteAnimation::show_blank);
        img->Start();
        closing = true;
        return;
      } else { // The parachute is closing
        if (img->IsFinished()) {
          // The animation is finished... We are done with the parachute
          open = false;
          closing = false;
          UseAmmoUnit();
        }
      }
    }
    m_used_this_turn = false;
  }
  if (open) {
    active.UpdateLastMovingTime();

    // If parachute is open => character can move a little to the left or to the right
    const LRMoveIntention * lr_move_intention = active.GetLastLRMoveIntention();
    if (lr_move_intention) {
      LRDirection direction = lr_move_intention->GetDirection();
      active.SetDirection(direction);
      if (direction == DIRECTION_LEFT)
        active.SetExternForce(-cfg().force_side_displacement, 0.0);
      else
        active.SetExternForce(cfg().force_side_displacement, 0.0);
    }
  }
}

std::string Parachute::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext("%s team has won %u parachute!",
                         "%s team has won %u parachutes!",
                         items_count), TeamName, items_count);
}

void Parachute::StartShooting()
{
  if (open) {
    img->Finish();
    open = false;
    closing = false;
    UseAmmoUnit();
  } else {
    Weapon::StartShooting();
  }
}

ParachuteConfig& Parachute::cfg()
{
  return static_cast<ParachuteConfig&>(*extra_params);
}
