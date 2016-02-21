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
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#ifdef _MSC_VER
#  include <process.h>
#  define getpid _getpid
#else
#  include <unistd.h>
#endif
#include "include/base.h"
#include "tool/debug.h"

bool debug_all = false;
/**
 * The debug modes in use.
 */
std::vector<std::string> debugModes;

/**
 * Check if a debug mode is in use
 */
#ifdef DEBUG
bool IsDEBUGGING(const char* mode)
{
  int mSize = strlen(mode);
  unsigned int i = 0;

  for (i = 0; i < debugModes.size(); i++) {
    int modeSize = debugModes[i].size();
    const char *strMode = debugModes[i].c_str();
    
    if (strncmp(strMode, mode, modeSize) == 0) {
      if ( (mSize != modeSize) && ( mode[modeSize] != '.' ) && modeSize != 0)
	continue;
      
      return true;
    }
  }

  return false;
}
#endif

/**
 * Print a debug message if needed.
 *
 * @param filename
 * @param line
 * @param level
 * @param message
 */
void PrintDebug (const char *filename, const char *function, unsigned long line,
                 const char *level, const char *message, ...)
{
  if (debug_all || IsDEBUGGING(level)) {
      va_list argp;
      int pid = (int)getpid();

      fprintf(stderr, "%i|%s:%s:%ld| %s : ", pid, filename, function, line, level);
      va_start(argp, message);
      vfprintf(stderr, message, argp);
      va_end(argp);
      fprintf(stderr, "\n");
  }
}



/**
 * Add a new debug mode to check.
 */
void AddDebugMode(const std::string& mode ){
  if(mode == "all") {
    debug_all = true;
    return;
  }
  debugModes.push_back( mode );
}
