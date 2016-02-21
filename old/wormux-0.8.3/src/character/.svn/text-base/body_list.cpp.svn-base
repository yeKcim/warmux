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
 *****************************************************************************/

#include "character/body_list.h"
//-----------------------------------------------------------------------------
#include <string>
#include <iostream>
#include "character/body.h"
#include "character/movement.h"
#include "game/config.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

const std::string CONFIG_FN = "config.xml";

BodyList::BodyList()
{
}

void BodyList::FreeMem()
{
  // The bodies member variable are freed from here, because the playing bodies
  //  only contains pointers to member/movement/clothes of the bodies in the body_list
  std::map<std::string, Body*>::iterator it = list.begin();
  while(it != list.end())
  {
    delete it->second;
    it++;
  }
  list.clear();
}

void BodyList::Load(const std::string &name)
{
  std::string dir = Config::GetInstance()->GetDataDir() + PATH_SEPARATOR + "body" + PATH_SEPARATOR + name + PATH_SEPARATOR;
  std::string fn = dir + CONFIG_FN;

  XmlReader doc;
  if (!doc.Load(fn)) {
     std::cerr << "Unable to find file " << fn << std::endl;
     return;
  }

  Body* body = new Body(doc.GetRoot(), dir);
  list[name] = body;
}

Body* BodyList::GetBody(const std::string &name)
{
  if(list[name] == NULL)
    Load(name);

  if(list[name] == NULL)
  {
    std::cerr << "Unable to load body \"" << name << "\"" << std::endl;
    return NULL;
  }
  return new Body(*list[name]);
}
