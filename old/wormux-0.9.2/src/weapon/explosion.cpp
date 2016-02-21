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
 * Useful for weapons: explode on one point
 *****************************************************************************/

#include "weapon/explosion.h"
#include "weapon/weapon_cfg.h"

#include "character/character.h"
#include "graphic/surface.h"
#include "graphic/video.h"
#include "include/action_handler.h"
#include "map/camera.h"
#include "map/map.h"
#include "object/objects_list.h"
#include "object/physical_obj.h"
#include "particles/particle.h"
#include "sound/jukebox.h"
#include "team/macro.h"
#include "team/team.h"
#include <WORMUX_debug.h>
#include "tool/math_tools.h"
#include "tool/string_tools.h"
#include "tool/resource_manager.h"
#include <WORMUX_random.h>
#include "weapon/weapon.h"

Profile *weapons_res_profile = NULL;

int GetDamageFromExplosion(const ExplosiveWeaponConfig &config, Double distance)
{
  if (distance > config.explosion_range)
    return 0;

  Double dmg;
  if( config.explosion_range != ZERO)
    dmg = cos(HALF_PI * distance / (Double)config.explosion_range);
  else
    dmg = cos(HALF_PI * distance);

  dmg *= config.damage;
  return (int) dmg;
}

Double GetForceFromExplosion(const ExplosiveWeaponConfig &config, Double distance)
{
  Double force;
  if(config.blast_range != ZERO)
    force = cos(HALF_PI * distance / (Double)config.blast_range);
  else
    force = cos(HALF_PI * distance);

  force *= config.blast_force;
  return force;
}

void ApplyExplosion (const Point2i &pos,
                     const ExplosiveWeaponConfig &config,
                     const std::string& son,
                     bool fire_particle,
                     ParticleEngine::ESmokeStyle smoke
                     )
{
  MSG_DEBUG("explosion", "explosion range : %s", Double2str(config.explosion_range,0).c_str());

#ifdef HAVE_A_REALLY_BIG_CPU
  // Add particles based on the ground image
  if(config.explosion_range >= 15)
  {
    for(int y=-config.explosion_range; y < (int)config.explosion_range; y += 10)
    {
      int dx = (int) (cos(asin((Double)y / config.explosion_range)) * (Double) y);
      for(int x=-dx; x < dx; x += 10)
        ParticleEngine::AddNow(pos + Point2i(x-5,y-5), 1, particle_GROUND, true);
    }
  }
  else
    ParticleEngine::AddNow(pos, 1, particle_GROUND, true);
#endif

  // Make a hole in the ground
  if(config.explosion_range != ZERO)
    GetWorld().Dig(pos, (int)config.explosion_range);

  // Play a sound
  if (son != "") {
    JukeBox::GetInstance()->Play("default", son);
  }

  // Apply damage on the character.
  // Do not care about the death of the active character.
  Double highest_force = 0.0;
  Character* fastest_character = NULL;
  FOR_ALL_CHARACTERS(team, character)
  {
    Double distance = pos.Distance(character -> GetCenter());
    if(distance < ONE)
      distance = ONE;

    // If the character is in the explosion range, apply damage on it !
    int dmg = GetDamageFromExplosion(config, distance);
    if (dmg != 0) {
      MSG_DEBUG("explosion", "\n*Character %s : distance= %f", character->GetName().c_str(), Double2str(distance).c_str());
      MSG_DEBUG("explosion", "hit_point_loss energy= %d", character->GetName().c_str(), dmg);
      character->SetEnergyDelta (-dmg);
    }

    // If the character is in the blast range, apply the blast on it !
    if (distance <= config.blast_range)
    {
      Double force = GetForceFromExplosion(config, distance);

      if ( force > highest_force )
      {
        if(!(*character).IsDead()){
          fastest_character = &(*character);
        }
        highest_force = force;
      }

      Double angle;
      if (!EqualsZero(distance))
      {
        angle  = pos.ComputeAngle(character -> GetCenter());
        if( angle > 0 )
          angle  = - angle;
      }
      else
        angle = -PI/2;


      MSG_DEBUG("explosion", "force = %s", Double2str(force).c_str());
      ASSERT(character->GetMass() != ZERO);
      character->AddSpeed (force / character->GetMass(), angle);
      character->SignalExplosion();
    }
  }

  if(fastest_character != NULL)
    Camera::GetInstance()->FollowObject(fastest_character);

  // Apply the blast on physical objects.
  FOR_EACH_OBJECT(it)
   {
     PhysicalObj *obj = *it;

     if (obj->CollidesWithGround() && !obj->IsGhost())
     {
       Double distance = pos.Distance(obj->GetCenter());
       if(distance < ONE)
         distance = ONE;

       int dmg = GetDamageFromExplosion(config, distance);
       if (dmg != 0) {
         obj->SetEnergyDelta(-dmg);
       }

       if (distance <= (Double)config.blast_range)
       {
         Double force = GetForceFromExplosion(config, distance);
         Double angle;
         if (!EqualsZero(distance))
           angle  = pos.ComputeAngle(obj->GetCenter());
         else
           angle = -HALF_PI;

         ASSERT( obj->GetMass() != ZERO);

         obj->AddSpeed (force / obj->GetMass(), angle);
       }
     }
   }

  ParticleEngine::AddExplosionSmoke(pos, (int)config.particle_range, smoke);

  // Do we need to generate some fire particles ?
  if (fire_particle)
     ParticleEngine::AddNow(pos , 5, particle_FIRE, true);

  // Add explosion sprite
  if ( config.explosion_range > 25 && config.damage > 0 )
  {
    ParticleEngine::AddNow(pos,1,particle_EXPLOSION,true);
  }

  // Shake the camera (FIXME: use actual vectors?)
  if ( config.explosion_range > 25 && config.damage > 0 )
  {
     int reduced_range = ( int )config.explosion_range / 2;
     Camera::GetInstance()->Shake( (int)(config.explosion_range * 15),
         Point2i( RandomLocal().GetLong( -reduced_range, reduced_range  ),
                (int)config.explosion_range ),
         Point2i( 0, 0 )
        );
  };
}

