/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
#include "graphic/video.h"
#include "gui/label.h"
#include "gui/scroll_box.h"
#include "include/app.h"
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
  if (with_email) {
    ss << " <" << email << ">";
  }
  if (!nickname.empty()) {
    ss << " " << _("aka") << " " << nickname;
  }
  if (!country.empty()) {
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
  const Surface& window = GetMainWindow();
  lbox_authors = new ScrollBox(window.GetSize()*0.9f - Point2i(0, 30));
  lbox_authors->SetBackgroundColor(Color(0, 200));
  lbox_authors->SetPosition(window.GetSize()*0.05f);

  PrepareAuthorsList();

  widgets.AddWidget(lbox_authors);
  widgets.Pack();
  widgets.SetFocusOn(lbox_authors);
}

void CreditsMenu::AddItem(const std::string & label,
                          Font::font_size_t fsize,
                          Font::font_style_t fstyle,
                          const Color & color)
{
  lbox_authors->AddWidget(new Label(label, lbox_authors->GetSizeX() - 10,
                                    fsize, fstyle, color));
}

void CreditsMenu::PrepareAuthorsList()
{
  std::string filename = Config::GetInstance()->GetDataDir() + "authors.xml";
  XmlReader doc;
  if (!doc.Load(filename)) {
    // Error: do something ...
    return;
  }
  // Use an array for this is the best solution I think, but there is perhaps a better code...
  static const std::string teams[] = { "team", "contributors", "thanks" };

  for (uint i = 0; i < (sizeof teams / sizeof* teams); ++i) {
    xmlNodeArray team = XmlReader::GetNamedChildren(doc.GetRoot(), teams[i]);

    if (team.empty())
      continue;

    std::string team_title = teams[i];
    std::transform(team_title.begin(), team_title.end(), team_title.begin(),
                   static_cast<int (*)(int)>(toupper) );

    // I think this is ugly, but someone can use a better presentation
    std::cout << "       ===[ " << team_title << " ]===" << std::endl << std::endl;

    AddItem(" ", Font::FONT_BIG, Font::FONT_BOLD);
    AddItem(team_title,  Font::FONT_BIG, Font::FONT_BOLD, c_red);

    // We think there is ONLY ONE occurence of team section, so we use the first
    xmlNodeArray sections = XmlReader::GetNamedChildren(team.front(), "section");
    xmlNodeArray::const_iterator section     = sections.begin(),
                                 end_section = sections.end();

    for (; section != end_section; ++section) {
      xmlNodeArray authors = XmlReader::GetNamedChildren(*section, "author");
      xmlNodeArray::const_iterator node = authors.begin(),
                                   end  = authors.end();
      std::string title;

      if (!XmlReader::ReadStringAttr(*section, "title", title))
        continue;

      std::cout << "== " << title << " ==" << std::endl;

      AddItem(" ");
      AddItem("   "+title, Font::FONT_MEDIUM, Font::FONT_BOLD, c_yellow);

      for (; node != end; ++node) {
        Author author;
        if (author.Feed(*node)) {
          std::cout << author.PrettyString(false) << std::endl;
          AddItem(author.PrettyString(false));
        }
      }
      std::cout << std::endl;
      AddItem("");
    }
  }
}
