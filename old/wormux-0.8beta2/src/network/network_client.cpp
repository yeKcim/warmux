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
 * Network client layer for Wormux.
 *****************************************************************************/

#include "network_client.h"
//-----------------------------------------------------------------------------
#include <SDL_thread.h>
#include "include/action_handler.h"
#include "game/game_mode.h"
#include "tool/debug.h"
#include "distant_cpu.h"

#include <sys/types.h>
#ifdef LOG_NETWORK
#  include <sys/stat.h>
#  include <fcntl.h>
#  ifdef WIN32
#    include <io.h>
#  endif
#endif
//-----------------------------------------------------------------------------

NetworkClient::NetworkClient()
{
#ifdef LOG_NETWORK
  fin = open("./network_client.in", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
  fout = open("./network_client.out", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP);
#endif
}

NetworkClient::~NetworkClient()
{
}

void NetworkClient::SendChatMessage(const std::string& txt)
{
  if (txt == "") return;

  Action a(Action::ACTION_CHAT_MESSAGE, txt);
  SendAction(&a);
}

std::list<DistantComputer*>::iterator NetworkClient::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  printf("Client disconnected\n");
  delete *closed;

  return cpu.erase(closed);
}

void NetworkClient::ReceiveActions()
{
  char* packet;

  while (cpu.size()==1 && ThreadToContinue()) // While connected to server
  {
    int num_ready;

    if (state == NETWORK_PLAYING && cpu.size() == 0)
    {
      // If while playing everybody disconnected, just quit
      break;
    }

    //Loop while nothing is received
    while (ThreadToContinue())
    {
      num_ready = SDLNet_CheckSockets(socket_set, 100);
      // Means something is available
      if (num_ready>0)
        break;
      // Means an error
      else if (num_ready == -1)
      {
        fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
      }
      // Means timeout, so continue
    }

    std::list<DistantComputer*>::iterator dst_cpu;
    for (dst_cpu = cpu.begin();
         dst_cpu != cpu.end() && ThreadToContinue();
         dst_cpu++)
    {
      if((*dst_cpu)->SocketReady()) // Check if this socket contains data to receive
      {
        // Read the size of the packet
        int packet_size = (*dst_cpu)->ReceiveDatas(packet);
        if( packet_size == -1) // An error occured during the reception
        {
          dst_cpu = CloseConnection(dst_cpu);
          continue;
        } else
        if( packet_size == 0) // We didn't received the full packet yet
          continue;

#ifdef LOG_NETWORK
        if (fin != 0) {
          int tmp = 0xFFFFFFFF;
          write(fin, &packet_size, 4);
          write(fin, packet, packet_size);
          write(fin, &tmp, 4);
        }
#endif

        Action* a = new Action(packet, (*dst_cpu));
        MSG_DEBUG("network.traffic","Received action %s",
                  ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());

        switch (a->GetType()) {
        case Action::ACTION_NICKNAME:
          {
            std::string nickname = a->PopString();
            std::cout<<"New nickname: " + nickname<< std::endl;
            (*dst_cpu)->nickname = nickname;
            delete a;
          }
          break;

        case Action::ACTION_MENU_ADD_TEAM:
        case Action::ACTION_MENU_DEL_TEAM:
          (*dst_cpu)->ManageTeam(a);
          delete a;
          break;

        case Action::ACTION_CHAT_MESSAGE:
          (*dst_cpu)->SendChatMessage(a);
          delete a;
          break;

        default:
          ActionHandler::GetInstance()->NewAction(a, false);
        }
        free(packet);
      }
    }
  }
}

//-----------------------------------------------------------------------------
const Network::connection_state_t
NetworkClient::ClientConnect(const std::string &host, const std::string& port)
{
  Init();

  MSG_DEBUG("network", "Client connect to %s:%s", host.c_str(), port.c_str());

  int prt = strtol(port.c_str(), NULL, 10);

  connection_state_t r = CheckHost(host, prt);
  if (r != Network::CONNECTED)
    return r;

  if (SDLNet_ResolveHost(&ip,host.c_str(),(Uint16)prt) == -1)
  {
    fprintf(stderr, "SDLNet_ResolveHost: %s to %s:%i\n", SDLNet_GetError(), host.c_str(), prt);
    return Network::CONN_BAD_HOST;
  }

  // CheckHost opens and closes a connection to the server, so before reconnecting
  // wait a bit, so the connection really gets closed ..
  SDL_Delay(500);

  TCPsocket socket = SDLNet_TCP_Open(&ip);

  if (!socket)
  {
    fprintf(stderr, "SDLNet_TCP_Open: %s to%s:%i\n", SDLNet_GetError(), host.c_str(), prt);
    return Network::CONN_REJECTED;
  }

  socket_set = SDLNet_AllocSocketSet(1);
  cpu.push_back(new DistantComputer(socket));
  //Send nickname to server
  Action a(Action::ACTION_NICKNAME, nickname);
  SendAction(&a);

  //Control to net_thread_func
  thread = SDL_CreateThread(Network::ThreadRun, NULL);
  return Network::CONNECTED;
}
