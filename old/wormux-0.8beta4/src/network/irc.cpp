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
 * Irc client
 *****************************************************************************/

#include <SDL_net.h>
#include "network/download.h"
#include "network/irc.h"


IrcChat::IrcChat()
{
}

IrcChat::~IrcChat()
{
}


connection_state_t IrcChat::Connect()
{
//  // If we don't the server adress download it
//  if(server_lst.size() == 0)
//    server_lst = Downloader::GetInstance()->GetServerList("irc_server");
//
//  // If the download failed go back
//  if(server_lst.size() == 0)
//    return CONN_REJECTED;

//  connection_state_t r = sock.Connect(server_lst.begin()->first,
//                                      server_lst.begin()->second );

  return CONNECTED;

  connection_state_t r = sock.Connect(std::string("localhost"), 6667);

  if(r != CONNECTED)
	  return r;

  printf("Connected\n");

  r = sock.SendString("USER ident * * :Wormux player\r\n");
  if(r != CONNECTED)
	  return r;

  r = sock.SendString("NICK wx_player\r\n");
  if(r != CONNECTED)
	  return r;

  char* str = NULL;
  while(!str)
  {
    r = sock.GetString(&str, '\n');
    if(r != CONNECTED)
      return r;
  }
  printf("%s\n", str);
  free(str);

  return CONNECTED;
}

