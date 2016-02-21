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
 * Refresh des erreurs, exceptions, ASSERTions, etc.
 *****************************************************************************/

#ifndef ERROR_H
#define ERROR_H

#ifndef BASE_H
#  error "You have to include base.h only (not the error.h file)"
#endif

#include <string>

void MissedAssertion (const char *filename, unsigned long line,
                      const char *message);
void WakeUpDebugger();

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
#include "network/network.h"

#define NET_ASSERT(COND) \
  ASSERT(COND); \
  if(!(COND) && Network::GetInstance()->IsConnected())
#endif

void TriggerError (const char *filename, unsigned long line,
                   const std::string &txt);

#define Error(txt) TriggerError(__FILE__, __LINE__, txt)


class CError : public std::exception
{
protected:
  std::string m_filename, m_txt;
  unsigned long m_line;

public:
  CError (const char *filename, unsigned long line, const std::string &txt);
  ~CError() throw();
  virtual const char* what() const throw();
  std::ostream& operator<< (std::ostream &os) const;
};

#endif
