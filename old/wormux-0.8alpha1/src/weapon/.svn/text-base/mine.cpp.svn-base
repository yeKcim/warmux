/******************************************************************************
 *  Wormux is a convivial mass murder game.
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
 * Mine : Detect if character is close and explode after a shot time.
 * Sometime the mine didn't explode randomly.
 *****************************************************************************/

#include "mine.h"
#include <iostream>
#include <sstream>
#include "explosion.h"
#include "../game/config.h"
#include "../game/time.h"
#include "../graphic/sprite.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../interface/game_msg.h"
#include "../map/map.h"
#include "../object/objects_list.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "../network/randomsync.h"
#include "../tool/resource_manager.h"

#ifdef __MINGW32__
#undef LoadImage
#endif

const double DEPART_FONCTIONNEMENT = 5;

ObjMine::ObjMine(MineConfig& cfg,
                 WeaponLauncher * p_launcher) :
  WeaponProjectile("mine", cfg, p_launcher)
{
  m_allow_negative_y = true; 
  animation = false;
  channel = -1;
  is_active = true;
  explode_with_collision = false;

  escape_time = 0;

  // is it a fake mine ?
  fake = !(randomSync.GetLong(0, 9));
}

void ObjMine::FakeExplosion()
{
  MSG_DEBUG("mine", "Fake explosion");

  jukebox.Play("share", "weapon/mine_fake");
  ParticleEngine::AddNow(GetPosition(), 5, particle_SMOKE, true);

  if ( animation )
  {
    MSG_DEBUG("mine", "Desactive detection..");

    animation = false;
    image->SetCurrentFrame(0);
  }
  if (launcher != NULL) launcher->SignalProjectileTimeout();
  // Mine fall into the ground after a fake explosion
  SetCollisionModel(false, false, false);
}

void ObjMine::StartTimeout()
{
  if (!animation)
  {
    animation=true;
    MSG_DEBUG("mine", "EnableDetection - CurrentTime : %d",Time::GetInstance()->ReadSec() );
    attente = Time::GetInstance()->ReadSec() + cfg.timeout;
    MSG_DEBUG("mine", "EnableDetection : %d", attente);

    channel = jukebox.Play("share", "weapon/mine_beep", -1);
  }
}

void ObjMine::Detection()
{
  uint current_time = Time::GetInstance()->ReadSec();

  if (escape_time == 0)
  {
    escape_time = current_time + static_cast<MineConfig&>(cfg).escape_time;
    MSG_DEBUG("mine", "Initialize escape_time : %d", current_time);
    return;
  }

  if (current_time < escape_time) return;

  MSG_DEBUG("mine", "Escape_time is finished : %d", current_time);

  FOR_ALL_LIVING_CHARACTERS(equipe, ver)
  { 
    if (MeterDistance (GetCenter(), ver->GetCenter()) < static_cast<MineConfig&>(cfg).detection_range
        && !animation)
    {
      std::string txt = Format(_("%s is next to a mine!"),
                                  ver -> GetName().c_str());
      GameMessages::GetInstance()->Add (txt);
      StartTimeout();
      return;
    }
  }
}

void ObjMine::Refresh()
{
  // the mine is now out of the map
  // or it's a fake mine that has already exploded!
  if (!is_active)
  {
    jukebox.Stop(channel);
    channel = -1;
    escape_time = 0;
    return;
  }

  // try to detect a character near the mine
  if (!animation)
  {
    Detection();
  }
  else 
  {
    image->Update();

    // the timeout is finished !!
    if (attente < Time::GetInstance()->ReadSec())
    {
      is_active = false;
      jukebox.Stop(channel);
      channel = -1;
      if (!fake) Explosion();
      else FakeExplosion();
      if (launcher != NULL) launcher->SignalProjectileTimeout();
    }
  }
}

bool ObjMine::IsImmobile() const
{
  if (is_active && animation) return false;
  return WeaponProjectile::IsImmobile();
}

void ObjMine::Draw()
{
  image->Draw(GetPosition());
}
//-----------------------------------------------------------------------------

MineConfig * MineConfig::singleton = NULL;

MineConfig * MineConfig::GetInstance() 
{
  if (singleton == NULL) {
    singleton = new MineConfig();
  }
  return singleton;
}

//-----------------------------------------------------------------------------

Mine::Mine() : WeaponLauncher(WEAPON_MINE, "minelauncher", MineConfig::GetInstance(), VISIBLE_ONLY_WHEN_INACTIVE)
{
  m_name = _("Mine");
  ReloadLauncher();
}

WeaponProjectile * Mine::GetProjectileInstance()
{
  return dynamic_cast<WeaponProjectile *>
      (new ObjMine(cfg(), dynamic_cast<WeaponLauncher *>(this)));
}

bool Mine::p_Shoot()
{
  int x,y;
  PosXY (x,y);
  Add (x, y);

  return true;
}

void Mine::Add (int x, int y)
{
  projectile -> SetXY ( Point2i(x, y) );
  projectile -> SetOverlappingObject(&ActiveCharacter());

  Point2d speed_vector;
  ActiveCharacter().GetSpeedXY(speed_vector);
  projectile -> SetSpeedXY (speed_vector);
  lst_objects.AddObject (projectile);
  projectile = NULL;
  ReloadLauncher();
}

MineConfig& Mine::cfg()
{
  return static_cast<MineConfig&>(*extra_params);
}

MineConfig::MineConfig()
{
  detection_range= 1;
  timeout = 3;
  escape_time = 2;
}

void MineConfig::LoadXml(xmlpp::Element *elem) 
{
  ExplosiveWeaponConfig::LoadXml (elem);
  LitDocXml::LitUint (elem, "escape_time", escape_time);
  LitDocXml::LitDouble (elem, "detection_range", detection_range);
}
