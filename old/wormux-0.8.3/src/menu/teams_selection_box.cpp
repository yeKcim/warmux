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
 *  Teams selection box
 *****************************************************************************/

#include "game/config.h"
#include "menu/teams_selection_box.h"
#include "menu/team_box.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"

#include <iostream>

TeamsSelectionBox::TeamsSelectionBox(const Point2i &_size, bool network) :
  HBox(_size.y, network)
{
  if (!network)
    SetNoBorder();

  // How many teams ?
  if (network) {
    local_teams_nb = new SpinButtonWithPicture(_("Local teams:"),
					       "menu/team_number",
					       Point2i(130, W_UNDEF),
					       0, 1,
					       0, MAX_NB_TEAMS-1);
  } else {
    local_teams_nb = new SpinButtonWithPicture(_("Number of teams:"),
					       "menu/team_number",
					       Point2i(130, W_UNDEF),
					       2, 1,
					       2, MAX_NB_TEAMS);
  }
  AddWidget(local_teams_nb);

  uint teams_box_w = _size.x - local_teams_nb->GetSizeX() - 5;
  Point2i team_box_size(teams_box_w / (MAX_NB_TEAMS /2) - 10, _size.y/2 -15);

  Box * teams_grid_box = new GridBox(teams_box_w, team_box_size, false);
  teams_grid_box->SetNoBorder();

  for (uint i=0; i < MAX_NB_TEAMS; i++) {
    std::string player_name = _("Player") ;
    char num_player[4];
    sprintf(num_player, " %d", i+1);
    player_name += num_player;
    teams_selections.push_back(new TeamBox(player_name, team_box_size));
    teams_grid_box->AddWidget(teams_selections.at(i));
  }

  AddWidget(teams_grid_box);

  // Load Teams' list
  GetTeamsList().full_list.sort(compareTeams);


  // initialize teams
  if (network) {
    // for network game
    GetTeamsList().Clear();

    // No selected team(s) by default
    for (uint i=0; i<teams_selections.size(); i++) {
      teams_selections.at(i)->ClearTeam();
    }

  } else {
    // for local game

    GetTeamsList().InitList(Config::GetInstance()->AccessTeamList());

    TeamsList::iterator
      it=GetTeamsList().playing_list.begin(),
      end=GetTeamsList().playing_list.end();

    uint j=0;
    for (; it != end && j<teams_selections.size(); ++it, j++)
      {
	teams_selections.at(j)->SetTeam((**it), true);
      }

    // we need at least 2 teams
    if (j < 2) {
      SetNbTeams(2);
      local_teams_nb->SetValue(2);
    } else {
      local_teams_nb->SetValue(j);
    }
  }
}

Widget* TeamsSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition)) return NULL;

  if (local_teams_nb->ClickUp(mousePosition, button)){
    SetNbTeams(local_teams_nb->GetValue());

  } else {
    for (uint i=0; i<teams_selections.size() ; i++) {

      if (teams_selections.at(i)->Contains(mousePosition)) {

        Widget * w = teams_selections.at(i)->ClickUp(mousePosition, button);

        if (w == NULL) {
          Rectanglei r(teams_selections.at(i)->GetPositionX(),
                       teams_selections.at(i)->GetPositionY(),
                       60,
                       60);
          if ( r.Contains(mousePosition) ) {
            if ( button == SDL_BUTTON_LEFT || button == SDL_BUTTON_WHEELDOWN ) {
              NextTeam(i);
            } else if ( button == SDL_BUTTON_RIGHT || button == SDL_BUTTON_WHEELUP ) {
              PrevTeam(i);
            }
          }
        } else {
          return w;
        }
        break;
      }
    }
  }

  return NULL;
}

Widget* TeamsSelectionBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void TeamsSelectionBox::PrevTeam(int i)
{
  if (teams_selections.at(i)->GetTeam() == NULL) return;

  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  GetTeamsList().FindById(teams_selections.at(i)->GetTeam()->GetId(), previous_index);

  index = previous_index-1;

  do
    {
      to_continue = false;

      // select the last team if we are outside list
      if ( index < 0 )
        index = int(GetTeamsList().full_list.size())-1;

      // Get the team at current index
      tmp = GetTeamsList().FindByIndex(index);

      // Check if that team is already selected
      for (int j = 0; j < local_teams_nb->GetValue(); j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index--;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue)
        teams_selections.at(i)->SetTeam(*tmp);
    } while ( index != previous_index && to_continue);
}

void TeamsSelectionBox::NextTeam(int i)
{
  if (teams_selections.at(i)->GetTeam() == NULL) return;

  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  GetTeamsList().FindById(teams_selections.at(i)->GetTeam()->GetId(), previous_index);

  index = previous_index+1;

  do
    {
      to_continue = false;

      // select the first team if we are outside list
      if ( index >= int(GetTeamsList().full_list.size()) )
        index = 0;

      // Get the team at current index
      tmp = GetTeamsList().FindByIndex(index);

      // Check if that team is already selected
      for (int j = 0; j < local_teams_nb->GetValue(); j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index++;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue)
        teams_selections.at(i)->SetTeam(*tmp);
    } while ( index != previous_index && to_continue);
}

void TeamsSelectionBox::SetNbTeams(uint nb_teams)
{
  // we hide the useless teams selector
  for (uint i=nb_teams; i<teams_selections.size(); i++) {
    teams_selections.at(i)->ClearTeam();
  }

  for (uint i=0; i<nb_teams;i++) {
    if (teams_selections.at(i)->GetTeam() == NULL) {
      // we should find an available team
      teams_selections.at(i)->SetTeam(*(GetTeamsList().FindByIndex(i)));
      NextTeam(i);
    }
  }
}

void TeamsSelectionBox::ValidTeamsSelection()
{
  uint nb_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL)
    {
      nb_teams++;
      teams_selections.at(i)->GetTeam()->AttachCustomTeam(teams_selections.at(i)->GetCustomTeam());
    }
  }

  if (nb_teams >= 2) {
    std::list<uint> selection;

    for (uint i=0; i < teams_selections.size(); i++) {

      if (teams_selections.at(i)->GetTeam() != NULL) {

        int index = -1;
        teams_selections.at(i)->ValidOptions();
        GetTeamsList().FindById(teams_selections.at(i)->GetTeam()->GetId(), index);
        if (index > -1)
        {
          selection.push_back(uint(index));

        }
      }
    }
    GetTeamsList().ChangeSelection (selection);

  }
}
