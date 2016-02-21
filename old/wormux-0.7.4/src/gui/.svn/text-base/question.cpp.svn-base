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
 * Display a text during the game, waiting for input by the user
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

int Question::TreatsKey (SDL_Event &event){

  // Tests the key
  choice_iterator it=choices.begin(), end=choices.end();
  for (; it != end; ++it){
    if (event.key.keysym.sym == it -> m_key)
      return it -> m_val;
  }

  // No key corresponding to the correct choice, so we use default choice
  if (default_choice.active)
    return default_choice.value;
  
  return -1;
}

void Question::Draw(){
  AppWormux * app = AppWormux::GetInstance();
  
  DrawTmpBoxTextWithReturns (*Font::GetInstance(Font::FONT_BIG),
                             app->video.window.GetSize() / 2,
                             message, 10);
  app->video.Flip();
}

int Question::AskQuestion (){
  SDL_Event event;

  int answer;
  bool end_of_boucle = false;

  do{
    Draw();

    while( SDL_PollEvent( &event) ){      
      if (( event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN ) && default_choice.active ){
	answer = default_choice.value;
	end_of_boucle = true;
      }

      if (event.type == SDL_KEYUP) {
	answer = TreatsKey(event);
	if (answer != -1) 
	  end_of_boucle = true;
      }
    } // SDL_PollEvent

  } while (!end_of_boucle);
  
  return answer;
}

void Question::Set (const std::string &pmessage, 
		    bool default_active, int default_value){
  message = pmessage;
  default_choice.active = default_active;
  default_choice.value = default_value;
}

