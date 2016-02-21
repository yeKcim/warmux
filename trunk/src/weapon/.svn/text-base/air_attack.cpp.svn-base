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
 * Air attack.
 *****************************************************************************/

#include <sstream>

#include "character/character.h"
#include "game/game_time.h"
#include "graphic/sprite.h"
#include "include/action_handler.h"
#include "interface/mouse.h"
#include "map/map.h"
#include "map/camera.h"
#include "network/network.h"
#include "network/randomsync.h"
#include "object/objects_list.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
#include "weapon/air_attack.h"
#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"


static const int FORCE_X_MIN = -50;
static const uint FORCE_X_MAX = 0;
static const uint FORCE_Y_MIN = 1;
static const uint FORCE_Y_MAX = 40;

// XXX Unused ?
//const Double OBUS_SPEED = 7 ;

class AirAttackConfig : public ExplosiveWeaponConfig
{
public:
  Double speed;
  uint nbr_obus;
  AirAttackConfig()
  {
    push_back(new UintConfigElement("nbr_obus", &nbr_obus, 3, 1, 8));
    push_back(new DoubleConfigElement("speed", &speed, 3));
  }
};

Obus::Obus(AirAttackConfig& cfg) :
  WeaponProjectile("air_attack_projectile", cfg, NULL)
{
  explode_colliding_character = true;
  falling_sound.Play("default", "weapon/aircraft_bomb_falling");
}

Obus::~Obus()
{
  falling_sound.Stop();

  if (Plane::last_dropped_bomb == this)
    Plane::last_dropped_bomb = NULL;
}

//-----------------------------------------------------------------------------

Obus* Plane::last_dropped_bomb = NULL;

Plane::Plane(AirAttackConfig &p_cfg) :
  PhysicalObj("air_attack_plane"),
  cfg(p_cfg)
{
  SetCollisionModel(false, false, false);

  image = GetResourceManager().LoadSprite(weapons_res_profile, "air_attack_plane");
  image->EnableCaches(true, 0);
  SetSize(image->GetSize());
  obus_dx = 100;
  obus_dy = GetY() + GetHeight();

  flying_sound.Play("default", "weapon/aircraft_flying");
}

Plane::~Plane()
{
  flying_sound.Stop();
}

void Plane::Shoot(Double speed, const Point2i& target)
{
  MSG_DEBUG("weapon.shoot", "Plane Shoot");
  nb_dropped_bombs = 0;
  last_dropped_bomb = NULL;

  cible_x = target.x;
  SetY(0);
  distance_to_release = (int)(speed * sqrt(TWO * (GetY() + target.y)));

  Point2d speed_vector;
  if (ActiveCharacter().GetDirection() == DIRECTION_RIGHT) {
    image->SetFlipped(false);
    speed_vector.SetValues(speed, 0);
    SetX(1 - image->GetWidth());
    //distance_to_release -= obus_dx;
    if (distance_to_release > cible_x)
      distance_to_release = 0;
  } else {
    image->SetFlipped(true);
    speed_vector.SetValues(-speed, 0) ;
    SetX(GetWorld().GetWidth() - 1);
    //distance_to_release += obus_dx;
    if (distance_to_release > GetWorld().GetWidth()-cible_x - obus_dx)
      distance_to_release = 0;
  }

  SetSpeedXY(speed_vector);

  Camera::GetRef().FollowObject(this);

  ObjectsList::GetRef().AddObject(this);
}

void Plane::DropBomb()
{
  Obus * instance = new Obus(cfg);
  instance->SetXY(Point2i(GetX(), obus_dy));

  Point2d speed_vector = GetSpeedXY();

  int fx = RandomSync().GetInt(FORCE_X_MIN, FORCE_X_MAX);
  fx    *= GetDirection();
  int fy = RandomSync().GetInt(FORCE_Y_MIN, FORCE_Y_MAX);

  static const Double factor = 1/30.0;
  speed_vector.SetValues(speed_vector.x + fx*factor, speed_vector.y + fy*factor);
  instance->SetSpeedXY(speed_vector);

  ObjectsList::GetRef().AddObject(instance);

  last_dropped_bomb = instance;
  nb_dropped_bombs++;

  if (nb_dropped_bombs == 1)
    Camera::GetRef().FollowObject(instance);
}

void Plane::Refresh()
{
  UpdatePosition();
  image->Update();
  // First shoot !!
  if (OnTopOfTarget() && !nb_dropped_bombs) {
    DropBomb();
    m_ignore_movements = true;
    MSG_DEBUG("random.get", "Plane::Refresh() first bomb");
    next_height = RandomSync().GetInt(20,100);
  } else if (nb_dropped_bombs &&  nb_dropped_bombs < cfg.nbr_obus) {
    // Get the last rocket and check the position to be sure to not collide with it
    if (!last_dropped_bomb || last_dropped_bomb->GetY() > GetY()+GetHeight()+next_height) {
      MSG_DEBUG("random.get", "Plane::Refresh() another bomb");
      next_height = RandomSync().GetInt(20, 100);
      DropBomb();
    }
  }
}

int Plane::GetDirection() const
{
  return image->IsFlipped() ? -1 : 1;
}

void Plane::Draw()
{
  if (IsGhost())
    return;
  image->Draw(GetPosition());
}

bool Plane::OnTopOfTarget() const
{
  if (GetDirection() == 1)
    return (cible_x <= GetX() + distance_to_release);
  else
    return (GetX() - (int)image->GetWidth() + obus_dx - distance_to_release <= cible_x);
}

//-----------------------------------------------------------------------------

AirAttack::AirAttack() :
  Weapon(WEAPON_AIR_ATTACK, "air_attack",new AirAttackConfig())//, plane(cfg())
{
  UpdateTranslationStrings();

  m_category = HEAVY;
  mouse_character_selection = false;
  can_be_used_on_closed_map = false;
  target_chosen = false;
  m_time_between_each_shot = 100;
}

void AirAttack::UpdateTranslationStrings()
{
  m_name = _("Air Attack");
  m_help = _("attack direction: Left/Right\nBombing: Left click on the target\nOne bombing per turn");
}

void AirAttack::ChooseTarget(Point2i mouse_pos)
{
  target = mouse_pos;
  target_chosen = true;
  Shoot();
}

bool AirAttack::p_Shoot()
{
  MSG_DEBUG("weapon.shoot", "AirAttack p_Shoot");
  if (!target_chosen)
    return false;

  // Go back to default cursor
  if (Network::GetInstance()->IsTurnMaster())
      Mouse::GetInstance()->SetPointer(Mouse::POINTER_SELECT);

  Plane * plane = new Plane(cfg());
  plane->Shoot(cfg().speed, target);
  // The plane instance is in fact added to an objects list,
  // which will delete it for us when needed.

  return true;
}

void AirAttack::p_Select()
{
  if (Network::GetInstance()->IsTurnMaster())
    Mouse::GetInstance()->SetPointer(Mouse::POINTER_ATTACK);
}

AirAttackConfig& AirAttack::cfg()
{
  return static_cast<AirAttackConfig&>(*extra_params);
}

std::string AirAttack::GetWeaponWinString(const char *TeamName, uint items_count) const
{
  return Format(ngettext("%s team has won %u air attack!",
                         "%s team has won %u air attacks!",
                         items_count), TeamName, items_count);
}
