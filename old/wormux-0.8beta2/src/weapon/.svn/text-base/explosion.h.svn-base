/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#ifndef WEAPON_TOOLS_H
#define WEAPON_TOOLS_H

#include "particles/particle.h"
#include "tool/point.h"

class Profile;
class ExplosiveWeaponConfig;

void ApplyExplosion(const Point2i &pos,
                    const ExplosiveWeaponConfig &config,
                    const std::string& son="weapon/explosion",
                    bool fire_particle = true,
                    ParticleEngine::ESmokeStyle smoke = ParticleEngine::BigESmoke,
                    std::string network_id = "LOCAL" // Useful only on network : this is used to know that we should not apply blast and damage to the projectile which has created the explosion (bugfix #8529)
                    );

void ApplyExplosion_common (const Point2i &pos,
                            const ExplosiveWeaponConfig &config,
                            const std::string& son,
                            bool fire_particle,
                            ParticleEngine::ESmokeStyle smoke,
                            std::string network_id
                           );

extern Profile *weapons_res_profile;

#endif //WEAPON_TOOLS_H
