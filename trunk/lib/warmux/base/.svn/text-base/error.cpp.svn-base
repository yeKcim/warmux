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
 * Refresh des erreurs, exceptions, assertions, etc.
 *****************************************************************************/

#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <WARMUX_error.h>
#include <WARMUX_i18n.h>
#include <WARMUX_types.h>

#ifdef ANDROID
#  include <android/log.h>
#endif

#ifndef _MSC_VER
#  include <sys/types.h>
#  include <unistd.h>
#endif

static const std::string WARMUX_VERSION = PACKAGE_VERSION;

void WakeUpDebugger()
{
#if !defined(WIN32) && !defined(GEKKO) && !defined(__SYMBIAN32__)
  kill(getpid(), SIGABRT);
#endif
}

void MissedAssertion(const char *filename, unsigned int line,
                     const char *message)
{
  std::cerr << std::endl;
  std::cerr << filename << ':' << line
            << ": Missed assertion \"" << message << "\"."
            << std::endl;
#ifdef DEBUG
  WakeUpDebugger();
  abort();
#endif
}

static std::string FormatError(const char *filename, unsigned int line,
                               const std::string &txt)
{
  return Format(_("Error in %s:%u (Warmux %s) : %s"),
                filename, line, WARMUX_VERSION.c_str(), txt.c_str());
}

void TriggerWarning(const char *filename, unsigned int line,
                    const std::string &txt)
{
  std::string err = FormatError(filename, line, txt);
  std::cerr << "! " << err << std::endl;
#ifdef ANDROID
  __android_log_print(ANDROID_LOG_WARN, "Warmux", err.c_str());
#endif
}

void TriggerError(const char *filename, unsigned int line,
                  const std::string &txt)
{
  std::string err = FormatError(filename, line, txt);
  std::cerr << "! " << err << std::endl;
#ifdef ANDROID
  __android_log_print(ANDROID_LOG_FATAL, "Warmux", err.c_str());
#endif

  ASSERT(false);
  exit(-1);
}
