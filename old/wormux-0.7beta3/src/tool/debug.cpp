/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include "../include/base.h"

std::vector<std::string> debugModes;

void PrintDebug (const char *filename, unsigned long line,
                 const char *level, std::string message)
{
  int levelSize = strlen(level);
  unsigned int i = 0;
  
  for( i = 0; i < debugModes.size(); i++ ){
    int modeSize = debugModes[i].size();
    const char *strMode = debugModes[i].c_str();
     
    if( strncmp(strMode, level, modeSize) == 0){
      if( (levelSize != modeSize) && ( level[modeSize] != '.' ) && modeSize != 0)
	continue;
        
        std::cerr << "DEBUG (" << filename << ':' << line 
                  << ") " << level << " : " << message << "."
                  << std::endl;
        return;
    }
  }
}

void AddDebugMode( std::string mode ){
  debugModes.push_back( mode );
}

void InitDebugModes( int argc, char **argv ){
  int i;

  for( i=0; i<argc; i++ ){
    if( strcmp(argv[i], "--AddDebugMode") == 0 ){
      i = i + 1;
      if( i == argc )
        Error( "Usage : --AddDebugMode mode.truc" );
      AddDebugMode( argv[i] );
    }
  }
}
