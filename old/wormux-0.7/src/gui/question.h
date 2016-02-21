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
 * Affiche un message dans le jeu, puis pose une question dans le jeu ou
 * attend au moins la pression d'une touche.
 *****************************************************************************/

#ifndef QUESTION_H
#define QUESTION_H
//-----------------------------------------------------------------------------
#include "../include/base.h"
#include <string>
#include <list>
#include <SDL_events.h>

//-----------------------------------------------------------------------------

class Question
{
public:
  // Message affiché durant le jeu
  std::string message;

  // Un choix = une touche renvoie une valeur
  typedef struct choix_t
  {
    int m_touche;
    int m_val;
    choix_t (int touche, int valeur) 
    { m_touche = touche; m_val = valeur; }
  } choix_t;

  // Liste des choix
  std::list<choix_t> choix;
  typedef std::list<choix_t>::iterator choix_iterator;

  // Choix par défaut : si une autre touche est pressée
  struct s_choix_defaut
  {
    bool actif;
    int valeur;
  } choix_defaut;

  // Réponse renvoyée par l'utilisateur
  int reponse;

private:
  void TraiteTouche (SDL_Event &event);
  bool m_fin_boucle;

private:
  void Draw();

 public:
  Question();
  void Init (const std::string &message, bool choix_defaut, int valeur);
  int PoseQuestion ();
};

//-----------------------------------------------------------------------------
#endif
