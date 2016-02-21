/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Handle custom team list
 *****************************************************************************/

#ifndef CUSTOM_TEAMS_LIST_H
#define CUSTOM_TEAMS_LIST_H
//-----------------------------------------------------------------------------

#include <string>
#include <vector>
#include "include/singleton.h"

//-----------------------------------------------------------------------------

// Forward declarations
class CustomTeam;

class CustomTeamsList : public Singleton<CustomTeamsList>
{
public:
  CustomTeamsList();
  ~CustomTeamsList();
  void LoadList();
  std::vector<CustomTeam *> GetList();
  CustomTeam* GetByName(std::string name);
  unsigned GetNumCustomTeam();

private:
  std::vector<CustomTeam *> full_list;

  void Clear();
  void Sort();
  void LoadOneTeam (const std::string &dir, const std::string &file);
  static bool CompareItems( CustomTeam* p1, CustomTeam* p2 );
};


inline CustomTeamsList &GetCustomTeamsList(void) { return CustomTeamsList::GetRef(); };

//-----------------------------------------------------------------------------
#endif
