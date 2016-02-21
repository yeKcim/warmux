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
 * Virtual class to handle weapon in wormux.
 * Weapon projectile are handled in WeaponLauncher (see launcher.cpp and launcher.h).
 *****************************************************************************/

#include <libxml/tree.h>
#include "weapon/weapon.h"
#include "weapon/weapon_strength_bar.h"
#include "weapon/weapon_cfg.h"
#include <sstream>
#include "character/character.h"
#include "game/time.h"
#include "game/game.h"
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "team/macro.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/math_tools.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

#ifdef DEBUG
#include "map/map.h"
#endif

// Enable it to debug the gun hole position
#ifdef DEBUG
//#define DEBUG_HOLE
#endif

extern Profile *weapons_res_profile;

const int INFINITE_AMMO = -1;
const uint MAX_TIME_LOADING = 2000;

// XXX Unused ?
//const uint WEAPON_BOX_BUTTON_DX = 20;
//const uint WEAPON_BOX_BUTTON_DY = 50;
//const uint UNIT_BOX_WIDTH = 22;
const uint UNIT_BOX_HEIGHT = 20;
const uint UNIT_BOX_GAP = 6;

const uint ANIM_DISPLAY_TIME = 400;

extern WeaponStrengthBar weapon_strength_bar;

Weapon::Weapon(Weapon_type type,
               const std::string &id,
               EmptyWeaponConfig * params,
               weapon_visibility_t visibility)
{
  m_type = type;
  m_category = INVALID;
  m_id = id;
  m_help = "";

  m_is_active = false;

  m_time_anim_begin = Time::GetInstance()->Read();
  m_initial_nb_ammo = INFINITE_AMMO;
  m_initial_nb_unit_per_ammo = 1;
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

  m_visibility = visibility;
  m_unit_visibility = ALWAYS_VISIBLE;

  m_image = NULL;
  m_weapon_fire = NULL;

  if (!use_flipping && !EqualsZero(min_angle - max_angle))
    use_flipping = true;

  extra_params = params;

  if (m_visibility != NEVER_VISIBLE)
  {
    m_image = new Sprite( resource_manager.LoadImage(weapons_res_profile, m_id));
    if(!EqualsZero(min_angle - max_angle))
      m_image->cache.EnableLastFrameCache();
  }

  icon = new Sprite(resource_manager.LoadImage(weapons_res_profile,m_id+"_ico"), true);
  icon->cache.EnableLastFrameCache();

  mouse_character_selection = true;

  xmlNode* elem = resource_manager.GetElement(weapons_res_profile, "position", m_id);
  if (elem != NULL) {
    // E.g. <position name="my_weapon_id" origin="hand" x="-1" y="0" />
    std::string origin_xml;
    XmlReader::ReadIntAttr (elem, "x", position.x);
    XmlReader::ReadIntAttr (elem, "y", position.y);
    if(!XmlReader::ReadStringAttr (elem, "origin", origin_xml))
    {
      std::cerr << "No \"origin\" flag found for weapon %s" << m_id <<std::endl;
      ASSERT(false);
    }
    if (origin_xml == "over")
      origin = weapon_origin_OVER;
    else
      origin = weapon_origin_HAND;
  }
  else
  {
    std::cerr << "No \"position\" flag found for weapon %s" << m_id <<std::endl;
    ASSERT(false);
  }

  elem = resource_manager.GetElement(weapons_res_profile, "hole", m_id);
  if (elem != NULL) {
    // E.g. <hole name="my_weapon_id" dx="-1" dy="0" />
    XmlReader::ReadIntAttr(elem, "dx", hole_delta.x);
    XmlReader::ReadIntAttr(elem, "dy", hole_delta.y);
  }
}

Weapon::~Weapon()
{
  if (m_image) delete m_image;
  if (m_weapon_fire) delete m_weapon_fire;
  if(extra_params) delete extra_params;
  if(icon) delete icon;
}

