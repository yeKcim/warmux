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
 *  Network teams selection box
 *****************************************************************************/

#include "menu/teams_selection_box.h"
#include "menu/network_teams_selection_box.h"
#include "menu/team_box.h"
#include "game/config.h"
#include "game/game_mode.h"
#include "gui/label.h"
#include "gui/picture_widget.h"
#include "gui/spin_button.h"
#include "gui/spin_button_picture.h"
#include "gui/text_box.h"
#include "include/action_handler.h"
#include "network/network.h"
#include "team/teams_list.h"
#include "team/team.h"
#include "tool/string_tools.h"

NetworkTeamsSelectionBox::NetworkTeamsSelectionBox(const Point2i &_size, bool w_border)
  : TeamsSelectionBox(_size, true, w_border)
{
  const std::list<Team *>& list = GetTeamsList().full_list; 
  for (std::list<Team *>::const_iterator it = list.begin(); it != list.end(); ++it)
    team_list.push_back(*it);

  local_teams_nb->SetMaxValue(GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer());

  GetTeamsList().Clear();

  // No selected team(s) by default
  for (uint i=0; i<teams_selections.size(); i++) {
    teams_selections[i]->ClearTeam();
  }
}

Widget* NetworkTeamsSelectionBox::ClickUp(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition))
    return NULL;

  uint current_nb_teams = local_teams_nb->GetValue();

  if (local_teams_nb->ClickUp(mousePosition, button)){
    int delta =  local_teams_nb->GetValue() - current_nb_teams;
    local_teams_nb->SetValue(current_nb_teams);
    if (delta > 0) {
      RequestTeam();
    } else if (delta < 0){
      RequestTeamRemoval();
    }
  } else {
    return DefaultClickUp(mousePosition, button);
  }

  return NULL;
}

const std::string NetworkTeamsSelectionBox::GetLocalPlayerName()
{
  std::string result;
#ifdef WIN32
  // The username might be in NLS !
  char* name = LocaleToUTF8(getenv("USERNAME"));
  result = name;
  delete[] name;
#elif defined(ANDROID)
  result = "Android";
#elif defined(__SYMBIAN32__)
  result = "Symbian";
#else
  result = getenv("USER");
#endif
  return result;
}

void NetworkTeamsSelectionBox::PrevTeam(uint i)
{
  ASSERT(teams_selections[i]->GetTeam());

  bool to_continue;
  uint max_teams = GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer();

  int previous_index = FindByPointer(teams_selections[i]->GetTeam());
  int index = previous_index-1;

  do {
    to_continue = false;

    // select the last team if we are outside list
    if (index < 0)
      index = int(team_list.size())-1;

    // Get the team at current index
    Team* tmp = team_list[index];

    // Check if that team is already selected
    for (uint j = 0; j < max_teams; j++) {
      if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
        index--;
        to_continue = true;
        break;
      }
    }

    // We have found a team which is not selected
    if (tmp && !to_continue) {
      SetLocalTeam(i, *tmp);
    }
  } while (index!=(int)i && to_continue);
}

void NetworkTeamsSelectionBox::NextTeam(uint i)
{
  bool to_continue;
  uint max_teams = GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer();

  int previous_index = FindByPointer(teams_selections[i]->GetTeam());
  uint index = uint(previous_index+1);

  do {
    to_continue = false;

    // select the first team if we are outside list
    if (index >= team_list.size())
      index = 0;

    // Get the team at current index
    Team* tmp = team_list[index];

    // Check if that team is already selected
    for (uint j = 0; j < max_teams; j++) {
      if (j!= i && tmp == teams_selections.at(j)->GetTeam()) {
        index++;
        to_continue = true;
        break;
      }
    }

    // We have found a team which is not selected
    if (tmp && !to_continue) {
      if (!teams_selections[i]->GetTeam())
        tmp->SetPlayerName(GetLocalPlayerName());
      SetLocalTeam(i, *tmp);
    }
  } while (index!=i && to_continue);
}

void NetworkTeamsSelectionBox::RequestTeam()
{
  ConfigTeam team_config;
  team_config.id = team_list[0]->GetId();
  team_config.player_name = GetLocalPlayerName();
  team_config.nb_characters = GameMode::GetInstance()->nb_characters;
  team_config.ai = NO_AI_NAME;

  uint nb_teams = 0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam())
      nb_teams++;
  }
  team_config.group = nb_teams;
  ActionHandler::GetInstance()->NewRequestTeamAction(team_config);
}

void NetworkTeamsSelectionBox::RequestTeamRemoval()
{
  int player_id = int(Network::GetInstance()->GetPlayer().GetId());
  Action* a = new Action(Action::ACTION_GAME_REQUEST_TEAM_REMOVAL);
  a->Push(player_id);
  ActionHandler::GetInstance()->NewAction(a);
}

