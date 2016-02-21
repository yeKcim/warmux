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
 *  Network teams selection box
 *****************************************************************************/

#include "menu/network_teams_selection_box.h"
#include "menu/team_box.h"
#include "game/config.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "include/action_handler.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/i18n.h"
#include "tool/string_tools.h"

NetworkTeamsSelectionBox::NetworkTeamsSelectionBox(const Point2i &_size) :
  TeamsSelectionBox(_size, true)
{
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

void NetworkTeamsSelectionBox::SetDefaultPlayerName(Team& team)
{
#ifdef WIN32
  // The username might be in NLS !
  char* name = LocaleToUTF8(getenv("USERNAME"));
  team.SetPlayerName(name);
  delete[] name;
#else
  team.SetPlayerName(getenv("USER"));
#endif
}

void NetworkTeamsSelectionBox::PrevTeam(uint i)
{
  ASSERT(teams_selections.at(i)->GetTeam() != NULL);

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
      for (uint j = 0; j < MAX_NB_TEAMS; j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index--;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue) {
        SetLocalTeam(i, *tmp);
      }
    } while (index != previous_index && to_continue);
}

void NetworkTeamsSelectionBox::NextTeam(uint i)
{
  bool to_continue;
  Team* tmp;
  int previous_index = -1, index;

  if (teams_selections.at(i)->GetTeam() != NULL)
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
      for (uint j = 0; j < MAX_NB_TEAMS; j++) {
        if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
          index++;
          to_continue = true;
          break;
        }
      }

      // We have found a team which is not selected
      if (tmp != NULL && !to_continue) {
	if (teams_selections.at(i)->GetTeam() == NULL)
	  SetDefaultPlayerName(*tmp);
	SetLocalTeam(i, *tmp);
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
  int pos;
  bool selected = false;
  std::list<ConfigTeam>::const_iterator
    it = Config::GetInstance()->AccessNetworkTeamsList().begin(),
    end = Config::GetInstance()->AccessNetworkTeamsList().end();

  // Check if previous team used in network are available
  for (; it != end && !selected; ++it) {
    ConfigTeam the_team_cfg = (*it);
    Team *the_team = GetTeamsList().FindById(the_team_cfg.id, pos);

    if (the_team != NULL) {

      // Check if that team is already selected
      for (uint j = 0; j < MAX_NB_TEAMS; j++) {
        if (the_team == teams_selections.at(j)->GetTeam()) {
          the_team = NULL;
          break;
        }
      }

      // We have found a team which is not selected
      if (the_team != NULL) {
	the_team->SetPlayerName(the_team_cfg.player_name);
	the_team->SetNbCharacters(the_team_cfg.nb_characters);
	selected = true;
        SetLocalTeam(i, *the_team);
      }

    } else {
      std::string msg = Format(_("Can't find team %s!"), the_team_cfg.id.c_str());
      std::cerr << msg << std::endl;
    }
  }

  if (!selected) {
    NextTeam(i);
  }
}

void NetworkTeamsSelectionBox::RemoveLocalTeam(uint i)
{
  ASSERT(teams_selections.at(i)->GetTeam() != NULL);

  ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_MENU_DEL_TEAM,
						     teams_selections.at(i)->GetTeam()->GetId()));
  ActionHandler::GetInstance()->ExecActions();
}

void NetworkTeamsSelectionBox::SetLocalTeam(uint i, Team& team)
{
  team.SetLocal();

  if (teams_selections.at(i)->GetTeam() != NULL) {

    teams_selections.at(i)->SetTeam(team, false);

  } else {
    Action* a = new Action(Action::ACTION_MENU_ADD_TEAM, team.GetId());
    a->Push(team.GetPlayerName());
    a->Push(int(team.GetNbCharacters()));
    ActionHandler::GetInstance()->NewAction(a);
    ActionHandler::GetInstance()->ExecActions();
  }
}

void NetworkTeamsSelectionBox::AddTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() == NULL) {
      int index;
      /* FindPlayingById should be faster */
      Team * tmp = GetTeamsList().FindById(team_id, index);

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

void NetworkTeamsSelectionBox::UpdateTeamCallback(const std::string& old_team_id,
						  const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections.at(i)->GetTeam() != NULL &&
        teams_selections.at(i)->GetTeam()->GetId() == old_team_id) {
      int index = 0;
      Team * tmp = GetTeamsList().FindById(team_id, index);

      // Force refresh of information
      teams_selections.at(i)->SetTeam(*tmp, true);
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
        GetTeamsList().FindById(teams_selections.at(i)->GetTeam()->GetId(), index);
        if (index > -1)
          selection.push_back(uint(index));
      }
    }
    GetTeamsList().ChangeSelection (selection);
  }

  Config::GetInstance()->SetNetworkLocalTeams();
}

void NetworkTeamsSelectionBox::SetMaxNbLocalPlayers(uint nb)
{
  uint current_nb_teams = local_teams_nb->GetValue();
  local_teams_nb->SetMaxValue(nb);
  if (nb < current_nb_teams) {
    SetNbLocalTeams(local_teams_nb->GetValue(), current_nb_teams);
  }
}
