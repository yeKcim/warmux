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
 * Game menu
 *****************************************************************************/

#include "game_menu.h"
#include "map_selection_box.h"
#include "teams_selection_box.h"

#include "game/game.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "graphic/video.h"
#include "graphic/font.h"
#include "include/app.h"
#include "tool/i18n.h"
#include "tool/string_tools.h"

#include <iostream>
const uint MARGIN_TOP    = 5;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 70;

const uint TEAMS_BOX_H = 170;
const uint OPTIONS_BOX_H = 150;


const uint TPS_TOUR_MIN = 10;
const uint TPS_TOUR_MAX = 120;
const uint TPS_FIN_TOUR_MIN = 1;
const uint TPS_FIN_TOUR_MAX = 10;



// ################################################
// ##  GAME MENU CLASS
// ################################################
GameMenu::GameMenu() :
  Menu("menu/bg_play")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Rectanglei rectZero(0, 0, 0, 0);
  Rectanglei stdRect (0, 0, 130, 30);

  Surface window = AppWormux::GetInstance()->video.window;

  // Calculate main box size
  uint mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  uint mapBoxHeight = (window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE)
    - TEAMS_BOX_H - OPTIONS_BOX_H;

  // ################################################
  // ##  TEAM SELECTION
  // ################################################
  team_box = new TeamsSelectionBox(Rectanglei(MARGIN_SIDE, MARGIN_TOP,
					      mainBoxWidth, TEAMS_BOX_H));

  widgets.AddWidget(team_box);

  // ################################################
  // ##  MAP SELECTION
  // ################################################
  map_box = new MapSelectionBox( Rectanglei(MARGIN_SIDE, team_box->GetPositionY()+team_box->GetSizeY()+ MARGIN_SIDE,
					    mainBoxWidth, mapBoxHeight));

  widgets.AddWidget(map_box);

  // ################################################
  // ##  GAME OPTIONS
  // ################################################
  game_options = new HBox( Rectanglei(MARGIN_SIDE, map_box->GetPositionY()+map_box->GetSizeY()+ MARGIN_SIDE,
					    mainBoxWidth/2, OPTIONS_BOX_H), true);
  game_options->AddWidget(new PictureWidget(Rectanglei(0,0,39,128), "menu/mode_label"));

  game_options->SetMargin(50);

  opt_duration_turn = new SpinButtonWithPicture(_("Duration of a turn"), "menu/timing_turn",
						stdRect,
						TPS_TOUR_MIN, 5,
						TPS_TOUR_MIN, TPS_TOUR_MAX);
  game_options->AddWidget(opt_duration_turn);

  opt_energy_ini = new SpinButtonWithPicture(_("Initial energy"), "menu/energy",
					     stdRect,
					     100, 5,
					     5, 200);
  game_options->AddWidget(opt_energy_ini);

  opt_scroll_on_border = new PictureTextCBox(_("Scroll on border"), "menu/scroll_on_border", stdRect);
  game_options->AddWidget(opt_scroll_on_border);

  game_options->AddWidget(new NullWidget(Rectanglei(0,0,50,10)));

  widgets.AddWidget(game_options);


  // Values initialization

  // Load game options
  GameMode::GetInstance()->Load();

  GameMode * game_mode = GameMode::GetInstance();
  opt_duration_turn->SetValue(game_mode->duration_turn);
  opt_energy_ini->SetValue(game_mode->character.init_energy);
  opt_scroll_on_border->SetValue(Config::GetInstance()->GetScrollOnBorder());

  resource_manager.UnLoadXMLProfile(res);
}

GameMenu::~GameMenu()
{
}

void GameMenu::OnClick(const Point2i &mousePosition, int button)
{
  widgets.Click(mousePosition, button);
}

void GameMenu::OnClickUp(const Point2i &mousePosition, int button)
{
  widgets.ClickUp(mousePosition, button);
}

void GameMenu::SaveOptions()
{
  // Map
  map_box->ValidMapSelection();

  // teams
  team_box->ValidTeamsSelection();

  //Save options in XML
  Config::GetInstance()->SetScrollOnBorder(opt_scroll_on_border->GetValue());
  Config::GetInstance()->Save();

  GameMode * game_mode = GameMode::GetInstance();
  game_mode->duration_turn = opt_duration_turn->GetValue() ;
  game_mode->character.init_energy = opt_energy_ini->GetValue() ;

}

bool GameMenu::signal_ok()
{
  SaveOptions();
  Game::GetInstance()->Start();
  return true;
}

bool GameMenu::signal_cancel()
{
  return true;
}

void GameMenu::Draw(const Point2i &mousePosition)
{

}

