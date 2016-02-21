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
 * Generic menu
 *****************************************************************************/

#include "menu.h"
#include "../graphic/sprite.h"
#include "../graphic/video.h"
#include "../include/app.h"
#include "../tool/resource_manager.h"
#include "../sound/jukebox.h"

Menu::Menu(char * bg, t_action _actions) :
  actions(_actions)
{
  close_menu = false ;
  AppWormux * app = AppWormux::GetInstance();

  uint x = app->video.window.GetWidth() / 2;
  uint y = app->video.window.GetHeight() - 50;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  background = new Sprite( resource_manager.LoadImage( res, bg));
  background->cache.EnableLastFrameCache();

  b_ok = NULL;
  b_cancel = NULL;
  if (actions == vNo) {
    actions_buttons = NULL;    
  } else {

    actions_buttons = new HBox( Rectanglei(x, y, 1, 40), false);

    if (actions == vOk || actions == vOkCancel) {
      b_ok = new Button( Point2i(0, 0), res, "menu/valider"); 
      actions_buttons->AddWidget(b_ok);
    }

    if (actions == vCancel  || actions == vOkCancel) {
      b_cancel = new Button( Point2i(0, 0), res, "menu/annuler");
      actions_buttons->AddWidget(b_cancel);
    }

    widgets.AddWidget(actions_buttons);
  }

  widgets.SetContainer(this);
  resource_manager.UnLoadXMLProfile(res);
}

Menu::~Menu()
{
  delete background;
}

void Menu::sig_ok()
{
  jukebox.Play("share", "menu/ok");
  __sig_ok();
  close_menu = true;
}

void Menu::sig_cancel()
{
  jukebox.Play("share", "menu/cancel");
  __sig_cancel();
  close_menu = true;
}

bool Menu::BasicOnClic(const Point2i &mousePosition)
{
  if( b_ok != NULL &&  b_ok->Contains(mousePosition) )
    sig_ok();
  else if( b_cancel != NULL && b_cancel->Contains(mousePosition) )
    sig_cancel();
  else
    return false;
  
  return true;
}

void Menu::DrawBackground()
{
  background->ScaleSize(AppWormux::GetInstance()->video.window.GetSize());
  background->Blit(AppWormux::GetInstance()->video.window, 0, 0);
}

void Menu::Redraw(const Rectanglei& rect, Surface& surf)
{
  background->Blit(surf, rect, rect.GetPosition());
}

void Menu::Run ()
{ 
  int x=0, y=0;

  close_menu = false;

  // Display the background
  DrawBackground();

  do
  {
    // Poll and treat events
    SDL_Event event;
     
    while( SDL_PollEvent( &event) )
    {
      Point2i mousePosition(event.button.x, event.button.y);
	   
      if( event.type == SDL_QUIT )
        sig_cancel();
      else if( event.type == SDL_KEYDOWN )
        switch( event.key.keysym.sym)
        {
        case SDLK_ESCAPE:
          if(b_cancel != NULL)
            sig_cancel();
          else
            key_cancel();
          break;
        case SDLK_RETURN:
          if(b_ok != NULL)
            sig_ok();
          else
            key_ok();
          break;
        case SDLK_F10:
          AppWormux::GetInstance()->video.ToggleFullscreen();
          break;
        default:
          widgets.SendKey(event.key.keysym);
          break;
        }
      else if( event.type == SDL_MOUSEBUTTONUP)
      if( !BasicOnClic(mousePosition) )
        OnClic(mousePosition, event.button.button);
    }

    // Avoid to calculate redraw menu when comming back for closing.
    if (!close_menu) {

      SDL_GetMouseState( &x, &y );
      Point2i mousePosition(x, y);
      
      Display(mousePosition);
    }

  } while (!close_menu);
}

void Menu::Display(const Point2i& mousePosition)
{
  // to limit CPU  
  uint sleep_fps=0;
  uint delay = 0;
  uint start = SDL_GetTicks();

  widgets.Update(mousePosition, AppWormux::GetInstance()->video.window);
  Draw(mousePosition);
  AppWormux::GetInstance()->video.Flip();

  // to limit CPU
  delay = SDL_GetTicks()-start;   
  if (delay < 200)
    sleep_fps = 200 - delay;
  else
    sleep_fps = 0;
  SDL_Delay(sleep_fps);
}

void Menu::SetActionButtonsXY(int x, int y){
  if (actions_buttons != NULL)
    actions_buttons->SetSizePosition( Rectanglei(x, y, actions_buttons->GetSizeX(), actions_buttons->GetSizeY()) );
}
