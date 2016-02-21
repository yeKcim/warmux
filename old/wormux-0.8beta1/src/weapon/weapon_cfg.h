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

#ifndef WEAPON_CONFIGURATION_H
#define WEAPON_CONFIGURATION_H
//-----------------------------------------------------------------------------
#include "include/base.h"
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

class EmptyWeaponConfig
{
public:
   virtual ~EmptyWeaponConfig() {};
   virtual void LoadXml(xmlpp::Element *elem) = 0;
};

//-----------------------------------------------------------------------------

class WeaponConfig : public EmptyWeaponConfig
{ 
public: 
  uint damage;
public:
  WeaponConfig();
  virtual ~WeaponConfig() {};
  virtual void LoadXml(xmlpp::Element *elem);
};

//-----------------------------------------------------------------------------

class ExplosiveWeaponConfig : public WeaponConfig
{
public:
  uint timeout;
  bool allow_change_timeout;
  double blast_range ;
  double blast_force ;  
  uint explosion_range;
  uint particle_range;
public:
  ExplosiveWeaponConfig();
  virtual ~ExplosiveWeaponConfig() {};
   virtual void LoadXml(xmlpp::Element *elem);
   
   
};

//-----------------------------------------------------------------------------
#endif
