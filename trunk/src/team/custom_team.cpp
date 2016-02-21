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
 * Custom Team
 *****************************************************************************/

#include <cstring>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <libxml/tree.h>

#include "game/config.h"
#include "team/custom_team.h"
#include "team/custom_teams_list.h"
#include <WARMUX_file_tools.h>
#include "tool/xml_document.h"

CustomTeam::CustomTeam(const std::string &team_name)
  : is_name_changed(false)
  , nb_characters(MAX_CHARACTERS)
  , name(team_name)
{
  Config *config = Config::GetInstance();
  int team_count = 0;

  do {
    team_count++;
    std::ostringstream oss;
    oss << team_count;
    // the default player name for custom teams
    name = _("custom player");
    name += "  " + oss.str();
    directory_name = config->GetPersonalConfigDir() + "custom_team" PATH_SEPARATOR + FormatFileName(name);

  } while (DoesFolderExist(directory_name));

  for (uint i = 1; i < nb_characters + 1; i++) {
    characters_name_list.push_back("");
  }
}

CustomTeam::CustomTeam(const std::string& team_name,
                       const std::string& directory,
                       const std::vector<std::string>& list)
  : is_name_changed(false)
  , name(team_name)
  , directory_name(directory)
  , characters_name_list(list)
{
  nb_characters = list.size();
}

CustomTeam* CustomTeam::LoadCustomTeam(const std::string &custom_teams_dir,
                                       const std::string &id, std::string& error)
{
  std::vector<std::string> list;
  std::string directory = custom_teams_dir + PATH_SEPARATOR + id;
  std::string filename = directory + PATH_SEPARATOR "team.xml";
  std::string teamname;
  XmlReader   doc;

  // Load XML
  if (!doc.Load(filename)) {
    error = "unable to load file "+ filename + "of team data";
    return NULL;
  }

  if (!XmlReader::ReadString(doc.GetRoot(), "name", teamname)) {
    error = "Invalid file structure: cannot find a name for team in " + filename;
    return NULL;
  }

  // Create the characters
  xmlNodeArray nodes = XmlReader::GetNamedChildren(XmlReader::GetMarker(doc.GetRoot(), "team"), "character");
  xmlNodeArray::const_iterator it = nodes.begin();

  do {
    std::string character_name = "Unknown Soldier (it's all over)";

    XmlReader::ReadString(*it, "name", character_name);
    list.push_back(character_name);

    MSG_DEBUG("team", "Add %s in  custom team %s", character_name.c_str(), teamname.c_str());

    // Did we reach the end ?
    ++it;

  } while (it != nodes.end() || list.size() < MAX_CHARACTERS);

  if (list.size() > MAX_CHARACTERS) {
    error = "Too many players in " + filename;
    return NULL;
  }

  return new CustomTeam(teamname, directory, list);
}


CustomTeam::~CustomTeam()
{
}

void CustomTeam::Delete()
{
  std::string fullpath = directory_name + PATH_SEPARATOR "team.xml";

  if (!DeleteFile(fullpath)) {
    std::cerr << "o "
              << Format(_("Error while deleting the file \"%s\". Unable to delete the custom team."),
                        fullpath.c_str())
              << " " << strerror(errno)
              << std::endl;
    return;
  }

  if (!DeleteFolder(directory_name)) {
    std::cerr << "o "
              << Format(_("Error while deleting the directory \"%s\". Unable to delete the custom team."),
                        directory_name.c_str())
              << " " << strerror(errno)
              << std::endl;
  }
}

bool CustomTeam::Save()
{
  Config *config = Config::GetInstance();

  if (is_name_changed) {
    Delete();
    directory_name = config->GetPersonalConfigDir() + "custom_team" PATH_SEPARATOR + FormatFileName(name);
    is_name_changed = false;
  }

  std::string rep = config->GetPersonalConfigDir();
  // Create the directory if it doesn't exist
  if (!config->MkdirPersonalConfigDir()) {
    std::cerr << "o "
        << Format(_("Error while creating directory \"%s\": unable to store configuration file."),
      rep.c_str())
        << " " << strerror(errno)
        << std::endl;
    return false;
  }

  rep = config->GetPersonalConfigDir() + "custom_team" PATH_SEPARATOR;

  if (!CreateFolder(config->GetPersonalConfigDir() + "custom_team" PATH_SEPARATOR)) {
    std::cerr << "o "
        << Format(_("Error while creating directory \"%s\": unable to store configuration file."),
      rep.c_str())
        << " " << strerror(errno)
        << std::endl;
    return false;
  }

  rep = directory_name;

  if (!CreateFolder(directory_name)) {
    std::cerr << "o "
        << Format(_("Error while creating directory \"%s\": unable to store configuration file."),
      rep.c_str())
        << " " << strerror(errno)
        << std::endl;
    return false;
  }

  return SaveXml();
}

bool CustomTeam::SaveXml()
{
  XmlWriter doc;
  std::string unix_name = name;

  for (uint i = 0; i < unix_name.size(); i++) {
    if (unix_name[i] == ' ') {
      unix_name[i] = '_';
    }
  }

  std::string m_filename = directory_name + PATH_SEPARATOR "team.xml";
  doc.Create(m_filename, "resources", "1.0", "utf-8");
  xmlNode *root = doc.GetRoot();
  doc.WriteElement(root, "name", name);

  xmlNode* team_node = xmlAddChild(root, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"team"));

  for (uint i = 0 ; i < characters_name_list.size(); i++) {
    xmlNode* character = xmlAddChild(team_node, xmlNewNode(NULL /* empty prefix */, (const xmlChar*)"character"));
    doc.WriteElement(character, "name", characters_name_list[i]);
  }

  return doc.Save();
}


void CustomTeam::SetName(const std::string &new_name)
{
  if (name.compare(new_name) != 0) {
    is_name_changed = true;
  }
  name = new_name;
}

void CustomTeam::SetCharacterName(uint id, const std::string &new_name)
{
  if (id < nb_characters) {
    characters_name_list[id] = new_name;
  }
}
