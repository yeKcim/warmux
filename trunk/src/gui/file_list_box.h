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
 * List of files, class declaration
 *****************************************************************************/

#ifndef FILE_LIST_BOX_H
#define FILE_LIST_BOX_H

#include <string>

#include "select_box.h"

class FileListBox : public ItemBox
{
  bool                     list_files;
  uint                     last_time;
  std::vector<const char*> extensions;
  std::string              new_path;

  std::string FolderString(const char* name);
  void PopulateFileList(const std::string& path);
  bool MatchFilter(const char *name) const;

public:
  FileListBox(const Point2i &size, bool list = true)
    : ItemBox(size, false), list_files(list) { }
  ~FileListBox() { Clear(); }

  void StartListing(const char* dirname = NULL);
  const std::string& GetCurrentFolder() const { return new_path; }

  /* Beware: all those strings are UTF-8, so you must convert them to WCHAR under Windows */
  virtual const std::string* GetSelectedFile() const;
  virtual const std::string* GetSelectedFolder() const;

  /* Beware that under Windows we are using shortnames, hence the extension is uppercase! */
  void AddExtensionFilter(const char* ext) { extensions.push_back(ext); }

  Widget* ClickUp(const Point2i & mousePosition, uint button);

  // This must be defined to properly handle the release of elements
  virtual void Empty();
  virtual void Clear();
  virtual void RemoveSelected();
};

#endif // FILE_LIST_BOX_H
