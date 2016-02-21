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
 * Base classes for weapons configuration.
 *****************************************************************************/

#include "weapon/weapon_cfg.h"
#include <iostream>
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

void EmptyWeaponConfig::LoadXml(xmlpp::Element */*elem*/)
{}

//-----------------------------------------------------------------------------

WeaponConfig::WeaponConfig()
{ damage = 10; }

void WeaponConfig::LoadXml(xmlpp::Element *elem)
{
  XmlReader::ReadUint(elem, "damage", damage);
}

//-----------------------------------------------------------------------------

ExplosiveWeaponConfig::ExplosiveWeaponConfig()
{
  timeout = 0;
  allow_change_timeout = false;
  explosion_range = 0 ;
  particle_range = explosion_range;
  blast_range = 0 ;
  blast_force = 0 ;
}

void ExplosiveWeaponConfig::LoadXml(xmlpp::Element *elem)
{
  WeaponConfig::LoadXml (elem);
  XmlReader::ReadUint(elem, "timeout", timeout);
  XmlReader::ReadBool(elem, "allow_change_timeout", allow_change_timeout);
  XmlReader::ReadUint(elem, "explosion_range", explosion_range);
  XmlReader::ReadUint(elem, "particle_range", particle_range);
  XmlReader::ReadUint(elem, "blast_range", blast_range);
  XmlReader::ReadUint(elem, "blast_force", blast_force);
}

//-----------------------------------------------------------------------------

// timeout before explosion
// max_strength
// min_angle, max_angle
// coeff rebond ?
// nb_replay
// can_change_weapon
// force, longueur (baseball)
// nbr_obus
