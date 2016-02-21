/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
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
 * Notify a top server of an opened wormux server
 * Obtain information about running games from a top server
 *****************************************************************************/

#ifndef TOP_SERVER_MSG_H
#define TOP_SERVER_MSG_H

enum IndexServerMsg
{
  TS_NO_MSG, // Don't send this

  //********  Messages shared by wormux clients, womux servers, and index servers ********
  TS_MSG_VERSION,        // Send this to identify the server
  TS_MSG_HOSTING,        // Send this if we are hosting a game
  TS_MSG_GET_LIST,       // Send this to obtain the list of the current hosting games
  TS_MSG_PING,
  TS_MSG_PONG,
  TS_MSG_REGISTER_GAME,  // Send this to give a name to the game hosted by the server

  //********  index servers to index servers messages  ***********************************
  TS_MSG_WIS_VERSION,    // Send the server version
  TS_MSG_JOIN_LEAVE,     // Send this when a wormux server joined or left the index server

  TS_LAST_MSG // Don't send this
};

const std::string server_list_url = "http://www.wormux.org/server_list";

#endif

