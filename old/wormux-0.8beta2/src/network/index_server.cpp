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
 * Notify an index server of an opened wormux server
 * Obtain information about running games from an index server
 *****************************************************************************/

#include <SDL_net.h>
#include <fstream>
#include "download.h"
#include "game/config.h"
#include "graphic/video.h"
#include "index_server.h"
#include "index_svr_msg.h"
#include "network.h"
#include "include/app.h"
#include "include/constant.h"
#include "tool/debug.h"
#include "tool/random.h"

IndexServer index_server;

IndexServer::IndexServer():
  socket(),
  ip(),
  sock_set(),
  server_lst(),
  first_server(server_lst.end()),
  current_server(server_lst.end()),
  hidden_server(false),
  connected(false)
{
}

IndexServer::~IndexServer()
{
  server_lst.clear();
  if(connected)
    Disconnect();
}

/*************  Connection  /  Disconnection  ******************/
Network::connection_state_t IndexServer::Connect()
{
  MSG_DEBUG("index_server", "Connecting..");
  ASSERT(!connected);

  if( hidden_server )
    return Network::CONNECTED;

  if( !GetServerList() )
    return Network::CONN_REJECTED;

  std::string addr;
  int port;

  // Cycle through the list of server
  // Until we find one running
  while( GetServerAddress( addr, port) )
  {
    if( ConnectTo( addr, port) )
      return Network::CONNECTED;
  }

  return Network::CONN_REJECTED;
}

bool IndexServer::ConnectTo(const std::string & address, const int & port)
{
  MSG_DEBUG("index_server", "Connecting to %s %i", address.c_str(), port);
  AppWormux::GetInstance()->video->Flip();

  Network::Init(); // To get SDL_net initialized

  MSG_DEBUG("index_server", "Opening connection");

  if( SDLNet_ResolveHost(&ip, address.c_str() , port) == -1 )
  {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return false;
  }

  socket = SDLNet_TCP_Open(&ip);
  if(!socket)
  {
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return false;
  }

  sock_set = SDLNet_AllocSocketSet(1);
  if(!sock_set)
  {
    printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
    return false;
  }
  SDLNet_TCP_AddSocket(sock_set, socket);

  connected = true;

  return HandShake();
}

void IndexServer::Disconnect()
{
  if( hidden_server )
  {
    hidden_server = false;
    return;
  }

  if( !connected )
    return;

  MSG_DEBUG("index_server", "Closing connection");
  first_server = server_lst.end();
  current_server = server_lst.end();

  SDLNet_TCP_DelSocket(sock_set, socket);
  SDLNet_TCP_Close(socket);
  connected = false;
  SDLNet_FreeSocketSet(sock_set);
}

static ssize_t getline(std::string& line, std::ifstream& file)
{
  line.clear();
  std::getline(file, line);
  if(file.eof())
    return -1;
  return line.size();
}

bool IndexServer::GetServerList()
{
  MSG_DEBUG("index_server", "Retrieving server list");
  // If we already have it, no need to redownload it
  if(server_lst.size() != 0)
    return true;

  // Download the list of user
  const std::string server_file = Config::GetInstance()->GetPersonalDir() + "server_list";

  if( !Downloader::GetInstance()->Get(server_list_url.c_str(), server_file.c_str()) )
    return false;

  // Parse the file
  std::ifstream fin;
  fin.open(server_file.c_str(), std::ios::in);
  if(!fin)
          return false;

  /*char * line = NULL;
  size_t len = 0;*/
  std::string line;

  // GNU getline isn't available on *BSD and Win32, so we use a new function, see getline above
  while (getline(line, fin) >= 0)
  {
    if(line.at(0) == '#' || line.at(0) == '\n' || line.at(0) == '\0')
      continue;

    std::string::size_type port_pos = line.find(':', 0);
    if(port_pos == std::string::npos)
      continue;

    std::string hostname = line.substr(0, port_pos);
    std::string portstr = line.substr(port_pos+1);
    int port = atoi(portstr.c_str());

    server_lst[ hostname ] = port;
  }

  fin.close();

  first_server = server_lst.end();
  current_server = server_lst.end();
  MSG_DEBUG("index_server", "Server list retrieved. %i servers are running", server_lst.size());

  return (server_lst.size() != 0);
}

