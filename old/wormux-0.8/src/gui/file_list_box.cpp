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
 * List of files, class definition
 *****************************************************************************/

#include <iostream>
#include <sys/stat.h>
#include "file_list_box.h"
#include "game/config.h"
#include "tool/file_tools.h"

#if !defined(WIN32) && !defined(_WIN32)
# include <dirent.h>
# include <unistd.h>
# define  GetCurrentDirectory(a, b) getcwd(b, a)
# define  SetCurrentDirectory(a)    chdir(a)
#else
# include <windows.h>
#endif

FileListBox::FileListBox (const Point2i &size, bool always, const char *first_dir)
  : ListBox(size, always)
{
  GetCurrentDirectory(1024, old_path);
  if (first_dir)
    PopulateFileList(first_dir);
  else
    PopulateFileList(Config::GetInstance()->GetPersonalDataDir().c_str());
}

FileListBox::~FileListBox()
{
  SetCurrentDirectory(old_path);
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

  if (extensions.size() == 0)
    return true;

  std::string ext = FileExtension(name);
  for (uint i=0; i<extensions.size(); i++)
  {
    if (name == extensions[i])
      return true;
  }

  return false;
}

void FileListBox::PopulateFileList(const char *new_path)
{
  ClearItems();

  SetCurrentDirectory(new_path);
  std::cout << "Searching in " << new_path << "\n";
  
#if !defined(WIN32) && !defined(_WIN32)
  struct dirent *file;
  DIR           *dir = opendir(".");

  if (dir != NULL)
  {
    while ((file = readdir(dir)) != NULL)
    {
      // We only want .dat files
      struct stat s;
      if (stat(file->d_name, &s) == 0)
      {
        if (S_ISDIR(s.st_mode))
        {
          //std::cout << "Adding directory " << file->d_name << "\n";
          AddItem(false, FolderString(file->d_name), file->d_name,
                  Font::FONT_SMALL, Font::FONT_NORMAL, c_yellow);
        }
        // We only want files ending with proper extensions
        else if (MatchFilter(file->d_name))
        {
          //std::cout << "Adding file " << file->d_name << "\n";
          AddItem(false, file->d_name, file->d_name);
        }
      }
    }
    closedir(dir);
  }
  else
    AddItem(false, "!! Nothing !!", "!! Nothing !!");
#else
  WIN32_FIND_DATA file;
  HANDLE          file_search = FindFirstFile("*.*", &file);

  if (file_search != INVALID_HANDLE_VALUE)
  {
    while (FindNextFile(file_search, &file))
    {
      if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        //std::cout << "Adding directory " << file.cFileName << "\n";
        AddItem(false, FolderString(file.cFileName), file.cFileName,
                Font::FONT_SMALL, Font::FONT_NORMAL, yellow_color);
      }
      else if (MatchFilter(file.cFileName))
      {
        //std::cout << "Adding file " << file.cFileName << "\n";
        AddItem(false, file.cFileName, file.cFileName);
      }
    }
  }
  else
    AddItem(false, "!! Nothing !!", "!! Nothing !!");

  FindClose(file_search);
#endif

  // Store last time to drop fast clicks
  last_time = SDL_GetTicks();
}

Widget* FileListBox::Click(const Point2i &mousePosition, uint button)
{
  if (!Contains(mousePosition))
    return NULL;

  if ( button == SDL_BUTTON_LEFT)
  {
    if (ScrollBarPos().Contains(mousePosition))
      scrolling = true;

    int item = MouseIsOnWhichItem(mousePosition);
    if (item == -1)
      return NULL;

    if (item != selected_item)
      Select (item);

    const char *name = ReadValue().c_str();
    struct stat s;
    if (stat(name, &s)==0 && S_ISDIR(s.st_mode))
    {
      Uint32 now = SDL_GetTicks();
      
      // Check we didn't click too fast
      if (now - last_time > 1000)
        PopulateFileList(name);
    }
  }
  return this;
}
