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
 * Snipe Rifle. Overide the Draw method in order to draw the laser beam.
 *****************************************************************************/

#include "weapon/snipe_rifle.h"
#include "weapon/weapon_cfg.h"

#include <sstream>
#include "weapon/explosion.h"
#include "character/character.h"
#include "game/game.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "include/app.h"
#include "interface/game_msg.h"
#include "map/map.h"
#include "map/camera.h"
#include "object/objects_list.h"
#include "sound/jukebox.h"
#include "team/teams_list.h"
#include "tool/resource_manager.h"


const uint SNIPE_RIFLE_BEAM_START = 5;
const uint SNIPE_RIFLE_BULLET_SPEED = 28;
const uint SNIPE_RIFLE_MAX_BEAM_SIZE = 500;
const uint SNIPE_RIFLE_FADE_BEAM_SIZE = 400;

//-----------------------------------------------------------------------------

BaseSnipeRifle::BaseSnipeRifle(Weapon_type type, const std::string &id)
  : WeaponLauncher(type, id, new ExplosiveWeaponConfig())
{
  m_category = RIFLE;

  last_angle = 0.0f;
  m_weapon_fire = new Sprite(GetResourceManager().LoadImage(weapons_res_profile,m_id+"_fire"));

  targeting_something = false;
  m_laser_image = new Sprite(GetResourceManager().LoadImage(weapons_res_profile,m_id+"_laser"));
  laser_beam_color = GetResourceManager().LoadColor(weapons_res_profile,m_id+"_laser_color");
  laser_beam_end_color = laser_beam_color;
  laser_beam_end_color.SetAlpha(0);
}

BaseSnipeRifle::~BaseSnipeRifle()
{
  delete m_laser_image;
}

bool BaseSnipeRifle::p_Shoot()
{
  if (IsOnCooldownFromShot())
    return false;

  projectile->Shoot(SNIPE_RIFLE_BULLET_SPEED);
  projectile = NULL;
  ReloadLauncher();
  return true;
}

// When an explosion occurs, we compute a new targeted point
void BaseSnipeRifle::SignalProjectileGhostState()
{
  ReloadLauncher();
  ComputeCrossPoint(true);
}

void BaseSnipeRifle::ComputeCrossPoint(bool force = false)
{
  // Did the current character is moving ?
  Point2i pos = GetGunHolePosition();
  float angle = ActiveCharacter().GetFiringAngle().tofloat();
  if (!force && last_rifle_pos==pos && last_angle==angle)
    return;
  else {
    last_rifle_pos = pos;
    last_angle = angle;
  }

  // Equation of movement : y = ax + b
  float a = sin(angle) / cos(angle);
  float b = pos.y - a*pos.x;
  Point2i delta_pos, size, start_point;
  start_point = pos;
  uint distance = 0;
  targeting_something = false;
  // While test is not finished
  float PI_3_div_4 = (float)(3*M_PI)/4;
  float PI_div_4 = (float)M_PI/4;
  while (distance < SNIPE_RIFLE_MAX_BEAM_SIZE) {
    // going upwards ( -3pi/4 < angle <-pi/4 )
    if (angle < -PI_div_4 && angle > -PI_3_div_4) {
      pos.x = (pos.y-b)/a;       //Calculate x
      delta_pos.y = -1;                   //Increment y
    // going downwards ( 3pi/4 > angle > pi/4 )
    } else if (angle > PI_div_4 && angle < PI_3_div_4) {
      pos.x = (pos.y-b)/a;       //Calculate x
      delta_pos.y = 1;                    //Increment y
    // else going at right or left
    } else {
      pos.y = a*pos.x + b;   //Calculate y
      delta_pos.x = ActiveCharacter().GetDirection();   //Increment x
    }
    // start point of the laser beam
    if (distance < SNIPE_RIFLE_BEAM_START)
      laser_beam_start = pos;

    // the point is outside the map
    if (GetWorld().IsOutsideWorldX(pos.x) || GetWorld().IsOutsideWorldY(pos.y))
      break;

    // is there a collision ??
    if (distance > 30 && !projectile->IsInVacuumXY(pos)) {
      targeting_something = true;
      break;
    }
    pos += delta_pos;
    distance = start_point.Distance(pos);
  }
  targeted_point=pos;
  return;
}

