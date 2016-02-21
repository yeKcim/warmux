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
 * Refresh des fichiers.
 *****************************************************************************/

#include "tool/file_tools.h"
#include <fstream>
#include <sys/stat.h>
#include <errno.h>

#ifdef WIN32
   // To get SHGetSpecialFolderPath
#  define _WIN32_IE   0x400
#  include <shlobj.h>
#  include <io.h>
#  include <direct.h>
#  undef DeleteFile  // windows.h defines it I think
#else
#  include <stdlib.h> // getenv
#endif

#include "tool/i18n.h"

// Test if a file exists
bool DoesFileExist(const std::string &name)
{
  std::ifstream f(name.c_str());
  bool exist = f.good();
  f.close();
  return exist;
}

// Check if the folder exists
bool IsFolderExist(const std::string &name)
{
  // Is it a directory ?
  struct stat stat_file;
  if (stat(name.c_str(), &stat_file) != 0)
        return false;
  return (stat_file.st_mode & S_IFMT) == S_IFDIR;
}

#ifndef WIN32
#  define MKDIR(dir) (mkdir(dir, 0750))
#else
#  define MKDIR(dir) (_mkdir(dir))
#  define rmdir(dir) (_rmdir(dir))
#endif

bool CreateFolder(const std::string &name)
{
  if (IsFolderExist(name))
    return true; // folder is already existing, nothing to do :-)

  std::string dir = name;
  std::string subdir;
  std::size_t pos;

  // Create the needed parent folders
  pos = dir.find("/");
  while (pos != dir.npos) {
    subdir = dir.substr(0, pos);
    printf("%s\n", subdir.c_str());

    if (subdir.size() != 0) {
      // Create the directory if it doesn't exist
      if (MKDIR(subdir.c_str()) != 0 && errno != EEXIST)
        return false;
    }
    pos = dir.find("/", pos+1);
  }

  // Create the directory if it doesn't exist
  if (MKDIR(dir.c_str()) != 0 && errno != EEXIST)
    return false;

  return true;
}


// Delete the folder if it exists
bool DeleteFolder(const std::string &name)
{
  if (IsFolderExist(name)){
    return (rmdir(name.c_str())==0);
  }
  return false;
}


// Delete the file if it exists
bool DeleteFile(const std::string &name)
{
  return (remove(name.c_str()) == 0);
}


// Find the extension part of a filename
std::string FileExtension (const std::string &name)
{
  int pos = name.rfind('.');
  if (pos != -1)
    return name.substr(pos+1);
  else
    return "";
}

#ifdef WIN32
// Return the path to the home directory of the user
std::string GetHome (){
  TCHAR szPath[MAX_PATH];

  // "Documents and Settings\user" is CSIDL_PROFILE
  if(SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE) == TRUE)
    return szPath;

  return "";
}

#include <windows.h>
struct _FolderSearch
{
  WIN32_FIND_DATA file;
  HANDLE          file_search;
};

FolderSearch *OpenFolder(const std::string& dirname)
{
  std::string  pattern = dirname + "*.*";
  FolderSearch *f      = new FolderSearch;

  f->file_search = FindFirstFile(pattern.c_str(), &f->file);
  if (f->file_search == INVALID_HANDLE_VALUE)
  {
     FindClose(f->file_search);
     delete f;
         return NULL;
  }

  return f;
}

const char* FolderSearchNext(FolderSearch *f)
{
  while (FindNextFile(f->file_search, &f->file))
  {
     if (f->file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
       return f->file.cFileName;
  }
  return NULL;
}

void CloseFolder(FolderSearch *f)
{
  if (f)
  {
    FindClose(f->file_search);
        delete f;
  }
}

#else
// Return the path to the home directory of the user
std::string GetHome()
{
  char *txt = getenv("HOME");

  if (txt == NULL)
    Error (_("HOME directory (environment variable $HOME) could not be found!"));

  return txt;
}

#include <dirent.h>
struct _FolderSearch
{
  DIR           *dir;
  struct dirent *file;
};

FolderSearch* OpenFolder(const std::string& dirname)
{
  FolderSearch *f = new FolderSearch;
  f->dir = opendir(dirname.c_str());

  if (!f->dir)
  {
    delete f;
    return NULL;
  }

  return f;
}

const char* FolderSearchNext(FolderSearch *f)
{
  f->file = readdir(f->dir);
  return (f->file) ? f->file->d_name : NULL;
}

void CloseFolder(FolderSearch *f)
{
  if (f)
  {
        closedir(f->dir);
    delete f;
  }
}
#endif

// Replace ~ by its true name
std::string TranslateDirectory(const std::string &directory)
{
  std::string home = GetHome();
  std::string txt = directory;

  for (int pos = txt.length()-1;
       (pos = txt.rfind ('~', pos)) != -1;
       --pos)
  {
    txt.replace(pos,1,home);
  }
  return txt;
}

std::string FormatFileName(const std::string &name)
{
 std::string formated_name = name;

    for(unsigned i = 0;i<formated_name.size();i++)
    {
      if(formated_name[i] == ' '){
          formated_name[i] = '_';
      }
      if(formated_name[i] == '.'){
          formated_name[i] = '_';
      }
      if(formated_name[i] == '/'){
          formated_name[i] = '_';
      }
      if(formated_name[i] == '\\'){
          formated_name[i] = '_';
      }
    }
  return formated_name;
}

