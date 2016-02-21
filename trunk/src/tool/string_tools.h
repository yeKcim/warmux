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

#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <WARMUX_base.h>
#include <string>

// Conversion string -> type
// Renvoie false cas d'erreur
bool str2int(const std::string &txt, int &value);
bool str2uint(const std::string &txt, uint &value);
bool str2Double(const std::string &txt, Double &value);
bool str2float(const std::string &txt, float& value);
bool str2bool(const std::string &str, bool &value);

// Conversion type -> string
std::string int2str(int x);
std::string uint2str(uint x);
std::string Double2str(Double x, int places = -1);
std::string float2str(float x);
std::string bool2str(bool x);

#ifdef _WIN32
// Allocated with new
char* LocaleToUTF8(const char* orig);
#endif

#endif // STRING_TOOLS_H
