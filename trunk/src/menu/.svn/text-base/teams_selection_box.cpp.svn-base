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
 *  Teams selection box
 *****************************************************************************/

#include "game/config.h"
#include "game/game_mode.h"
#include "menu/teams_selection_box.h"
#include "menu/team_box.h"
#include "gui/grid_box.h"
#include "gui/label.h"
#include "gui/scroll_box.h"
#include "gui/null_widget.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "gui/vertical_box.h"
#include "team/teams_list.h"
#include "team/team.h"

#include <iostream>

TeamScrollBox::TeamScrollBox(const std::vector<TeamBox*>& teams, const Point2i &size)
  : ScrollBox(size)
  , teams(teams)
  , count(2)
{
  // SetNbTeams not called immediately
  AddWidget(teams[0]);
  AddWidget(teams[1]);
}

TeamScrollBox::~TeamScrollBox()
{
  // Don't let the vbox delete the items, we're doing it ourselves
  box->Empty();

  // Destroy widgets
  for (uint i=0; i<teams.size() ; i++)
    delete (teams[i]);

  teams.clear();
}

void TeamScrollBox::SetNbTeams(uint nb)
{
  // Reset the list and readd the widget
  count = 0;
  box->Empty();

  for (uint i = 0; count < nb; i++) {
    ASSERT(i < teams.size());

    // It is easy to have hole in the selection
    // with network game
    if (teams[i]->GetTeam()) {
      AddWidget(teams[i]);
      count++;
    }
  }
  ASSERT(count == nb);

  Pack();
  NeedRedrawing();
}

TeamsSelectionBox::TeamsSelectionBox(const Point2i &_size, bool network, bool w_border) :
  HBox(_size.y, w_border, false)
{
  if (!w_border)
    SetNoBorder();
  SetMargin(0);

  // How many teams ?
  VBox *tmp = new VBox(120, false, false, true);
  if (network) {
    local_teams_nb =
      new SpinButtonWithPicture(_("Local teams:"), "menu/team_number",
                                Point2i(100, 130), 0, 1, 0, MAX_NB_TEAMS);
  } else {
    local_teams_nb =
      new SpinButtonWithPicture(_("Number of teams:"), "menu/team_number",
                                Point2i(100, 130), 2, 1, 2, MAX_NB_TEAMS);
  }
  tmp->AddWidget(local_teams_nb);
  //tmp->AddWidget(new NullWidget(Point2i(120, 120)));
  AddWidget(tmp);

  uint box_w = _size.x - local_teams_nb->GetSizeX() - 10;
  Point2i grid_size = Point2i(box_w, _size.y);
  Point2i grid_dim = grid_size / Point2i(300 + 10, 130 + 10);
  Point2i box_size;
  bool use_list;
  if (grid_dim.x*grid_dim.y < (int)MAX_NB_TEAMS) {
    use_list = true;
    box_size.SetValues(box_w - 40, 120);
  } else {
    use_list = false;
    box_size.SetValues((grid_size / grid_dim) - 10);
  }

  for (uint i=0; i < MAX_NB_TEAMS; i++) {
    std::string player_name = _("Player") ;
    char num_player[4];
    sprintf(num_player, " %d", i+1);
    player_name += num_player;
    teams_selections.push_back(new TeamBox(player_name, box_size, i));
  }

  // If the intended gridbox would be too big for the intended size,
  // instead create a listbox
  if (use_list) {
    // Warning: this box takes the ownership of the widgets in teams_selections:
    // while any other Box will delete the ones it contains, TeamScrollBox
    // doesn't really contain them as widgets. They therefore aren't released
    // through this mechanism, but with a manual one. This manual mechanism
    // requires we have a *real* copy of the vector for when it is destroyed.
    list_box = new TeamScrollBox(teams_selections, Point2i(box_w-20, _size.y-10));
    list_box->SetNbTeams(0);

    AddWidget(list_box);
  } else {
    list_box = NULL;
    Box * teams_grid_box = new GridBox(grid_dim.y, grid_dim.x, 10, false);
    teams_grid_box->SetNoBorder();

    for (uint i=0; i<MAX_NB_TEAMS; i++)
      teams_grid_box->AddWidget(teams_selections[i]);

    AddWidget(teams_grid_box);
  }

  // Load Teams' list
  GetTeamsList().full_list.sort(compareTeams);
}

void TeamsSelectionBox::Draw(const Point2i& mousePosition)
{
  if (list_box)
    list_box->Draw(mousePosition);
}

Widget* TeamsSelectionBox::Click(const Point2i &mousePosition, uint button)
{
  return (list_box) ? list_box->Click(mousePosition, button) : NULL;
}

