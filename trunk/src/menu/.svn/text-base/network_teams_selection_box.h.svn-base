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

#ifndef NETWORK_TEAMS_SELECTION_BOX_H
#define NETWORK_TEAMS_SELECTION_BOX_H

#include <vector>
#include <WARMUX_team_config.h>
#include "menu/teams_selection_box.h"

class Team;

class NetworkTeamsSelectionBox : public TeamsSelectionBox
{
  std::vector<Team *> team_list;

  void RemoveLocalTeam(uint i);
  void SetLocalTeam(uint i, Team& team);

  const std::string GetLocalPlayerName();

  bool IsSelected(Team * team);

  void RequestTeamRemoval();
  void UpdateNbTeams();

protected:
  virtual void PrevTeam(uint i);
  virtual void NextTeam(uint i);

public:
  NetworkTeamsSelectionBox(const Point2i &size, bool w_border);

  virtual void ValidTeamsSelection();
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  void AddTeamCallback(const std::string& team_id);
  void UpdateTeamCallback(const std::string& old_team_id, const std::string& team_id);
  void DelTeamCallback(const std::string& team_id);
  Team * FindUnusedTeam(const std::string& default_team_id);
  Team * FindById(const std::string& id, int &index);
  int FindByPointer(const Team* team)
  {
    for (uint i=0; i<team_list.size(); i++)
      if (team_list[i] == team)
        return i;
    return -1;
  }
  bool HasOpenTeamSlot();
  void RequestTeam();
  void ChangeTeamListCallback(const std::vector<uint>& list);
};
#endif
