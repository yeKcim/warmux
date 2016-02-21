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
 * Virtual class to handle weapon in warmux.
 * Weapon projectile are handled in WeaponLauncher (see launcher.cpp and launcher.h).
 *****************************************************************************/

#include <libxml/tree.h>
#include "weapon/weapon.h"
#include "weapon/weapon_cfg.h"
#include <sstream>
#include "character/character.h"
#include "game/game_time.h"
#include "game/game.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "interface/game_msg.h"
#include "map/camera.h"
#include "network/network.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "tool/string_tools.h"

#ifdef DEBUG
# include "character/body.h"
# include "map/map.h"
// Enable it to debug the gun hole position
//# define DEBUG_HOLE
#endif

extern Profile *weapons_res_profile;

const int INFINITE_AMMO = -1;
const uint MAX_TIME_LOADING = 3500;

// XXX Unused ?
//const uint WEAPON_BOX_BUTTON_DX = 20;
//const uint WEAPON_BOX_BUTTON_DY = 50;
//const uint UNIT_BOX_WIDTH = 22;
const uint UNIT_BOX_HEIGHT = 20;
const uint UNIT_BOX_GAP = 6;

const uint ANIM_DISPLAY_TIME = 400;

Weapon::Weapon(Weapon_type type,
               const std::string &id,
               EmptyWeaponConfig * params,
               bool drawable):
  drawable(drawable)
{
  m_type = type;
  m_category = INVALID;
  m_id = id;
  m_help = "";

  m_time_anim_begin = GameTime::GetInstance()->Read();
  m_available_after_turn = 0;
  m_initial_nb_ammo = INFINITE_AMMO;
  m_initial_nb_unit_per_ammo = 1;
  ammo_per_drop = 1;
  drop_probability = 0;
  use_unit_on_first_shoot = true;
  can_be_used_on_closed_map = true;

  m_strength = 0;
  m_first_time_loading = 0;
  m_last_fire_time = 0;
  m_time_between_each_shot = 400;
  m_fire_remanence_time = 100;
  max_strength = min_angle = max_angle = 0;
  use_flipping = true;
  m_display_crosshair = true;

  origin = weapon_origin_HAND;

  m_can_change_weapon = false;

  m_image = NULL;
  m_weapon_fire = NULL;

  if (!use_flipping && !EqualsZero(min_angle - max_angle))
    use_flipping = true;

  extra_params = params;

  if (drawable) {
    m_image = new Sprite(GetResourceManager().LoadImage(weapons_res_profile, m_id));
    m_image->EnableCaches(use_flipping, 0);
  }

  icon = new Sprite(GetResourceManager().LoadImage(weapons_res_profile,m_id+"_ico"));

  mouse_character_selection = true;

  const xmlNode* elem = GetResourceManager().GetElement(weapons_res_profile, "position", m_id);
  if (elem) {
    // E.g. <position name="my_weapon_id" origin="hand" x="-1" y="0" />
    std::string origin_xml;
    XmlReader::ReadIntAttr (elem, "x", position.x);
    XmlReader::ReadIntAttr (elem, "y", position.y);
    if (!XmlReader::ReadStringAttr (elem, "origin", origin_xml)) {
      std::cerr << "No \"origin\" flag found for weapon %s" << m_id <<std::endl;
      ASSERT(false);
    }
    if (origin_xml == "over")
      origin = weapon_origin_OVER;
    else
      origin = weapon_origin_HAND;
  } else {
    std::cerr << "No \"position\" flag found for weapon %s" << m_id <<std::endl;
    ASSERT(false);
  }

  elem = GetResourceManager().GetElement(weapons_res_profile, "hole", m_id);
  if (elem) {
    // E.g. <hole name="my_weapon_id" dx="-1" dy="0" />
    XmlReader::ReadIntAttr(elem, "dx", hole_delta.x);
    XmlReader::ReadIntAttr(elem, "dy", hole_delta.y);
  }
}

