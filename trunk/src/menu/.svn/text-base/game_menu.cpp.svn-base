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
#include "gui/question.h"
#include "gui/combo_box.h"
#include "include/app.h"
#include "tool/resource_manager.h"
#include "team/team.h"
#include "team/teams_list.h"

static const uint MARGIN_TOP    = 5;
static const uint MARGIN_SIDE   = 5;
static const uint MARGIN_BOTTOM = 40;

// ################################################
// ##  GAME MENU CLASS
// ################################################
GameMenu::GameMenu() :
  Menu("menu/bg_play")
{
  Surface& window = GetMainWindow();

  // Calculate main box size
  int  team_box_height = 240;
  uint mainBoxWidth = window.GetWidth() - 2*MARGIN_SIDE;
  uint mainBoxHeight = window.GetHeight() - MARGIN_TOP - MARGIN_BOTTOM - 2*MARGIN_SIDE;
  uint mapsHeight = mainBoxHeight - team_box_height - 60;
  uint multitabsWidth = mainBoxWidth;
  bool multitabs = false;
  if (window.GetWidth() > 640 && mapsHeight > 200) {
    multitabs = true;
    multitabsWidth = mainBoxWidth - 20;
    mapsHeight = 200;
    team_box_height = mainBoxHeight - 200 - 60;
  } else {
    mapsHeight = mainBoxHeight - 60;
    team_box_height = mainBoxHeight - 60;
  }

  MultiTabs * tabs = new MultiTabs(Point2i(mainBoxWidth, mainBoxHeight));

  // ################################################
  // ##  TEAM AND MAP SELECTION
  // ################################################

  team_box = new LocalTeamsSelectionBox(Point2i(multitabsWidth-4, team_box_height), multitabs);

  map_box = new MapSelectionBox(Point2i(multitabsWidth-4, mapsHeight), multitabs);

  if (!multitabs) {
    tabs->AddNewTab("TAB_Team", _("Teams"), team_box);
    tabs->AddNewTab("TAB_Map", _("Map"), map_box);
  } else {
    VBox *box = new VBox(mainBoxWidth, false, false, true);
    std::string tabs_title = _("Teams") + std::string(" - ");
    tabs_title += _("Map");

    box->AddWidget(team_box);
    box->AddWidget(map_box);
    tabs->AddNewTab("TAB_Team_Map", tabs_title, box);
  }

  // ################################################
  // ##  GAME OPTIONS
  // ################################################
  game_options = new GameModeEditor(Point2i(multitabsWidth-8, mainBoxHeight-tabs->GetHeaderHeight()-4),
                                    (mainBoxHeight-8)/420.0f, false);
  tabs->AddNewTab("TAB_Game", _("Game"), game_options);

  tabs->SetPosition(MARGIN_SIDE, MARGIN_TOP);

  widgets.AddWidget(tabs);
  widgets.Pack();
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

  const std::vector<Team*>& playing_list = GetTeamsList().playing_list;
  std::vector<Team*>::const_iterator it  = playing_list.begin();

  if (playing_list.size() <= 1) {
    Question q(Question::WARNING);
    uint num = playing_list.size();
    q.Set(Format(ngettext("There is only %u team.", "There are only %u teams.", num),
                 num), true, 0);
    q.Ask();
    return false;
  }

  bool found       = false;
  uint first_group = (*it)->GetGroup();
  for (; it != playing_list.end(); it++) {
    if ((*it)->GetGroup() != first_group) {
      found = true;
      break;
    }
  }

  if (!found) {
    Question q(Question::WARNING);
    q.Set(_("Please select a group different from your opponent!"), true, 0);
    q.Ask();
    return false;
  }

  play_ok_sound();
  Game::UpdateGameRules()->Start();
  return true;
}
