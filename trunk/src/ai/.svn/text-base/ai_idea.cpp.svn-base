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
 * A factory for AI strategies. It contains no turn specfic data.
 *****************************************************************************/

#include <WARMUX_random.h>
#include "ai/ai_idea.h"
#include "ai/trajectory.h"
#include "character/character.h"
#include "game/game.h"
#include "game/game_mode.h"
#include "map/map.h"
#include "map/wind.h"
#include "object/objects_list.h"
#include "team/team.h"
#include "team/macro.h"
#include "tool/math_tools.h"
#include "weapon/explosion.h"
#include "weapon/weapon_launcher.h"
#include "weapon/weapons_list.h"
#include "weapon/shotgun.h"

// This constant defines how how much damage is worth killing one character?
// e.g. Killing one Character with 20 health is about the same worth like doing a sum of 120 damage (60 each) to two characters without killing them. Both cases would get a rating of 120 when this constant is 100.
const float BONUS_FOR_KILLING_CHARACTER = 100;
const float MALUS_PER_UNUSED_DAMGE_POINT = 0.1f;
const float MIN_GROUND_BONUS = 0.1f;
const float MAX_GROUND_BONUS = 1.0f;
const float GROUND_BONUS_RANGE = 2000.0f;
// At the time this code has been written the
// bazooka did about 30-60 additional damage at 2500 force
const float MIN_DAMAGE_PER_FORCE_UNIT = 30.0f/2500.0f;
const float MAX_DAMAGE_PER_FORCE_UNIT = 60.0f/2500.0f;

bool AIIdea::CanUseWeapon(const Weapon * weapon)
{
  bool correct_weapon = weapon == &(ActiveTeam().GetWeapon());
  bool can_change_weapon = ActiveTeam().GetWeapon().CanChangeWeapon()
    && (Game::GetInstance()->ReadState() == Game::PLAYING);
  return correct_weapon || (can_change_weapon && weapon->EnoughAmmo());
}

bool AIIdea::CanUseCharacter(const Character & character)
{
  if (character.IsDead())
    return false;

  bool can_change_character = GameMode::GetInstance()->AllowCharacterSelection()
    && (Game::GetInstance()->ReadState() == Game::PLAYING)
    && !Game::GetInstance()->IsCharacterAlreadyChosen();
  return (character.IsActiveCharacter() || can_change_character);
}

float AIIdea::GetDirectionRelativeAngle(LRDirection direction, float angle)
{
  return (direction == DIRECTION_LEFT) ? InverseAngleRad(angle) : angle;
}

float AIIdea::RateDamageDoneToEnemy(int damage, const Character & enemy)
{
  float rating = std::min(damage, enemy.GetEnergy());
  if (damage >= enemy.GetEnergy()) {
    rating += BONUS_FOR_KILLING_CHARACTER;
    float unused_damage = damage - enemy.GetEnergy();
    rating -= MALUS_PER_UNUSED_DAMGE_POINT * unused_damage;
  }
  return rating;
}

float AIIdea::RateDamageDoneToEnemy(int min_damage, int max_damage, const Character & enemy)
{
  float min_rating = RateDamageDoneToEnemy(min_damage, enemy);
  float max_rating = RateDamageDoneToEnemy(max_damage, enemy);
  return (min_rating + max_rating)* 0.5f;
}

float AIIdea::RateExplosion(const Character & shooter, const Point2i& position,
                            const ExplosiveWeaponConfig& config,
                            const float& expected_additional_distance)
{
  float rating = 0.0f;

  FOR_ALL_LIVING_CHARACTERS(team, character) {
    float distance = position.Distance(character->GetCenter());
    distance += expected_additional_distance;
    if (distance < 1.0f)
      distance = 1.0f;
    Double Dist = distance;
    float min_damage = GetDamageFromExplosion(config, Dist);
    float max_damage = min_damage;
    if (Dist <= config.blast_range) {
      float force = GetForceFromExplosion(config, Dist).tofloat();
      min_damage += MIN_DAMAGE_PER_FORCE_UNIT * force;
      max_damage += MAX_DAMAGE_PER_FORCE_UNIT * force;
    }
    bool is_friend = shooter.GetTeamIndex() == character->GetTeamIndex();
    if (is_friend) {
      rating -= RateDamageDoneToEnemy(min_damage, max_damage, *character);
    } else {
      rating += RateDamageDoneToEnemy(min_damage, max_damage, *character);
    }
  }
  return rating;
}

