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
 * Classes virtuelles permettant de définir une arme et un projectile. Les
 * armes ont un nom, une image, un état actif/inactif et une icône (affichée
 * dans l'interface). Les projectiles sont des objets physiques qui ont un
 * comportement spécial lorsqu'ils entrent en collision ou qu'ils sortent du
 * terrain.
 *****************************************************************************/

#include "weapon.h"
#include <SDL.h>
#include <SDL_rotozoom.h>
#include <SDL_gfxPrimitives.h>
#include <iostream>
#include <sstream>
#include "weapon_tools.h"
#include "../game/time.h"
#include "../game/game_loop.h"
#include "../graphic/video.h"
#include "../graphic/sprite.h"
#include "../gui/progress_bar.h"
#include "../include/app.h"
#include "../include/action_handler.h"
#include "../graphic/font.h"
#include "../interface/interface.h"
#include "../map/camera.h"
#include "../object/objects_list.h"
#include "../team/macro.h"
#include "../team/team.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../tool/math_tools.h"
#include "../tool/point.h"
#include "../tool/resource_manager.h"
#include "../tool/xml_document.h"

const int INFINITE_AMMO = -1;
const uint MAX_TIME_LOADING = 2000;

const uint UNIT_BOX_WIDTH = 22;
const uint UNIT_BOX_HEIGHT = 20;
const uint UNIT_BOX_GAP = 6;

const uint WEAPON_BOX_BUTTON_DX = 20;
const uint WEAPON_BOX_BUTTON_DY = 50;

extern WeaponStrengthBar weapon_strength_bar;


Weapon::Weapon(Weapon_type type, 
	       const std::string &id,
	       EmptyWeaponConfig * params,
	       uint visibility)
{
  m_type = type;
  m_id = id;

  m_is_active = false;

  m_initial_nb_ammo = INFINITE_AMMO;
  m_initial_nb_unit_per_ammo = 1;
  use_unit_on_first_shoot = true;
  can_be_used_on_closed_map = true;

  m_strength = 0;
  m_first_time_loading = 0;

  max_strength = min_angle = max_angle = 0;
  use_flipping = true;

  override_keys = false ;

  position.origin = weapon_origin_HAND;
  position.dx = 0;
  position.dy = 0;

  m_can_change_weapon = false;

  m_visibility = visibility;
  m_unit_visibility = ALWAYS_VISIBLE;
  extra_params = NULL;
   
  m_image = NULL;

  channel_load = -1;

  if (!use_flipping and (min_angle != max_angle))
    use_flipping = true;
  
  extra_params = params;

  if (m_visibility != NEVER_VISIBLE)
  {
    m_image = new Sprite( resource_manager.LoadImage(weapons_res_profile, m_id));
    m_image->cache.EnableLastFrameCache();
  }
     
  icone = resource_manager.LoadImage(weapons_res_profile,m_id+"_ico");
}

void Weapon::p_Select () {}
void Weapon::p_Deselect () {}
void Weapon::HandleKeyEvent(int key, int event_type) {}

void Weapon::Select() 
{  
  MSG_DEBUG("weapon", "Select %s", m_name.c_str());
  
  m_is_active = false;
  m_strength = 0;
  ActiveTeam().ResetNbUnits();
  ActiveCharacter().SetSkin("weapon-" + m_id);

  // is there a crosshair ?
  if (min_angle != max_angle) 
    ActiveTeam().crosshair.enable = true; 
     
  p_Select();

  if (max_strength == 0) return ;
  
  // prepare the strength bar
  weapon_strength_bar.InitVal (0, 0, uint(max_strength*100));

  // init stamp on the stength_bar
  double val = ActiveCharacter().previous_strength;
  weapon_strength_bar.Reset_Marqueur();
  if (0 < val && val < max_strength)
  weapon_strength_bar.AjouteMarqueur (uint(val*100), primary_red_color);
}

void Weapon::Deselect()
{
  ActiveTeam().crosshair.enable = false;
  MSG_DEBUG("weapon", "Deselect %s", m_name.c_str());
  p_Deselect();
}

void Weapon::Manage()
{
  if (!m_is_active)
    return ;

  Refresh();

  GameLoop * game_loop = GameLoop::GetInstance();

  if (game_loop->ReadState() != GameLoop::PLAYING)
    return;

  if ( (ActiveTeam().ReadNbUnits() == 0) )
    {
      Deselect();

      if (m_can_change_weapon)
	Select();
      else
	game_loop->SetState(GameLoop::HAS_PLAYED);
    }
}

bool Weapon::CanChangeWeapon() const
{
  if ( (ActiveTeam().ReadNbUnits() != m_initial_nb_unit_per_ammo) &&
       (m_can_change_weapon == false))
    return false;

  return true;
}

void Weapon::NewActionShoot() const
{
  ActionHandler::GetInstance()->NewAction (ActionDoubleInt(
				       ACTION_SHOOT,
				       m_strength,	
				       ActiveTeam().crosshair.GetAngleVal()));
}

bool Weapon::Shoot(double strength, int angle)
{
  MSG_DEBUG("weapon_shoot", "Try to shoot");

  ActiveTeam().crosshair.ChangeAngleVal(angle);
  m_strength = strength;

  MSG_DEBUG("weapon_shoot", "Enough ammo ? %d", EnoughAmmo() );
  MSG_DEBUG("weapon_shoot", "Enough ammo unit ? %d", EnoughAmmoUnit() );
  MSG_DEBUG("weapon_shoot", "Use unit on 1st shoot ? %d", use_unit_on_first_shoot );


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
	     
  MSG_DEBUG("weapon_shoot", "Enough ammo");
  
  if (!p_Shoot()) return false;
  
  MSG_DEBUG("weapon_shoot", "shoot!");

  // Is this the first shoot for this ammo use ?
  if (ActiveTeam().ReadNbUnits() == m_initial_nb_unit_per_ammo) {
    UseAmmo();
  }

  if (use_unit_on_first_shoot){
    UseAmmoUnit();
  }

  m_is_active = true;

  if (max_strength != 0) ActiveCharacter().previous_strength = m_strength;

  GameLoop::GetInstance()->character_already_chosen = true;

  return true;
}

// Calcule la position de l'image de l'arme
void Weapon::PosXY (int &x, int &y) const
{
  Point2i handPos = ActiveCharacter().GetHandPosition();
  y = handPos.y + position.dy;
  if (ActiveCharacter().GetDirection() == 1)
    x = handPos.x + position.dx;
  else {
    x = handPos.x - position.dx;
  }

  if(min_angle!=max_angle && ActiveCharacter().GetDirection()==-1)
    x -= m_image->GetWidth();
}

// Return the absolute rotation point of the weapon
void Weapon::RotationPointXY (int &x, int &y) const
{
  PosXY(x,y);

  x += m_image->GetWidth()/2;
  y += m_image->GetHeight()/2;
}

bool Weapon::EnoughAmmo() const
{
  int ammo = ActiveTeam().ReadNbAmmos();
  return ((ammo == INFINITE_AMMO) || (0 < ammo));
}

void Weapon::UseAmmo()
{
  // Use one ammo...
  int *ammo = &ActiveTeam().AccessNbAmmos();
  if (*ammo != INFINITE_AMMO) (*ammo)--;

  assert (*ammo >= 0 || *ammo == INFINITE_AMMO);
}

bool Weapon::EnoughAmmoUnit() const
{
  int unit = ActiveTeam().ReadNbUnits();
  return (unit > 0);
}

void Weapon::UseAmmoUnit()
{
  // Use one ammo unit.
  int *unit = &ActiveTeam().AccessNbUnits();
  (*unit)--;

  assert (unit >= 0);
}

int Weapon::ReadInitialNbAmmo() const{
  return m_initial_nb_ammo;
}

int Weapon::ReadInitialNbUnit() const{
  return m_initial_nb_unit_per_ammo;
}

bool Weapon::CanBeUsedOnClosedMap() const{
  return can_be_used_on_closed_map;
}

const std::string& Weapon::GetName() const { 
  assert (!m_name.empty());
  return m_name;
}

const std::string& Weapon::GetID() const { 
  assert (!m_name.empty());
  return m_id;
}

Weapon_type Weapon::GetType() const { 
  return m_type;
}

void Weapon::UpdateStrength(){
  if( max_strength == 0 || m_first_time_loading == 0 )
    return ;
  
  uint time = Time::GetInstance()->Read() - m_first_time_loading;
  double val = (max_strength * time) / MAX_TIME_LOADING;

  m_strength = BorneDouble (val, 0.0, max_strength);

  weapon_strength_bar.Actu ((int)(m_strength*100));
}

bool Weapon::IsReady() const{
  return EnoughAmmo() ;
}

void Weapon::InitLoading(){
  // no loading for weapon with max_strength = 0
  if (max_strength == 0)
    return ;

  channel_load = jukebox.Play("share","weapon/load");
   
  m_first_time_loading = Time::GetInstance()->Read();
  
  m_strength = 0;

  GameLoop::GetInstance()->character_already_chosen = true;
}

void Weapon::StopLoading(){
  m_first_time_loading = 0 ;

  jukebox.Stop(channel_load);
}

void Weapon::DrawWeaponBox(){
  int c_x;
  int c_y;

  c_x =  + BUTTON_ICO_WIDTH / 2 + WEAPON_BOX_BUTTON_DX;
  c_y =  + BUTTON_ICO_HEIGHT / 2 + WEAPON_BOX_BUTTON_DY;

  AppWormux * app = AppWormux::GetInstance();

  Point2i dest( (int)(c_x - 0.5 * BUTTON_ICO_WIDTH), (int)(c_y - 0.5 * BUTTON_ICO_HEIGHT));
  app->video.window.Blit( Interface::GetInstance()->weapon_box_button, dest);

  Point2i  dr2( (int)(c_x - 0.5 * WEAPON_ICO_WIDTH), (int)(c_y - 0.5 * WEAPON_ICO_HEIGHT));
  app->video.window.Blit( icone, dr2);
}

void Weapon::Draw(){
  if(GameLoop::GetInstance()->ReadState() != GameLoop::PLAYING)
    return;

  weapon_strength_bar.visible = false;

  switch (m_unit_visibility)
    {
      case VISIBLE_ONLY_WHEN_ACTIVE:
	if (!m_is_active)
	  break;

      default:
	if (m_initial_nb_unit_per_ammo > 1)
	  DrawUnit(ActiveTeam().ReadNbUnits());
    }

  // Do we need to draw strength_bar ? (real draw is done by class Interface
  if (max_strength != 0 && IsReady() && !m_is_active)
    weapon_strength_bar.visible = true;

  switch (m_visibility)
    {
      case ALWAYS_VISIBLE:
	break ;
	
      case NEVER_VISIBLE:
	return ;
	
      case VISIBLE_ONLY_WHEN_ACTIVE:
	if (!m_is_active)
	  return ;
	break;
	
      case VISIBLE_ONLY_WHEN_INACTIVE:
	if (m_is_active)
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

  // rotate weapon if needed
  if (min_angle != max_angle)
  {
    if(ActiveCharacter().GetDirection() == 1)
      m_image->SetRotation_deg (ActiveTeam().crosshair.GetAngle());
    else
      m_image->SetRotation_deg (ActiveTeam().crosshair.GetAngle() - 180.0);
  }

  // flip image if needed
  if (use_flipping)
  {
    m_image->Scale(ActiveCharacter().GetDirection(), 1.0);
  }

  // Calculate position of the image
  int x,y;
  switch (position.origin) 
  {
  case weapon_origin_OVER:
    x = ActiveCharacter().GetCenterX()-m_image->GetWidth()/2+position.dx;
    y = ActiveCharacter().GetY()-m_image->GetHeight()+position.dy;
     break;
  case weapon_origin_HAND:
    PosXY (x, y);
     break;
  }
  if ( m_image )
    m_image->Blit( AppWormux::GetInstance()->video.window, Point2i(x, y) - camera.GetPosition());
}

void Weapon::DrawUnit(int unit){
  Rectanglei rect;

  std::ostringstream ss;

  ss << unit;
 
  DrawTmpBoxText(*Font::GetInstance(Font::FONT_SMALL),
		 Point2i( ActiveCharacter().GetCenterX(), ActiveCharacter().GetY() - UNIT_BOX_HEIGHT / 2 - UNIT_BOX_GAP )
		 - camera.GetPosition(),
		 ss.str());
}

bool Weapon::LoadXml(xmlpp::Element * weapon) 
{
  xmlpp::Element *elem = LitDocXml::AccesBalise (weapon, m_id);
  if (elem == NULL)
  {
      std::cout << Format(_("No element <%s> found in the xml config file!"),
                          m_id.c_str())
                << std::endl;
    return false;
  }

  xmlpp::Element *pos_elem = LitDocXml::AccesBalise (elem, "position");
  if (pos_elem != NULL) {
    // E.g. <position origin="hand" x="-1" y="0" />
    std::string origin;
    LitDocXml::LitAttrInt (pos_elem, "x", position.dx);
    LitDocXml::LitAttrInt (pos_elem, "y", position.dy);
    LitDocXml::LitAttrString (pos_elem, "origin", origin);
    if (origin == "over")
      position.origin = weapon_origin_OVER;
    else
      position.origin = weapon_origin_HAND;
  }

  LitDocXml::LitInt (elem, "nb_ammo", m_initial_nb_ammo);       
  LitDocXml::LitInt (elem, "unit_per_ammo", m_initial_nb_unit_per_ammo);

  // max strength
  // if max_strength = 0, no strength_bar !
  LitDocXml::LitDouble (elem, "max_strength", max_strength);

  // change weapon after ? (for the ninja cord = true)
  LitDocXml::LitBool (elem, "change_weapon", m_can_change_weapon);
      
  // angle of weapon when drawing
  // if (min_angle == max_angle) no cross_hair !
  // between -90 to 90 degrees
  LitDocXml::LitInt (elem, "min_angle", min_angle);
  LitDocXml::LitInt (elem, "max_angle", max_angle);

  // Load extra parameters if existing
  if (extra_params != NULL) extra_params->LoadXml(elem);

  return true;
}

bool Weapon::IsActive() const{
  return m_is_active;
}

const double Weapon::ReadStrength() const{
  return m_strength;
}

bool Weapon::IsLoading() const{
  return m_first_time_loading;
}

void Weapon::ChooseTarget(){
}

void Weapon::SignalTurnEnd(){
}

