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
 * Help Menu
 *****************************************************************************/

#include "include/app.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/picture_widget.h"
#include "menu/help_menu.h"
#include "game/config.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

static const uint BORDER        = 20;
static const uint CHECKBOX_SIZE = 50;

HelpMenu::HelpMenu()  :
  Menu("help/background", vOk)
{
  Point2i size = AppWormux::GetInstance()->video->window.GetSize()
               - Point2i(2*BORDER,2*BORDER+CHECKBOX_SIZE);

  std::string lang = Config::GetInstance()->GetLanguage();
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  xmlNode *elem = resource_manager.GetElement (res, "surface", "help/help_shortkeys_" + lang);
  std::string filename;
  if (elem == NULL || !res->doc->ReadStringAttr(elem, "file", filename))
    lang = "en";
  resource_manager.UnLoadXMLProfile(res);
  PictureWidget *help_image = new PictureWidget(size, "help/help_shortkeys_" + lang, true);

  VBox *help = new VBox(size.x);
  help->SetPosition(BORDER, BORDER);
  help->AddWidget(help_image);
  widgets.AddWidget(help);
  widgets.Pack();
}

HelpMenu::~HelpMenu()
{
}

bool HelpMenu::signal_ok()
{
  return true;
}

bool HelpMenu::signal_cancel()
{
  return true;
}

void HelpMenu::Draw(const Point2i& /*mousePosition*/)
{
}

void HelpMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void HelpMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

//-----------------------------------------------------------------------------