AIStrategy * SkipTurnIdea::CreateStrategy(float) const
{
  const WeaponsList * weapons_list = Game::GetInstance()->GetWeaponsList();
  const Weapon * weapon = weapons_list->GetWeapon(Weapon::WEAPON_SKIP_TURN);
  if (!CanUseWeapon(weapon))
    return NULL;
  return new SkipTurnStrategy();
}

AIStrategy * WasteAmmoUnitsIdea::CreateStrategy(float) const
{
  if (ActiveTeam().GetWeapon().CanChangeWeapon())
    return NULL;
  Weapon::Weapon_type weapon_type = ActiveTeam().GetWeapon().GetType();
  int used_ammo_units = ActiveTeam().ReadNbUnits(weapon_type);
  float max_angle = -ActiveTeam().GetWeapon().GetMinAngle().tofloat();
  return new ShootWithGunStrategy(-0.1f, ActiveCharacter(), weapon_type,
                                  ActiveCharacter().GetDirection(), max_angle, used_ammo_units);
}

bool AIShootIdea::NoLongerPossible() const
{
  return shooter.IsDead() || enemy.IsDead();
}

float AIShootIdea::GetMaxRating(bool one_shot) const
{
  const WeaponsList *weapons_list = Game::GetInstance()->GetWeaponsList();
  const WeaponLauncher *weapon = weapons_list->GetWeaponLauncher(weapon_type);
  int damage = weapon->GetDamage();
  int units = (one_shot) ? 1 : weapon->ReadInitialNbUnit();

  if (weapon_type == Weapon::WEAPON_SHOTGUN)
    damage *= SHOTGUN_BULLETS;

  return weapons_weighting.GetFactor(weapon_type)*damage*units;
}

ShootDirectlyAtEnemyIdea::ShootDirectlyAtEnemyIdea(const WeaponsWeighting & weapons_weighting,
                                                   const Character & shooter, const Character & enemy,
                                                   Weapon::Weapon_type weapon_type,
                                                   int max_distance)
  : AIShootIdea(weapons_weighting, shooter, enemy, weapon_type)
  , max_sq_distance(max_distance*max_distance)
{
}

static const PhysicalObj* GetObjectAt(const Point2i & pos)
{
  const ObjectsList * objects = ObjectsList::GetConstInstance();
  ObjectsList::const_iterator it = objects->begin();
  while(it != objects->end()) {
    const PhysicalObj* object = *it;
    if (object->GetTestRect().Contains(pos) && !object->IsDead())
      return object;
    it++;
  }

  FOR_ALL_CHARACTERS(team, character) {
    if (character->GetTestRect().Contains(pos) && !character->IsDead())
      return &(*character);
  }

  return NULL;
}

static bool ObjectLiesOnSegment(const PhysicalObj* object,
                                const Point2i& from, const Point2i& to)
{
  const Rectanglei& r = object->GetTestRect();
  const Point2i& center = object->GetCenter();

  if (from.y == to.y) {
    // Horizontal shot
    return r.Contains(Point2i(center.x, to.y));
  } else if (from.x == to.x) {
    // Vertical shot
    return r.Contains(Point2i(to.x, center.y));
  }

  // Are the ordinates within the segment
  if ((center.x<from.x && center.x<to.x) || (center.x>from.x && center.x>to.x) ||
      (center.y<from.y && center.y<to.y) || (center.y>from.y && center.y>to.y))
    return false;

  // Find point on corresponding line
  int y = from.y + ((center.x-from.x)*(to.y-from.y))/(to.x-from.x);
  return r.Contains(Point2i(center.x, y));
}


