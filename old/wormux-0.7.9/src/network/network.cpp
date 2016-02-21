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
 ******************************************************************************
 * Network layer for Wormux.
 *****************************************************************************/

#include "network.h"
//-----------------------------------------------------------------------------
#include <SDL_net.h>
#include <SDL_thread.h>
#include "../game/game_mode.h"
#include "../game/game.h"
#include "../gui/question.h"
#include "../include/action_handler.h"
#include "../tool/debug.h"
#include "../tool/i18n.h"
#include "distant_cpu.h"

#if defined(DEBUG) && not defined(WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
//-----------------------------------------------------------------------------
Network network;

//-----------------------------------------------------------------------------

Network::Network()
{
  max_player_number = 0;
  m_is_connected = false;
  m_is_server = false;
  m_is_client = false;
  state = NETWORK_NOT_CONNECTED;
  inited = false;
  sync_lock = false;
  network_menu = NULL;

  //Set nickname
#ifdef WIN32
  nickname = getenv("USERNAME");
#else
  nickname = getenv("USER");
#endif
}

//-----------------------------------------------------------------------------
int net_thread_func(void* no_param)
{
  network.ReceiveActions();
  network.Disconnect();
  return 1;
}

//-----------------------------------------------------------------------------

void Network::Init()
{
  if(inited) return;
  if (SDLNet_Init()) {
      Error(_("Failed to initialize network library!"));
  }
  inited = true;
  max_player_number = GameMode::GetInstance()->max_teams;
  connected_player = 0;

#if defined(DEBUG) && not defined(WIN32)
  fin = open("./network.in", O_RDWR | O_CREAT | O_SYNC, S_IRWXU | S_IRWXG);
  fout = open("./network.out", O_RDWR | O_CREAT | O_SYNC, S_IRWXU | S_IRWXG);
#endif
}

//-----------------------------------------------------------------------------

Network::~Network()
{
  Disconnect();
  if(inited)
  {
    SDLNet_Quit();
#if defined(DEBUG) && not defined(WIN32)
    close(fin);
    close(fout);
#endif
  }
}

//-----------------------------------------------------------------------------

void Network::Disconnect()
{
  if(!m_is_connected) return;

  m_is_connected = false; // To make the threads terminate

  SDL_WaitThread(thread,NULL);
  printf("Network thread finished\n");
  for(std::list<DistantComputer*>::iterator client = cpu.begin();
      client != cpu.end();
      client++)
  {
    delete *client;
  }
  cpu.clear();
  SDLNet_FreeSocketSet(socket_set);

  if(m_is_server)
    SDLNet_TCP_Close(server_socket);

  m_is_server = false;
  m_is_client = false;
}

//-----------------------------------------------------------------------------
//----------------       Client specific methods   ----------------------------
//-----------------------------------------------------------------------------
void Network::ClientConnect(const std::string &host, const std::string& port)
{
  MSG_DEBUG("network", "Client connect to %s:%s", host.c_str(), port.c_str());

  int prt=0;
  sscanf(port.c_str(),"%i",&prt);

  if(SDLNet_ResolveHost(&ip,host.c_str(),(Uint16)prt)==-1)
  {
    Question question;
    question.Set(_("Invalid server adress!"),1,0);
    question.Ask();
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return;
  }

  TCPsocket socket = SDLNet_TCP_Open(&ip);

  if(!socket)
  {
    Question question;
    question.Set(_("Unable to contact server!"),1,0);
    question.Ask();
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return;
  }

  m_is_client = true;
  m_is_server = false;
  state = NETWORK_OPTION_SCREEN;
  m_is_connected = true;

  socket_set = SDLNet_AllocSocketSet(1);
  connected_player = 1;
  cpu.push_back(new DistantComputer(socket));
  //Send nickname to server
  Action a(Action::ACTION_NICKNAME, nickname);
  network.SendAction(&a);

  //Control to net_thread_func
  thread = SDL_CreateThread(net_thread_func,NULL);
}

//-----------------------------------------------------------------------------
//----------------       Server specific methods   ----------------------------
//-----------------------------------------------------------------------------

void Network::ServerStart(const std::string &port)
{
  // The server starts listening for clients
  MSG_DEBUG("network", "Start server on port %s", port.c_str());

  cpu.clear();
  // Convert port number (std::string port) into SDL port number format:
  int prt;
  sscanf(port.c_str(),"%i",&prt);

  if(SDLNet_ResolveHost(&ip,NULL,(Uint16)prt)!=0)
  {
    Question question;
    question.Set(_("Invalid port!"),1,0);
    question.Ask();
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return;
  }

  m_is_server = true;
  m_is_client = false;
  m_is_connected = true;

  // Open the port to listen to
  state = NETWORK_OPTION_SCREEN;
  AcceptIncoming();
  connected_player = 1;
  printf("\nConnected\n");
  socket_set = SDLNet_AllocSocketSet(GameMode::GetInstance()->max_teams);
  thread = SDL_CreateThread(net_thread_func,NULL);
}

std::list<DistantComputer*>::iterator Network::CloseConnection(std::list<DistantComputer*>::iterator closed)
{
  printf("Client disconnected\n");
  delete *closed;
  if(m_is_server && connected_player == max_player_number)
  {
    // A new player will be able to connect, so we reopen the socket
    // For incoming connections
    printf("Allowing new connections\n");
    AcceptIncoming();
  }

  connected_player--;
  return cpu.erase(closed);
}

