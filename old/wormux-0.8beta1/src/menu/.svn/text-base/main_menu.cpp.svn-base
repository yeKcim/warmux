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
 * Game menu from which one may start a new game, modify options, obtain some
 * infomations or leave the game.
 *****************************************************************************/

#include "main_menu.h"
#include <string>
#include "game/config.h"
#include "game/time.h"
#include "graphic/effects.h"
#include "graphic/font.h"
#include "graphic/fps.h"
#include "include/app.h"
#include "include/constant.h"
#include "sound/jukebox.h"
#include "tool/i18n.h"
#include "tool/file_tools.h"
#include "tool/resource_manager.h"

#ifndef WIN32
#include <dirent.h>
#endif

// Position du texte de la version
const int VERSION_DY = -40;

const int DEFAULT_SCREEN_HEIGHT = 768 ;

Main_Menu::~Main_Menu()
{
 // delete skin_left;
 // delete skin_right;
  delete version_text;
  delete website_text;
}

Main_Menu::Main_Menu() :
    Menu("main_menu/bg_main", vNo)
{
  int x_button;
  double y_scale;

  int button_width = 402;
  int button_height = 64;

  y_scale = (double)AppWormux::GetInstance()->video.window.GetHeight() / DEFAULT_SCREEN_HEIGHT ;

  x_button = AppWormux::GetInstance()->video.window.GetWidth()/2 - button_width/2;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  /* skin_left = new Sprite( resource_manager.LoadImage(res,"main_menu/skin_1"));
   skin_right = new Sprite( resource_manager.LoadImage(res,"main_menu/skin_2"));

  s_title = resource_manager.LoadImage(res,"main_menu/title");
  title = new PictureWidget(Rectanglei(AppWormux::GetInstance()->video.window.GetWidth()/2  - s_title.GetWidth()/2 + 10, 0, 648, 168));
  title->SetSurface(s_title); */

  int y = int(290 * y_scale) ;
  const int y2 = AppWormux::GetInstance()->video.window.GetHeight() + VERSION_DY -20 - button_height;

  int dy = std::max((y2-y)/3, button_height);
  if(Config::GetInstance()->IsNetworkActivated())
    dy = std::max((y2-y)/4, button_height);

  play = new ButtonText(Point2i(x_button, y),
                        res, "main_menu/button",
                        _("Play"),
                        Font::FONT_LARGE, Font::FONT_NORMAL);
  y += dy;

  if(Config::GetInstance()->IsNetworkActivated()) {
    network = new ButtonText( Point2i(x_button, y),
                              res, "main_menu/button",
                              _("Network Game"),
                              Font::FONT_LARGE, Font::FONT_NORMAL );
    y += dy;
  } else {
    network = NULL;
  }

  options = new ButtonText(Point2i(x_button, y),
                           res, "main_menu/button",
                           _("Options"),
                           Font::FONT_LARGE, Font::FONT_NORMAL);
  y += dy;

  infos =  new ButtonText(Point2i(x_button, y),
                          res, "main_menu/button",
                          _("Credits"),
                          Font::FONT_LARGE, Font::FONT_NORMAL);
  y += dy;

  quit =  new ButtonText(Point2i(x_button, y),
                         res, "main_menu/button",
                         _("Quit"),
                         Font::FONT_LARGE, Font::FONT_NORMAL);

  widgets.AddWidget(play);
  if(Config::GetInstance()->IsNetworkActivated())
    widgets.AddWidget(network);
  widgets.AddWidget(options);
  widgets.AddWidget(infos);
  widgets.AddWidget(quit);
 // widgets.AddWidget(title);

  resource_manager.UnLoadXMLProfile( res);

  std::string s("Version "+Constants::VERSION);
  version_text = new Text(s, green_color, Font::FONT_MEDIUM, Font::FONT_NORMAL, false);

  std::string s2(Constants::WEB_SITE);
  website_text = new Text(s2, green_color, Font::FONT_MEDIUM, Font::FONT_NORMAL, false);

  if(!jukebox.IsPlayingMusic())
     jukebox.PlayMusic("menu");
}

void Main_Menu::button_click()
{
  jukebox.Play("share", "menu/clic");
}

void Main_Menu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* b = widgets.ClickUp(mousePosition,button);
  if (b == play)
  {
    choice = menuPLAY;
    close_menu = true;
    button_click();
  }
  else if(b == network && Config::GetInstance()->IsNetworkActivated())
  {
    choice = menuNETWORK;
    close_menu = true;
    button_click();
  }
  else if(b == options)
  {
    choice = menuOPTIONS;
    close_menu = true;
    button_click();
  }
  else if(b == infos)
  {
    choice = menuCREDITS;
    close_menu = true;
    button_click();
  }
  else if(b == quit)
  {
    choice = menuQUIT;
    close_menu = true;
    button_click();
  }
}

void Main_Menu::OnClick(const Point2i &mousePosition, int button)
{
  // nothing to do while button is still not released
}

menu_item Main_Menu::Run ()
{
  choice = menuNULL;

  Menu::Run();

  assert( choice != menuNULL );
  return choice;
}

bool Main_Menu::signal_cancel()
{
  choice = menuQUIT;
  return true;
}

bool Main_Menu::signal_ok()
{
  choice = menuPLAY;
  return true;
}

void Main_Menu::DrawBackground()
{
  Surface& window = AppWormux::GetInstance()->video.window;

  Menu::DrawBackground();
  // skin_left->Blit(window, 0, window.GetHeight() - skin_left->GetHeight());
  // skin_right->Blit(window, window.GetWidth()  - skin_right->GetWidth(),
  // 		   window.GetHeight() - skin_right->GetHeight());

  version_text->DrawCenter( window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY);
  website_text->DrawCenter( window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY/2);

}

void Main_Menu::Redraw(const Rectanglei& rect, Surface &window)
{
  Menu::Redraw(rect, window);

  // we never had to redraw texts
  // but sometimes we need to redraw the skins...

  /*Rectanglei dest(0, window.GetHeight() - skin_left->GetHeight(),
		  skin_left->GetWidth(), skin_left->GetHeight());
  dest.Clip(rect);

  Rectanglei src(rect.GetPositionX() - 0,
		 rect.GetPositionY() - (window.GetHeight() - skin_left->GetHeight()),
		 dest.GetSizeX(), dest.GetSizeY());

  skin_left->Blit(window, src, dest.GetPosition());

  Rectanglei dest2(window.GetWidth()  - skin_right->GetWidth(),
		   window.GetHeight() - skin_right->GetHeight(),
		   skin_right->GetWidth(), skin_right->GetHeight());
  dest2.Clip(rect);

  Rectanglei src2(dest2.GetPositionX() - (window.GetWidth()  - skin_right->GetWidth()),
		  dest2.GetPositionY() - (window.GetHeight() - skin_right->GetHeight()),
		  dest2.GetSizeX(), dest2.GetSizeY());

  skin_right->Blit(window, src2, dest2.GetPosition());*/


}