bool IndexServer::GetServerAddress( std::string & address, int & port)
{
  MSG_DEBUG("index_server", "Trying a new server");
  // Cycle through the server list to find the first one
  // accepting connection
  if( first_server == server_lst.end() )
  {
    // First try :
    // Randomly select a server in the list
    int nbr = randomObj.GetLong( 0, server_lst.size()-1 );
    first_server = server_lst.begin();
    while(nbr--)
      ++first_server;

    ASSERT(first_server != server_lst.end());

    current_server = first_server;

    address = current_server->first;
    port = current_server->second;
    return true;
  }

  ++current_server;
  if( current_server == server_lst.end() )
    current_server = server_lst.begin();

  address = current_server->first;
  port = current_server->second;

  return (current_server != first_server);
}

/*************  Basic transmissions  ******************/
void IndexServer::Send(const int& nbr)
{
  char packet[4];
  // this is not cute, but we don't want an int -> uint conversion here
  Uint32 u_nbr = *((const Uint32*)&nbr);

  SDLNet_Write32(u_nbr, packet);
  SDLNet_TCP_Send(socket, packet, sizeof(packet));
}

void IndexServer::Send(const std::string &str)
{
  Send(str.size());
  SDLNet_TCP_Send(socket, (void*)str.c_str(), str.size());
}

int IndexServer::ReceiveInt()
{
  char packet[4];
  //somehow we can get here while being disconnected... this should not be
  if(!connected)
    return -1;
  if(SDLNet_CheckSockets(sock_set, 5000) == 0)
    return -1;

  if(!SDLNet_SocketReady(socket))
    return -1;

  if( SDLNet_TCP_Recv(socket, packet, sizeof(packet)) < 1 )
  {
    Disconnect();
    return 0;
  }

  Uint32 u_nbr = SDLNet_Read32(packet);
  int nbr = *((int*)&u_nbr);
  return nbr;
}

std::string IndexServer::ReceiveStr()
{
  if(!connected)
    return "";

  int size = ReceiveInt();
  if(size <= 0)
    return "";

  if(SDLNet_CheckSockets(sock_set, 5000) == 0)
    return "";

  if(!SDLNet_SocketReady(socket))
    return "";

  char* str = new char[size+1];
  if( SDLNet_TCP_Recv(socket, str, size) < 1 )
  {
    delete[] str;
    Disconnect();
    return "";
  }

  str[size] = '\0';

  std::string st(str);
  delete []str;
  return st;
}

bool IndexServer::HandShake()
{
  Send(TS_MSG_VERSION);
  Send(Constants::VERSION);

  int msg = ReceiveInt();
  if(msg == -1)
    return false;
  std::string sign;

  if(msg == TS_MSG_VERSION)
    sign = ReceiveStr();

  if(msg != TS_MSG_VERSION || sign != "MassMurder!")
  {
    Disconnect();
    return false;
  }
  return true;
}

void IndexServer::SendServerStatus()
{
  ASSERT(Network::GetInstance()->IsServer());

  if(hidden_server)
    return;
  Send(TS_MSG_HOSTING);
  Send(Network::GetInstance()->GetPort());
}

std::list<address_pair> IndexServer::GetHostList()
{
  Send(TS_MSG_GET_LIST);
  int lst_size = ReceiveInt();
  std::list<address_pair> lst;
  if(lst_size == -1)
    return lst;
  while(lst_size--)
  {
    IPaddress ip;
    ip.host = ReceiveInt();
    ip.port = ReceiveInt();
    const char* addr = SDLNet_ResolveIP(&ip);
    char port[10];
    sprintf(port, "%d", ip.port);

    address_pair addr_pair;
    addr_pair.second = std::string(port);

    if(addr == NULL)
    {
      // We can't resolve the hostname, so just show the ip address
      unsigned char* str_ip = (unsigned char*)&ip.host;
      char formated_ip[16];
      snprintf(formated_ip, 16, "%i.%i.%i.%i", (int)str_ip[0],
                                           (int)str_ip[1],
                                           (int)str_ip[2],
                                           (int)str_ip[3]);
      addr_pair.first = std::string(formated_ip);
    }
    else
      addr_pair.first = std::string(addr);
    lst.push_back(addr_pair);
  }
  return lst;
}

void IndexServer::Refresh()
{
  if(!connected)
    return;

  if(SDLNet_CheckSockets(sock_set, 100) == 0)
    return;

  if(!SDLNet_SocketReady(socket))
    return;

  int msg_id = ReceiveInt();
  if(msg_id == -1)
    return;

  if( msg_id == TS_MSG_PING )
    Send(TS_MSG_PONG);
  else
    Disconnect();
}
