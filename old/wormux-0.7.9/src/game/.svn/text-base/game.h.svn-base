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
 * Classe principale qui gère le jeu : initialisation, dessin, gestion
 * des différents composants, et boucle de jeu.
 *****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "../include/base.h"
#include "../gui/question.h"

class Game
{
private:
  bool isGameLaunched;
  bool endOfGameStatus;
  bool isGamePaused;

  int NbrRemainingTeams() const;

  Game();
  static Game * singleton;
  
  int AskQuestion (Question &question, bool draw=true);
  void DisplayPause();
  bool DisplayQuit();

public:
  static Game * GetInstance();

  void Start();
  void UnloadDatas();

  bool IsGameFinished() const;
  bool IsGamePaused() const;
  bool IsGameLaunched() const;

  void MessageLoading() const;
  void MessageEndOfGame() const;

  void Pause();

  bool GetEndOfGameStatus() const;
  void SetEndOfGameStatus(bool status);
};
#endif