Weapon::~Weapon()
{
  if (m_image) delete m_image;
  if (m_weapon_fire) delete m_weapon_fire;
  if (extra_params) delete extra_params;
  if (icon) delete icon;
}

void Weapon::Select()
{
  MSG_DEBUG("weapon.change", "Select %s", m_name.c_str());

  m_time_anim_begin = GameTime::GetInstance()->Read();
  m_strength = 0;
  m_last_fire_time = 0;
  ActiveTeam().ResetNbUnits();

  ActiveCharacter().SetWeaponClothe();

  // is there a crosshair ?
  if (m_display_crosshair)
    ActiveTeam().crosshair.SetActive(true);

  p_Select();

  // be sure that angle is correct
  ActiveCharacter().SetFiringAngle(ActiveCharacter().GetAbsFiringAngle());
}

void Weapon::Deselect()
{
  if (IsLoading())
    StopLoading();

  ActiveTeam().crosshair.SetActive(false);
  ActiveCharacter().SetFiringAngle(0);
  ActiveCharacter().SetMovement("breathe");
  MSG_DEBUG("weapon.change", "Deselect %s", m_name.c_str());
  p_Deselect();
}

void Weapon::Manage()
{
  Refresh();

  Game * game_loop = Game::GetInstance();

  if (game_loop->ReadState() != Game::PLAYING)
    return;

  if (IsOnCooldownFromShot())
    return ;

  if (ActiveTeam().ReadNbUnits() == 0) {
    Deselect();

    if (m_can_change_weapon)
      Select();
    else
      game_loop->SetState(Game::HAS_PLAYED);
  }
}

bool Weapon::CanChangeWeapon() const
{
  if (ActiveTeam().ReadNbUnits() != m_initial_nb_unit_per_ammo &&
      !m_can_change_weapon)
    return false;

  if (ActiveTeam().GetWeapon().IsOnCooldownFromShot())
    return false;

  return true;
}

void Weapon::PrepareShoot()
{
  MSG_DEBUG("weapon.shoot", "Try to shoot with strength=%f, angle:%f",
            m_strength.tofloat(), ActiveCharacter().GetFiringAngle().tofloat());
  StopLoading();

  ActiveCharacter().PrepareShoot();
  MSG_DEBUG("weapon.shoot", "End of shoot");
}

bool Weapon::Shoot()
{
  MSG_DEBUG("weapon.shoot", "Enough ammo ? %d", EnoughAmmo());
  MSG_DEBUG("weapon.shoot", "Enough ammo unit ? %d", EnoughAmmoUnit());
  MSG_DEBUG("weapon.shoot", "Use unit on 1st shoot ? %d", use_unit_on_first_shoot);

  if (!IsReady())
    return false;

  MSG_DEBUG("weapon.shoot", "Enough ammo");

  #ifdef DEBUG
  Point2i hand;
  ActiveCharacter().GetHandPosition(hand);
  MSG_DEBUG("weapon.shoot", "%s Shooting at position:%d,%d (hand: %d,%d)",
            ActiveCharacter().GetName().c_str(),
            ActiveCharacter().GetX(),
            ActiveCharacter().GetY(),
            hand.GetX(),
            hand.GetY());
  ActiveCharacter().body->DebugState();
  #endif
  if (!p_Shoot()) {
    MSG_DEBUG("weapon.shoot", "shoot has failed!!");
    return false;
  }
  m_last_fire_time = GameTime::GetInstance()->Read();

  MSG_DEBUG("weapon.shoot", "shoot!");

  // Is this the first shoot for this ammo use ?
  if (ActiveTeam().ReadNbUnits() == m_initial_nb_unit_per_ammo) {
    UseAmmo();
  }

  if (use_unit_on_first_shoot){
    UseAmmoUnit();
  }

  if (max_strength.IsNotZero())
    ActiveCharacter().previous_strength = m_strength;

  Game::GetInstance()->SetCharacterChosen(true);

  return true;
}

