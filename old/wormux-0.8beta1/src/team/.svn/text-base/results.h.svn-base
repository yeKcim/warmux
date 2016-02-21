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
 * Manage the end of game results of a team
 *****************************************************************************/

#ifndef RESULTS_H
#define RESULTS_H

#include <vector>
#include <map>
#include <string>

#include "team.h"

class Character;

// Could be a 5-tuple
class TeamResults
{
private:
  TeamResults(const TeamResults&);
  const TeamResults& operator=(const TeamResults&);

  //std::couple<const sting&, int> ?
  const std::string teamName;
  const Surface* team_logo;
  const Character* mostViolent;
  const Character* mostUseful;
  const Character* mostUseless;
  const Character* biggestTraitor;
  const Character* mostClumsy;

  static TeamResults* createTeamResults(Team* team);
  static TeamResults* createGlobalResults();

protected:
  TeamResults(const std::string& name,
	      const Surface* team_logo,
	      const Character* MV,
	      const Character* MUl,
	      const Character* MUs,
	      const Character* BT,
	      const Character* MS);
public:
  static std::vector<TeamResults*>* createAllResults(void);
  static void deleteAllResults(std::vector<TeamResults*>* results_list);

  const std::string& getTeamName() const { return teamName; };
  const Surface* getTeamLogo() const {return team_logo;};
  const Character* getMostViolent() const { return mostViolent; };
  const Character* getMostUseful() const { return mostUseful; };
  const Character* getMostUseless() const { return mostUseless; };
  const Character* getBiggestTraitor() const { return biggestTraitor; };
  const Character* getMostClumsy() const { return mostClumsy; };
};

typedef std::vector<TeamResults*>::iterator res_iterator;
typedef std::vector<TeamResults*>::const_iterator const_res_iterator;
#endif //RESULTS_H
