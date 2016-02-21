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
#include <unistd.h>
#include "include/base.h"

/**
 * The debug modes in use.
 */
std::vector<std::string> debugModes;

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
	int levelSize = strlen(level);
	unsigned int i = 0;

	for( i = 0; i < debugModes.size(); i++ ){
		int modeSize = debugModes[i].size();
		const char *strMode = debugModes[i].c_str();

		if( strncmp(strMode, level, modeSize) == 0){
			if( (levelSize != modeSize) && ( level[modeSize] != '.' ) && modeSize != 0)
				continue;
	        
			va_list argp;
			int pid = (int)getpid();

			fprintf(stderr, "%i|%s:%s:%ld| %s : ", pid, filename, function, line, level);
	        va_start(argp, message);
	        vfprintf(stderr, message, argp);
	        va_end(argp);
			fprintf(stderr, "\n");
			return;
		}
	}
}

/** 
 * Add a new debug mode to check.
 */
void AddDebugMode( std::string mode ){
	debugModes.push_back( mode );
}

/**
 * Parse the command line arguments to find new debug mode to use.
 * 
 * @param argc Number of arguments.
 * @param argv The arguments.
 */
void InitDebugModes( int argc, char **argv ){
	int i;

	for( i=0; i<argc; i++ ){
		if( strcmp(argv[i], "-d") == 0 ){
			i = i + 1;
			if( i == argc )
				Error( "Usage : -d mode.truc" );
			AddDebugMode( argv[i] );
		}
	}
}
