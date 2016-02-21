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
 * Particle Engine
 *****************************************************************************/

#include "character/character.h"
#include "graphic/sprite.h"
#include "particles/polecat_fart.h"
#include "particles/particle.h"
#include "team/macro.h"
#include "team/team.h"

PolecatFart::PolecatFart() :
  Particle("polecat_fart_particle")
{
  m_initial_time_to_live = 50;
  m_left_time_to_live = m_initial_time_to_live;
  m_time_between_scale = 100;
  SetCollisionModel(true, false, false);
  is_active = true;

  image = ParticleEngine::GetSprite(POLECAT_FART_spr);
  image->Scale(1.0,1.0);
  SetSize( Point2i(10, 10) );
}

void PolecatFart::Refresh()
{
  Particle::Refresh();

  FOR_ALL_LIVING_CHARACTERS(team, c) {
    if ((c->GetTestRect()).Intersect(GetTestRect())) {
      //c->SetEnergyDelta(-10);
      c->SetDiseaseDamage(5, std::numeric_limits<uint>::max());
      is_active = false;
    }
  }
}
