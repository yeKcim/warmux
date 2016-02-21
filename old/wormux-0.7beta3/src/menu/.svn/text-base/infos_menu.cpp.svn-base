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
 * Menu informations.
 *****************************************************************************/

#include "infos_menu.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include <iostream>
#include "../game/config.h"
using namespace Wormux;
//-----------------------------------------------------------------------------
MenuInfos menu_infos;
//-----------------------------------------------------------------------------

class Author
{
public:
  std::string name;
  std::string nickname;
  std::string email;
  std::string country;
  std::string description;

  bool Feed (const xmlpp::Node *node);
  std::string PrettyString(bool with_email);
};    

//-----------------------------------------------------------------------------

bool Author::Feed (const xmlpp::Node *node)
{
   if (!LitDocXml::LitString(node, "name", name)) return false;
   if (!LitDocXml::LitString(node, "description", description)) return false;
   return true;
}

//-----------------------------------------------------------------------------

std::string Author::PrettyString(bool with_email)
{
   std::ostringstream ss;
   ss << name;
   if (with_email)
   {
     ss << " <" << email << ">";
   }
   if (!nickname.empty())
   {
     ss << " aka " << nickname;
   }
   if (!country.empty())
   {
     ss << "from " << country;
   }
   ss << ": " << description;
   return ss.str();
}

//-----------------------------------------------------------------------------

void MenuInfos::Run()
{
  std::string filename = config.data_dir+"authors.xml";
  LitDocXml doc;
  if (!doc.Charge (filename))
  {
    // Error: do something ...
    return;
  } 

  xmlpp::Node::NodeList sections = doc.racine() -> get_children("section");
  xmlpp::Node::NodeList::iterator
    section=sections.begin(),
    end_section=sections.end();

  for (; section != end_section; ++section)
  {
    xmlpp::Node::NodeList authors = (**section).get_children("author");
    xmlpp::Node::NodeList::iterator
      node=authors.begin(),
      end=authors.end();
    std::string title;
    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*>(*section);
    if (!elem)
    {
        std::cerr << "cast error" << std::endl;
        continue;
    }
    if (!LitDocXml::LitAttrString(elem, "title", title)) continue;
    std::cout << "=== " << title << " ===" << std::endl;
    
    for (; node != end; ++node)
    {
        Author author;
        if (author.Feed(*node))
        {
          std::cout << "* " << author.PrettyString(false) << std::endl;
        }
    }
    std::cout << std::endl;
  }

}

//-----------------------------------------------------------------------------
