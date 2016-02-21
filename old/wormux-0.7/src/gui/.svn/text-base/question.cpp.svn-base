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

#include "question.h"
#include <SDL.h>
#include "../graphic/text.h"
#include "../graphic/font.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../map/map.h"

Question::Question()
{}

void Question::TraiteTouche (SDL_Event &event){
  // Teste les différents choix
  choix_iterator it=choix.begin(), fin=choix.end();
  for (; it != fin; ++it){
    if (event.key.keysym.sym == it -> m_touche){
      reponse = it -> m_val;
      m_fin_boucle = true;
      return;
    }
  }

  // Sinon, on utilise le choix par défaut ?
  if (choix_defaut.actif){
    reponse = choix_defaut.valeur;
    m_fin_boucle = true;
    return;
  }
}

void Question::Draw(){
  AppWormux * app = AppWormux::GetInstance();
  
  DrawTmpBoxTextWithReturns (*Font::GetInstance(Font::FONT_BIG),
                             app->video.window.GetSize() / 2,
                             message, 10);
  app->video.Flip();
}

int Question::PoseQuestion (){
  SDL_Event event;
  
  m_fin_boucle = false;
  do{
    Draw();

    while( SDL_PollEvent( &event) ){      
      if ( event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN ){  
        reponse = choix_defaut.valeur;
        m_fin_boucle = true;
      }
      if (event.type == SDL_KEYUP)
        TraiteTouche(event); 
    }
  } while (!m_fin_boucle);
  
  return reponse;
}

void Question::Init (const std::string &pmessage, 
		     bool pchoix_defaut, int pvaleur){
  message = pmessage;
  choix_defaut.actif = pchoix_defaut;
  choix_defaut.valeur = pvaleur;
}