bool Weapon::IsReady() const
{
  // WARNING: The following commented code is wrong! Please see explanation following
  //   if (!EnoughAmmo()
  //       || (use_unit_on_first_shoot && !EnoughAmmoUnit()))
  //     return false;


  // Gentildemon : YES the following code seems strange!
  // BUT when have only one ammo left, you shoot, then nb_ammo == 0
  // then you need to be able to use the left ammo units

  if (use_unit_on_first_shoot && !EnoughAmmoUnit())
    return false;

  if (!EnoughAmmo())
    if (!(ActiveTeam().ReadNbAmmos() == 0
          && use_unit_on_first_shoot && EnoughAmmoUnit()))
      return false;
  return true;
}

void Weapon::Refresh()
{
  if (IsLoading() && !ActiveCharacter().IsPreparingShoot()) {
    // Strength == max strength -> Fire !!!
    if (ReadStrength() >= max_strength) {
        PrepareShoot();
    } else {
        // still pressing the Space key
        UpdateStrength();
    }
  }
}

void Weapon::StartShooting()
{
  if (ActiveCharacter().IsPreparingShoot())
    return;

  if (max_strength.IsNotZero() && IsReady())
    InitLoading();
}

void Weapon::StopShooting()
{
  if (max_strength.IsNotZero() && !IsLoading())
    /* User has probably exceed the max_strength */
    return;

  if (!ActiveCharacter().IsPreparingShoot()) {
    PrepareShoot();
  }
}

void Weapon::RepeatShoot()
{
  uint current_time = GameTime::GetInstance()->Read();

  if (current_time - m_last_fire_time >= m_time_between_each_shot) {
    PrepareShoot();
    // this is done in Weapon::Shoot() but let's set meanwhile,
    // to prevent problems with rapid fire weapons such as submachine
    m_last_fire_time = current_time;
  }
}

void Weapon::StartMovingLeftForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_MOVING_LEFT);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StopMovingLeftForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_MOVING_LEFT);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StartMovingRightForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_MOVING_RIGHT);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StopMovingRightForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_MOVING_RIGHT);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StartMovingUpForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_MOVING_UP);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StopMovingUpForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_MOVING_UP);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StartMovingDownForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_MOVING_DOWN);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::StopMovingDownForAllPlayers()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_MOVING_DOWN);
  ActionHandler::GetInstance()->NewAction(a);
}

// Compute position of weapon's image
void Weapon::PosXY (int &x, int &y) const
{
  if (origin == weapon_origin_HAND)
  {
    Point2i handPos;
    ActiveCharacter().GetHandPosition(handPos);
    y = handPos.y - position.y;
    if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
      x = handPos.x - position.x;
    else
      x = handPos.x + position.x - m_image->GetWidth();
  }
  else
  if (origin == weapon_origin_OVER)
  {
    x = ActiveCharacter().GetCenterX() - m_image->GetWidth() / 2 + position.x;
    y = ActiveCharacter().GetY()       - m_image->GetHeight()    + position.y;
  }
  else
    ASSERT(false);
}

Point2i Weapon::GetGunHolePosition() const
{
  Point2i pos;
  ActiveCharacter().GetHandPosition(pos);
  Point2i hole(pos + hole_delta * Point2i(ActiveCharacter().GetDirection(),1));
  Double dst = pos.Distance(hole);
  Double angle = pos.ComputeAngle(hole);

  if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
    angle += ActiveCharacter().GetFiringAngle();
  else
    angle += ActiveCharacter().GetFiringAngle() - PI;

  return pos + Point2i(dst * cos(angle), dst * sin(angle));
}

bool Weapon::EnoughAmmo() const
{
  int ammo = ActiveTeam().ReadNbAmmos(m_type);
  return (ammo == INFINITE_AMMO || 0 < ammo);
}

void Weapon::UseAmmo() const
{
  // Use one ammo...
  int *ammo = &ActiveTeam().AccessNbAmmos();
  if (*ammo != INFINITE_AMMO) (*ammo)--;

  ASSERT(*ammo >= 0 || *ammo == INFINITE_AMMO);
}

