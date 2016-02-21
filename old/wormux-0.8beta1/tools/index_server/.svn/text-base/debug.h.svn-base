/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
#include "clock.h"

#define	ALL		0
#define TRAFFIC		1
#define	MSG		2
#define	CONN		3
#define INFO		4

#define LOG_LEVEL	INFO


#ifdef DEBUG
#define DPRINT(LEVEL, ARGS...) \
{ \
	if((LEVEL) >= LOG_LEVEL ) \
	{ \
		printf("%s| %10s,%3i : ", wx_clock.TimeStr(),__FILE__,__LINE__); \
		printf(ARGS); \
		printf("\n"); \
	} \
}
#else
#define DPRINT(LEVEL, ARGS...)
#endif

#define TELL_ERROR \
{ \
	PRINT_ERROR; \
	exit(1); \
}

#define PRINT_ERROR \
{ \
	DPRINT(INFO , "%10s,%3i : ERROR! %s",__FILE__,__LINE__, strerror(errno)); \
}


#endif

