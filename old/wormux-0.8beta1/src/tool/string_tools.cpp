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
 * Refresh des chaînes de caractère (string).
 *****************************************************************************/

#include "tool/string_tools.h"
#include <sstream>

bool str2long (const std::string &txt, long &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
  return true;
}

bool str2int (const std::string &txt, int &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
  return true;
}

bool str2double (const std::string &txt, double &valeur)
{
  std::stringstream ss;
  ss << txt;
  ss >> valeur;
  return true;
}

bool str2bool(const std::string &str, bool &value)
{
  // Try to convert str to a boolean value
  // return true and set the value on succeed
  // return if false its not a boolean
  if(str=="1" || str=="true" || str=="on")
  {
    value = true;
    return true;
  }
  if(str=="0" || str=="false" || str=="off")
  {
    value = false;
    return true;
  }
  return false;
}

std::string double2str (double x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

std::string long2str (long x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

std::string ulong2str (ulong x)
{
  std::ostringstream ss;
  ss << x;
  return ss.str();
}

std::string bol2str (bool x)
{
  std::ostringstream ss;
  if(x)
    ss << "true";
  else
    ss << "false";
  return ss.str();
}