bool NetworkTeamsSelectionBox::IsSelected(Team * team)
{
  uint max_teams = GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer();
  for (uint j = 0; j < max_teams ; j++) {
    if (team == teams_selections.at(j)->GetTeam()) {
      return true;
    }
  }
  return false;
}

bool NetworkTeamsSelectionBox::HasOpenTeamSlot()
{
  uint max_teams = GameMode::GetInstance()->GetMaxTeamsPerNetworkPlayer();
  for (uint j = 0; j < max_teams; j++) {
    if (!teams_selections.at(j)->GetTeam()) {
      return true;
    }
  }
  return false;
}

Team * NetworkTeamsSelectionBox::FindById(const std::string& id, int& index)
{
  for (uint i=0; i<team_list.size(); i++) {
    if (team_list[i]->GetId() == id) {
      index = i;
      return team_list[i];
    }
  }
  index = -1;
  return NULL;
}

Team * NetworkTeamsSelectionBox::FindUnusedTeam(const std::string& default_team_id)
{
  int index;
  Team * team = FindById(default_team_id, index);
  int team_count = team_list.size();
  int checked_teams = 0;
  while((!team || IsSelected(team)) && checked_teams <= team_count) {
    index++;
    if (index >= team_count)
      index = 0;
    team = team_list[index];
    checked_teams++;
  }
  ASSERT(team);
  return team;
}

void NetworkTeamsSelectionBox::SetLocalTeam(uint i, Team& team)
{
  team.SetRemote(false);

  if (teams_selections[i]->GetTeam()) {
    teams_selections[i]->SetTeam(team, false);
  }
}

void NetworkTeamsSelectionBox::AddTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (!teams_selections[i]->GetTeam()) {
      int index;
      /* FindPlayingById should be faster */
      Team * tmp = FindById(team_id, index);

      teams_selections[i]->SetTeam(*tmp, true);
      break;
    }
  }

  // Count the current number of local teams
  uint nb_local_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam() && teams_selections[i]->IsLocal()) {
      nb_local_teams++;
    }
  }

  local_teams_nb->SetValue(nb_local_teams);
  UpdateNbTeams();
}

void NetworkTeamsSelectionBox::UpdateTeamCallback(const std::string& old_team_id,
                                                  const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam() &&
        teams_selections[i]->GetTeam()->GetId() == old_team_id) {
      int index = 0;
      Team * tmp = FindById(team_id, index);

      // Force refresh of information
      teams_selections[i]->SetTeam(*tmp, true);
      break;
    }
  }
}

void NetworkTeamsSelectionBox::DelTeamCallback(const std::string& team_id)
{
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam() &&
        teams_selections[i]->GetTeam()->GetId() == team_id) {

      teams_selections[i]->ClearTeam();
      break;
    }
  }

  // Count the current number of local teams
  uint nb_local_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam() &&
        teams_selections[i]->IsLocal()) {
      nb_local_teams++;
    }
  }
  local_teams_nb->SetValue(nb_local_teams);
  UpdateNbTeams();
}


void NetworkTeamsSelectionBox::ValidTeamsSelection()
{
  std::list<uint> selection;

  uint nb_teams=0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam())
      nb_teams++;
  }

  if (nb_teams >= 2) {
    std::list<uint> selection;

    for (uint i=0; i < teams_selections.size(); i++) {
      TeamBox *teambox = teams_selections[i];
      if (teambox->GetTeam()) {
        int index = -1;
        teambox->ValidOptions();
        // We are using the index inside the *full* list at this point
        GetTeamsList().FindById(teambox->GetTeam()->GetId(), index);
        if (index > -1)
          selection.push_back(uint(index));
      }
    }
    GetTeamsList().ChangeSelection (selection);
  }

  Config::GetInstance()->SetNetworkLocalTeams();
}

void NetworkTeamsSelectionBox::UpdateNbTeams()
{
  // If we are not running this special widget, this is useless
  if (!list_box)
    return;

  uint nb_teams = 0;
  for (uint i=0; i < teams_selections.size(); i++) {
    if (teams_selections[i]->GetTeam())
      nb_teams++;
  }

  list_box->SetNbTeams(nb_teams);
  // Fix scrollbox not being redrawn: this is performed *after* the
  // screen has been refreshed, because of the callbacks
  list_box->NeedRedrawing();
}

void NetworkTeamsSelectionBox::ChangeTeamListCallback(const std::vector<uint>& list)
{
  team_list.clear();
  std::vector<Team*> tmp_list;
  const std::list<Team *>& flist = GetTeamsList().full_list; 
  for (std::list<Team *>::const_iterator it = flist.begin(); it != flist.end(); ++it)
    tmp_list.push_back(*it);

  for (uint i=0; i<list.size(); i++)
    team_list.push_back(tmp_list[list[i]]);
}
