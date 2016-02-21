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
 * List of files, class definition
 *****************************************************************************/

#include <algorithm>
#include <iostream>

#include <WARMUX_file_tools.h>

#include "game/config.h"
#include "gui/file_list_box.h"
#include "tool/ansi_convert.h"

void FileListBox::Empty()
{
  for (uint i=0; i<m_values.size(); i++) {
    if (m_values[i]) {
      const std::string* str = static_cast<const std::string*>(m_values[i]);
      delete str;
    }
  }
  ItemBox::Empty();
}

void FileListBox::Clear()
{
  for (uint i=0; i<m_values.size(); i++) {
    if (m_values[i]) {
      const std::string* str = static_cast<const std::string*>(m_values[i]);
      delete str;
    }
  }
  ItemBox::Clear();
}

void FileListBox::RemoveSelected()
{
  if (selected_item != -1) {
    if (m_values[selected_item]) {
      const std::string* str = static_cast<const std::string*>(m_values[selected_item]);
      delete str;
    }
    ItemBox::RemoveSelected();
  }
}

const std::string* FileListBox::GetSelectedFile() const
{
  if (list_files) {
    const std::string* str = static_cast<const std::string*>(ItemBox::GetSelectedValue());
    if (str && DoesFileExist(*str))
      return str;
  }
  return NULL;
}

const std::string* FileListBox::GetSelectedFolder() const
{
  const std::string* str = static_cast<const std::string*>(ItemBox::GetSelectedValue());
  if (str && DoesFolderExist(*str))
    return str;
  return NULL;
}

void FileListBox::StartListing(const char* dirname)
{
  if (dirname)
    PopulateFileList(dirname);
  else
    PopulateFileList(Config::GetInstance()->GetPersonalDataDir().c_str());
}

std::string FileListBox::FolderString(const char* name)
{
  std::string str = "[ ";
  return str + name + " ]";
}

bool FileListBox::MatchFilter(const char *name) const
{
  if (!name)
    return false;

  // Special case: no extension means no filtering
  if (extensions.size() == 0)
    return true;

  std::string ext = FileExtension(name);
  return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

void FileListBox::PopulateFileList(const std::string& path)
{
  new_path = path;
  if (path.compare(path.size()-1, sizeof(PATH_SEPARATOR), PATH_SEPARATOR))
    new_path += PATH_SEPARATOR;
  MSG_DEBUG("file", "Searching in %s\n", new_path.c_str());

  FolderSearch *f = OpenFolder(new_path);

  // Now that we have made use of new_path, it can be freed:
  // clearing the list is now possible
  Clear();

  if (f) {
    bool is_file = list_files;
    const char *name;

    while ((name = FolderSearchNext(f, is_file)) != NULL) {
      if (is_file) {
        // We have a file, check that it validates the list
        if (MatchFilter(name)) {
          std::string* filename = new std::string(new_path);
          *filename += name;
          MSG_DEBUG("file", "Adding file %s\n", name);
          AddLabelItem(false, ANSIToUTF8(new_path, name), filename, Font::FONT_MEDIUM);
        } else {
          MSG_DEBUG("file", "NOT adding file %s, invalid extension\n", name);
        }
      } else if (strcmp(name, ".")) {
        std::string* filename;
        if (!strcmp(name, "..")) {
          // Are we at the root?
          if (!strcmp(name, PATH_SEPARATOR))
            break;
          size_t pos = new_path.find_last_of(PATH_SEPARATOR, new_path.size()-2, sizeof(PATH_SEPARATOR));
          filename = new std::string(new_path.substr(0, pos+1));
        } else
          filename = new std::string(new_path);
        *filename += name;
        MSG_DEBUG("file", "Adding directory %s\n", name);
        AddLabelItem(false, std::string("[") + ANSIToUTF8(new_path, name) + "]", filename,
                     Font::FONT_MEDIUM, Font::FONT_NORMAL, c_yellow);
      } else
        MSG_DEBUG("file", "Rejecting %s\n", name);

      // Prepare again for searching files
      is_file = list_files;
    }

    CloseFolder(f);
    Pack();
    NeedRedrawing();
  } else {
    MSG_DEBUG("file", "Search failed?\n");
  }

  // Store last time to drop fast clicks
  last_time = SDL_GetTicks();
}

Widget* FileListBox::ClickUp(const Point2i & mousePosition, uint button)
{
  if (!Contains(mousePosition))
    return NULL;

  ItemBox::ClickUp(mousePosition, button);

  const std::string *name = (std::string*)GetSelectedValue();
  if (name && DoesFolderExist(*name)) {
    Uint32 now = SDL_GetTicks();

    // Check we didn't click too fast
    if (now - last_time > 1000)
      PopulateFileList(name->c_str());
    return NULL;
  }

  return (name) ? this : NULL;
}
