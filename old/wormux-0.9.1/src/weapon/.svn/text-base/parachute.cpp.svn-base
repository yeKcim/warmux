/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
     double wind_factor;
     double air_resist_factor;
     double force_side_displacement;
     ParachuteConfig();
     void LoadXml(const xmlNode* elem);
};


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
  /* TODO: FILL IT */
  /* m_help = _(""); */
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
  ActiveCharacter().ResetConstants();
  ActiveCharacter().SetMovement("breathe");
}

bool Parachute::p_Shoot()
{
  m_used_this_turn = false;
  GameMessages::GetInstance()->Add(_("The parachute is activated automatically."));
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
  if (Game::GetInstance()->GetRemainingTime() <= 0)
    return;
  if (Game::GetInstance()->ReadState() != Game::PLAYING)
    return;

  double speed;
  double angle;

  ActiveCharacter().GetSpeed(speed, angle);

  if(ActiveCharacter().FootsInVacuum() && speed != 0.0) { // We are falling
    if(!open && (speed > GameMode::GetInstance()->safe_fall)) { // with a sufficient speed
      if(EnoughAmmo()) { // We have enough ammo => start opening the parachute
        if(!m_used_this_turn)
        {
          UseAmmo();
          m_used_this_turn = true;
        }
        
        ActiveCharacter().SetAirResistFactor(cfg().air_resist_factor);
        ActiveCharacter().SetWindFactor(cfg().wind_factor);
        open = true;
        img->animation.SetPlayBackward(false);
        img->Start();
        ActiveCharacter().SetSpeedXY(Point2d(0,0));
        ActiveCharacter().SetMovement("parachute");
        Camera::GetInstance()->FollowObject(&ActiveCharacter());
      }
    }
  } else { // We are on the ground
    if (open) { // The parachute is opened
      ActiveCharacter().SetMovement("walk");
      if (!closing) { // We have just hit the ground. Start closing animation
        img->animation.SetPlayBackward(true);
        img->animation.SetShowOnFinish(SpriteAnimation::show_blank);
        img->Start();
        closing = true;
        return;
      } else { // The parachute is closing
        if(img->IsFinished()) {
          // The animation is finished... We are done with the parachute
          open = false;
          closing = false;
          UseAmmoUnit();
        }
      }
    }
  }
  if (open) {
    ActiveCharacter().UpdateLastMovingTime();

    // If parachute is open => character can move a little to the left or to the right
    const LRMoveIntention * lr_move_intention = ActiveCharacter().GetLastLRMoveIntention();
    if (lr_move_intention) {
      LRDirection direction = lr_move_intention->GetDirection();
      ActiveCharacter().SetDirection(direction);
      if (direction == DIRECTION_LEFT)
        ActiveCharacter().SetExternForce(-cfg().force_side_displacement, 0.0);
      else
        ActiveCharacter().SetExternForce(cfg().force_side_displacement, 0.0);
    }
  }
}

std::string Parachute::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u parachute!",
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

ParachuteConfig& Parachute::cfg() {
  return static_cast<ParachuteConfig&>(*extra_params);
}

ParachuteConfig::ParachuteConfig(){
  wind_factor = 10.0;
  air_resist_factor = 140.0;
  force_side_displacement = 2000.0;
}

void ParachuteConfig::LoadXml(const xmlNode* elem){
  WeaponConfig::LoadXml(elem);
  XmlReader::ReadDouble(elem, "wind_factor", wind_factor);
  XmlReader::ReadDouble(elem, "air_resist_factor", air_resist_factor);
  XmlReader::ReadDouble(elem, "force_side_displacement", force_side_displacement);
}
