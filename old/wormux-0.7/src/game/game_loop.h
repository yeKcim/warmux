/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
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
 * Boucle de jeu : dessin et gestion des données.
 *****************************************************************************/

#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "../graphic/fps.h"
#include "../include/base.h"
#include "../team/character.h"

class GameLoop
{
private:
  int state;
  uint pause_seconde;
  uint duration;

public:
  static const int PLAYING = 0;
  static const int HAS_PLAYED = 1;
  static const int END_TURN = 2;

  FramePerSecond fps;
  
  static GameLoop * singleton;

public:
  static GameLoop * GetInstance();

  void Init();
  
  bool character_already_chosen;
  bool interaction_enabled;

  // Dessin du jeu au complet.
  void Draw();

  // La boucle principale du jeu
  void Run();

  // Refresh du jeu en entier !
  void Refresh();

  // Read/Set State
  int ReadState() const { return state; }
  void SetState(int new_state, bool begin_game=false);

  // Signal death of a player
  void SignalCharacterDeath (Character *character);

  // Signal fall of a player
  void SignalCharacterDamageFalling (Character *character);

private:
  GameLoop();

  void GameLoop::InitGameData_NetServer();
  void GameLoop::InitGameData_NetClient();
  void GameLoop::InitData_Local();
  void GameLoop::InitData();
    
  void RefreshClock();
  void CallDraw();

  PhysicalObj* GetMovingObject();
  bool IsAnythingMoving();
};
#endif
