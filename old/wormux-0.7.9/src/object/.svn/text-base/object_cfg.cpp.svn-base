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
 * Class containing object physic constants
 *****************************************************************************/

//-----------------------------------------------------------------------------
#include <cstring>
#include <iostream>
#include "object_cfg.h"
#include "../game/config.h"
#include "../include/base.h"
#include "../tool/xml_document.h"
//-----------------------------------------------------------------------------

ObjectConfig::ObjectConfig()
{
  m_rebounding = false;
  m_rebound_factor = 0.01;
  m_air_resist_factor = 1.0;
  m_wind_factor = 1.0;
  m_gravity_factor = 1.0;
  m_mass = 1.0;
}

ObjectConfig::~ObjectConfig()
{
}

void ObjectConfig::LoadXml(const std::string& obj_name, const std::string &config_file)
{
  std::string file;
  if(config_file=="")
    file = Config::GetInstance()->GetDataDir() + PATH_SEPARATOR + "game_mode" + PATH_SEPARATOR + "objects.xml";
  else
    file = config_file;

  // Load Xml configuration
  XmlReader doc;
  assert(doc.Load(file));
  xmlpp::Element* elem = XmlReader::GetMarker(doc.GetRoot(), obj_name);

  assert(elem != NULL);
  XmlReader::ReadDouble(elem, "mass", m_mass);
  XmlReader::ReadDouble(elem, "wind_factor", m_wind_factor);
  XmlReader::ReadDouble(elem, "air_resist_factor", m_air_resist_factor);
  XmlReader::ReadDouble(elem, "gravity_factor", m_gravity_factor);
  XmlReader::ReadDouble(elem, "rebound_factor", m_rebound_factor);
  XmlReader::ReadBool(elem, "rebounding", m_rebounding);
}
