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
 * Refresh des fichiers.
 *****************************************************************************/

#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H
#include "include/base.h"

// Check if a file exists
bool IsFileExist(const std::string &name);

// Check if the folder exists
bool IsFolderExist(const std::string &name);

// Find the extension part of a filename
std::string FileExtension(const std::string &name);

// Return the path to the home directory of the user
std::string GetHome ();

// Replace ~ by its true name
std::string TranslateDirectory(const std::string &directory);

typedef struct _FolderSearch FolderSearch;

FolderSearch *OpenFolder(const std::string& dirname);
const char* FolderSearchNext(FolderSearch *f);
void CloseFolder(FolderSearch *f);

#endif
