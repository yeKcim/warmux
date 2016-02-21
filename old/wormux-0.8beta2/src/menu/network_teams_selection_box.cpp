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
 *  Network teams selection box
 *****************************************************************************/

#include "network_teams_selection_box.h"
#include "team_box.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/spin_button_big.h"
#include "gui/text_box.h"
#include "include/action_handler.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"


NetworkTeamsSelectionBox::NetworkTeamsSelectionBox(const Rectanglei &rect) : HBox(rect, true)
{
  AddWidget(new PictureWidget(Rectanglei(0,0,38,150), "menu/teams_label"));
  Rectanglei rectZero(0, 0, 0, 0);

  // How many teams ?
  local_teams_nb = new SpinButtonBig(_("Local teams:"), Rectanglei(0, 0, 130, 30),
                                     0, 1,
                                     0, NMAX_NB_TEAMS);
  AddWidget(local_teams_nb);

  Box * top_n_bottom_team_options = new VBox( Rectanglei(0, 0,
                                                         rect.GetSizeX() - local_teams_nb->GetSizeX() - 60, 0)
                                              ,false);
  top_n_bottom_team_options->SetBorder(Point2i(5,0));
  top_n_bottom_team_options->SetMargin(10);
  Box * top_team_options = new HBox ( Rectanglei(0, 0, 0, rect.GetSizeY()/2 - 20), false);
  Box * bottom_team_options = new HBox ( Rectanglei(0, 0, 0, rect.GetSizeY()/2 - 20), false);
  top_team_options->SetBorder(Point2i(0,0));
  bottom_team_options->SetBorder(Point2i(0,0));

  // Initialize teams
  uint team_w_size= top_n_bottom_team_options->GetSizeX() * 2 / NMAX_NB_TEAMS;

  for (uint i=0; i < NMAX_NB_TEAMS; i++) {
    std::string player_name = _("Player") ;
    char num_player[4];
    sprintf(num_player, " %d", i+1);
    player_name += num_player;
    teams_selections.push_back(new TeamBox(player_name, Rectanglei(0,0,team_w_size,rect.GetSizeY()/2)));
    if ( i%2 == 0)
      top_team_options->AddWidget(teams_selections.at(i));
    else
      bottom_team_options->AddWidget(teams_selections.at(i));
  }

  top_n_bottom_team_options->AddWidget(top_team_options);
  top_n_bottom_team_options->AddWidget(bottom_team_options);

  AddWidget(top_n_bottom_team_options);

  // Load Teams' list
  teams_list.full_list.sort(compareTeams);
  teams_list.Clear();

  // No selected team by default
  for (uint i=0; i<teams_selections.size(); i++) {
    teams_selections.at(i)->ClearTeam();
  }
}

