/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Petrol barrels
 *****************************************************************************/

#include "object/barrel.h"
#include "object/objects_list.h"
#include "object/physical_obj.h"
#include "game/game_mode.h"
#include "graphic/sprite.h"
#include "particles/particle.h"
#include "tool/resource_manager.h"
#include "weapon/explosion.h"

PetrolBarrel::PetrolBarrel() : PhysicalObj("barrel")
{
  Profile *res = resource_manager.LoadXMLProfile("graphism.xml", false);
  img = resource_manager.LoadSprite(res, "object/barrel");
  ASSERT(img);
  resource_manager.UnLoadXMLProfile(res);

  m_energy = 25;

  SetCollisionModel(false, true, true);
  SetSize(img->GetSize());
  SetTestRect (1, 1, 2, 2);
}

PetrolBarrel::~PetrolBarrel()
{
  delete img;
}

void PetrolBarrel::Draw()
{
  img->Update();
  img->Draw(GetPosition());
}

void PetrolBarrel::SignalGhostState(bool /*was_already_dead*/)
{
  ParticleEngine::AddNow(GetCenter(), 20, particle_FIRE, true);
  ApplyExplosion(GetCenter(), GameMode::GetInstance()->barrel_explosion_cfg,
                 "weapon/explosion", false);
}
