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

#include "tool/file_tools.h"
#include <fstream>
#ifdef _WIN32
   // To get SHGetSpecialFolderPath
#  define _WIN32_IE   0x400
#  include <shlobj.h>
#else
#  include <stdlib.h> // getenv
#endif

#include "i18n.h"

// Test if a file exists
bool IsFileExist(const std::string &name)
{
  std::ifstream f(name.c_str());
  bool exist = f;
  f.close();
  return exist;
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

// Return the path to the home directory of the user
#ifndef WIN32
std::string GetHome()
{
  char *txt = getenv("HOME");
  
  if (txt == NULL) 
    Error (_("HOME directory (environment variable $HOME) could not be found!"));
  
  return txt;
}
#else
std::string GetHome (){
  TCHAR szPath[MAX_PATH];

  // "Documents and Settings\user" is CSIDL_PROFILE
  if(SHGetSpecialFolderPath(NULL, szPath,
                            CSIDL_APPDATA, FALSE) == TRUE)
  {
    return szPath;
  }
  return "";
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
