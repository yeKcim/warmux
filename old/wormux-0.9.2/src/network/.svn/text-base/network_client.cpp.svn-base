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
 ******************************************************************************
 * Network client layer for Wormux.
 *****************************************************************************/
#include <WORMUX_socket.h>
#include <SDL_thread.h>
#include <WORMUX_debug.h>

//-----------------------------------------------------------------------------
#include "include/action_handler.h"
#include "include/app.h"
#include "include/constant.h"
#include "network/network_client.h"
#include "network/net_error_msg.h"
#include "tool/string_tools.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif
//-----------------------------------------------------------------------------

NetworkClient::NetworkClient(const std::string& password) : Network("-", password)
{
#ifdef LOG_NETWORK
  fin = open("./network_client.in", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
  fout = open("./network_client.out", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
#endif
}

NetworkClient::~NetworkClient()
{
}

void NetworkClient::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  printf("- client disconnected: %s\n", (*closed)->ToString().c_str());

  RemoveRemoteHost(closed);
}

void NetworkClient::HandleAction(Action* a, DistantComputer* /*sender*/)
{
  ActionHandler::GetInstance()->NewAction(a, false);
}

//-----------------------------------------------------------------------------

connection_state_t NetworkClient::HandShake(WSocket& server_socket)
{
  int ack;
  int player_id;
  connection_state_t ret = CONN_REJECTED;
  std::string version;
  std::string game_name;

  MSG_DEBUG("network", "Client: Handshake !");

  // Adding the socket to a temporary socket set
  if (!server_socket.AddToTmpSocketSet())
    goto error_no_socket_set;

  // 1) Send the version number
  MSG_DEBUG("network", "Client: sending version number");

  if (!server_socket.SendStr(Constants::WORMUX_VERSION))
    goto error;

  // is it ok ?
  if (!server_socket.ReceiveStr(version, 40))
    goto error;

  MSG_DEBUG("network", "Client: server version number is %s", version.c_str());

  if (Constants::WORMUX_VERSION != version) {
    std::string str = Format(_("The client and server versions are incompatible "
			       "(local=%s, server=%s). Please try another server."),
			     Constants::WORMUX_VERSION.c_str(), version.c_str());
    AppWormux::DisplayError(str);
    goto error;
  }

  // 2) Send the password

  MSG_DEBUG("network", "Client: sending password");
  if (!server_socket.SendStr(GetPassword()))
    goto error;

  // is it ok ?
  if (!server_socket.ReceiveInt(ack))
    goto error;

  if (ack) {
    ret = CONN_WRONG_PASSWORD;
    goto error;
  }

  // 3) Am I the game master ?
  if (!server_socket.ReceiveInt(ack))
    goto error;

  if (ack) {
    game_master_player = true;
    MSG_DEBUG("network", "Client will be master! (%d)", ack);
  }

  // 4) Send my nickname
  if (!server_socket.SendStr(GetPlayer().GetNickname()))
    goto error;

  // 5) Receive the game name
  if (!server_socket.ReceiveStr(game_name, 100))
    goto error;

  // 6) Receive my player id
  if (!server_socket.ReceiveInt(player_id))
    goto error;

  GetPlayer().SetId(uint(player_id));

  SetGameName(game_name);

  MSG_DEBUG("network", "Client: Handshake done successfully :)");
  ret = CONNECTED;

 error:
  server_socket.RemoveFromTmpSocketSet();

 error_no_socket_set:
  if (ret != CONNECTED)
    std::cerr << "Client: HandShake with server has failed!" << std::endl;
  return ret;
}

connection_state_t
NetworkClient::ClientConnect(const std::string &host, const std::string& port)
{
  connection_state_t r;
  WSocket* socket;
  DistantComputer* server;
  int prt;

  MSG_DEBUG("network", "Client connect to %s:%s", host.c_str(), port.c_str());

  if (!str2int(port, prt)) {
    r = CONN_BAD_PORT;
    goto err_no_socket;
  }

  socket = new WSocket();
  r = socket->ConnectTo(host, prt);
  if (r != CONNECTED)
    goto error;

  r = HandShake(*socket);
  if (r != CONNECTED)
    goto error;

  socket_set = WSocketSet::GetSocketSet(1);
  if (!socket_set) {
    r = CONN_REJECTED;
    goto error;
  }
  socket_set->AddSocket(socket);
  server = new DistantComputer(socket);

  AddRemoteHost(server);

  NetworkThread::Start();
  return CONNECTED;

 error:
  delete socket;
 err_no_socket:
  return r;
}

std::string NetworkClient::GetServerAddress() const
{
  std::list<WSocket*>& sockets = socket_set->GetSockets();
  if (sockets.size() != 1)
    return "??";

  return (*sockets.begin())->GetAddress();
}
