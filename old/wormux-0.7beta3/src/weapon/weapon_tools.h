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

#ifndef WEAPON_TOOLS_H
#define WEAPON_TOOLS_H
//-----------------------------------------------------------------------------
#include <SDL.h>
#include "../tool/resource_manager.h"
#include "../include/base.h"
#include "../weapon/weapon.h"
#include "../object/physical_obj.h"
#include "../tool/Point.h"
//-----------------------------------------------------------------------------

// Applique au explosion au point centre
// Lance ExceptionMortVer() si le ver actif meurt

void AppliqueExplosion (const Point2i &explosion,
			const Point2i &trou,
			SDL_Surface *impact,
			const ExplosiveWeaponConfig &config,
			PhysicalObj *obj_exclu, 
			const std::string& son="weapon/explosion",
			bool fire_particle = true
			);

void UpdateStrengthBar(double strength);

extern Profile *weapons_res_profile; 

//-----------------------------------------------------------------------------
#endif //WEAPON_TOOLS_H
