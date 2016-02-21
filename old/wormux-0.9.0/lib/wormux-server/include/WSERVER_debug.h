/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2010 Wormux Team.
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

#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <WSERVER_clock.h>
#include <WSERVER_config.h>

#define ALL        0
#define TRAFFIC    1
#define MSG        2
#define CONN       3
#define INFO       4

#define LOG_LEVEL    TRAFFIC

#define DPRINTMSG(STREAM, ARGS...)					\
  {									\
  fprintf(STREAM, "%s %s| %18s,%4i : ", BasicClock::DateStr(),		\
          BasicClock::TimeStr(),__FILE__,__LINE__);			\
  fprintf(STREAM, ARGS);						\
  fprintf(STREAM,"\n");							\
  }									\

#define DPRINT(LEVEL, ARGS...)                                          \
  {                                                                     \
    if (WSERVER_Verbose)                                                \
      if((LEVEL) >= LOG_LEVEL )                                         \
        DPRINTMSG(stdout, ARGS);						\
  }

#define PRINT_FATAL_ERROR						\
  {									\
    PRINT_ERROR;							\
    exit(EXIT_FAILURE);								\
  }

#define PRINT_ERROR							\
  {									\
    DPRINTMSG(stderr, "ERROR: %s", strerror(errno));			\
  }


#endif
