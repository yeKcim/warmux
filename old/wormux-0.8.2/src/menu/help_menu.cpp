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
#include "graphic/text.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "menu/help_menu.h"
#include "game/config.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"

static const uint BORDER        = 20;
static const uint CHECKBOX_SIZE = 50;

HelpMenu::HelpMenu()  :
  Menu("help/background", vOk)
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  img_keyboard = new Sprite(resource_manager.LoadImage(res, "help/shortkeys"), true);
  img_keyboard->cache.EnableLastFrameCache();
  resource_manager.UnLoadXMLProfile(res);
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

void HelpMenu::DrawBackground()
{
  Menu::DrawBackground();
  img_keyboard->Blit(GetMainWindow(), BORDER, BORDER);

  const uint MIDDLE_X = 64;
  const uint MIDDLE_Y = 13;

  Text tmp(_("Quit game"), dark_gray_color, Font::FONT_TINY, Font::FONT_NORMAL, false);
  tmp.SetMaxWidth(130);

  tmp.DrawCenter(Point2i(13+BORDER+MIDDLE_X, 5+BORDER+MIDDLE_Y));
  tmp.Set(_("Show/hide interface")); tmp.DrawCenter(Point2i(148+BORDER+MIDDLE_X, 5+BORDER+MIDDLE_Y));
  tmp.Set(_("Fullscreen / window")); tmp.DrawCenter(Point2i(311+BORDER+MIDDLE_X, 5+BORDER+MIDDLE_Y));
  tmp.Set(_("High jump")); tmp.DrawCenter(Point2i(460+BORDER+MIDDLE_X, 5+BORDER+MIDDLE_Y));
  tmp.Set(_("Talk in network battle")); tmp.DrawCenter(Point2i(13+BORDER+MIDDLE_X, 32+BORDER+MIDDLE_Y));
  tmp.Set(_("Change weapon category")); tmp.DrawCenter(Point2i(148+BORDER+MIDDLE_X, 32+BORDER+MIDDLE_Y));
  tmp.Set(_("Configuration menu")); tmp.DrawCenter(Point2i(311+BORDER+MIDDLE_X, 32+BORDER+MIDDLE_Y));
  tmp.Set(_("Jump")); tmp.DrawCenter(Point2i(460+BORDER+MIDDLE_X, 32+BORDER+MIDDLE_Y));
  tmp.Set(_("Drag&drop: Move camera")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 82+BORDER+MIDDLE_Y));
  tmp.Set(_("Click: Center camera on character")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 111+BORDER+MIDDLE_Y));
  tmp.Set(_("Change weapon countdown")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 140+BORDER+MIDDLE_Y));
  tmp.Set(_("Change aim angle")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 169+BORDER+MIDDLE_Y));
  tmp.Set(_("Move character")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 198+BORDER+MIDDLE_Y));
  tmp.Set(_("On map: Select a target")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 227+BORDER+MIDDLE_Y));
  tmp.Set(_("On a character: Changes active one")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 255+BORDER+MIDDLE_Y));
  tmp.Set(_("Show weapons menu")); tmp.DrawCenter(Point2i(539+BORDER+MIDDLE_X, 284+BORDER+MIDDLE_Y));
  tmp.Set(_("Smaller aim angle and walk step")); tmp.DrawCenter(Point2i(25+BORDER+MIDDLE_X, 253+BORDER+MIDDLE_Y));
  tmp.Set(_("Jump backwards")); tmp.DrawCenter(Point2i(188+BORDER+MIDDLE_X, 253+BORDER+MIDDLE_Y));
  tmp.Set(_("Pause")); tmp.DrawCenter(Point2i(337+BORDER+MIDDLE_X, 253+BORDER+MIDDLE_Y));
  tmp.Set(_("Move camera with mouse or arrows")); tmp.DrawCenter(Point2i(25+BORDER+MIDDLE_X, 281+BORDER+MIDDLE_Y));
  tmp.Set(_("Weapon: Fire / Bonus box: falls fast")); tmp.DrawCenter(Point2i(188+BORDER+MIDDLE_X, 281+BORDER+MIDDLE_Y));
  tmp.Set(_("Show/hide minimap")); tmp.DrawCenter(Point2i(337+BORDER+MIDDLE_X, 281+BORDER+MIDDLE_Y));
  tmp.Set(_("Change active character")); tmp.DrawCenter(Point2i(25+BORDER+MIDDLE_X, 310+BORDER+MIDDLE_Y));
  tmp.Set(_("Center camera to character")); tmp.DrawCenter(Point2i(188+BORDER+MIDDLE_X, 310+BORDER+MIDDLE_Y));
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
