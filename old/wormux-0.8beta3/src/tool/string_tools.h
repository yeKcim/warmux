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
 * Refresh character strings.
 *****************************************************************************/

#ifndef STRING_TOOLS_H
#define STRING_TOOLS__H

#include "include/base.h"
#include <string>

// Conversion string -> type
// Renvoie false cas d'erreur
bool str2long (const std::string &txt, long &valeur);
bool str2int (const std::string &txt, int &valeur);
bool str2double (const std::string &txt, double &valeur);
bool str2bool(const std::string &str, bool &value);

// Conversion type -> string
std::string long2str (long x);
std::string ulong2str (ulong x);
std::string double2str (ulong x);
std::string bool2str (bool x);

#endif
