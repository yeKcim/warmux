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
 * Credits Menu
 *****************************************************************************/

#include "menu/credits_menu.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include <iostream>
#include <algorithm>  //std::transform
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "gui/list_box.h"
#include "include/app.h"
#include "tool/i18n.h"
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

class Author
{
public:
  std::string name;
  std::string nickname;
  std::string email;
  std::string country;
  std::string description;

  bool Feed (const xmlNode* node);
  std::string PrettyString(bool with_email) const;
};

//-----------------------------------------------------------------------------

bool Author::Feed (const xmlNode* node)
{
  if (!XmlReader::ReadString(node, "name", name))
    return false;
  if (!XmlReader::ReadString(node, "description", description))
    return false;
  XmlReader::ReadString(node, "nickname", nickname);
  XmlReader::ReadString(node, "email", email);
  XmlReader::ReadString(node, "country", country);
  return true;
}

//-----------------------------------------------------------------------------

std::string Author::PrettyString(bool with_email) const
{
  std::ostringstream ss;
  ss << "* " << name;
  if (with_email)
  {
    ss << " <" << email << ">";
  }
  if (!nickname.empty())
  {
    ss << " " << _("aka") << " " << nickname;
  }
  if (!country.empty())
  {
    ss << " " << _("from") << " " << country;
  }
  ss << ": " << description;
  return ss.str();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CreditsMenu::CreditsMenu()  :
  Menu("credit/background", vOk)
{
  ListBox * lbox_authors = new ListBox(Point2i(GetMainWindow().GetWidth()-60,
					       GetMainWindow().GetHeight()-60-30),
				       false);
  lbox_authors->SetBackgroundColor(Color(0,0,0,200));
  lbox_authors->SetPosition(30, 30);

  widgets.AddWidget(lbox_authors);
  widgets.Pack();

  PrepareAuthorsList(lbox_authors);
}

CreditsMenu::~CreditsMenu()
{
}

bool CreditsMenu::signal_ok()
{
  return true;
}

bool CreditsMenu::signal_cancel()
{
  return true;
}


void CreditsMenu::PrepareAuthorsList(ListBox * lbox_authors) const
{
  std::string filename = Config::GetInstance()->GetDataDir() + "authors.xml";
  XmlReader doc;
  if(!doc.Load(filename))
  {
    // Error: do something ...
    return;
  }
  // Use an array for this is the best solution I think, but there is perhaps a better code...
  static const std::string teams[] = { "team", "contributors", "thanks" };

  for(uint i = 0; i < (sizeof teams / sizeof* teams); ++i)
  {
    xmlNodeArray team = XmlReader::GetNamedChildren(doc.GetRoot(), teams[i]);

    if (team.empty())
      continue;

    std::string team_title = teams[i];
    std::transform( team_title.begin(), team_title.end(), team_title.begin(), static_cast<int (*)(int)>(toupper) );

    // I think this is ugly, but someone can use a better presentation
    std::cout << "       ===[ " << team_title << " ]===" << std::endl << std::endl;

    lbox_authors->AddItem (false, " ", "",
                           Font::FONT_BIG, Font::FONT_NORMAL);
    lbox_authors->AddItem (false, team_title, teams[i],
                           Font::FONT_BIG, Font::FONT_NORMAL, c_red);

    // We think there is ONLY ONE occurence of team section, so we use the first
    xmlNodeArray sections = XmlReader::GetNamedChildren(team.front(), "section");
    xmlNodeArray::const_iterator section=sections.begin(), end_section=sections.end();

    for (; section != end_section; ++section)
    {
      xmlNodeArray authors = XmlReader::GetNamedChildren(*section, "author");
      xmlNodeArray::const_iterator node=authors.begin(), end=authors.end();
      std::string title;

      if (!XmlReader::ReadStringAttr(*section, "title", title))
        continue;

      std::cout << "== " << title << " ==" << std::endl;

      lbox_authors->AddItem (false, " ", "");
      lbox_authors->AddItem (false, "   "+title, title,
                             Font::FONT_MEDIUM, Font::FONT_NORMAL, c_yellow);

      for (; node != end; ++node)
      {
        Author author;
        if (author.Feed(*node))
        {
          std::cout << author.PrettyString(false) << std::endl;
          lbox_authors->AddItem (false, author.PrettyString(false), author.name);
        }
      }
      std::cout << std::endl;
      lbox_authors->AddItem (false, "", "");
    }
  }
}

void CreditsMenu::Draw(const Point2i& /*mousePosition*/)
{
}

void CreditsMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void CreditsMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

//-----------------------------------------------------------------------------
