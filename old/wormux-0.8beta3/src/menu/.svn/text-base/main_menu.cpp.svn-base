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

#include "menu/main_menu.h"
#include "gui/button_text.h"
#include "game/config.h"
#include "graphic/text.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "sound/jukebox.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/stats.h"

#ifndef WIN32
#include <dirent.h>
#endif

// Position du texte de la version
const int VERSION_DY = -40;

const int DEFAULT_SCREEN_HEIGHT = 768 ;

MainMenu::~MainMenu()
{
  delete version_text;
  delete website_text;
  StatStop("Main:Menu");
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

  int y = int(290 * y_scale) ;
  const int y2 = AppWormux::GetInstance()->video->window.GetHeight() + VERSION_DY -20 - button_height;

  int dy = std::max((y2-y)/3, button_height);
  if(Config::GetInstance()->IsNetworkActivated())
    dy = std::max((y2-y)/4, button_height);

  play = new ButtonText(res, "main_menu/button",
                        _("Play"),
                        Font::FONT_LARGE, Font::FONT_NORMAL);
  play->SetXY(x_button, y);
  y += dy;

  if(Config::GetInstance()->IsNetworkActivated()) {
    network = new ButtonText(res, "main_menu/button",
                             _("Network Game"),
                             Font::FONT_LARGE, Font::FONT_NORMAL );
    network->SetXY(x_button, y);
    y += dy;
  } else {
    network = NULL;
  }

  options = new ButtonText(res, "main_menu/button",
                           _("Options"),
                           Font::FONT_LARGE, Font::FONT_NORMAL);
  options->SetXY(x_button, y);
  y += dy;

  infos = new ButtonText(res, "main_menu/button",
			 _("Credits"),
			 Font::FONT_LARGE, Font::FONT_NORMAL);
  infos->SetXY(x_button, y);
  y += dy;

  quit =  new ButtonText(res, "main_menu/button",
                         _("Quit"),
                         Font::FONT_LARGE, Font::FONT_NORMAL);
  quit->SetXY(x_button, y);
  widgets.AddWidget(play);
  if(Config::GetInstance()->IsNetworkActivated())
    widgets.AddWidget(network);
  widgets.AddWidget(options);
  widgets.AddWidget(infos);
  widgets.AddWidget(quit);

  resource_manager.UnLoadXMLProfile( res);

  std::string s("Version "+Constants::VERSION);
  version_text = new Text(s, green_color, Font::FONT_MEDIUM, Font::FONT_NORMAL, false);

  std::string s2(Constants::WEB_SITE);
  website_text = new Text(s2, green_color, Font::FONT_MEDIUM, Font::FONT_NORMAL, false);

  if(!jukebox.IsPlayingMusic())
     jukebox.PlayMusic("menu");

  StatStart("Main:Menu");
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

  version_text->DrawCenter( Point2i(window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY));
  website_text->DrawCenter( Point2i(window.GetWidth()/2,
                            window.GetHeight() + VERSION_DY/2));

}

void MainMenu::Redraw(const Rectanglei& rect, Surface &window)
{
  Menu::Redraw(rect, window);
}