/* Returns the object the missile has collided with or NULL if the missile has collided with the ground. */
static bool ShotMisses(const Character *shooter, const Character *enemy,
                       const Point2i& from, const Point2i& to) {
  Point2i pos = from;
  Point2i delta = to - from;

  const ObjectsList * objects = ObjectsList::GetConstInstance();
  ObjectsList::const_iterator it = objects->begin();
  while(it != objects->end()) {
    const PhysicalObj* object = *it;
    if (!object->IsDead() && ObjectLiesOnSegment(object, from, to))
      return true;
    it++;
  }

  FOR_ALL_CHARACTERS(team, character) {
    const PhysicalObj* object = &(*character);
    if (object!=shooter && object!=enemy && !object->IsDead() &&
        ObjectLiesOnSegment(object, from, to))
      return true;
  }

  int steps_x = abs(delta.x);
  int steps_y = abs(delta.y);
  int step_x = delta.x < 0 ? -1 : 1;
  int step_y = delta.y < 0 ? -1 : 1;
  int done_x_mul_steps_y  = 0;
  int done_y_mul_steps_x  = 0;
  // explanation of done_x_mul_steps_y:
  // done_x = how often has step_x been added to pos.x
  // done_x_mul_steps_y = done_x * steps_y
  // example of algorithm:
  // given: departure = (0,0) arrival = (-2,7);
  // => steps_x = 2; steps_y = 7; step_x = -1; step_y = 1
  // progress:
  // pos.x | pos.y | done_x_mul_steps_y | done_y_mul_steps_x
  // -------------------------------------------------------
  // 0     | 0     | 0                  | 0
  // 0     | 1     | 0                  | 2
  // -1    | 2     | 7                  | 4
  // -1    | 3     | 7                  | 6
  // -1    | 4     | 7                  | 8
  // -1    | 5     | 7                  | 10
  // -2    | 6     | 14                 | 12
  // -2    | 7     | 14                 | 14
  // The algorithm tries to keep the difference small between:
  // done_x_mul_steps_y and done_y_mul_steps_x.
  // By doing so it gets assured that all intermediate positions pos form a straight line.
  // (Or at least something close to a straight line)
  while (pos != to) {
    int new_done_x_mul_steps_y = done_x_mul_steps_y + steps_y;
    int new_done_y_mul_steps_x = done_y_mul_steps_x + steps_x;
    int diff_after_step_x = abs(done_y_mul_steps_x - new_done_x_mul_steps_y);
    int diff_after_step_y = abs(new_done_y_mul_steps_x - done_x_mul_steps_y);

    if (diff_after_step_x <= diff_after_step_y) {
      pos.x += step_x;
      done_x_mul_steps_y = new_done_x_mul_steps_y;
    }
    if (diff_after_step_y <= diff_after_step_x) {
      pos.y += step_y;
      done_y_mul_steps_x = new_done_y_mul_steps_x;
    }

    if (GetWorld().IsOutsideWorld(pos))
      return true;

    if (!GetWorld().IsInVacuum(pos))
      return true;
  }
  return false;
}

AIStrategy * ShootDirectlyAtEnemyIdea::CreateStrategy(float accuracy) const {
  if (enemy.IsDead())
    return NULL;

  if (!CanUseCharacter(shooter))
    return NULL;

  const WeaponsList *weapons_list = Game::GetInstance()->GetWeaponsList();
  const WeaponLauncher *weapon = weapons_list->GetWeaponLauncher(weapon_type);

  if (!CanUseWeapon(weapon))
    return NULL;

  // We need to use center point, because gunholePosition is location
  // of last weapon of the ActiveTeam() and not the future gunholePos
  // which will be select.
  // TODO: Please find an alternative to solve this tempory solution
  Point2i departure = shooter.GetCenter();
  Point2i arrival   = enemy.GetCenter();
  int     sq_dist   = departure.SquareDistance(arrival);

  if (sq_dist > max_sq_distance)
    return NULL;

  float original_angle = departure.ComputeAngleFloat(arrival);

  LRDirection direction = XDeltaToDirection(arrival.x - departure.x);
  float shoot_angle = GetDirectionRelativeAngle(direction, original_angle);

  if (!weapon->IsAngleValid(shoot_angle))
    return NULL;

  if (ShotMisses(&shooter, &enemy, departure, arrival))
    return NULL;

  int available_ammo_units = ActiveTeam().ReadNbUnits(weapon_type);

  int damage_per_ammo_unit = weapon->GetDamage();
  if (weapon_type == Weapon::WEAPON_SHOTGUN) {
    damage_per_ammo_unit *= SHOTGUN_BULLETS;
  }
  int required_ammo_units = (enemy.GetEnergy() + damage_per_ammo_unit -1)
                          / damage_per_ammo_unit;
  int used_ammo_units = std::min(required_ammo_units, available_ammo_units);
  int damage = used_ammo_units * damage_per_ammo_unit;

  float rating = RateDamageDoneToEnemy(damage, enemy);
  float confidence = sqrtf((max_sq_distance - sq_dist) / (float)max_sq_distance);
  rating *= confidence * weapons_weighting.GetFactor(weapon_type);

  // Apply our accuracy
  if (accuracy>0.0f && accuracy<1.0f) {
    // stddev is 0 for accuracy and increases when it decreases
    shoot_angle += RandomLocal().GetGaussianfloat(0.0f, (1-accuracy)*M_PI*0.1f);
    // Revalidate value
    if (!weapon->IsAngleValid(shoot_angle))
      return NULL;
  }

  return new ShootWithGunStrategy(rating, shooter, weapon_type, direction,
                                  shoot_angle, used_ammo_units);
}

