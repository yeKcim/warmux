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

class ResultsMenu : public Menu
{
 private:
    ResultsMenu(const ResultsMenu&);
    const ResultsMenu& operator=(const ResultsMenu&);

    std::vector<TeamResults*>& results;
    const Team *first_team, *second_team, *third_team;
    int     index;

    // Box sizes
    int     max_height;
    Point2i team_size;
    Point2i type_size;
    Point2i name_size;
    Point2i score_size;

    /* Team controllers */
    Button  *bt_prev_team;
    Button  *bt_next_team;
    PictureWidget *team_logo;
    Label   *team_name;
    Box    *team_box;

    Box    *winner_box;
    Box    *statistics_box;
    ResultBox* most_violent;
    ResultBox* most_useful;
    ResultBox* most_useless;
    ResultBox* biggest_traitor;
    ResultBox* most_clumsy;
    ResultBox* most_accurate;

    Surface podium_img;

    bool signal_ok() { return true;};
    bool signal_cancel() { return true;};

    void ComputeTeamsOrder();
    void SetResult(int i);
    void OnClick(const Point2i &mousePosition, int button);
    void OnClickUp(const Point2i &mousePosition, int button);
    void DrawTeamGraph(const Team* team,
                       int x, int y,
                       double energy_scale, double time_scale,
                       const Color& color) const;
    void DrawGraph(int x, int y, int w, int h);
    void DrawPodium(const Point2i &position) const;
    void DrawTeamOnPodium(const Team& team, const Point2i& podium_position,
                          const Point2i& relative_position) const;
    void Draw(const Point2i &mousePosition);
 public:
    ResultsMenu(std::vector<TeamResults*>& v);
    ~ResultsMenu();
};

#endif //RESULTS_MENU_H
