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
#include "gui/button_text.h"
#include "game/config.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "sound/jukebox.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

#ifndef WIN32
#include <dirent.h>
#endif

// Position du texte de la version
const int VERSION_DY = -40;

const int DEFAULT_SCREEN_HEIGHT = 768 ;

MainMenu::~MainMenu()
{
 // delete skin_left;
 // delete skin_right;
  delete version_text;
  delete website_text;
}

MainMenu::MainMenu() :
    Menu("main_menu/bg_main", vNo)
{
  int x_button;
  double y_scale;

  int button_width = 402;
  int button_height = 64;

  y_scale = (double)AppWormux::GetInstance()->video->window.GetHeight() / DEFAULT_SCREEN_HEIGHT ;

  x_button = AppWormux::GetInstance()->video->window.GetWidth()/2 - button_width/2;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);

  /* skin_left = new Sprite( resource_manager.LoadImage(res,"main_menu/skin_1"));
   skin_right = new Sprite( resource_manager.LoadImage(res,"main_menu/skin_2"));

  s_title = resource_manager.LoadImage(res,"main_menu/title");
  title = new PictureWidget(Rectanglei(AppWormux::GetInstance()->video->window.GetWidth()/2  - s_title.GetWidth()/2 + 10, 0, 648, 168));
  title->SetSurface(s_title); */

  int y = int(290 * y_scale) ;
  const int y2 = AppWormux::GetInstance()->video->window.GetHeight() + VERSION_DY -20 - button_height;

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

void MainMenu::button_click() const
{
  jukebox.Play("share", "menu/clic");
}

void MainMenu::SelectAction(const Widget *w)
{
  if (w == play) {
    choice = PLAY;
    close_menu = true;
  } else if(w == network) {
    choice = NETWORK;
    close_menu = true;
  } else if(w == options) {
    choice = OPTIONS;
    close_menu = true;
  } else if(w == infos) {
    choice = CREDITS;
    close_menu = true;
  } else if(w == quit) {
    choice = QUIT;
    close_menu = true;
  }
}

void MainMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  Widget* b = widgets.ClickUp(mousePosition,button);
  SelectAction(b);
  button_click();
}

void MainMenu::OnClick(const Point2i &/*mousePosition*/, int /*button*/)
{
  // nothing to do while button is still not released
}

MainMenu::menu_item MainMenu::Run ()
{
  choice = NONE;

  Menu::Run();

  ASSERT( choice != NONE );
  return choice;
}

bool MainMenu::signal_cancel()
{
  choice = QUIT;
  return true;
}

bool MainMenu::signal_ok()
{
  Widget * w = widgets.GetCurrentSelectedWidget();
  if(w != NULL) {
    SelectAction(widgets.GetCurrentSelectedWidget());
  } else {
    choice = PLAY;
  }
  return true;
}

void MainMenu::DrawBackground()
{
  Surface& window = AppWormux::GetInstance()->video->window;

  Menu::DrawBackground();
  // skin_left->Blit(window, 0, window.GetHeight() - skin_left->GetHeight());
  // skin_right->Blit(window, window.GetWidth()  - skin_right->GetWidth(),
  //                  window.GetHeight() - skin_right->GetHeight());

  version_text->DrawCenter( Point2i(window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY));
  website_text->DrawCenter( Point2i(window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY/2));

}

void MainMenu::Redraw(const Rectanglei& rect, Surface &window)
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
