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
  public :
    CustomTeam();
    CustomTeam (const std::string &teams_dir, const std::string &id);
    ~CustomTeam();
    std::string GetName();
    std::vector<std::string> GetCharactersNameList();
    void NewTeam();
    bool Save();
    void Delete();

    void SetName(const std::string &new_name);
    void SetCharacterName(unsigned id, const std::string &new_name);

    protected :
    bool is_name_changed;
    uint nb_characters;
    std::string name;
    std::string directory_name;
    std::vector<std::string> characters_name_list;

    bool SaveXml();

};


//-----------------------------------------------------------------------------
#endif
