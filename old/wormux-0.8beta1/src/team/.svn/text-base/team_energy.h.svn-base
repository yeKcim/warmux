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
 * Energy (progress) bar of a team
 *****************************************************************************/

#ifndef TEAM_ENERGY_H
#define TEAM_ENERGY_H

#include "graphic/text.h"
#include "graphic/sprite.h"
#include "gui/EnergyBar.h"
#include "object/physical_obj.h"

class Team;

typedef enum {
  // Energy bar are waiting for a new change
  EnergyStatusOK,

  // Energy bar can change their values
  EnergyStatusValueChange,

  // Energy bar can change there ranking
  EnergyStatusRankChange,

  // Waiting for a change to be finished before moving
  EnergyStatusWait
} energy_t;

class TeamEnergy
{
  private :
    /* If you need this, implement it (correctly) */
    TeamEnergy(const TeamEnergy&);
    TeamEnergy operator=(const TeamEnergy&);
    /**********************************************/

    EnergyBar energy_bar;
    // displayed value
    uint value;
    // team value
    uint new_value;
    // initial energy
    uint max_value;

    Team *team;
    Sprite *icon;
    Text * t_team_energy;

    int dx;
    int dy;

    uint rank;
    uint new_rank;

    std::string team_name;

    uint move_start_time;

  public :
    uint rank_tmp;
    energy_t status;

    TeamEnergy(Team * _team);
    ~TeamEnergy();
    void Config(uint _current_energy,
                uint _max_energy);

    void Refresh();
    void Draw(const Point2i& pos);

    void SetValue(uint nv_energie);

    void SetRanking(uint classem); // no animation
    void NewRanking(uint nv_classem);
    // Move energy bar (change ranking)
    void Move();
    bool IsMoving() const;
    // Move energy bar immediatly to there destination
    void FinalizeMove();
};

#endif /* TEAM_ENERGY_H */
