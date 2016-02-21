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
 * Refresh des erreurs, exceptions, ASSERTions, etc.
 *****************************************************************************/

#ifndef WARMUX_ERROR_H
#define WARMUX_ERROR_H

#include <string>

#ifdef _MSC_VER
#  define ATTRIBUTE_NORETURN __declspec(noreturn)
#elif defined(__GNUC__)
#  define ATTRIBUTE_NORETURN __attribute__((noreturn))
#else
#  define ATTRIBUTE_NORETURN
#endif

#ifdef DEBUG
ATTRIBUTE_NORETURN
#endif
void MissedAssertion(const char *filename, unsigned int line, const char *message);

// Assertion (disabled in release mode)
#undef ASSERT
#  define ASSERT(COND) \
     if (!(COND)) MissedAssertion (__FILE__, __LINE__, #COND);



// Usage:
// NET_ASSERT(COND)
// {
//   // What to do if ASSERTion failed
//   // This code is only executed during a network game
//   // and if DEBUG isn't defined
//   return;
// }
#ifdef DEBUG
#define NET_ASSERT(COND) \
  ASSERT(COND); \
  if(false)
#else
#define NET_ASSERT(COND) \
  ASSERT(COND); \
  if(!(COND))
#endif

void TriggerWarning(const char *filename, unsigned line, const std::string &txt);
ATTRIBUTE_NORETURN void
TriggerError(const char *filename, unsigned line, const std::string &txt);

#define Warning(txt) TriggerWarning(__FILE__, __LINE__, txt)
#define Error(txt) TriggerError(__FILE__, __LINE__, txt)

#endif
