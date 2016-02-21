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
 *****************************************************************************/

#include "tool/xml_document.h"
#include "clothe.h"
#include "member.h"
#include <map>
#include <iostream>

Clothe::Clothe(xmlpp::Element *xml, std::map<std::string, Member*>& members_lst):
  name(),
  layers()
{
  XmlReader::ReadStringAttr( xml, "name", name);

  xmlpp::Node::NodeList nodes = xml -> get_children("member");
  xmlpp::Node::NodeList::iterator
    it=nodes.begin(),
    end=nodes.end();

  for (; it != end; ++it)
  {
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    ASSERT (elem != NULL);
    std::string att;
    if (!XmlReader::ReadStringAttr(elem, "name", att))
    {
      std::cerr << "Malformed attached member definition" << std::endl;
      continue;
    }

    Member* member = NULL;
    if(members_lst.find(att) != members_lst.end())
    {
      member = members_lst.find(att)->second;
    }
    else
    {
      std::cerr << "Undefined member \"" << att << "\"" << std::endl;
    }

    layers.push_back( member );
  }

  std::vector<Member*>::iterator i = layers.begin();
  while( i != layers.end())
  if(*i!=NULL)
    i++;
  else
    i=layers.erase(i);
}

Clothe::Clothe(Clothe* c, std::map<std::string, Member*>& members_lst):
  name(c->name),
  layers()
{
  for (std::vector<Member*>::iterator it = c->layers.begin();
      it != c->layers.end();
      ++it)
  {
    layers.push_back(members_lst.find((*it)->name)->second);
  }
}

Clothe::~Clothe()
{
  layers.clear();
}
