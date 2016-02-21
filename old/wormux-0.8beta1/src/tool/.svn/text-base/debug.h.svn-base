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

#ifndef DEBUG_H
#define DEBUG_H

/** Usage example : 
 *
 * MSG_DEBUG( "game.pause", "Salut %s", "Truc" )
 * 
 * MSG_DEBUG use standart printf style for the message.
 * 
 * A debug will be printed only if there is a mode game or game.pause. A mode can be added by running wormux with:
 * ./wormux -d game        # print all messages in game section
 * ./wormux -d ""          # print all debug messages
 *
 */
#ifdef DEBUG
#  define MSG_DEBUG(LEVEL, MESSAGE, ARGS...) \
	PrintDebug( __FILE__, __PRETTY_FUNCTION__, __LINE__, LEVEL, MESSAGE, ## ARGS)
#else
#  define MSG_DEBUG(LEVEL, MESSAGE, ARGS...) do {} while (0)
#endif

void PrintDebug (const char *filename, const char *function, unsigned long line,
                 const char *level, const char *message, ...);
void AddDebugMode( std::string mode );
void InitDebugModes( int argc, char **argv );

#endif
