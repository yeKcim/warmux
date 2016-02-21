/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 * Blitz Mode
 *****************************************************************************/

#ifndef GAME_BLITZ_H
#define GAME_BLITZ_H

#include <map>
#include "game/game.h"

class Team;

class GameBlitz : public Game
{
public:
  typedef std::map<uint, uint>::iterator  time_iterator;
  uint    counter;

  GameBlitz();

  // Overload it to retrieve teams.
  bool Run();

  // Get remaining time to play
  uint GetRemainingTime() const;
  bool IsGameFinished() const;
  void EndOfGame();

private:
  std::map<uint, uint> times;

  void RefreshClock();
  void __SetState_PLAYING();
  void __SetState_HAS_PLAYED();
  void __SetState_END_TURN();

  time_iterator GetCurrentTeam();
  time_iterator KillGroup(time_iterator cur);
};

#endif // GAME_BLITZ_H
