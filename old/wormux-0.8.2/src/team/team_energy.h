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
 * Energy (progress) bar of a team
 *****************************************************************************/

#ifndef TEAM_ENERGY_H
#define TEAM_ENERGY_H

#include <vector>
#include "gui/energy_bar.h"

class Team;
class Sprite;
class Text;

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

class EnergyValue : public std::pair<uint, uint>
{
protected:
  friend class EnergyList;
  EnergyValue(uint time, uint energy): std::pair<uint, uint>(time, energy) { }
public:
  uint GetDuration() const { return this->first; };
  uint GetValue() const { return this->second; };
};

// Should not need to be sorted, as time of addition sorts them automatically
class EnergyList : public std::vector<EnergyValue*>
{
  uint m_max_value;
  uint m_last_value; // To remove duplicated values
  typedef std::vector<EnergyValue*>::iterator iterator;
protected:
  friend class TeamEnergy;
  void Reset();
  void AddValue(uint value);
  EnergyList() : m_max_value(0), m_last_value(0) { };
  ~EnergyList() {
    Reset();
  };
public:
  typedef std::vector<EnergyValue*>::const_iterator const_iterator;
  uint GetMaxValue() const { return m_max_value; };
  uint GetDuration() const { return at(size()-1)->GetDuration(); }
};

class TeamEnergy
{
  private :
    /* If you need this, implement it (correctly) */
    TeamEnergy(const TeamEnergy&);
    TeamEnergy operator=(const TeamEnergy&);
    /**********************************************/

    EnergyBar   energy_bar;
    // displayed value
    uint        value;
    // team value
    uint        new_value;
    // initial energy
    uint        max_value;

    Team        *team;
    Sprite      *icon;
    Text        *t_team_energy;

    int         dx;
    int         dy;

    uint        rank;
    uint        new_rank;

    std::string team_name;

    uint        move_start_time;

  public :
    uint        rank_tmp;
    energy_t    status;
    EnergyList  energy_list;

    TeamEnergy(Team * _team);
    ~TeamEnergy();
    void Config(uint _current_energy, uint _max_energy);

    void Refresh();
    void Draw(const Point2i& pos);

    void SetIcon(const Surface & icon);
    void SetValue(uint new_energy);

    void SetRanking(uint value) { rank = new_rank = value; }
    void NewRanking(uint value) { new_rank = value; }
    // Move energy bar (change ranking)
    void Move();
    bool IsMoving() const { return (dx!=0 || dy!=0); }
    // Move energy bar immediatly to its destination
    void FinalizeMove();
};

#endif /* TEAM_ENERGY_H */
