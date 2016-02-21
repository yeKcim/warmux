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
 * Group of one or more teams
 *****************************************************************************/

#ifndef TEAM_GROUP_H
#define TEAM_GROUP_H

#include <vector>

class Team;
class Color;

#define MAX_TEAM_GROUPS 8

class TeamGroup : public std::vector<Team*>
{
public:
  static const Color Colors[MAX_TEAM_GROUPS];
  iterator active_team;
};

#endif // TEAM_GROUP_H