bool Weapon::EnoughAmmoUnit() const
{
  int unit = ActiveTeam().ReadNbUnits(m_type);
  return (unit > 0);
}

void Weapon::UseAmmoUnit() const
{
  // Use one ammo unit.
  int *unit = &ActiveTeam().AccessNbUnits();
  (*unit)--;

  ASSERT(*unit >= 0);
}

void Weapon::UpdateStrength()
{
  if (max_strength==0 || !m_first_time_loading)
    return;

  uint time = GameTime::GetInstance()->Read() - m_first_time_loading;
  Double val = (max_strength * time*time) / (MAX_TIME_LOADING*MAX_TIME_LOADING);

  m_strength = InRange_Double (val, 0.0, max_strength);
}

bool Weapon::IsOnCooldownFromShot() const
{
  return (m_last_fire_time > 0 && m_last_fire_time + m_time_between_each_shot > GameTime::GetInstance()->Read());
}

void Weapon::InitLoading()
{
  // no loading for weapon with max_strength = 0
  if (max_strength==0)
    return ;

  loading_sound.Play("default","weapon/load");

  m_first_time_loading = GameTime::GetInstance()->Read();

  m_strength = 0;

  Game::GetInstance()->SetCharacterChosen(true);
}

void Weapon::StopLoading()
{
  m_first_time_loading = 0 ;

  loading_sound.Stop();
}

void Weapon::Draw()
{
  if (Game::GetInstance()->ReadState() != Game::PLAYING &&
      m_last_fire_time + 100 < GameTime::GetInstance()->Read())
    return;

#ifndef DEBUG_HOLE
  if (m_last_fire_time + m_fire_remanence_time > GameTime::GetInstance()->Read())
#endif
    DrawWeaponFire();

  DrawAmmoUnits();

  ASSERT(drawable || !ShouldBeDrawn());
  if (!(drawable && ShouldBeDrawn()))
    return;

  if (ActiveCharacter().IsGhost()
      || ActiveCharacter().IsDrowned()
      || ActiveCharacter().IsDead())
    return;

  // rotate weapon if needed
  if (!EqualsZero(min_angle - max_angle)) {
    if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
      m_image->SetRotation_rad(ActiveCharacter().GetFiringAngle());
    else
      m_image->SetRotation_rad(ActiveCharacter().GetFiringAngle()-PI);
  } else {
    m_image->SetRotation_rad(ZERO);
  }

  // flip image if needed
  if (use_flipping) {
    m_image->SetFlipped(DIRECTION_LEFT == ActiveCharacter().GetDirection());
  }

  // Calculate position of the image
  int x,y;
  PosXY(x, y);

  // Animate the display of the weapon:
  if (m_time_anim_begin + ANIM_DISPLAY_TIME > GameTime::GetInstance()->Read()) {
    if (!EqualsZero(min_angle - max_angle)) {
      Double angle = m_image->GetRotation_rad();
      angle += sin(HALF_PI * Double(GameTime::GetInstance()->Read() - m_time_anim_begin) / ANIM_DISPLAY_TIME)
             * TWO_PI;
      m_image->SetRotation_rad(angle);
      m_image->Scale(ONE, ONE);
    }
    else {
      Double scale = sin((Double)1.5 * HALF_PI * Double(GameTime::GetInstance()->Read() - m_time_anim_begin) / ANIM_DISPLAY_TIME)
                   / sin((Double)1.5 * HALF_PI);
      if (scale.IsNotZero()) {
        m_image->Scale(scale, scale);
        m_image->SetFlipped(DIRECTION_LEFT == ActiveCharacter().GetDirection());
      }

      // Recompute position to get the icon centered over the skin
      if (origin == weapon_origin_OVER)
        PosXY(x,y);
    }
  } else
    m_image->Scale(ONE, ONE);

  if (m_image)
    m_image->Blit(GetMainWindow(), Point2i(x, y) - Camera::GetInstance()->GetPosition());

#ifdef DEBUG
  if (IsLOGGING("weapon")) {
    Point2i hand;
    ActiveCharacter().GetHandPosition(hand);
    Rectanglei rect(hand - 1 - Camera::GetInstance()->GetPosition(),
                    Point2i(3, 3));

    GetWorld().ToRedrawOnMap(rect);

    GetMainWindow().RectangleColor(rect, c_red);

    MSG_DEBUG("weapon.handposition", "Position: %d, %d - hand: %d, %d",
              ActiveCharacter().GetX(), ActiveCharacter().GetY(),
              hand.GetX(), hand.GetY());
  }
  if (IsLOGGING("weapon.hole")) {
    Rectanglei rect(GetGunHolePosition() - Camera::GetInstance()->GetPosition() - 1,
                    Point2i(3, 3));

    GetWorld().ToRedrawOnMap(rect);
    GetMainWindow().RectangleColor(rect, c_red);
  }
#endif
}

