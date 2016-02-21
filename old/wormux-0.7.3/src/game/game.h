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
 * Classe principale qui gère le jeu : initialisation, dessin, gestion
 * des différents composants, et boucle de jeu.
 *****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "../gui/question.h"
#include "../include/base.h"

class Game
{
private:
  bool isGameLaunched;
  bool endOfGameStatus;
  Question question;

  int NbrRemainingTeams();

  Game();
  static Game * singleton;
  
public:
  static Game * GetInstance();

  void Start();
  
  bool IsGameFinished();
  bool IsGameLaunched() const;

  void MessageLoading();
  void MessageEndOfGame();

  int AskQuestion (bool draw=true);
  
  void Pause();

  bool GetEndOfGameStatus();
  void SetEndOfGameStatus(bool status);
};
#endif
