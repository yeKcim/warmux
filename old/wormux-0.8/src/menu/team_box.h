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
 *  Team box
 *****************************************************************************/

#ifndef TEAM_BOX_H
#define TEAM_BOX_H

#include <string>
#include "gui/box.h"

class Team;
class SpinButton;
class Label;
class PictureWidget;
class TextBox;

class TeamBox : public HBox
{
 private:
  /* If you need this, implement it (correctly) */
  TeamBox(const TeamBox&);
  TeamBox operator=(const TeamBox&);
  /**********************************************/

  bool is_local; // local/remote team
  std::string previous_name; // only for network

  Team * associated_team;
  PictureWidget *team_logo;
  Label * team_name;
  TextBox * player_name;
  SpinButton * nb_characters;

 public:
  TeamBox(const std::string& player_name, const Point2i &size);

  void SetTeam(Team& _team, bool read_team_values=false);
  void ClearTeam();
  Team* GetTeam() const;
  void ValidOptions() const;

  bool IsLocal() const;

  void Update(const Point2i &mousePosition,
              const Point2i &lastMousePosition);

  Widget* Click(const Point2i &mousePosition, uint button);
  Widget* ClickUp(const Point2i &mousePosition, uint button);
};

#endif
