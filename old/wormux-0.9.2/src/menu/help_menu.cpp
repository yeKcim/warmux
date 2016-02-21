/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "menu/help_menu.h"
#include "game/config.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

HelpMenu::HelpMenu()  :
  Menu("help/background", vOk),
  img_keyboard(NULL)
{
  Profile *res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  img_keyboard = new Sprite(GetResourceManager().LoadImage(res, "help/shortkeys"), true);
  img_keyboard->cache.EnableLastFrameCache();
  GetResourceManager().UnLoadXMLProfile(res);
}

HelpMenu::~HelpMenu()
{
  if (NULL != img_keyboard) {
    delete img_keyboard;
  }
}

bool HelpMenu::signal_ok()
{
  return true;
}

bool HelpMenu::signal_cancel()
{
  return true;
}

void HelpMenu::DrawBackground()
{
  Menu::DrawBackground();

  int border_x = int(GetMainWindow().GetWidth() - img_keyboard->GetWidth()) / 2;
  if (border_x < 0)
    border_x = 0;

  int border_y = int(GetMainWindow().GetHeight() - img_keyboard->GetHeight()) / 2;
  if (border_y < 0)
    border_y = 0;

  img_keyboard->Blit(GetMainWindow(), border_x, border_y);

  const uint MIDDLE_X = 64;
  const uint MIDDLE_Y = 13;

  Text tmp(_("Quit game"), dark_gray_color, Font::FONT_TINY, Font::FONT_BOLD, false); tmp.SetMaxWidth(130);
  tmp.DrawCenter(Point2i(15 + border_x + MIDDLE_X, 1 + border_y + MIDDLE_Y));
  tmp.SetText(_("High jump")); tmp.DrawCenter(Point2i(373 + border_x + MIDDLE_X, 313 + border_y + MIDDLE_Y));
  tmp.SetText(_("Jump")); tmp.DrawCenter(Point2i(373 + border_x + MIDDLE_X, 284 + border_y + MIDDLE_Y));
  tmp.SetText(_("Jump backwards")); tmp.DrawCenter(Point2i(373 + border_x + MIDDLE_X, 342 + border_y + MIDDLE_Y));
  tmp.SetText(_("Drag&drop: Move the camera")); tmp.DrawCenter(Point2i(454 + border_x + MIDDLE_X, 380 + border_y + MIDDLE_Y));
  tmp.SetText(_("Click: Center the camera on the character")); tmp.DrawCenter(Point2i(454 + border_x + MIDDLE_X, 410 + border_y + MIDDLE_Y));
  tmp.SetText(_("Show/hide the interface")); tmp.DrawCenter(Point2i(205 + border_x + MIDDLE_X, 31 + border_y + MIDDLE_Y));
  tmp.SetText(_("Fullscreen / window")); tmp.DrawCenter(Point2i(425 + border_x + MIDDLE_X, 30 + border_y + MIDDLE_Y));
  tmp.SetText(_("Configuration menu")); tmp.DrawCenter(Point2i(425 + border_x + MIDDLE_X, 59 + border_y + MIDDLE_Y));
  tmp.SetText(_("Talk in network battles")); tmp.DrawCenter(Point2i(26 + border_x + MIDDLE_X, 284 + border_y + MIDDLE_Y));
  tmp.SetText(_("Change the weapon category")); tmp.DrawCenter(Point2i(15 + border_x + MIDDLE_X, 60 + border_y + MIDDLE_Y));
  tmp.SetText(_("Change the weapon countdown")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 153 + border_y + MIDDLE_Y));
  tmp.SetText(_("Change the aim angle")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 182 + border_y + MIDDLE_Y));
  tmp.SetText(_("Move the character")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 274 + border_y + MIDDLE_Y));
  tmp.SetText(_("On map: Select a target")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 213 + border_y + MIDDLE_Y));
  tmp.SetText(_("On a character: Select it")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 244 + border_y + MIDDLE_Y));
  tmp.SetText(_("Show the weapons menu")); tmp.DrawCenter(Point2i(552 + border_x + MIDDLE_X, 121 + border_y + MIDDLE_Y));
  tmp.SetText(_("Smaller aim angle and walk step")); tmp.DrawCenter(Point2i(26 + border_x + MIDDLE_X, 314 + border_y + MIDDLE_Y));
  tmp.SetText(_("Move the camera with mouse or arrows")); tmp.DrawCenter(Point2i(320 + border_x + MIDDLE_X, 380 + border_y + MIDDLE_Y));
  tmp.SetText(_("Weapon: Fire / Bonus box: fall fast")); tmp.DrawCenter(Point2i(194 + border_x + MIDDLE_X, 313 + border_y + MIDDLE_Y));
  tmp.SetText(_("Show/hide the minimap")); tmp.DrawCenter(Point2i(205 + border_x + MIDDLE_X, 60 + border_y + MIDDLE_Y));
  tmp.SetText(_("Change the active character")); tmp.DrawCenter(Point2i(26 + border_x + MIDDLE_X, 343 + border_y + MIDDLE_Y));
  tmp.SetText(_("Center the camera on the character")); tmp.DrawCenter(Point2i(320 + border_x + MIDDLE_X, 410 + border_y + MIDDLE_Y));
  tmp.SetText(_("Quickly quit game with Ctrl")); tmp.DrawCenter(Point2i(15 + border_x + MIDDLE_X, 30 + border_y + MIDDLE_Y));
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
