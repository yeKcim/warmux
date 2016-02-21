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
 * Manage the end of game results of a team
 *****************************************************************************/

#ifndef RESULTS_H
#define RESULTS_H

#include <vector>
#include <map>
#include <string>

class Character;
class Team;

class TopCharacters
{
 public:
  const Character* Violent;
  uint             violence;
  const Character* Useful;
  uint             usefulness;
  const Character* Useless;
  uint             uselessness;
  const Character* Traitor;
  uint             treachery;
  const Character* Clumsy;
  uint             clumsyness;
  const Character* Accurate;
  double           accuracy;

  uint             death_time;

  TopCharacters();
  void rankPlayer(const Character *player);
  void merge(const TopCharacters* other);
};

// Could be a 5-tuple
class TeamResults
{
private:
  TeamResults(const TeamResults&);
  const TeamResults& operator=(const TeamResults&);

  const Team* team;
  TopCharacters* top;

  static TeamResults* createTeamResults(Team* team);
  static TeamResults* createGlobalResults();

protected:
  TeamResults(const Team* team, TopCharacters *top);
  ~TeamResults();

public:
  static std::vector<TeamResults*>* createAllResults(void);
  static void deleteAllResults(std::vector<TeamResults*>* results_list);

  const Team* getTeam() const { return team; };
  const Character* getMostViolent() const { return top->Violent; };
  const Character* getMostUseful() const { return top->Useful; };
  const Character* getMostUseless() const { return top->Useless; };
  const Character* getBiggestTraitor() const { return top->Traitor; };
  const Character* getMostClumsy() const { return top->Clumsy; };
  const Character* getMostAccurate() const { return top->Accurate; };
  const TopCharacters* getTopCharacters() const { return top; };
  uint GetDeathTime() const { return top->death_time; }
};

typedef std::vector<TeamResults*>::iterator res_iterator;
typedef std::vector<TeamResults*>::const_iterator const_res_iterator;
#endif //RESULTS_H
