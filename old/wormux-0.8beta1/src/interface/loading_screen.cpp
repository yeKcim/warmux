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
 * Loading screen
 *****************************************************************************/

#include "loading_screen.h"
#include "include/app.h"
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/sprite.h"

LoadingScreen * LoadingScreen::singleton = NULL;

LoadingScreen::LoadingScreen()
{
  // Get the background image
  Config * config = Config::GetInstance();
  AppWormux * app = AppWormux::GetInstance();

  loading_bg = new Sprite(Surface((
				   config->GetDataDir() + PATH_SEPARATOR
				   + "menu" + PATH_SEPARATOR
				   + "loading.png").c_str()));
  loading_bg->cache.EnableLastFrameCache();
  loading_bg->ScaleSize(app->video.window.GetWidth(), app->video.window.GetHeight());

  // Get profile from resource manager
  res = resource_manager.LoadXMLProfile( "graphism.xml", false);
}

LoadingScreen::~LoadingScreen()
{
  delete loading_bg;
  resource_manager.UnLoadXMLProfile(res);
}

// to manage singleton
LoadingScreen * LoadingScreen::GetInstance()
{
  if (singleton == NULL) {
    singleton = new LoadingScreen();
  }
  return singleton;
}

void LoadingScreen::DrawBackground()
{
  loading_bg->ScaleSize(AppWormux::GetInstance()->video.window.GetWidth(), AppWormux::GetInstance()->video.window.GetHeight());
  loading_bg->Blit( AppWormux::GetInstance()->video.window, 0, 0);
  AppWormux::GetInstance()->video.Flip();
}

void LoadingScreen::StartLoading(uint nb, std::string resource,
				 std::string label)
{
  Surface image = resource_manager.LoadImage(res, "loading_screen/"+resource);

  int slot_margin_x = (120/2 - image.GetWidth()/2);
  int x = (AppWormux::GetInstance()->video.window.GetWidth()/2)- (3*120) + nb*120;
  int y = (AppWormux::GetInstance()->video.window.GetHeight()/2)+40;

  Rectanglei dest ( x+slot_margin_x,
		    y,
		    image.GetWidth(),
		    image.GetHeight() );
  AppWormux::GetInstance()->video.window.Blit( image, dest.GetPosition());

  Font::GetInstance(Font::FONT_MEDIUM)->WriteCenter(Point2i(x+120/2, y+80), label, white_color);

  AppWormux::GetInstance()->video.Flip();
}




