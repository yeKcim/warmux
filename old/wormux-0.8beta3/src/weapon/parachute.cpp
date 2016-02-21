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
 * Parachute !
 *****************************************************************************/

#include "weapon/parachute.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "graphic/sprite.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "object/physical_obj.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

class ParachuteConfig : public WeaponConfig
{
  public:
     double wind_factor;
     double air_resist_factor;
     double force_side_displacement;
     ParachuteConfig();
     void LoadXml(xmlpp::Element *elem);
};


Parachute::Parachute() : Weapon(WEAPON_PARACHUTE, "parachute", new ParachuteConfig(), NEVER_VISIBLE)
{
  m_name = _("Parachute");
  m_category = MOVE;
  m_initial_nb_ammo = 2;
  m_x_extern = 0.0;
  use_unit_on_first_shoot = false;

  image = resource_manager.LoadSprite(weapons_res_profile, "parachute_sprite");
}

void Parachute::p_Select()
{
  open = false;
  closing = false;
  image->animation.SetShowOnFinish(SpriteAnimation::show_last_frame);
}

void Parachute::p_Deselect()
{
  ActiveCharacter().ResetConstants();
}

bool Parachute::IsInUse() const
{
  return Game::GetInstance()->GetRemainingTime() > 0 &&
         Game::GetInstance()->ReadState() == Game::PLAYING;
}

bool Parachute::p_Shoot()
{
  GameMessages::GetInstance()->Add(_("Parachute is activated automatically."));
  return false;
}

void Parachute::Draw()
{
  if(open) {
    image->Update();
    image->Draw(ActiveCharacter().GetHandPosition() - Point2i(image->GetWidth()/2,image->GetHeight()));
  }
}

void Parachute::Refresh()
{
  double speed;
  double angle;

  ActiveCharacter().GetSpeed(speed, angle);

  if(ActiveCharacter().FootsInVacuum() && speed != 0.0) { // We are falling
    if(!open && (speed > GameMode::GetInstance()->safe_fall)) { // with a sufficient speed
      if(EnoughAmmo()) { // We have enough ammo => start opening the parachute
        UseAmmo();
        ActiveCharacter().SetAirResistFactor(cfg().air_resist_factor);
        ActiveCharacter().SetWindFactor(cfg().wind_factor);
        open = true;
        image->animation.SetPlayBackward(false);
        image->Start();
        ActiveCharacter().SetSpeedXY(Point2d(0,0));
        ActiveCharacter().SetMovement("parachute");
        Camera::GetInstance()->FollowObject(&ActiveCharacter(), true);
      }
    }
  } else { // We are on the ground
    ActiveCharacter().SetMovement("walk");
    if(open) { // The parachute is opened
      if (!closing) { // We have just hit the ground. Start closing animation
        image->animation.SetPlayBackward(true);
        image->animation.SetShowOnFinish(SpriteAnimation::show_blank);
        image->Start();
        closing = true;
        return;
      } else { // The parachute is closing
        if(image->IsFinished()) {
          // The animation is finished... We are done with the parachute
          open = false;
          closing = false;
          UseAmmoUnit();
        }
      }
    }
  }
  // If parachute is open => character can move a little to the left or to the right
  if(open)
    ActiveCharacter().SetExternForce(m_x_extern, 0.0);
}

std::string Parachute::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u parachute!",
            "%s team has won %u parachutes!",
            items_count), TeamName, items_count);
}

void Parachute::HandleKeyPressed_Shoot(bool shift)
{
  if(open) {
    image->Finish();
    open = false;
    closing = false;
    UseAmmoUnit();
  } else {
    Weapon::HandleKeyPressed_Shoot(shift);
  }
}

void Parachute::HandleKeyPressed_MoveRight(bool shift)
{
  if (closing) {
    ActiveCharacter().BeginMovementRL(0);
  }

  if(open) {
    ActiveCharacter().SetDirection(DIRECTION_RIGHT);
    m_x_extern = cfg().force_side_displacement;
  } else {
    Weapon::HandleKeyPressed_MoveRight(shift);
  }
}

void Parachute::HandleKeyReleased_MoveRight(bool shift)
{
  if(open)
    m_x_extern = 0.0;
  else
    Weapon::HandleKeyReleased_MoveRight(shift);
}

void Parachute::HandleKeyPressed_MoveLeft(bool shift)
{
  if (closing) {
    ActiveCharacter().BeginMovementRL(0);
  }

  if(open) {
    ActiveCharacter().SetDirection(DIRECTION_LEFT);
    m_x_extern = -cfg().force_side_displacement;
  } else {
    Weapon::HandleKeyPressed_MoveLeft(shift);
  }
}

void Parachute::HandleKeyReleased_MoveLeft(bool shift)
{
  if(open)
    m_x_extern = 0.0;
  else
    Weapon::HandleKeyReleased_MoveLeft(shift);
}

ParachuteConfig& Parachute::cfg() {
  return static_cast<ParachuteConfig&>(*extra_params);
}

ParachuteConfig::ParachuteConfig(){
  wind_factor = 10.0;
  air_resist_factor = 140.0;
  force_side_displacement = 2000.0;
}

void ParachuteConfig::LoadXml(xmlpp::Element *elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadDouble(elem, "wind_factor", wind_factor);
  XmlReader::ReadDouble(elem, "air_resist_factor", air_resist_factor);
  XmlReader::ReadDouble(elem, "force_side_displacement", force_side_displacement);
}
