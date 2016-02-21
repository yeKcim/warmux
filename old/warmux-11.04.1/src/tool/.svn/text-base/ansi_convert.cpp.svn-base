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
 *****************************************************************************
 * Retrieve string pasted depending on OS mechanisms.
 *****************************************************************************/

#ifdef _WIN32
#  include <windows.h>
#  include "ansi_convert.h"

std::string UTF8ToUTF16(std::string path, const std::string& name)
{
  path += name;
  int    len = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
  if (len) {
    std::string tmp;
    tmp.resize(len*2);
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, (LPWSTR)tmp.c_str(), len*2);
    return tmp;
  }
  return path;
}

std::string ANSIToUTF8(std::string path, const std::string& name)
{
  path += name;
  int    len = MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, NULL, 0);
  LPWSTR buf = (LPWSTR)malloc(2*len);

  if (buf) {
    MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, buf, len);
    int plen = GetLongPathNameW(buf, NULL, 0);
    LPWSTR buf2 = (LPWSTR)malloc(plen*2+2);
    if (buf2) {
      GetLongPathNameW(buf, buf2, plen);
      LPWSTR buf3 = wcsrchr((wchar_t*)buf2, L'\\')+1;
      int ulen = WideCharToMultiByte(CP_UTF8, 0, buf3, -1, NULL, 0, NULL, NULL);
      if (ulen) {
        std::string tmp;
        tmp.resize(ulen-1);
        WideCharToMultiByte(CP_UTF8, 0, buf3, -1, (LPSTR)tmp.c_str(), ulen-1, NULL, NULL);
        free(buf2);
        free(buf);
        return tmp;
      }
      free(buf2);
    }
    free(buf);
  }
  return "";
}
#endif
