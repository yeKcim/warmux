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

#ifndef SERVER_H
#define SERVER_H
#include <netinet/in.h>
#include <stdio.h>

class Client;

class Server
{
	int fd;
	fd_set sock_set;
	struct sockaddr_in address;

public:
	Server(int port);
	~Server();

	int & GetFD();
	fd_set & GetSockSet();

	// Open a connection requested by a new client
	Client* NewConnection();
	void CloseConnection(int client_fd);
};

#endif