FireMissileWithFixedDurationIdea::FireMissileWithFixedDurationIdea(const WeaponsWeighting & weapons_weighting,
                                                                   const Character & shooter, const Character & enemy,
                                                                   Weapon::Weapon_type weapon_type,
                                                                   float duration, int timeout)
  : AIShootIdea(weapons_weighting, shooter, enemy, weapon_type)
  , duration(duration)
  , timeout(timeout)
  // Weight for the rating according to distance/flight duration
  , confidence((duration > 1.0f) ? 1.0f / duration : 1.0f)
{
}

static bool IsPositionEmpty(const Character & character_to_ignore,
                            const Point2i& pos, const PhysicalObj** object)
{
  *object = NULL;
  if (GetWorld().IsOutsideWorld(pos))
    return false;

  if (!GetWorld().IsInVacuum(pos))
    return false;

  *object = GetObjectAt(pos);
  if (*object != NULL && *object != &character_to_ignore)
    return false;
  *object = NULL;
  return true;
}

#define STEP_IN_PIXEL 2

static const Point2i GetFirstContact(const Character & character_to_ignore,
                                     const Trajectory & trajectory,
                                     const PhysicalObj** object)
{
  float time = 0;
  Point2i pos;
  do {
    pos = trajectory.GetPositionAt(time);
    float pixel_per_second = trajectory.GetSpeedAt(time);
    time += STEP_IN_PIXEL / pixel_per_second;
  } while (IsPositionEmpty(character_to_ignore, pos, object));
  return pos;
}

AIStrategy * FireMissileWithFixedDurationIdea::CreateStrategy(float accuracy) const
{
  if (enemy.IsDead())
    return NULL;

  if (!CanUseCharacter(shooter))
    return NULL;

  const WeaponsList * weapons_list = Game::GetInstance()->GetWeaponsList();
  const WeaponLauncher * weapon = weapons_list->GetWeaponLauncher(weapon_type);

  if (!CanUseWeapon(weapon))
    return NULL;
  float g = GameMode::GetInstance()->gravity;
  float wind_factor = weapon->GetWindFactor().tofloat();
  float mass = weapon->GetMass().tofloat();
  Point2f f(Wind::GetRef().GetStrength().tofloat() * wind_factor, g * mass);
  Point2f a = f / mass * PIXEL_PER_METER;
  const Point2f pos_0 = shooter.GetCenter();
  const Point2f pos_t = enemy.GetCenter();
  float t = duration;
  // Calculate v_0 using "pos_t = 1/2 * a_x * t*t + v_0*t + pos_0":
  Point2f v_0 = (pos_t - pos_0)/t - a/2 * t;

  float strength = v_0.Norm() / PIXEL_PER_METER;
  float angle = v_0.ComputeAngleFloat();
  LRDirection direction = XDeltaToDirection(v_0.x<0);
  float shoot_angle = GetDirectionRelativeAngle(direction, angle);
  if (!weapon->IsAngleValid(shoot_angle))
    return NULL;

  if (strength > weapon->GetMaxStrength().tofloat())
    return NULL;

  Trajectory trajectory(pos_0, v_0, a);
  const PhysicalObj * aim;
  Point2i explosion_pos = GetFirstContact(shooter, trajectory, &aim);
  float rating;
  bool explodes_on_contact = weapon_type == Weapon::WEAPON_BAZOOKA;
  if (aim == &enemy || explodes_on_contact) {
    float expected_additional_distance = explodes_on_contact ? 0.0f : 30.0f;
    rating = RateExplosion(shooter, explosion_pos, weapon->cfg(), expected_additional_distance);

    // Explosions remove ground and make it possible to hit the characters behind the ground.
    // That is why ground hits get rewared with a small positive rating.
    if (explodes_on_contact) {
      float distance = explosion_pos.Distance(enemy.GetCenter());
      // Give more bonus if the explosion is near the target.
      // This will make the AI focus on one character
      float ground_bonus = max(MIN_GROUND_BONUS, MAX_GROUND_BONUS - distance/GROUND_BONUS_RANGE);
      rating += ground_bonus;
    }
  } else {
    return NULL;
  }
  rating *= confidence * weapons_weighting.GetFactor(weapon_type);

  // Apply our accuracy
  if (accuracy>0.0f && accuracy<1.0f) {
    // stddev is 0 for accuracy and increases when it decreases
    // stddev smaller compared to straight shot because it can more easily fail
    shoot_angle += RandomLocal().GetGaussianfloat(0.0f, (1-accuracy)*M_PI*0.1f);
    // Revalidate value
    if (!weapon->IsAngleValid(shoot_angle))
      return NULL;
  }

  return new LoadAndFireStrategy(rating, shooter, weapon_type, direction, shoot_angle, strength, timeout);
}
