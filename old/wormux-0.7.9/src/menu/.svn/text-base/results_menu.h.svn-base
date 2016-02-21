/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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

#include "menu.h"
#include "../include/base.h"
#include "../graphic/font.h"

class TeamResults;
class ResultBox;

class ResultsMenu : public Menu
{
 private:
    const std::vector<TeamResults*>* results;
    int     index;

    // Box sizes
    int     total_width;
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
    HBox    *team_box;
    
    ResultBox* most_violent;
    ResultBox* most_usefull;
    ResultBox* most_useless;
    ResultBox* biggest_traitor;
    
    void __sig_ok() { };
    void __sig_cancel() { };
    
    void SetResult(int i);
    void OnClic(const Point2i &mousePosition, int button);
    void Draw(const Point2i &mousePosition);
 public:
    ResultsMenu(const std::vector<TeamResults*>* v,
                const char *winner_name);
    ~ResultsMenu();
};

#endif //RESULTS_MENU_H