void Network::AcceptIncoming()
{
  assert(m_is_server);
  if(state != NETWORK_OPTION_SCREEN) return;

  server_socket = SDLNet_TCP_Open(&ip);
  if(!server_socket)
  {
    Question question;
    question.Set(_("Unable to listen for client!"),1,0);
    question.Ask();
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return;
  }
  printf("\nStart listening");
}

void Network::RejectIncoming()
{
  assert(m_is_server);
  if(!server_socket) return;
  SDLNet_TCP_Close(server_socket);
  server_socket = NULL;
  printf("\nStop listening");
}

//-----------------------------------------------------------------------------
//----------------       Action handling methods   ----------------------------
//-----------------------------------------------------------------------------
void Network::ReceiveActions()
{
  char* packet;

  while(m_is_connected && (cpu.size()==1 || m_is_server))
  {
    if(state == NETWORK_PLAYING && cpu.size() == 0)
    {
      // If while playing everybody disconnected, just quit
      break;
    }

    while(SDLNet_CheckSockets(socket_set, 100) == 0 && m_is_connected) //Loop while nothing is received
    if(m_is_server && server_socket)
    {
      // Check for an incoming connection
      TCPsocket incoming;
      incoming = SDLNet_TCP_Accept(server_socket);
      if(incoming)
      {
        cpu.push_back(new DistantComputer(incoming));
        connected_player++;
        printf("New client connected\n");
        if(connected_player >= max_player_number)
          RejectIncoming();
        ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_ASK_VERSION));
      }
    }

    std::list<DistantComputer*>::iterator dst_cpu = cpu.begin();
    while(dst_cpu != cpu.end() && m_is_connected)
    {
      if((*dst_cpu)->SocketReady()) // Check if this socket contains data to receive
      {
        // Read the size of the packet
        int packet_size = (*dst_cpu)->ReceiveDatas(packet);
        if( packet_size <= 0)
        {
          dst_cpu = CloseConnection(dst_cpu);
          continue;
        }

#if defined(DEBUG) && not defined(WIN32)
	int tmp = 0xFFFFFFFF;
	write(fin, &packet_size, 4);
	write(fin, packet, packet_size);
	write(fin, &tmp, 4);
#endif

        Action* a = new Action(packet);
        MSG_DEBUG("network.traffic","Received action %s",
                ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());

	//Add relation between nickname and socket
	if( a->GetType() == Action::ACTION_NICKNAME){
	  std::string nickname = a->PopString();
	  std::cout<<"New nickname: " + nickname<< std::endl;
	  (*dst_cpu)->nickname = nickname;
	  delete a;
	  break;
	}

        if( a->GetType() == Action::ACTION_NEW_TEAM
        ||  a->GetType() == Action::ACTION_DEL_TEAM)
        {
          (*dst_cpu)->ManageTeam(a);
          delete a;
        }
        else
        if(a->GetType() == Action::ACTION_CHAT_MESSAGE)
        {
          (*dst_cpu)->SendChatMessage(a);
          delete a;
        }
        else
        {
          ActionHandler::GetInstance()->NewAction(a, false);
        }

        // Repeat the packet to other clients:
        if(a->GetType() != Action::ACTION_SEND_VERSION
        && a->GetType() != Action::ACTION_CHANGE_STATE
        && a->GetType() != Action::ACTION_CHAT_MESSAGE)
        for(std::list<DistantComputer*>::iterator client = cpu.begin();
            client != cpu.end();
            client++)
        if(client != dst_cpu)
        {
          (*client)->SendDatas(packet, packet_size);
        }
        free(packet);
      }
      dst_cpu++;
    }
  }
  Game::GetInstance()->SetEndOfGameStatus( true );
}

// Send Messages
void Network::SendAction(Action* a)
{
  if (!m_is_connected) return;

  MSG_DEBUG("network.traffic","Send action %s",
        ActionHandler::GetInstance()->GetActionName(a->GetType()).c_str());

  int size;
  char* packet;
  a->WritePacket(packet, size);

  assert(*((int*)packet) != 0 );
  SendPacket(packet, size);

  free(packet);
}

void Network::SendPacket(char* packet, int size)
{
#if defined(DEBUG) && not defined(WIN32)
	int tmp = 0xFFFFFFFF;
	write(fout, &size, 4);
	write(fout, packet, size);
	write(fout, &tmp, 4);
#endif
  for(std::list<DistantComputer*>::iterator client = cpu.begin();
      client != cpu.end();
      client++)
  {
    (*client)->SendDatas(packet, size);
  }
}

void Network::SendChatMessage(std::string txt)
{
  if(IsServer())
  {
    ActionHandler::GetInstance()->NewAction(new Action(Action::ACTION_CHAT_MESSAGE, nickname + std::string("> ") + txt));
  }
  else
  {
    Action a(Action::ACTION_CHAT_MESSAGE, txt);
    network.SendAction(&a);
  }
}

//-----------------------------------------------------------------------------

const bool Network::IsConnected() const { return m_is_connected; }
const bool Network::IsLocal() const { return !m_is_server && !m_is_client; }
const bool Network::IsServer() const { return m_is_server; }
const bool Network::IsClient() const { return m_is_client; }

const uint Network::GetPort()
{
  Uint16 prt;
  prt = SDLNet_Read16(&ip.port);
  return (uint)prt;
}

//-----------------------------------------------------------------------------
