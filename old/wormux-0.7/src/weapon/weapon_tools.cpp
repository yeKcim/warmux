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
 * Utilitaires pour les armes : applique une explosion en un point.
 *****************************************************************************/

#include "weapon_tools.h"
#include "../graphic/surface.h"
#include "../graphic/video.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../object/objects_list.h"
#include "../object/particle.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/math_tools.h"

Profile *weapons_res_profile = NULL;

void ApplyExplosion (const Point2i &pos,
		     const ExplosiveWeaponConfig &config,
		     PhysicalObj *obj_exclu,
		     const std::string& son,
		     bool fire_particle,
		     ParticleEngine::ESmokeStyle smoke
		     )
{
  bool cam_follow_character = false; //Set to true if an explosion is applied to a character. Then the camera shouldn't be following an object

  MSG_DEBUG("explosion", "explosion range : %f\n", config.explosion_range);

  // Make a hole in the ground
  world.Dig(pos, config.explosion_range);
  float range = config.explosion_range / PIXEL_PER_METER;
  range *= 1.5;

  // Play a sound
  jukebox.Play("share", son);
   
  // Apply damage on the worms.
  // Do not care about the death of the active worm.
  FOR_ALL_CHARACTERS(equipe,ver)
  {
    // Is it the object we do not want to manage ?
    if ((obj_exclu != NULL) && (&(*ver) == obj_exclu)) continue;

    double distance, angle;
    distance = MeterDistance (pos, ver -> GetCenter());

    MSG_DEBUG("explosion", "\n*Character %s : distance= %e", ver->m_name.c_str(), distance);

    // If the worm is in the explosion range, apply damage on it !
    if (distance <= range)
    {
      uint hit_point_loss = (uint)(distance*config.damage/range);
      hit_point_loss = config.damage-hit_point_loss;

      //MSG_DEBUG("explosion", "hit_point_loss energy= %d", ver->m_name.c_str(), hit_point_loss);

      ver -> SetEnergyDelta (-hit_point_loss);
    }

    // If the worm is in the blast range, apply the blast on it !
    if (distance <= config.blast_range)
    {
      double force = distance;
      if (!EgalZero(distance))
      {
        force *= config.blast_force / config.blast_range;
        force  = config.blast_force - force;
        angle  = pos.ComputeAngle(ver -> GetCenter());
      }
      else
      {
        force = config.blast_force;
        angle = -M_PI/2;
      }
      
      //MSG_DEBUG("explosion", "force = %e", force);

//      camera.ChangeObjSuivi ((PhysicalObj*)&ver, true, true);
      cam_follow_character = true;
      ver -> AddSpeed (force, angle);
      ver -> UpdatePosition();
    } else {

      MSG_DEBUG("explosion", " -> too far");

    }

    // Update the worm state.
    ver -> Refresh();

  }

  // Apply the blast on physical objects.
  FOR_EACH_OBJECT(obj) if (obj -> ptr -> GetObjectType() == objCLASSIC)
  { 
    // Is this the object we do not want to manage ?
    if ((obj_exclu != NULL) && (obj -> ptr == obj_exclu)) continue;

    double distance, angle;
    distance = MeterDistance (pos, obj -> ptr -> GetCenter());
    if (distance <= config.blast_range)
    {
      if (!EgalZero(distance)) {
	distance *= config.blast_force / config.blast_range;
	distance  = config.blast_force - distance;
	angle  = pos.ComputeAngle(obj -> ptr -> GetCenter());
      } else {
	distance = config.blast_force;
	angle = -M_PI_2;
      }
      if(!cam_follow_character)
        camera.ChangeObjSuivi (obj->ptr, true, true);
      obj -> ptr -> AddSpeed (distance, angle);
      obj -> ptr -> UpdatePosition();
    }
  }

  ParticleEngine::AddExplosionSmoke(pos, config.explosion_range, smoke);

  // Do we need to generate some fire particles ?
  if (fire_particle)
     ParticleEngine::AddNow(pos , 5, particle_FIRE, true);
}

