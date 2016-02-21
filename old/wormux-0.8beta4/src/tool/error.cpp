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
 * Refresh des erreurs, exceptions, assertions, etc.
 *****************************************************************************/

#include "include/base.h"
#include <iostream>
#include <signal.h>
#include "tool/i18n.h"

#if !defined WIN32 || defined __MINGW32__
#include <sys/types.h>
#include <unistd.h>
#endif

void WakeUpDebugger()
{

#ifdef LOVE_HAYPO_HACKS
  // Generate SIGTRAP
  asm ("int $0x03");
#endif

#if !defined WIN32
  kill (getpid(), SIGABRT);
#endif
}

void MissedAssertion (const char *filename, unsigned long line,
                      const char *message)
{
  std::cout << std::endl;
  std::cerr << filename << ':' << line
            << ": Missed assertion \"" << message << "\"."
            << std::endl;
#if defined DEBUG
  WakeUpDebugger();
  abort();
#endif
}

CError::CError (const char *filename, unsigned long line,
                const std::string &txt)
  : m_filename(filename), m_txt(txt), m_line(line)
{}

CError::~CError() throw()
{}

const char* CError::what() const throw()
{
  return m_txt.c_str();
}

std::ostream& CError::operator<< (std::ostream &os) const
{
  os << m_txt;
  return os;
}

void TriggerError (const char *filename, unsigned long line,
                   const std::string &txt)
{
  std::cerr << "! "
            << Format(_("Error in %s:%lu"), filename, line)
            << ": " << txt << std::endl;

  ASSERT(false);
  throw CError (filename, line, txt);
}
