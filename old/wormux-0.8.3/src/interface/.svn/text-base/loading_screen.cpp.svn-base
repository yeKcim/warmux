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
 * Loading screen
 *****************************************************************************/

#include "interface/loading_screen.h"
#include "include/app.h"
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "tool/resource_manager.h"

LoadingScreen::LoadingScreen()
{
  // Get the background image
  Config * config = Config::GetInstance();
  AppWormux * app = AppWormux::GetInstance();

  loading_bg = new Sprite(Surface((
                                   config->GetDataDir()
                                   + "menu" + PATH_SEPARATOR
                                   + "loading.png").c_str()),
			  true);
  loading_bg->cache.EnableLastFrameCache();
  loading_bg->ScaleSize(app->video->window.GetWidth(), app->video->window.GetHeight());

  // Get profile from resource manager
  res = GetResourceManager().LoadXMLProfile( "graphism.xml", false);
  DrawBackground();
}

LoadingScreen::~LoadingScreen()
{
  delete loading_bg;
  GetResourceManager().UnLoadXMLProfile(res);
}

void LoadingScreen::DrawBackground()
{
  loading_bg->ScaleSize(GetMainWindow().GetWidth(), GetMainWindow().GetHeight());
  loading_bg->Blit( GetMainWindow(), 0, 0);
  AppWormux::GetInstance()->video->Flip();
}

void LoadingScreen::StartLoading(uint nb, const std::string& resource,
                                 const std::string& label) const
{
  const Surface& image = GetResourceManager().LoadImage(res, "loading_screen/"+resource);

  int slot_margin_x = (120/2 - image.GetWidth()/2);
  int x = (GetMainWindow().GetWidth()/2)- (3*120) + nb*120;
  int y = (GetMainWindow().GetHeight()/2)+40;

  Rectanglei dest ( x+slot_margin_x,
                    y,
                    image.GetWidth(),
                    image.GetHeight() );
  GetMainWindow().Blit( image, dest.GetPosition());

  Font::GetInstance(Font::FONT_MEDIUM)->WriteCenter(Point2i(x+120/2, y+80), label, white_color);

  AppWormux::GetInstance()->video->Flip();
}




