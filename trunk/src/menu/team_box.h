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
 *  Team box
 *****************************************************************************/

#ifndef TEAM_BOX_H
#define TEAM_BOX_H

#include <string>
#include <vector>
#include "gui/horizontal_box.h"

class Button;
class CustomTeam;
class Team;
class SpinButtonWithPicture;
class Label;
class PictureWidget;
class TextBox;
class NullWidget;

class TeamBox : public HBox
{
  std::string previous_player_name; // only for network
  int         ai_level;
  uint        group;

  Surface player_local[4];
  Surface player_remote[4];

  Team * associated_team;
  PictureWidget *team_logo, *player_type;
  Label * team_name;
  TextBox * player_name;
  Button * next_custom_team;
  Button * previous_custom_team;
  SpinButtonWithPicture * nb_characters;
  NullWidget *nullw;

  std::vector<CustomTeam *> custom_team_list;
  unsigned custom_team_current_id;

  CustomTeam *GetCustomTeam() const;
  void UpdateTeam(const std::string& old_team_id) const;

  void UpdatePlayerNameColor();

public:
  TeamBox(const std::string& player_name, const Point2i &size, uint group=0);

  void SetTeam(Team& _team, bool read_team_values=false);
  void SetAILevel(uint i) { ai_level = i; UpdatePlayerType(); }
  void UpdatePlayerType();
  void ClearTeam();
  Team* GetTeam() const { return associated_team; }

  void ValidOptions() const;

  bool IsLocal() const;

  uint GetGroup() const { return group; }
  void SetGroup(uint group);

  virtual bool Update(const Point2i &mousePosition,
                      const Point2i &lastMousePosition);

  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);

  void SwitchPlayerType();

  bool IsTeamSwitcherAt(const Point2i &mousePosition) const;
  bool IsAISwitcherAt(const Point2i &mousePosition) const;
};

#endif
