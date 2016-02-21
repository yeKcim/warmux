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
#include "gui/box.h"
#include "gui/big/button_pic.h"
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
  Box* box = new HBox(110, true);
  Point2i size(120,100);

  play = new ButtonPic(_("Play"), "menu/ico_play", size);
  box->AddWidget(play);

  network = new ButtonPic(_("Network Game"), "menu/ico_network_menu", size);
  box->AddWidget(network);

  options = new ButtonPic(_("Options"), "menu/ico_options_menu", size);
  box->AddWidget(options);

  help = new ButtonPic(_("Help"), "menu/ico_help", size);
  box->AddWidget(help);

  credits = new ButtonPic(_("Credits"), "menu/ico_credits", size);
  box->AddWidget(credits);

  quit =  new ButtonPic(_("Quit"), "menu/ico_quit", size);
  box->AddWidget(quit);

  widgets.AddWidget(box);

  uint center_x = AppWormux::GetInstance()->video->window.GetWidth()/2;
  uint center_y = AppWormux::GetInstance()->video->window.GetHeight()/2;
  box->SetXY(center_x - box->GetSizeX()/2, center_y - box->GetSizeY()/2);


  std::string s("Version "+Constants::WORMUX_VERSION);
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
  } else if(w == credits) {
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
  Widget * w = widgets.GetCurrentKeyboardSelectedWidget();
  if(w != NULL) {
    SelectAction(widgets.GetCurrentKeyboardSelectedWidget());
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

void MainMenu::RedrawBackground(const Rectanglei& rect, Surface &window)
{
  Menu::RedrawBackground(rect, window);
}
