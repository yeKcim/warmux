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
 * Resulst menu
 *****************************************************************************/

#ifndef RESULTS_MENU_H
#define RESULTS_MENU_H

#include <vector>
#include "graphic/surface.h"
#include "menu.h"

class ResultBox;
class PictureWidget;
class TeamResults;
class Team;
class MultiTabs;
class TalkBox;

class ResultsMenu : public Menu
{
 private:
    ResultsMenu(const ResultsMenu&);
    const ResultsMenu& operator=(const ResultsMenu&);

    std::vector<TeamResults*>& results;
    const Team *first_team, *second_team, *third_team;
    int     index;

    MultiTabs *tabs;
    MultiTabs *stats;

    /* Chat controller */
    TalkBox* msg_box;

    Box    *winner_box;
    Surface podium_img;

    void key_ok();
    bool signal_ok() { return true;};
    bool signal_cancel() { return true;};

    void ComputeTeamsOrder();
    void OnClick(const Point2i &mousePosition, int button);
    void OnClickUp(const Point2i &mousePosition, int button);
    void DrawPodium(const Point2i &position) const;
    void DrawTeamOnPodium(const Team& team, const Point2i& podium_position,
                          const Point2i& relative_position) const;
    void Draw(const Point2i &mousePosition);
 public:
    ResultsMenu(std::vector<TeamResults*>& v, bool disconnected = false);
    ~ResultsMenu();

    void ReceiveMsgCallback(const std::string& msg);
};

#endif //RESULTS_MENU_H
