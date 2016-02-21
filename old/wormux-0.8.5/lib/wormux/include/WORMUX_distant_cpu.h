/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2009 Wormux Team.
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
 * Handle distant computers
 *****************************************************************************/

#ifndef DISTANT_CPU_H
#define DISTANT_CPU_H
//-----------------------------------------------------------------------------
#include <list>
#include <string>
#include <WORMUX_player.h>
#include <WORMUX_types.h>
//-----------------------------------------------------------------------------

class WSocket;

class DistantComputer
{
public:
  typedef enum {
    STATE_ERROR,
    STATE_NOT_INITIALIZED,
    STATE_INITIALIZED,
    STATE_READY,
    STATE_CHECKED,
    STATE_NEXT_GAME
  } state_t;

private:
  /* If you need this, implement it (correctly)*/
  DistantComputer(const DistantComputer&);
  const DistantComputer& operator=(const DistantComputer&);
  /*********************************************/

  WSocket* sock;
  DistantComputer::state_t state;
  uint game_id;

  // a remote computer may act as a relay for several players (this is true when it is a server)
  std::list<Player> players;

public:
  DistantComputer(WSocket* new_sock, const std::string& nickname, uint initial_player_id);
  DistantComputer(WSocket* new_sock, const std::string& nickname, uint game_id, uint initial_player_id);
  ~DistantComputer();

  bool SocketReady() const;

  // ReceiveData may return true with *data = NULL and len = 0
  // That means that client is still valid BUT there are not enough data CURRENTLY
  bool ReceiveData(char** data, size_t& len);
  bool SendData(const char* data, size_t len);

  std::string GetAddress() const;
  std::string GetNicknames() const;

  void AddPlayer(uint player_id);
  void DelPlayer(uint player_id);

  Player* GetPlayer(uint player_id);
  const std::list<Player>& GetPlayers() const;

  void SetState(DistantComputer::state_t _state);
  DistantComputer::state_t GetState() const;

  uint GetGameId() const;

  void ForceDisconnection();
  bool MustBeDisconnected();

  const std::string ToString() const;
};

// It's up to the program using class DistantComputer to define WORMUX_[Dis]connectHost();
extern void WORMUX_ConnectHost(DistantComputer& cpu);
extern void WORMUX_DisconnectHost(DistantComputer& cpu);

#endif

