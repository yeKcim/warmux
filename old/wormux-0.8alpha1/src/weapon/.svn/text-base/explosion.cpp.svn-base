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
 * Utilitaires pour les armes : applique une explosion en un point.
 *****************************************************************************/

#include "explosion.h"
#include "../graphic/surface.h"
#include "../graphic/video.h"
#include "../include/action_handler.h"
#include "../map/camera.h"
#include "../map/map.h"
#include "../network/network.h"
#include "../object/objects_list.h"
#include "../particles/particle.h"
#include "../object/physical_obj.h"
#include "../sound/jukebox.h"
#include "../team/macro.h"
#include "../tool/debug.h"
#include "../tool/math_tools.h"

Profile *weapons_res_profile = NULL;

// Network explosion defined below
void ApplyExplosion_server (const Point2i &pos,
		     const ExplosiveWeaponConfig &config,
		     const std::string& son,
		     bool fire_particle,
		     ParticleEngine::ESmokeStyle smoke);

void ApplyExplosion (const Point2i &pos,
		     const ExplosiveWeaponConfig &config,
		     const std::string& son,
		     bool fire_particle,
		     ParticleEngine::ESmokeStyle smoke
		     )
{
  if(network.IsLocal())
    ApplyExplosion_common(pos, config, son, fire_particle, smoke);
  else
  if(network.IsServer())
    ApplyExplosion_server(pos, config, son, fire_particle, smoke);
  else
  if(network.IsClient())
    return;
  // client receives explosion via the action handler
}

void ApplyExplosion_common (const Point2i &pos,
		     const ExplosiveWeaponConfig &config,
		     const std::string& son,
		     bool fire_particle,
		     ParticleEngine::ESmokeStyle smoke
		     )
{
  MSG_DEBUG("explosion", "explosion range : %i\n", config.explosion_range);

#ifdef HAVE_A_REALLY_BIG_CPU
  // Add particles based on the ground image
  if(config.explosion_range >= 15)
  {
    for(int y=-config.explosion_range; y < (int)config.explosion_range; y += 10)
    {
      int dx = (int) (cos(asin((float)y / config.explosion_range)) * (float) y);
      for(int x=-dx; x < dx; x += 10)
        ParticleEngine::AddNow(pos + Point2i(x-5,y-5), 1, particle_GROUND, true);
    }
  }
  else
    ParticleEngine::AddNow(pos, 1, particle_GROUND, true);
#endif

  // Make a hole in the ground
  world.Dig(pos, config.explosion_range);

  // Play a sound
  jukebox.Play("share", son);

  // Apply damage on the character.
  // Do not care about the death of the active worm.
  double highest_force = 0.0;
  Character* fastest_character = NULL;
  FOR_ALL_CHARACTERS(equipe,ver)
  {
    double distance = pos.Distance(ver -> GetCenter());

    // If the character is in the explosion range, apply damage on it !
    if (distance <= config.explosion_range)
    {
      MSG_DEBUG("explosion", "\n*Character %s : distance= %f", ver->GetName().c_str(), distance);
      double dmg = cos(M_PI_2 * distance / config.explosion_range);
      dmg *= config.damage;
      MSG_DEBUG("explosion", "hit_point_loss energy= %i", ver->GetName().c_str(), dmg);
      ver -> SetEnergyDelta (-(int)dmg);
    }

    // If the character is in the blast range, apply the blast on it !
    if (distance <= config.blast_range)
    {
      double angle;
      double force = cos(M_PI_2 * distance / config.blast_range);
      force *= config.blast_force;

      if ( force > highest_force )
      {
        fastest_character = &(*ver);
        highest_force = force;
      }

      if (!EgalZero(distance))
      {
        angle  = pos.ComputeAngle(ver -> GetCenter());
        if( angle > 0 )
          angle  = - angle;
      }
      else
        angle = -M_PI/2;


      MSG_DEBUG("explosion", "force = %f", force);
      ver->AddSpeed (force / ver->GetMass(), angle);
      ver->SignalExplosion();
    }
  }

  if(fastest_character != NULL)
    camera.ChangeObjSuivi (fastest_character, true, true);

  // Apply the blast on physical objects.
  FOR_EACH_OBJECT(obj) if ( !(obj -> ptr -> GoesThroughWall()) )
  { 
    double distance = pos.Distance(obj -> ptr -> GetCenter());
    if (distance <= config.explosion_range)
    {
      double dmg = cos(M_PI_2 * distance / config.explosion_range);
      dmg *= config.damage;
      obj -> ptr -> AddDamage (config.damage);
    }

    if (distance <= config.blast_range)
    {
      double angle;
      double force = cos(M_PI_2 * distance / config.blast_range);
      force *= config.blast_force;

      if (!EgalZero(distance))
        angle  = pos.ComputeAngle(obj->ptr->GetCenter());
      else
        angle = -M_PI_2;

      if(fastest_character != NULL)
        camera.ChangeObjSuivi (obj->ptr, true, true);
      obj -> ptr -> AddSpeed (force / obj->ptr->GetMass(), angle);
    }
  }

  ParticleEngine::AddExplosionSmoke(pos, config.particle_range, smoke);

  // Do we need to generate some fire particles ?
  if (fire_particle)
     ParticleEngine::AddNow(pos , 5, particle_FIRE, true);
}

void ApplyExplosion_server (const Point2i &pos,
		     const ExplosiveWeaponConfig &config,
		     const std::string& son,
		     bool fire_particle,
		     ParticleEngine::ESmokeStyle smoke
		     )
{
  ActionHandler* action_handler = ActionHandler::GetInstance();

  Action a_begin_sync(ACTION_SYNC_BEGIN);
  network.SendAction(&a_begin_sync);

  TeamsList::iterator
    it=teams_list.playing_list.begin(),
    end=teams_list.playing_list.end();

  for (int team_no = 0; it != end; ++it, ++team_no)
  {
    Team& team = **it;
    Team::iterator
        tit = team.begin(),
        tend = team.end();

    for (int char_no = 0; tit != tend; ++tit, ++char_no)
    {
      Character &character = *tit;

      double distance = pos.Distance( character.GetCenter());

      // If the character is in the explosion range, apply damage on it !
      if (distance <= config.explosion_range || distance < config.blast_range)
      {
        // cliens : Place characters
        Action* a = BuildActionSendCharacterPhysics(team_no, char_no);
        action_handler->NewAction(a);
      }
    }
  }

  Action* a = new Action(ACTION_EXPLOSION);
  a->Push(pos.x);
  a->Push(pos.y);
  a->Push((int)config.explosion_range);
  a->Push((int)config.particle_range);
  a->Push((int)config.damage);
  a->Push(config.blast_range);
  a->Push(config.blast_force);
  a->Push(son);
  a->Push(fire_particle);
  a->Push(smoke);

  action_handler->NewAction(a);
//  network.SendAction(&a);
  Action a_sync_end(ACTION_SYNC_END);
  network.SendAction(&a_sync_end);
}