// Draw the weapon fire when firing
void Weapon::DrawWeaponFire()
{
  if (!m_weapon_fire)
    return;
  Point2i hand;
  ActiveCharacter().GetHandPosition(hand);
  Point2i hole(hand +  hole_delta * Point2i(ActiveCharacter().GetDirection(),1));

  if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
    hole = hole -  Point2i(0, m_weapon_fire->GetHeight()/2);
  else
    hole = hole +  Point2i(0, m_weapon_fire->GetHeight()/2);
  Double dst = hand.Distance(hole);
  Double angle = hand.ComputeAngle(hole);

  angle += ActiveCharacter().GetFiringAngle();

  if (ActiveCharacter().GetDirection() == DIRECTION_LEFT)
    angle -= PI;

  Point2i spr_pos =  hand + Point2i(static_cast<int>(dst * cos(angle)),
                                    static_cast<int>(dst * sin(angle)));

  m_weapon_fire->SetRotation_HotSpot(Point2i(0,0));
  m_weapon_fire->SetRotation_rad(ActiveCharacter().GetFiringAngle());
  m_weapon_fire->Draw(spr_pos);
}

void Weapon::DrawAmmoUnits() const
{
  if (!ShouldAmmoUnitsBeDrawn())
    return;

  if (m_initial_nb_unit_per_ammo > 1) {
    Rectanglei rect;

    std::ostringstream ss;

    ss << ActiveTeam().ReadNbUnits();

    DrawTmpBoxText(*Font::GetInstance(Font::FONT_SMALL),
                   Point2i(ActiveCharacter().GetCenterX(),
                           ActiveCharacter().GetY() - UNIT_BOX_HEIGHT / 2 - UNIT_BOX_GAP )
                   - Camera::GetInstance()->GetPosition(),
                   ss.str());
  }
}

bool Weapon::LoadXml(const xmlNode*  weapon)
{
  const xmlNode* elem = XmlReader::GetMarker(weapon, m_id);
  if (!elem) {
    std::cout << Format(_("No element <%s> found in the xml config file!"),
                        m_id.c_str())
              << std::endl;
    return false;
  }

  XmlReader::ReadInt(elem, "available_after_turn", m_available_after_turn);
  XmlReader::ReadInt(elem, "nb_ammo", m_initial_nb_ammo);
  XmlReader::ReadInt(elem, "unit_per_ammo", m_initial_nb_unit_per_ammo);
  XmlReader::ReadInt(elem, "ammo_per_drop", ammo_per_drop);
  XmlReader::ReadDouble(elem, "drop_probability", drop_probability);
  if (m_initial_nb_ammo == INFINITE_AMMO && drop_probability.IsNotZero()) {
    std::cerr << Format(_("The weapon %s has infinite ammo, but bonus boxes might contain ammo for it!"), m_id.c_str());
    std::cerr << std::endl;
  }

  // max strength
  // if max_strength = 0, no strength_bar !
  XmlReader::ReadDouble(elem, "max_strength", max_strength);

  // change weapon after ? (for the grapple = true)
  XmlReader::ReadBool(elem, "change_weapon", m_can_change_weapon);

  // Disable crosshair ?
  XmlReader::ReadBool(elem, "display_crosshair", m_display_crosshair);
  // angle of weapon when drawing
  // if (min_angle == max_angle) no cross_hair !
  // between -90 to 90 degrees
  int min_angle_deg = 0, max_angle_deg = 0;
  XmlReader::ReadInt(elem, "min_angle", min_angle_deg);
  XmlReader::ReadInt(elem, "max_angle", max_angle_deg);
  min_angle = min_angle_deg * PI / 180;
  max_angle = max_angle_deg * PI / 180;
  if (EqualsZero(min_angle - max_angle))
    m_display_crosshair = false;
#if 0
  if (m_weapon_fire) {
    uint num = 32 * (max_angle - min_angle) / TWO_PI;
    m_weapon_fire->EnableCaches(true, num, min_angle, max_angle);
  }
#endif

  // Load extra parameters if existing
  if (extra_params)
    extra_params->LoadXml(elem);

  if (drawable && origin == weapon_origin_HAND)
    m_image->SetRotation_HotSpot(position);

  return true;
}

