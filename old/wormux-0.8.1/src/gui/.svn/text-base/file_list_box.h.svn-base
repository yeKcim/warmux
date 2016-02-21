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
 * List of files, class declaration
 *****************************************************************************/

#ifndef FILE_LIST_BOX_H
#define FILE_LIST_BOX_H

#include "list_box.h"

class FileListBox : public ListBox
{
  uint                     last_time;
  char                     old_path[1024];
  std::vector<const char*> extensions;

  std::string FolderString(const char* name);
  void PopulateFileList(const char *new_path);
  bool MatchFilter(const char *name) const;

public:
  FileListBox (const Point2i &size, bool always_one_selected_b = true,
               const char *first_dir = NULL);
  ~FileListBox();

  void AddExtensionFilter(const char* ext) { extensions.push_back(ext); }
  Widget* Click(const Point2i &mousePosition, uint button);
};

#endif // FILE_LIST_BOX_H