// Reset crosshair when switching from a weapon to another to avoid misused
void BaseSnipeRifle::p_Deselect()
{
  ActiveCharacter().SetFiringAngle(ZERO);
  ActiveCharacter().SetMovement("breathe");
}

void BaseSnipeRifle::DrawBeam()
{
  Double dst = laser_beam_start.Distance(targeted_point);

  // Safeguard - this should not happen though
  if (dst < EPSILON)
    return;

  Point2i cam_pos = Camera::GetInstance()->GetPosition();
  Point2i pos1    = laser_beam_start - cam_pos;
  Point2i pos2    = targeted_point - cam_pos;

  GetMainWindow().AAFadingLineColor(pos1.x, pos2.x, pos1.y, pos2.y, laser_beam_color, laser_beam_end_color);

  // Set area of the screen to be redrawn:
  // Splited into little rectangles to avoid too large area of redraw
  Double redraw_size = 20.0;
  Point2d pos = laser_beam_start;
  Point2d delta = (Point2d(targeted_point.x, targeted_point.y) - pos) * redraw_size / dst;
  Point2i delta_i = delta;

  // the Map::ToRedraw method doesn't support negative rectangles
  if (delta_i.x < 0)
    delta_i.x = - delta_i.x;
  if (delta_i.y < 0)
    delta_i.y = - delta_i.y;
  // We have to increase the size of the rectangle so the corner of the rectangles overlaps
  delta_i.x += 6;
  delta_i.y += 6;

  int i = 0;
  while (redraw_size * i < dst) {
    // Double to int conversion...
    Point2i pos_i = pos;
    if (delta.x < ZERO) {
      pos_i.x -= delta_i.x;
      pos_i.x += 3;
    }
    else
      pos_i.x -= 3;

    if (delta.y < ZERO) {
      pos_i.y -= delta_i.y;
      pos_i.y += 3;
    }
    else
      pos_i.y -= 3;

    GetWorld().ToRedrawOnMap(Rectanglei(pos_i, delta_i));
    pos += delta;
    i++;
  }
}

void BaseSnipeRifle::Draw()
{
  WeaponLauncher::Draw();
  if (Game::GetInstance()->ReadState()!=Game::PLAYING || IsOnCooldownFromShot()) return;
  ComputeCrossPoint();
  DrawBeam();
  // Draw the laser impact
  if (targeting_something)
    m_laser_image->Draw(targeted_point - (m_laser_image->GetSize()>>1));
}

//------------------------------------------------------------------------------

class SnipeBullet : public WeaponBullet
{
public:
  SnipeBullet(ExplosiveWeaponConfig& cfg, WeaponLauncher * p_launcher);
protected:
  void ShootSound();
};


SnipeBullet::SnipeBullet(ExplosiveWeaponConfig& cfg,
                         WeaponLauncher * p_launcher) :
    WeaponBullet("snipe_rifle_bullet", cfg, p_launcher)
{
}

void SnipeBullet::ShootSound()
{
  JukeBox::GetInstance()->Play("default","weapon/gun");
}

//-----------------------------------------------------------------------------

SnipeRifle::SnipeRifle()
  : BaseSnipeRifle(WEAPON_SNIPE_RIFLE, "snipe_rifle")
{
  UpdateTranslationStrings();
  ReloadLauncher();
}

void SnipeRifle::UpdateTranslationStrings()
{
  m_name = _("Sniper Rifle");
  m_help = _("Press space to shoot\nUse Up/Down to change initial angle");
}

WeaponProjectile * SnipeRifle::GetProjectileInstance()
{
  return new SnipeBullet(cfg(), this);
}

std::string SnipeRifle::GetWeaponWinString(const char *TeamName, uint items_count ) const
{
  return Format(ngettext(
            "%s team has won %u sniper rifle! Aim and shoot between the eyes!",
            "%s team has won %u sniper rifles! Aim and shoot between the eyes!",
            items_count), TeamName, items_count);
}