void Weapon::Select()
{
  MSG_DEBUG("weapon.change", "Select %s", m_name.c_str());

  m_time_anim_begin = Time::GetInstance()->Read();
  m_is_active = false;
  m_strength = 0;
  m_last_fire_time = 0;
  ActiveTeam().ResetNbUnits();

  ActiveCharacter().SetWeaponClothe();

  // is there a crosshair ?
  if (m_display_crosshair)
    ActiveTeam().crosshair.display = true;

  if(!EqualsZero(min_angle - max_angle))
    ActiveTeam().crosshair.enable = true;

  p_Select();

  // be sure that angle is correct
  ActiveCharacter().SetFiringAngle(ActiveCharacter().GetAbsFiringAngle());

  if (max_strength == 0) return ;

  // prepare the strength bar
  weapon_strength_bar.InitVal (0, 0, uint(max_strength*100));

  // init stamp on the stength_bar
  double val = ActiveCharacter().previous_strength;
  weapon_strength_bar.ResetTag();
  if (0 < val && val < max_strength)
    weapon_strength_bar.AddTag (uint(val*100), primary_red_color);
}

void Weapon::Deselect()
{
  ActiveTeam().crosshair.enable = false;
  ActiveTeam().crosshair.display = false;
  m_is_active = false;
  MSG_DEBUG("weapon.change", "Deselect %s", m_name.c_str());
  p_Deselect();
}

void Weapon::Manage()
{
  if (!IsInUse())
    return ;

  Refresh();

  Game * game_loop = Game::GetInstance();

  if (game_loop->ReadState() != Game::PLAYING)
    return;

  if ( (ActiveTeam().ReadNbUnits() == 0) )
    {
      Deselect();

      if (m_can_change_weapon)
        Select();
      else
        game_loop->SetState(Game::HAS_PLAYED);
    }
}

bool Weapon::CanChangeWeapon() const
{
  if ( !ActiveTeam().IsLocal() )
    return false;

  if ( (ActiveTeam().ReadNbUnits() != m_initial_nb_unit_per_ammo) &&
       (m_can_change_weapon == false))
    return false;

  return true;
}

void Weapon::NewActionWeaponShoot() const
{
  ASSERT(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());

  if (ActiveCharacter().IsPreparingShoot()) { // a shot is already in progress
#ifdef DEBUG
    fprintf(stderr, "\nWARNING: Weapon::NewActionWeaponShoot: a shot is already in progress!\n");
    fprintf(stderr, "         Maybe, shot anim for this weapon is longer than m_time_between_each_shot\n\n");
#endif
    return;
  }

  Action* a_shoot = new Action(Action::ACTION_WEAPON_SHOOT,
                               m_strength,
                               ActiveCharacter().GetAbsFiringAngle());
  ActionHandler::GetInstance()->NewActionActiveCharacter(a_shoot);
}

void Weapon::NewActionWeaponStopUse() const
{
  ASSERT(ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI());

  Action* a = new Action(Action::ACTION_WEAPON_STOP_USE);
  ActionHandler::GetInstance()->NewActionActiveCharacter(a);
}

void Weapon::PrepareShoot(double strength, double angle)
{
  MSG_DEBUG("weapon.shoot", "Try to shoot with strength:%f, angle:%f",
            strength, angle);
  ActiveCharacter().SetFiringAngle(angle);
  m_strength = strength;
  StopLoading();

  ActiveCharacter().PrepareShoot();
  MSG_DEBUG("weapon.shoot", "End of shoot");
}

