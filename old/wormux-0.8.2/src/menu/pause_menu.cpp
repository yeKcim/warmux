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
 * Pause Menu
 *****************************************************************************/

#include "menu/pause_menu.h"
//-----------------------------------------------------------------------------
#include <sstream>
#include <iostream>
#include <algorithm>  //std::transform
#include "game/config.h"
#include "graphic/font.h"
#include "graphic/video.h"
#include "gui/box.h"
#include "gui/big/button_pic.h"
#include "include/app.h"
#include "menu/options_menu.h"
#include "menu/help_menu.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"
#include "tool/xml_document.h"
//-----------------------------------------------------------------------------

PauseMenu::PauseMenu(bool& _exit_game)  :
  Menu("interface/quit_screen", vNo),
  exit_game(_exit_game)
{
  Point2i size(100,100);

  Box* box = new GridBox(GetMainWindow().GetWidth()/2, size, true);

  bt_continue_play = new ButtonPic(_("Back to battle"),
				   "menu/ico_back_to_battle", size);
  box->AddWidget(bt_continue_play);

  bt_options_menu = new ButtonPic(_("Options menu"),
				  "menu/ico_options_menu", size);
  box->AddWidget(bt_options_menu);

  bt_help_menu = new ButtonPic(_("Help"),
 			  "menu/ico_help", size);
  box->AddWidget(bt_help_menu);

  bt_main_menu = new ButtonPic(_("Quit battle"),
			       "menu/ico_quit_battle", size);
  box->AddWidget(bt_main_menu);

  widgets.AddWidget(box);
  widgets.Pack();
  box->SetPosition(GetMainWindow().GetWidth()/2 - box->GetSizeX()/2,
		   280 * GetMainWindow().GetHeight() / 768); // those constants comes from the image "quit_screen"
  widgets.Pack();
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::BackToMainMenu()
{
  exit_game = true;
  close_menu = true;
}

void PauseMenu::BackToGame()
{
  exit_game = false;
  close_menu = true;
}

void PauseMenu::RunOptionsMenu()
{
  OptionMenu menu;
  menu.Run();
  close_menu = true;
}

void PauseMenu::RunHelpMenu()
{
  HelpMenu menu;
  menu.Run();
  close_menu = true;
}

bool PauseMenu::signal_ok()
{
  bool r = false;

  Widget * w = widgets.GetCurrentKeyboardSelectedWidget();
  if (w == bt_main_menu) {
    BackToMainMenu();
  } else if (w == bt_continue_play) {
    BackToGame();
  } else if (w == bt_options_menu) {
    RunOptionsMenu();
  } else if (w == bt_help_menu) {
    RunHelpMenu();
  }

  return r;
}

bool PauseMenu::signal_cancel()
{
  return false;
}


void PauseMenu::Draw(const Point2i& /*mousePosition*/)
{
}

void PauseMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void PauseMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);

  if (button == SDL_BUTTON_LEFT) {
    if (bt_main_menu->Contains(mousePosition)) {
      BackToMainMenu();
    } else if (bt_continue_play->Contains(mousePosition)) {
      BackToGame();
    } else if (bt_options_menu->Contains(mousePosition)) {
      RunOptionsMenu();
    } else if (bt_help_menu->Contains(mousePosition)) {
      RunHelpMenu();
    }
  }
}

//-----------------------------------------------------------------------------
