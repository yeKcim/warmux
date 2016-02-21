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
 * Display a text during the game, waiting for input by the user
 *****************************************************************************/

#include "question.h"
#include <SDL.h>
#include "../graphic/text.h"
#include "../graphic/font.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../interface/interface.h"
#include "../map/map.h"
#include "../tool/resource_manager.h"

Question::Question()
{
  background = NULL;
}

Question::~Question()
{
  if(background != NULL)
    delete background;
}

int Question::TreatsKey (SDL_Event &event){

  // Tests the key
  choice_iterator it=choices.begin(), end=choices.end();
  for (; it != end; ++it){
    if (event.key.keysym.sym == it -> key())
      return it -> val();
  }

  // No key corresponding to the correct choice, so we use default choice
  if (default_choice.active)
    return default_choice.value;

  return -1;
}

void Question::Draw(){
  AppWormux * app = AppWormux::GetInstance();

  if(background != NULL)
  {
    background->Blit(app->video.window,  app->video.window.GetSize() / 2 - background->GetSize() / 2);
  }
  if(message != "")
  {
    DrawTmpBoxTextWithReturns (*Font::GetInstance(Font::FONT_BIG),
                               app->video.window.GetSize() / 2,
                               message, 10);
  }
}

int Question::Ask () {
  SDL_Event event;

  int answer = default_choice.value;
  bool end_of_boucle = false;

  Draw();
  do{

    while( SDL_PollEvent( &event) ){
      if ( (event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN) &&
          default_choice.active ){
	answer = default_choice.value;
	end_of_boucle = true;
      }

      if (event.type == SDL_KEYUP) {
	answer = TreatsKey(event);
	if (answer != -1)
	  end_of_boucle = true;
      }
    } // SDL_PollEvent

    AppWormux::GetInstance()->video.Flip();
  } while (!end_of_boucle);

  return answer;
}

void Question::Set (const std::string &pmessage,
		    bool default_active, int default_value,const std::string& bg_sprite){
  message = pmessage;
  default_choice.active = default_active;
  default_choice.value = default_value;

  if(background != NULL)
  {
    delete background;
    background = NULL;
  }

  if(bg_sprite != "")
  {
    Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
    background = new Sprite(resource_manager.LoadImage(res,bg_sprite));
    background->cache.EnableLastFrameCache();
    background->ScaleSize(AppWormux::GetInstance()->video.window.GetSize());
    resource_manager.UnLoadXMLProfile( res);
  }
}