Widget* TeamsSelectionBox::DefaultClickUp(const Point2i &mousePosition, uint button)
{
  Widget *w = (list_box) ? list_box->ClickUp(mousePosition, button)
                          : WidgetList::ClickUp(mousePosition, button);

  for (uint i=0; i<teams_selections.size() ; i++) {

    if (teams_selections[i]->Contains(mousePosition) &&
        teams_selections[i]->IsLocal()) {
      TeamBox * at = teams_selections[i];

      if (at->IsTeamSwitcherAt(mousePosition)) {
        if (button == Mouse::BUTTON_LEFT() || button == SDL_BUTTON_WHEELDOWN) {
          NextTeam(i);
        } else if (button == Mouse::BUTTON_RIGHT() || button == SDL_BUTTON_WHEELUP) {
          PrevTeam(i);
        }
        return at;
      }
        
      if (at->IsAISwitcherAt(mousePosition)) {
        at->SwitchPlayerType();
        return at;
      }

      return w;
    }
  }

  return w;
}

// =============================================================================

LocalTeamsSelectionBox::LocalTeamsSelectionBox(const Point2i &size, bool border) :
  TeamsSelectionBox(size, false, border)
{
  GetTeamsList().InitList(Config::GetInstance()->AccessTeamList());

  TeamsList::iterator it  = GetTeamsList().playing_list.begin(),
    end = GetTeamsList().playing_list.end();

  uint j=0;
  for (; it != end && j<teams_selections.size(); ++it, j++) {
    teams_selections.at(j)->SetTeam((**it), true);
  }

  // we need at least 2 teams
  if (j < 2) {
    SetNbTeams(2);
    local_teams_nb->SetValue(2);
    teams_selections.at(1)->SetAILevel(1);
  } else {
    SetNbTeams(j);
    local_teams_nb->SetValue(j);
  }
}

Widget* LocalTeamsSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition))
    return NULL;

  if (local_teams_nb->ClickUp(mousePosition, button))
    SetNbTeams(local_teams_nb->GetValue());
  else
    return DefaultClickUp(mousePosition, button);

  return NULL;
}

void LocalTeamsSelectionBox::PrevTeam(uint i)
{
  TeamBox *at = teams_selections[i];
  if (!at->GetTeam())
    return;

  bool stop;
  int  previous_index = -1, index;

  GetTeamsList().FindById(at->GetTeam()->GetId(), previous_index);

  index = previous_index-1;

  do {
    stop = true;

    // select the last team if we are outside list
    if (index < 0)
      index = int(GetTeamsList().full_list.size())-1;

    // Get the team at current index
    Team *tmp = GetTeamsList().FindByIndex(index);

    // Check if that team is already selected
    for (uint j = 0; j < (uint)local_teams_nb->GetValue(); j++) {
      if (j!= i && tmp == teams_selections[j]->GetTeam()) {
        index--;
        stop = false;
        break;
      }
    }

    // We have found a team which is not selected
    if (tmp && stop)
      at->SetTeam(*tmp);
  } while (index != previous_index && !stop);
}

void LocalTeamsSelectionBox::NextTeam(uint i)
{
  TeamBox *at = teams_selections[i];
  if (!at->GetTeam())
    return;

  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  GetTeamsList().FindById(at->GetTeam()->GetId(), previous_index);

  index = previous_index+1;

  do {
    to_continue = false;

    // select the first team if we are outside list
    if (index >= int(GetTeamsList().full_list.size()))
      index = 0;

    // Get the team at current index
    tmp = GetTeamsList().FindByIndex(index);

    // Check if that team is already selected
    for (uint j = 0; j < (uint)local_teams_nb->GetValue(); j++) {
      if (j!= i && tmp == teams_selections[j]->GetTeam()) {
        index++;
        to_continue = true;
        break;
      }
    }

    // We have found a team which is not selected
    if (tmp && !to_continue)
      at->SetTeam(*tmp);
  } while (index != previous_index && to_continue);
}

void LocalTeamsSelectionBox::SetNbTeams(uint nb_teams)
{
  // we hide the useless teams selector
  for (uint i=nb_teams; i<teams_selections.size(); i++) {
    teams_selections[i]->ClearTeam();
  }

  for (uint i=0; i<nb_teams;i++) {
    if (!teams_selections[i]->GetTeam()) {
      // we should find an available team
      teams_selections[i]->SetTeam(*(GetTeamsList().FindByIndex(i)));
      NextTeam(i);
    }
  }

  if (list_box)
    list_box->SetNbTeams(nb_teams);
}

void LocalTeamsSelectionBox::ValidTeamsSelection()
{
  uint nb_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam())
      nb_teams++;
  }

  if (nb_teams >= 2) {
    std::list<uint> selection;

    for (uint i=0; i < teams_selections.size(); i++) {
      TeamBox *at = teams_selections[i];
      if (at->GetTeam()) {

        int index = -1;
        at->ValidOptions();
        GetTeamsList().FindById(at->GetTeam()->GetId(), index);
        if (index > -1) {
          selection.push_back(uint(index));
        }
      }
    }

    GetTeamsList().ChangeSelection(selection);
  }
}
