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

#ifndef TEAMS_SELECTION_BOX_H
#define TEAMS_SELECTION_BOX_H

#include <vector>
#include "gui/box.h"

// Forward declarations
class SpinButtonWithPicture;
class TeamBox;

const uint MAX_NB_TEAMS=4;

// -----------------------------------------------
// -----------------------------------------------

class TeamsSelectionBox : public HBox
{
 private:
  /* If you need this, implement it (correctly) */
  TeamsSelectionBox(const TeamsSelectionBox&);
  TeamsSelectionBox operator=(const TeamsSelectionBox&);
  /**********************************************/
  void SetNbTeams(uint nb_teams);
  void PrevTeam(int i);
  void NextTeam(int i);

  void Init(bool network);
 protected:
  SpinButtonWithPicture *local_teams_nb;
  std::vector<TeamBox*> teams_selections;

 public:
  TeamsSelectionBox(const Point2i &size, bool network = false);

  virtual void ValidTeamsSelection();
  virtual Widget* Click(const Point2i &mousePosition, uint button);
  virtual Widget* ClickUp(const Point2i &mousePosition, uint button);
};

#endif
