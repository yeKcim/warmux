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

#include <fstream>
#include <sys/stat.h>
#include <errno.h>
#include <cstdlib>

#ifdef WIN32
   // To get SHGetSpecialFolderPath
#  define _WIN32_IE   0x400
#  include <shlobj.h>
#  include <io.h>
#  include <fcntl.h>
#  include <direct.h>
#  undef DeleteFile  // windows.h defines it I think
#else
#  include <stdlib.h> // getenv
#  include <unistd.h> // not needed by mingw
#endif

#include <WARMUX_error.h>
#include <WARMUX_file_tools.h>


// Test if a file exists
bool DoesFileExist(const std::string &name)
{
  struct stat stat_file;
  if (stat(name.c_str(), &stat_file))
    return false;
  return (stat_file.st_mode & S_IFMT) == S_IFREG;
}

// Check if the folder exists
bool DoesFolderExist(const std::string &name)
{
  // Is it a directory ?
  struct stat stat_file;
  if (stat(name.c_str(), &stat_file))
    return false;
  return (stat_file.st_mode & S_IFMT) == S_IFDIR;
}

#ifndef WIN32
#  define MKDIR(dir) (mkdir(dir, 0750))
#  if defined(GEKKO)
#    define rmdir(dir) remove(dir)
#    define dup(fd) fcntl(fd, F_DUPFD) // This is strictly not equivalent
#  endif
#else
#  define MKDIR(dir) (_mkdir(dir))
#  define rmdir(dir) (_rmdir(dir))
#endif