Widget* NetworkTeamsSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition)) return NULL;

  uint current_nb_teams = local_teams_nb->GetValue();

  if (local_teams_nb->ClickUp(mousePosition, button)){
    SetNbLocalTeams(local_teams_nb->GetValue(), current_nb_teams);

  } else {
    for (uint i=0; i<teams_selections.size() ; i++) {

      if ( teams_selections.at(i)->Contains(mousePosition) &&
           teams_selections.at(i)->IsLocal() ) {

        Widget * w = teams_selections.at(i)->ClickUp(mousePosition, button);

        if ( w == NULL ) {
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

Widget* NetworkTeamsSelectionBox::Click(const Point2i &/*mousePosition*/, uint /*button*/)
{
  return NULL;
}

void NetworkTeamsSelectionBox::PrevTeam(uint i)
{
  if (teams_selections.at(i)->GetTeam() == NULL) return;

  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  teams_list.FindById(teams_selections.at(i)->GetTeam()->GetId(), previous_index);

  index = previous_index-1;

  do
    {
      to_continue = false;

      // select the last team if we are outside list
      if ( index < 0 )
        index = int(teams_list.full_list.size())-1;

      // Get the team at current index
      tmp = teams_list.FindByIndex(index);

      // Check if that team is already selected
      for (uint j = 0; j < NMAX_NB_TEAMS; j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index--;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue) {
        SetLocalTeam(i, *tmp, true);
      }
    } while ( index != previous_index && to_continue);
}

void NetworkTeamsSelectionBox::NextTeam(uint i,
                                        bool check_null_prev_team)
{
  if (check_null_prev_team &&
      teams_selections.at(i)->GetTeam() == NULL)
    return;

  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  if (check_null_prev_team) {
    teams_list.FindById(teams_selections.at(i)->GetTeam()->GetId(), previous_index);
  }

  index = previous_index+1;

  do
    {
      to_continue = false;

      // select the first team if we are outside list
      if ( index >= int(teams_list.full_list.size()) )
        index = 0;

      // Get the team at current index
      tmp = teams_list.FindByIndex(index);

      // Check if that team is already selected
      for (uint j = 0; j < NMAX_NB_TEAMS; j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index++;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue) {
        SetLocalTeam(i, *tmp, check_null_prev_team);
      }
    } while ( index != previous_index && to_continue);
}

void NetworkTeamsSelectionBox::SetNbLocalTeams(uint nb_teams, uint previous_nb)
{

  int delta_team = nb_teams - previous_nb;

  if (delta_team < 0) {
    // we hide the useless LOCAL teams selector
    for (uint i=teams_selections.size()-1; int(i) >= 0 && delta_team < 0; i--) {
      if (teams_selections.at(i)->GetTeam() != NULL &&
          teams_selections.at(i)->IsLocal()) {
        RemoveLocalTeam(i);
        delta_team++;
      }
    }
  } else if (delta_team > 0) {
    // we had the mandatory LOCAL teams selection
    for (uint i=0; delta_team > 0 && i < teams_selections.size(); i++) {
      if (teams_selections.at(i)->GetTeam() == NULL) {
        AddLocalTeam(i);
        delta_team--;
      }
    }

  }
}

void NetworkTeamsSelectionBox::AddLocalTeam(uint i)
{
  // we should find an available team
  NextTeam(i, false);
}

void NetworkTeamsSelectionBox::RemoveLocalTeam(uint i)
{
  if ( teams_selections.at(i)->GetTeam() != NULL ) {
    ActionHandler::GetInstance()->NewAction (new Action(Action::ACTION_MENU_DEL_TEAM,
                                                        teams_selections.at(i)->GetTeam()->GetId()));
    ActionHandler::GetInstance()->ExecActions();
  }
}

void NetworkTeamsSelectionBox::SetLocalTeam(uint i, Team& team, bool remove_previous_team)
{
  if (remove_previous_team) {
    RemoveLocalTeam(i);
  }

  team.SetLocal();
#ifdef WIN32
  team.SetPlayerName(getenv("USERNAME"));
#else
  team.SetPlayerName(getenv("USER"));
#endif
  std::string team_id = team.GetId();

  Action* a = new Action(Action::ACTION_MENU_ADD_TEAM, team_id);
  a->Push(team.GetPlayerName());
  a->Push(int(team.GetNbCharacters()));
  ActionHandler::GetInstance()->NewAction(a);
  ActionHandler::GetInstance()->ExecActions();
}

void NetworkTeamsSelectionBox::AddTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() == NULL) {
      int index = 0;
      Team * tmp = teams_list.FindById(team_id, index);

      teams_selections.at(i)->SetTeam(*tmp, true);
      break;
    }
  }

  // Count the current number of local teams
  uint nb_local_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL &&
        teams_selections.at(i)->IsLocal()) {
      nb_local_teams++;
    }
  }
  local_teams_nb->SetValue(nb_local_teams);
}

void NetworkTeamsSelectionBox::UpdateTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL &&
        teams_selections.at(i)->GetTeam()->GetId() == team_id) {
      int index = 0;
      Team * tmp = teams_list.FindById(team_id, index);
      // Force refresh of information
      teams_selections.at(i)->SetTeam(*tmp, true);
      std::cout << "Update " << team_id << std::endl;
      break;
    }
  }
}

void NetworkTeamsSelectionBox::DelTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL &&
        teams_selections.at(i)->GetTeam()->GetId() == team_id) {

      teams_selections.at(i)->ClearTeam();
      break;
    }
  }

  // Count the current number of local teams
  uint nb_local_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL &&
        teams_selections.at(i)->IsLocal()) {
      nb_local_teams++;
    }
  }
  local_teams_nb->SetValue(nb_local_teams);
}


void NetworkTeamsSelectionBox::ValidTeamsSelection()
{
  std::list<uint> selection;

  uint nb_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL)
      nb_teams++;
  }

  if (nb_teams >= 2) {
    std::list<uint> selection;

    for (uint i=0; i < teams_selections.size(); i++) {
      if (teams_selections.at(i)->GetTeam() != NULL) {
        int index = -1;
        teams_selections.at(i)->ValidOptions();
        teams_list.FindById(teams_selections.at(i)->GetTeam()->GetId(), index);
        if (index > -1)
          selection.push_back(uint(index));
      }
    }
    teams_list.ChangeSelection (selection);
  }
}
