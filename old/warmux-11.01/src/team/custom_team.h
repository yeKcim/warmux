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
 * Custom team
 *****************************************************************************/

#ifndef CUSTOM_TEAM_H
#define CUSTOM_TEAM_H
//-----------------------------------------------------------------------------
#include <list>
#include <vector>

//-----------------------------------------------------------------------------

class CustomTeam
{
  bool is_name_changed;
  uint nb_characters;
  std::string name;
  std::string directory_name;
  std::vector<std::string> characters_name_list;

  bool SaveXml();

protected:
  CustomTeam(const std::string& team_name,
             const std::string& directory,
             const std::vector<std::string>& list);

public:
  static const uint MAX_CHARACTERS = 10;
  static CustomTeam* LoadCustomTeam(const std::string &teams_dir,
                                    const std::string &id,
                                    std::string& error);

  CustomTeam(const std::string &team_name);
  ~CustomTeam();
  std::string GetName() { return name; }
  std::vector<std::string> GetCharactersNameList()
  {
    std::vector<std::string> list = characters_name_list;
    return list;
  }
  bool Save();
  void Delete();

  void SetName(const std::string &new_name);
  void SetCharacterName(uint id, const std::string &new_name);
};

//-----------------------------------------------------------------------------
#endif