bool CreateFolder(const std::string &name)
{
  if (DoesFolderExist(name))
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
  if (DoesFolderExist(name)){
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
std::string FileExtension(const std::string &name)
{
  int pos = name.rfind('.');
  if (pos != -1)
    return name.substr(pos+1);
  else
    return "";
}

#ifdef WIN32
// Return the path to the home directory of the user
static std::string GetWindowsPath(int csidl){
  static TCHAR szPath[MAX_PATH];

  // "Documents and Settings\user" is CSIDL_PROFILE
  if (SHGetSpecialFolderPath(NULL, szPath, csidl, FALSE) == TRUE)
    return szPath;

  return "";
}

std::string GetOldPersonalDir() { return GetWindowsPath(CSIDL_APPDATA) + "\\Wormux\\"; }
std::string GetHome() { return GetWindowsPath(CSIDL_PERSONAL); }
bool Rename(const std::string& old_name, const std::string& new_name)
{
  DWORD flags = MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH;
  if (!MoveFileEx(old_name.c_str(), new_name.c_str(), flags)) {

    // Don't report the error if the source doesn't exist
    if (DoesFolderExist(old_name))
      fprintf(stderr, "%s not moved to %s: %i\n",
              old_name.c_str(), new_name.c_str(), GetLastError());
    return false;
  }
  return true;
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
  if (f->file_search == INVALID_HANDLE_VALUE) {
    FindClose(f->file_search);
    delete f;
    return NULL;
  }

  return f;
}

const char* FolderSearchNext(FolderSearch *f, bool& file)
{
  WIN32_FIND_DATA *find = &f->file;
  while (FindNextFile(f->file_search, find)) {
    const char *name = (find->cAlternateFileName && find->cAlternateFileName[0])
                     ? find->cAlternateFileName : find->cFileName;
    if (find->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // If we are also looking for files, report it isn't one
      if (file)
        file = false;
      return name;
    }

    // We're not searching for folder, so exit with failure
    if (!file)
      continue;

    // This is a file and we do search for files
    file = true;
    return name;
  }
  return NULL;
}

void CloseFolder(FolderSearch *f)
{
  if (f) {
    FindClose(f->file_search);
    delete f;
  }
}

std::string GetTmpDir()
{
  char *txt = std::getenv("TMPDIR");
  if (txt != NULL)
    return txt;

  txt = std::getenv("TMP");
  if (txt != NULL)
    return txt;

  txt = std::getenv("TEMP");
  if (txt != NULL)
    return txt;

  if (txt == NULL)
    Error (_("TEMP directory could not be found!"));

  return txt;
}

#else

#  if defined(ANDROID)
std::string GetHome() { return "."; }
std::string GetOldPersonalDir() { return ""; }
std::string GetTmpDir() { return "."; }
#  elif defined(GEKKO) // Wii
std::string GetHome() { return "sd:/apps/Warmux"; }
std::string GetOldPersonalDir() { return ""; }
std::string GetTmpDir() { return "sd:/apps/Warmux"; }
#  elif defined(__SYMBIAN32__)
std::string GetHome() { return "."; }
std::string GetOldPersonalDir() { return "."; }
std::string GetTmpDir() { return "."; }
#  else // Linux or Apple
std::string GetHome()
{
  char *txt = std::getenv("HOME");

  if (txt == NULL)
    Error(_("HOME directory (environment variable $HOME) could not be found!"));

  return txt;
}

std::string GetTmpDir() { return "/tmp"; }

#    ifdef __APPLE__
std::string GetOldPersonalDir() { return ""; }
#    else
std::string GetOldPersonalDir() { return GetHome() + "/.wormux/"; }
#    endif
#  endif // Linux or Apple

bool Rename(const std::string& old_name, const std::string& new_name)
{
  return rename(old_name.c_str(), new_name.c_str()) == 0;
}

#include <dirent.h>
struct _FolderSearch
{
  DIR           *dir;
  struct dirent *file;
#ifdef __SYMBIAN32__
  std::string    dname;
#endif
};

FolderSearch* OpenFolder(const std::string& dirname)
{
  FolderSearch *f = new FolderSearch;
  f->dir = opendir(dirname.c_str());
#ifdef __SYMBIAN32__
  f->dname = dirname;
#endif

  if (!f->dir) {
    delete f;
    return NULL;
  }

  return f;
}

const char* FolderSearchNext(FolderSearch *f, bool& file)
{
  while ((f->file = readdir(f->dir)) != NULL) {

#ifdef __SYMBIAN32__
    if (f->file->d_namlen && DoesFolderExist(f->dname+"/"+std::string(f->file->d_name))) {
#else
    if (f->file->d_type == DT_DIR) {
#endif
      // If we are also looking for files, report it isn't one
      if (file)
        file = false;
      return f->file->d_name;
    }

    // We're not searching for folder, exit with failure
    if (!file)
      continue;

    // This is a file and we do search for file
#ifdef __SYMBIAN32__
    if (f->file->d_namlen && DoesFileExist(f->dname+"/"+std::string(f->file->d_name))) {
#else
    if (f->file->d_type == DT_REG) {
#endif
      file = true;
      return f->file->d_name;
    }

    // Not something we like, so skip it
  }
  return NULL;
}

void CloseFolder(FolderSearch *f)
{
  if (f) {
    closedir(f->dir);
    delete f;
  }
}
#endif

std::string CreateTmpFile(const std::string& prefix, int* fd)
{
  ASSERT(fd);
  char path[512];
  snprintf(path, 512, "%s/%sXXXXXX", GetTmpDir().c_str(), prefix.c_str());

#ifdef WIN32
  mktemp(path);
  *fd = open(path, O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
#else
  *fd = mkstemp(path);
#endif
  return path;
}

#ifndef _WIN32
// Replace ~ by its true name
std::string TranslateDirectory(const std::string &directory)
{
  std::string home = GetHome();
  std::string txt = directory;

  for (int pos = txt.length()-1;
       (pos = txt.rfind ('~', pos)) != -1;
       --pos) {
    txt.replace(pos,1,home);
  }
  return txt;
}
#endif

std::string FormatFileName(const std::string &name)
{
 std::string formated_name = name;

  for (uint i=0; i<formated_name.size();i++) {
    if (formated_name[i] == ' '){
      formated_name[i] = '_';
    }
    if (formated_name[i] == '.') {
      formated_name[i] = '_';
    }
    if (formated_name[i] == '/') {
      formated_name[i] = '_';
    }
    if (formated_name[i] == '\\') {
      formated_name[i] = '_';
    }
  }

  return formated_name;
}
