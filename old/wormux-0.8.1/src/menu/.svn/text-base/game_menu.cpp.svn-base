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
 * Game menu
 *****************************************************************************/

#include "menu/game_menu.h"
#include "menu/game_mode_editor.h"
#include "menu/map_selection_box.h"
#include "menu/teams_selection_box.h"

#include "game/game.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "graphic/video.h"
#include "gui/tabs.h"
#include "gui/combo_box.h"
#include "include/app.h"
#include "tool/i18n.h"
#include "tool/resource_manager.h"

const uint MARGIN_TOP    = 5;
const uint MARGIN_SIDE   = 5;
const uint MARGIN_BOTTOM = 50;

const uint TEAMS_BOX_H = 205;
const uint OPTIONS_BOX_H = 150;

// ################################################
// ##  GAME MENU CLASS
// ################################################
GameMenu::GameMenu() :
  Menu("menu/bg_play")
{
  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml",false);
  Point2i stdSize(130, W_UNDEF);

  Surface& window = GetMainWindow();

  // Calculate main box size
  uint mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  uint multitabsHeight = (window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE)
    - TEAMS_BOX_H;

  // ################################################
  // ##  TEAM SELECTION
  // ################################################
  MultiTabs * tabs_team = new MultiTabs(Point2i(mainBoxWidth, TEAMS_BOX_H));

  team_box = new TeamsSelectionBox(Point2i(mainBoxWidth, TEAMS_BOX_H - 25));
  tabs_team->AddNewTab("TAB_Team", _("Teams"), team_box);

  tabs_team->SetPosition(MARGIN_SIDE, MARGIN_TOP);

  widgets.AddWidget(tabs_team);

  // ################################################
  // ##  MAP SELECTION
  // ################################################
  MultiTabs * tabs = new MultiTabs(Point2i(mainBoxWidth, multitabsHeight));

  map_box = new MapSelectionBox(Point2i(mainBoxWidth-10, multitabsHeight - 50));
  tabs->AddNewTab("TAB_Map", _("Map"), map_box);

  // ################################################
  // ##  GAME OPTIONS
  // ################################################
  Point2i option_size(130, 130);

  game_options = new GameModeEditor(mainBoxWidth, option_size, false);
  tabs->AddNewTab("TAB_Game", _("Game"), game_options);

  tabs->SetPosition(MARGIN_SIDE, tabs_team->GetPositionY()+tabs_team->GetSizeY()+ MARGIN_TOP);

  widgets.AddWidget(tabs);
  widgets.Pack();

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
  Widget *w = widgets.ClickUp(mousePosition, button);

  if (w == game_options->GetGameModeComboBox()) {
    game_options->LoadGameMode();
  }
}

void GameMenu::SaveOptions()
{
  // Map
  map_box->ValidMapSelection();

  // teams
  team_box->ValidTeamsSelection();

  //Save options in XML (including current selected teams, selected map)
  Config::GetInstance()->Save(true);

  game_options->ValidGameMode();
}

bool GameMenu::signal_ok()
{
  SaveOptions();
  play_ok_sound();
  Game::UpdateGameMode()->Start();
  return true;
}

bool GameMenu::signal_cancel()
{
  return true;
}

void GameMenu::key_left()
{
  map_box->ChangeMapDelta(-1);
}

void GameMenu::key_right()
{
  map_box->ChangeMapDelta(1);
}

void GameMenu::Draw(const Point2i &/*mousePosition*/)
{

}

