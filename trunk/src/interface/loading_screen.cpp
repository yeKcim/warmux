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
 * Loading screen
 *****************************************************************************/

#include "interface/loading_screen.h"
#include "include/app.h"
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "tool/resource_manager.h"

LoadingScreen::LoadingScreen(int icon_count):
  icon_count(icon_count)
{
  // Get the background image
  Config * config = Config::GetInstance();
  AppWarmux * app = AppWarmux::GetInstance();

  loading_bg = new Sprite(Surface((config->GetDataDir()
                                   + "menu" + PATH_SEPARATOR
                                   + "background_loading.jpg").c_str()));
  loading_bg->ScaleSize(app->video->window.GetWidth(), app->video->window.GetHeight());

  std::string loading_str(_("loading..."));
  loading_text = new Text(loading_str, white_color, Font::FONT_HUGE);

  // Get profile from resource manager
  res = GetResourceManager().LoadXMLProfile("graphism.xml", false);
  DrawBackground();
}

LoadingScreen::~LoadingScreen()
{
  delete loading_bg;
  delete loading_text;
}

void LoadingScreen::DrawBackground()
{
  loading_bg->ScaleSize(GetMainWindow().GetWidth(), GetMainWindow().GetHeight());
  loading_bg->Blit(GetMainWindow(), 0, 0);

  Point2i loading_text_pos(GetMainWindow().GetWidth()/2,
			   GetMainWindow().GetHeight()/2 - 80);
  loading_text->DrawCenter(loading_text_pos);

  AppWarmux::GetInstance()->video->Flip();
}

void LoadingScreen::StartLoading(uint nb, const std::string& resource,
                                 const std::string& label) const
{
  int index = nb-1;
  const Surface& image = GetResourceManager().LoadImage(res, "loading_screen/"+resource);

  int slot_margin_x = (120/2 - image.GetWidth()/2);
  int x = ((GetMainWindow().GetWidth() - icon_count*120)/2)+ index*120;
  int y = (GetMainWindow().GetHeight()/2)+40;

  Rectanglei dest (x+slot_margin_x, y, image.GetWidth(), image.GetHeight());
  GetMainWindow().Blit(image, dest.GetPosition());

  Text tmp(label, white_color, Font::FONT_MEDIUM, Font::FONT_BOLD, true);
  tmp.DrawCenter(Point2i(x+120/2, y+80));

  AppWarmux::GetInstance()->video->Flip();
}
