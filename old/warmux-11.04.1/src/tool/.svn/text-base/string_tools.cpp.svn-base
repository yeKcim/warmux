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
 * Refresh character strings.
 *****************************************************************************/

#include "tool/string_tools.h"
#include <sstream>
#include <assert.h>

template<typename T>
bool str2type(const std::string &txt, T &value)
{
  std::stringstream ss;
  ss << txt;
  ss >> value;
  return true;
}

bool str2uint(const std::string &txt, uint &value) { return str2type<uint>(txt, value); }

bool str2int(const std::string &txt, int &value) { return str2type<int>(txt, value); }

bool str2Double(const std::string &txt, Double &value) { return str2type<Double>(txt, value); }

bool str2float(const std::string &txt, float& value) { return str2type<float>(txt, value); }

bool str2bool(const std::string &str, bool &value)
{
  // Try to convert str to a boolean value
  // return true and set the value on succeed
  // return if false its not a boolean
  if (str=="1" || str=="true" || str=="on") {
    value = true;
    return true;
  }
  if (str=="0" || str=="false" || str=="off") {
    value = false;
    return true;
  }
  return false;
}

std::string Double2str(Double x, int places)
{
  std::ostringstream ss;
  printTo(ss, x, places);
  return ss.str();
}

template<typename T>
std::string type2str(T value)
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

std::string float2str(float x) { return type2str<float>(x); }
std::string int2str(int x) { return type2str<int>(x); }
std::string uint2str(uint x) { return type2str<uint>(x); }

std::string bool2str(bool x)
{
  return (x) ? "true" : "false";
}

#ifdef _WIN32
#  include <windows.h>
char* LocaleToUTF8(const char* data)
{
  assert(data);
  //printf("Converting %s\n", data);

  // Convert from OEM to UTF-16
  int len = 2*MultiByteToWideChar(CP_OEMCP, 0, data, -1, NULL, 0);
  assert(len > 0);
  char* utf16 = new char[len+1];
  int ret = MultiByteToWideChar(CP_OEMCP, 0, data, -1, (LPWSTR)utf16, len);
  assert(ret > 0);

  // Now convert from UTF-16 to UTF-8
  len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)utf16, -1, NULL, 0, NULL, NULL);
  assert(len > 0);
  char* str = new char[len+1];
  ret = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)utf16, -1, str, len, NULL, NULL);
  assert(ret > 0);

  delete[] utf16;
  return str;
}
#endif
