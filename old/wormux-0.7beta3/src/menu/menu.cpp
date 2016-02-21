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
 * Generic menu
 *****************************************************************************/

#include "menu.h"
//-----------------------------------------------------------------------------

#include "../graphic/sprite.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../tool/resource_manager.h"
#include "../sound/jukebox.h"

//-----------------------------------------------------------------------------

Menu::Menu(char * bg)
{
  close_menu = false ;
   
  uint x = (video.GetWidth()/2);
  uint y = video.GetHeight()-50;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  background = new Sprite( resource_manager.LoadImage( res, bg));

  //-----------------------------------------------------------------------------
  // Widget creation
  //-----------------------------------------------------------------------------

  /* actions buttons */
  actions_buttons = new HBox(x, y, 40, false);

  b_ok = new Button(0, 0, res, "menu/valider"); 
  actions_buttons->AddWidget(b_ok);

  b_cancel = new Button(0, 0, res, "menu/annuler");
  actions_buttons->AddWidget(b_cancel);
}

//-----------------------------------------------------------------------------

Menu::~Menu()
{
  delete background;
  delete actions_buttons;
}

//-----------------------------------------------------------------------------

void Menu::sig_ok()
{
  jukebox.Play("share", "menu/ok");
  __sig_ok();
  close_menu = true;
}

//-----------------------------------------------------------------------------

void Menu::sig_cancel()
{
  jukebox.Play("share", "menu/cancel");
  __sig_cancel();
  close_menu = true;
}

//-----------------------------------------------------------------------------

bool Menu::BasicOnClic(int mouse_x, int mouse_y)
{
  if (b_ok->MouseIsOver (mouse_x, mouse_y)) {
    sig_ok();
  } else if (b_cancel->MouseIsOver (mouse_x, mouse_y)) {
    sig_cancel();
  } else {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

void Menu::BasicDraw(int mouse_x, int mouse_y)
{
  background->ScaleSize(app.sdlwindow->w, app.sdlwindow->h);
  background->Blit( app.sdlwindow, 0, 0);
  
  actions_buttons->Draw(mouse_x,mouse_y);
}

//-----------------------------------------------------------------------------

void Menu::Run ()
{ 
  int x=0, y=0;

  close_menu = false;

  // to limit CPU  
  uint sleep_fps=0;
  uint delay = 0;
  
  do
  {
    // to limit CPU  
    uint start = SDL_GetTicks();

   // Poll and treat events
   SDL_Event event;
     
   while( SDL_PollEvent( &event) ) 
     {      
       if ( event.type == SDL_QUIT) 
	 {  
	   sig_cancel();
	 }
       else if ( event.type == SDL_KEYDOWN )
	 {	       
	   switch ( event.key.keysym.sym)
	     { 
	     case SDLK_ESCAPE: 
	       sig_cancel();
	       break;
	       
	     case SDLK_RETURN: 
	       sig_ok();
	       break;
		  
	     default:
	       break;
	     }  
	 }
       else if ( event.type == SDL_MOUSEBUTTONDOWN )
	 {
	   if (! BasicOnClic(event.button.x, event.button.y) )
	     {
	       OnClic( event.button.x, event.button.y, event.button.button); //uint8 SDL_BUTTON_LEFT SDL_BUTTON_MIDDLE SDL_BUTTON_RIGHT SDL_BUTTON_WHEELUP SDL_BUTTON_WHEELDOWN
	     }
	 }
     }

   SDL_GetMouseState( &x, &y);

   BasicDraw(x, y);
   Draw(x, y);
   SDL_Flip( app.sdlwindow);

   // to limit CPU
   delay = SDL_GetTicks()-start;   
   if (delay < 200)
     sleep_fps = 200 - delay;
   else
     sleep_fps = 0;
   SDL_Delay(sleep_fps);

  } while (!close_menu);

}

//-----------------------------------------------------------------------------

void Menu::SetActionButtonsXY(int x, int y)
{
  actions_buttons->SetSizePosition(x, y, actions_buttons->GetW(), actions_buttons->GetH());
}
