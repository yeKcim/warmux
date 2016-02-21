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
 * Refresh des fichiers.
 *****************************************************************************/

#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include "WARMUX_base.h"

// Check if a file exists
bool DoesFileExist(const std::string &name);

// Check if the folder exists
bool DoesFolderExist(const std::string &name);

// Create the folder if it does not exist yet
bool CreateFolder(const std::string &name);

// Delete the folder if it exists
bool DeleteFolder(const std::string &name);


// Delete the file if it exists
bool DeleteFile(const std::string &name);

// Find the extension part of a filename
std::string FileExtension(const std::string &name);

// Return the path to the home directory of the user
std::string GetHome();

// Return the path to the old home user directory, for removal
std::string GetOldPersonalDir();

// Return the path to the tmp directory
std::string GetTmpDir();

// Create a tmp file
std::string CreateTmpFile(const std::string& prefix, int *fd);

// Move a file or folder
bool Rename(const std::string& old_name, const std::string& new_name);

#ifdef _WIN32
#  define TranslateDirectory(a) (a)
#else
// Replace ~ by its true name
std::string TranslateDirectory(const std::string &directory);
#endif

typedef struct _FolderSearch FolderSearch;

FolderSearch *OpenFolder(const std::string& dirname);
/**
 * Set file to true to have in it on return whether the returned
 * string is a file
 * @warning The string returned is a temporary!
 */
const char* FolderSearchNext(FolderSearch *f, bool& file);
void CloseFolder(FolderSearch *f);

std::string FormatFileName(const std::string &name);

#endif