bool Weapon::Shoot()
{
  MSG_DEBUG("weapon.shoot", "Enough ammo ? %d", EnoughAmmo() );
  MSG_DEBUG("weapon.shoot", "Enough ammo unit ? %d", EnoughAmmoUnit() );
  MSG_DEBUG("weapon.shoot", "Use unit on 1st shoot ? %d", use_unit_on_first_shoot );


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
      if ( ! (ActiveTeam().ReadNbAmmos() == 0
              && use_unit_on_first_shoot && EnoughAmmoUnit()) )
        return false;
  }

  MSG_DEBUG("weapon.shoot", "Enough ammo");

  MSG_DEBUG("weapon.shoot", "%s Shooting at position:%d,%d (hand: %d,%d)",
            ActiveCharacter().GetName().c_str(),
            ActiveCharacter().GetX(),
            ActiveCharacter().GetY(),
            ActiveCharacter().GetHandPosition().GetX(),
            ActiveCharacter().GetHandPosition().GetY());
  ActiveCharacter().body->DebugState();
  if (!p_Shoot()) return false;
  m_last_fire_time = Time::GetInstance()->Read();

  MSG_DEBUG("weapon.shoot", "shoot!");

  // Is this the first shoot for this ammo use ?
  if (ActiveTeam().ReadNbUnits() == m_initial_nb_unit_per_ammo) {
    UseAmmo();
  }

  if (use_unit_on_first_shoot){
    UseAmmoUnit();
  }

  m_is_active = true;

  if (max_strength != 0) ActiveCharacter().previous_strength = m_strength;

  Game::GetInstance()->character_already_chosen = true;

  return true;
}

void Weapon::RepeatShoot()
{
  uint current_time = Time::GetInstance()->Read();

  if (current_time - m_last_fire_time >= m_time_between_each_shot) {
    NewActionWeaponShoot();
    // this is done in Weapon::Shoot() but let's set meanwhile,
    // to prevent problems with rapid fire weapons such as submachine
    m_last_fire_time = current_time;
  }
}

