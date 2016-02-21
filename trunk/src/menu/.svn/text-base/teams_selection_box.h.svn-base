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

#ifndef TEAMS_SELECTION_BOX_H
#define TEAMS_SELECTION_BOX_H

#include <vector>
#include "gui/horizontal_box.h"
#include "gui/scroll_box.h"

// Forward declarations
class SpinButtonWithPicture;
class TeamBox;

const uint MAX_NB_TEAMS=8;

class TeamScrollBox : public ScrollBox
{
  // We need a real copy around for when we get destroyed
  std::vector<TeamBox*> teams;
  // Number of teams to be displayed
  uint  count;
public:
  TeamScrollBox(const std::vector<TeamBox*>& teams, const Point2i &size);
  ~TeamScrollBox();
  void SetNbTeams(uint nb);
};

// -----------------------------------------------
// -----------------------------------------------

class TeamsSelectionBox : public HBox
{
protected:
  TeamScrollBox* list_box;
  SpinButtonWithPicture *local_teams_nb;
  std::vector<TeamBox*> teams_selections;

  Widget* DefaultClickUp(const Point2i &mousePosition, uint button);
  virtual void PrevTeam(uint i) = 0;
  virtual void NextTeam(uint i) = 0;

public:
  TeamsSelectionBox(const Point2i &size, bool network, bool w_border);

  virtual void Draw(const Point2i &mousePosition);

  virtual void ValidTeamsSelection() = 0;
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button) = 0;
  virtual Widget* Click(const Point2i &mousePosition, uint button);
};

class LocalTeamsSelectionBox : public TeamsSelectionBox
{
  void SetNbTeams(uint nb_teams);

protected:
  virtual void PrevTeam(uint i);
  virtual void NextTeam(uint i);

public:
  LocalTeamsSelectionBox(const Point2i &size, bool border);

  virtual void ValidTeamsSelection();
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
};

#endif