xmlNode* Weapon::SaveXml(XmlWriter& writer, xmlNode*  weapon) const
{
  xmlNode* elem = XmlWriter::AddNode(weapon, m_id.c_str());
  if (!elem) {
    fprintf(stderr, "Couldn't save weapon config for %s\n", m_id.c_str());
    return NULL;
  }

  writer.WriteElement(elem, "available_after_turn", int2str(m_available_after_turn));
  writer.WriteElement(elem, "nb_ammo", int2str(m_initial_nb_ammo));
  if (m_initial_nb_unit_per_ammo!=1)
    writer.WriteElement(elem, "unit_per_ammo", int2str(m_initial_nb_unit_per_ammo));
  writer.WriteElement(elem, "ammo_per_drop", int2str(ammo_per_drop));
  writer.WriteElement(elem, "drop_probability", Double2str(drop_probability));
  // max strength
  // if max_strength = 0, no strength_bar !
  if (max_strength.IsNotZero())
    writer.WriteElement(elem, "max_strength", Double2str(max_strength));

  // change weapon after ? (for the grapple = true)
  if (m_can_change_weapon)
    writer.WriteElement(elem, "change_weapon", bool2str(m_can_change_weapon));

  // Disable crosshair ?
  if (m_display_crosshair)
    writer.WriteElement(elem, "display_crosshair", bool2str(m_display_crosshair));
  // angle of weapon when drawing
  // if (min_angle == max_angle) no cross_hair !
  // between -90 to 90 degrees
  int min_angle_deg = uround(min_angle * 180 / PI);
  int max_angle_deg = uround(max_angle * 180 / PI);
  if (min_angle_deg || max_angle_deg) {
    writer.WriteElement(elem, "min_angle", int2str(min_angle_deg));
    writer.WriteElement(elem, "max_angle", int2str(max_angle_deg));
  }

  // Save extra parameters if existing
  if (extra_params)
    extra_params->SaveXml(writer, elem);

  return elem;
}

// Handle keyboard events

// #################### SHOOT
void Weapon::HandleKeyPressed_Shoot()
{
  Action *a = new Action(Action::ACTION_WEAPON_START_SHOOTING);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::HandleKeyReleased_Shoot()
{
  Action *a = new Action(Action::ACTION_WEAPON_STOP_SHOOTING);
  ActionHandler::GetInstance()->NewAction(a);
}

void Weapon::p_Deselect()
{
  Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  ActiveCharacter().SetMovement("breathe");
}

bool Weapon::IsAngleValid(Double angle) const
{
  angle = -angle; // work around incorrect sign
  return min_angle <= angle && angle <= max_angle;
}

void Weapon::Message(const std::string& msg)
{
  GameMessages::GetInstance()->Add(msg, ActiveTeam().GetColor());
}