// Compute position of weapon's image
void Weapon::PosXY (int &x, int &y) const
{
  if (origin == weapon_origin_HAND)
  {
    Point2i handPos = ActiveCharacter().GetHandPosition();
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

const Point2i Weapon::GetGunHolePosition() const
{
  const Point2i &pos = ActiveCharacter().GetHandPosition();
  Point2i hole(pos + hole_delta * Point2i(ActiveCharacter().GetDirection(),1));
  double dst = pos.Distance(hole);
  double angle = pos.ComputeAngle(hole);

  if(ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
    angle += ActiveCharacter().GetFiringAngle();
  else
    angle += ActiveCharacter().GetFiringAngle() - M_PI;

  return pos + Point2i(static_cast<int>(dst * cos(angle)),
		       static_cast<int>(dst * sin(angle)));
}

bool Weapon::EnoughAmmo() const
{
  int ammo = ActiveTeam().ReadNbAmmos();
  return ((ammo == INFINITE_AMMO) || (0 < ammo));
}

void Weapon::UseAmmo() const
{
  // Use one ammo...
  int *ammo = &ActiveTeam().AccessNbAmmos();
  if (*ammo != INFINITE_AMMO) (*ammo)--;

  ASSERT (*ammo >= 0 || *ammo == INFINITE_AMMO);
}

bool Weapon::EnoughAmmoUnit() const
{
  int unit = ActiveTeam().ReadNbUnits();
  return (unit > 0);
}

void Weapon::UseAmmoUnit() const
{
  // Use one ammo unit.
  int *unit = &ActiveTeam().AccessNbUnits();
  (*unit)--;

  ASSERT (*unit >= 0);
}

const std::string& Weapon::GetName() const {
  ASSERT (!m_name.empty());
  return m_name;
}

const std::string& Weapon::GetHelp() const {
  ASSERT (!m_help.empty());
  return m_help;
}

const std::string& Weapon::GetID() const {
  ASSERT (!m_id.empty());
  return m_id;
}

void Weapon::UpdateStrength(){
  if( max_strength == 0 || m_first_time_loading == 0 )
    return ;

  uint time = Time::GetInstance()->Read() - m_first_time_loading;
  double val = (max_strength * time) / MAX_TIME_LOADING;

  m_strength = InRange_Double (val, 0.0, max_strength);

  weapon_strength_bar.UpdateValue ((int)(m_strength*100));
}

void Weapon::InitLoading(){
  // no loading for weapon with max_strength = 0
  if (max_strength == 0)
    return ;

  loading_sound.Play("share","weapon/load");

  m_first_time_loading = Time::GetInstance()->Read();

  m_strength = 0;

  Game::GetInstance()->character_already_chosen = true;
}

void Weapon::StopLoading(){
  m_first_time_loading = 0 ;

  loading_sound.Stop();
}

void Weapon::Draw(){
  if(Game::GetInstance()->ReadState() != Game::PLAYING &&
     m_last_fire_time + 100 < Time::GetInstance()->Read())
    return;

#ifndef DEBUG_HOLE
  if (m_last_fire_time + m_fire_remanence_time > Time::GetInstance()->Read())
#endif
    DrawWeaponFire();
  weapon_strength_bar.visible = false;

  // Do we need to draw strength_bar ? (real draw is done by class Interface)
  // We do not draw on the network
  if (max_strength != 0 && IsReady() && !IsInUse() &&
      (ActiveTeam().IsLocal() || ActiveTeam().IsLocalAI()))
    weapon_strength_bar.visible = true;

  DrawAmmoUnits();

  switch (m_visibility)
    {
      case ALWAYS_VISIBLE:
        break;

      case NEVER_VISIBLE:
        return;
        break;

      case VISIBLE_ONLY_WHEN_ACTIVE:
        if (!IsInUse())
          return ;
        break;

      case VISIBLE_ONLY_WHEN_INACTIVE:
        if (IsInUse())
          return ;
        break;

      default:
        printf ("Hum... there is a problem !!!\n");
        break;
    }

  if(ActiveCharacter().IsGhost()
  || ActiveCharacter().IsDrowned()
  || ActiveCharacter().IsDead())
    return;

  // Reset the Sprite:
  m_image->SetRotation_rad(0.0);
  m_image->Scale(1.0,1.0);

  // rotate weapon if needed
  if (!EqualsZero(min_angle - max_angle))
  {
    if(ActiveCharacter().GetDirection() == 1)
      m_image->SetRotation_rad (ActiveCharacter().GetFiringAngle());
    else
      m_image->SetRotation_rad (ActiveCharacter().GetFiringAngle() - M_PI);
  }

  // flip image if needed
  if (use_flipping)
  {
    m_image->Scale(ActiveCharacter().GetDirection(), 1.0);
  }

  // Calculate position of the image
  int x,y;
  PosXY (x, y);

  // Animate the display of the weapon:
  if( m_time_anim_begin + ANIM_DISPLAY_TIME > Time::GetInstance()->Read())
  {
    if (!EqualsZero(min_angle - max_angle))
    {
      double angle = m_image->GetRotation_rad();
      angle += sin( M_PI_2 * double(Time::GetInstance()->Read() - m_time_anim_begin) /(double) ANIM_DISPLAY_TIME) * 2 * M_PI;
      m_image->SetRotation_rad (angle);
    }
    else
    {
      float scale = sin( 1.5 * M_PI_2 * double(Time::GetInstance()->Read() - m_time_anim_begin) /(double) ANIM_DISPLAY_TIME) / sin(1.5 * M_PI_2);
      m_image->Scale(ActiveCharacter().GetDirection() * scale,scale);

      if(origin == weapon_origin_OVER) PosXY(x,y); //Recompute position to get the icon centered over the skin
    }
  }

  if ( m_image )
    m_image->Blit( AppWormux::GetInstance()->video->window, Point2i(x, y) - Camera::GetInstance()->GetPosition());

#ifdef DEBUG
  if (IsLOGGING("weapon")) {
    Rectanglei rect(ActiveCharacter().GetHandPosition().GetX()-1 - Camera::GetInstance()->GetPositionX(),
		    ActiveCharacter().GetHandPosition().GetY()-1 - Camera::GetInstance()->GetPositionY(),
		    3,
		    3);

    world.ToRedrawOnMap(rect);

    AppWormux::GetInstance()->video->window.RectangleColor(rect, c_red);

    MSG_DEBUG("weapon.handposition", "Position: %d, %d - hand: %d, %d",
	      ActiveCharacter().GetX(),
	      ActiveCharacter().GetY(),
	      ActiveCharacter().GetHandPosition().GetX(),
	      ActiveCharacter().GetHandPosition().GetY());
  }
#endif
#ifdef DEBUG_HOLE
  Rectanglei rect(GetGunHolePosition().GetX() - Camera::GetInstance()->GetPositionX()- 1,
                  GetGunHolePosition().GetY() - Camera::GetInstance()->GetPositionY()- 1,
      	    	  3, 3);

  world.ToRedrawOnMap(rect);
  AppWormux::GetInstance()->video->window.RectangleColor(rect, c_red);

//  rect = Rectangle(
#endif
}

// Draw the weapon fire when firing
void Weapon::DrawWeaponFire()
{
  if (m_weapon_fire == NULL) return;
  Point2i pos = ActiveCharacter().GetHandPosition();
  Point2i hole(pos +  hole_delta * Point2i(ActiveCharacter().GetDirection(),1));

  if( ActiveCharacter().GetDirection() == DIRECTION_RIGHT)
    hole = hole -  Point2i(0, m_weapon_fire->GetHeight()/2);
  else
    hole = hole +  Point2i(0, m_weapon_fire->GetHeight()/2);
  double dst = pos.Distance(hole);
  double angle = pos.ComputeAngle(hole);

  angle += ActiveCharacter().GetFiringAngle();

  if( ActiveCharacter().GetDirection() == DIRECTION_LEFT)
    angle -= M_PI;

  Point2i spr_pos =  pos + Point2i(static_cast<int>(dst * cos(angle)),
                                   static_cast<int>(dst * sin(angle)));

  m_weapon_fire->SetRotation_HotSpot (Point2i(0,0));
  m_weapon_fire->SetRotation_rad (ActiveCharacter().GetFiringAngle());
  m_weapon_fire->Draw( spr_pos );
}

void Weapon::DrawAmmoUnits() const
{
  switch (m_unit_visibility) {

  case VISIBLE_ONLY_WHEN_ACTIVE:
    if (!IsInUse())
      return;
    break;

  case VISIBLE_ONLY_WHEN_INACTIVE:
    if (IsInUse())
      return ;
    break;

  case NEVER_VISIBLE:
    return;
    break;

  default:
    ; // nothing to do
  }

  if (m_initial_nb_unit_per_ammo > 1)
  {
    Rectanglei rect;

    std::ostringstream ss;

    ss << ActiveTeam().ReadNbUnits();

    DrawTmpBoxText(*Font::GetInstance(Font::FONT_SMALL),
                   Point2i( ActiveCharacter().GetCenterX(),
                            ActiveCharacter().GetY() - UNIT_BOX_HEIGHT / 2 - UNIT_BOX_GAP )
                   - Camera::GetInstance()->GetPosition(),
                   ss.str());
  }
}

bool Weapon::LoadXml(xmlNode*  weapon)
{
  xmlNode* elem = XmlReader::GetMarker(weapon, m_id)->children;
  if (elem == NULL)
  {
      std::cout << Format(_("No element <%s> found in the xml config file!"),
                          m_id.c_str())
                << std::endl;
    return false;
  }

  XmlReader::ReadInt(elem, "nb_ammo", m_initial_nb_ammo);
  XmlReader::ReadInt(elem, "unit_per_ammo", m_initial_nb_unit_per_ammo);

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
  min_angle = static_cast<double>(min_angle_deg) * M_PI / 180.0;
  max_angle = static_cast<double>(max_angle_deg) * M_PI / 180.0;
  if(EqualsZero(min_angle - max_angle))
    m_display_crosshair = false;

  // Load extra parameters if existing
  if (extra_params != NULL) extra_params->LoadXml(elem);

  if (m_visibility != NEVER_VISIBLE && origin == weapon_origin_HAND)
    m_image->SetRotation_HotSpot(position);

  return true;
}

void Weapon::ActionStopUse()
{
  ASSERT(false);
}

// Handle keyboard events

// #################### SHOOT
void Weapon::HandleKeyPressed_Shoot(bool)
{
  if(ActiveCharacter().IsPreparingShoot())
    return;

  if (max_strength == 0)
    NewActionWeaponShoot();
  else if ( IsReady() )
    InitLoading();
}

void Weapon::HandleKeyRefreshed_Shoot(bool)
{
  if(ActiveCharacter().IsPreparingShoot())
    return;
  if ( !IsLoading() )
    return;

  // Strength == max strength -> Fire !!!
  if (ReadStrength() >= max_strength) {
    NewActionWeaponShoot();
  } else {
    // still pressing the Space key
    UpdateStrength();
  }
}

void Weapon::HandleKeyReleased_Shoot(bool)
{
  if(ActiveCharacter().IsPreparingShoot())
    return;
  if ( !IsLoading())
    return;

  NewActionWeaponShoot();
}

void Weapon::HandleKeyPressed_MoveRight(bool shift)
{
  ActiveCharacter().HandleKeyPressed_MoveRight(shift);
}

void Weapon::HandleKeyRefreshed_MoveRight(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_MoveRight(shift);
}

void Weapon::HandleKeyReleased_MoveRight(bool shift)
{
  ActiveCharacter().HandleKeyReleased_MoveRight(shift);
}

void Weapon::HandleKeyPressed_MoveLeft(bool shift)
{
  ActiveCharacter().HandleKeyPressed_MoveLeft(shift);
}

void Weapon::HandleKeyRefreshed_MoveLeft(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_MoveLeft(shift);
}

void Weapon::HandleKeyReleased_MoveLeft(bool shift)
{
  ActiveCharacter().HandleKeyReleased_MoveLeft(shift);
}

void Weapon::HandleKeyPressed_Up(bool shift)
{
  ActiveCharacter().HandleKeyPressed_Up(shift);
}

void Weapon::HandleKeyRefreshed_Up(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_Up(shift);
}

void Weapon::HandleKeyReleased_Up(bool shift)
{
  ActiveCharacter().HandleKeyReleased_Up(shift);
}

void Weapon::HandleKeyPressed_Down(bool shift)
{
  ActiveCharacter().HandleKeyPressed_Down(shift);
}

void Weapon::HandleKeyRefreshed_Down(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_Down(shift);
}

void Weapon::HandleKeyReleased_Down(bool shift)
{
  ActiveCharacter().HandleKeyReleased_Down(shift);
}

void Weapon::HandleKeyPressed_Jump(bool shift)
{
  ActiveCharacter().HandleKeyPressed_Jump(shift);
}

void Weapon::HandleKeyRefreshed_Jump(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_Jump(shift);
}

void Weapon::HandleKeyReleased_Jump(bool shift)
{
  ActiveCharacter().HandleKeyReleased_Jump(shift);
}

void Weapon::HandleKeyPressed_HighJump(bool shift)
{
  ActiveCharacter().HandleKeyPressed_HighJump(shift);
}

void Weapon::HandleKeyRefreshed_HighJump(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_HighJump(shift);
}

void Weapon::HandleKeyReleased_HighJump(bool shift)
{
  ActiveCharacter().HandleKeyReleased_HighJump(shift);
}

void Weapon::HandleKeyPressed_BackJump(bool shift)
{
  ActiveCharacter().HandleKeyPressed_BackJump(shift);
}

void Weapon::HandleKeyRefreshed_BackJump(bool shift)
{
  ActiveCharacter().HandleKeyRefreshed_BackJump(shift);
}

void Weapon::HandleKeyReleased_BackJump(bool shift)
{
  ActiveCharacter().HandleKeyReleased_BackJump(shift);
}
