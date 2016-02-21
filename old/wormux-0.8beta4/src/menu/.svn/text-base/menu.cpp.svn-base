/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "menu/menu.h"
#include "include/app.h"
#include "graphic/sprite.h"
#include "graphic/video.h"
#include "gui/button.h"
#include "gui/box.h"
#include "interface/mouse.h"
#include "sound/jukebox.h"
#include "tool/resource_manager.h"

static const int MENU_DELAY = 100;  // 10 fps, much sufficient for menu

Menu::Menu(const std::string& bg, t_action _actions) :
  actions(_actions)
{
  close_menu = false ;
  AppWormux * app = AppWormux::GetInstance();

  uint x = app->video->window.GetWidth() / 2;
  uint y = app->video->window.GetHeight() - 50;

  Profile *res = resource_manager.LoadXMLProfile( "graphism.xml", false);
  background = new Sprite( resource_manager.LoadImage( res, bg), true);
  background->cache.EnableLastFrameCache();

  b_ok = NULL;
  b_cancel = NULL;
  if (actions == vNo) {
    actions_buttons = NULL;
  } else {

    actions_buttons = new HBox(50, false);

    if (actions == vOk || actions == vOkCancel) {
      b_ok = new Button(res, "menu/valider");
      actions_buttons->AddWidget(b_ok);
    }

    if (actions == vCancel  || actions == vOkCancel) {
      b_cancel = new Button(res, "menu/annuler");
      actions_buttons->AddWidget(b_cancel);
    }

    actions_buttons->SetXY(x, y);
    widgets.AddWidget(actions_buttons);
  }

  widgets.SetContainer(this);
  resource_manager.UnLoadXMLProfile(res);
}

Menu::~Menu()
{
  delete background;
}

void Menu::play_ok_sound()
{
  jukebox.Play("share", "menu/ok");
}

void Menu::play_cancel_sound()
{
  jukebox.Play("share", "menu/cancel");
}

void Menu::play_error_sound()
{
  jukebox.Play("share", "menu/error");
}

void Menu::mouse_ok()
{
  if (signal_ok()) {
    play_ok_sound();
    close_menu = true;
  }
}

void Menu::mouse_cancel()
{
  if (signal_cancel()) {
    play_cancel_sound();
    close_menu = true;
  }
}

bool Menu::BasicOnClickUp(const Point2i &mousePosition)
{
  if( b_ok != NULL &&  b_ok->Contains(mousePosition) )
    mouse_ok();
  else if( b_cancel != NULL && b_cancel->Contains(mousePosition) )
    mouse_cancel();
  else
    return false;

  return true;
}

void Menu::key_ok()
{
  if (signal_ok()) {
    play_ok_sound();
    close_menu = true;
  }
}

void Menu::key_cancel()
{
  if (signal_cancel()) {
    play_cancel_sound();
    close_menu = true;
  }
}

void Menu::key_up()
{
  widgets.SetKeyboardFocusOnPreviousWidget();
  RedrawMenu();
}

void Menu::key_down()
{
  widgets.SetKeyboardFocusOnNextWidget();
  RedrawMenu();
}

void Menu::key_left()
{
}

void Menu::key_right()
{
}

void Menu::DrawBackground()
{
  background->ScaleSize(AppWormux::GetInstance()->video->window.GetSize());
  background->Blit(AppWormux::GetInstance()->video->window, 0, 0);
}

void Menu::RedrawBackground(const Rectanglei& rect, Surface& surf)
{
  background->Blit(surf, rect, rect.GetPosition());
}

void Menu::RedrawMenu()
{
  DrawBackground();
  widgets.NeedRedrawing();
}

void Menu::Run (bool skip_menu)
{
  if (skip_menu) {
    signal_ok();
    return;
  }

  Mouse::pointer_t old_pointer = Mouse::GetInstance()->SetPointer(Mouse::POINTER_STANDARD);
  int x=0, y=0;

  close_menu = false;

  // Display the background
  DrawBackground();

  do
  {
    // this is the current menu (here in case we had run a submenu)
    AppWormux::GetInstance()->SetCurrentMenu(this);

    // Poll and treat events
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      Point2i mousePosition(event.button.x, event.button.y);

      if (event.type == SDL_QUIT) {
        key_cancel();
      } else if (event.type == SDL_KEYDOWN) {
	bool used_by_widget = false;

	if (event.key.keysym.sym != SDLK_ESCAPE &&
	    event.key.keysym.sym != SDLK_RETURN)
	  used_by_widget = widgets.SendKey(event.key.keysym);
	
	if (!used_by_widget) {
	  switch (event.key.keysym.sym)
	    {
	    case SDLK_ESCAPE:
	      key_cancel();
	      break;
	    case SDLK_RETURN:
	      key_ok();
	      break;          
	    case SDLK_UP:
	      key_up();
	      break;
	    case SDLK_DOWN:
	      key_down();
	      break;
	    case SDLK_LEFT:
	      key_left();
	      break;
	    case SDLK_RIGHT:
	      key_right();
	      break;
	    case SDLK_F10:
	      AppWormux::GetInstance()->video->ToggleFullscreen();
	      break;
	    default:
	      // should have been handle upper!
	      break;
	    }
	}
      } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (!BasicOnClickUp(mousePosition))
          OnClickUp(mousePosition, event.button.button);
      } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        OnClick(mousePosition, event.button.button);
      }
    }

    // Avoid to calculate redraw menu when comming back for closing.
    if (!close_menu) {

      SDL_GetMouseState( &x, &y );
      Point2i mousePosition(x, y);

      Display(mousePosition);
    }

  } while (!close_menu);

  Mouse::GetInstance()->SetPointer(old_pointer);
}

void Menu::Display(const Point2i& mousePosition)
{
  // to limit CPU
  uint start = SDL_GetTicks();

  widgets.Update(mousePosition, AppWormux::GetInstance()->video->window);
  Draw(mousePosition);
  AppWormux::GetInstance()->video->Flip();

  // to limit CPU
  int delay = MENU_DELAY - (SDL_GetTicks()-start);
  if (delay > 0)
    SDL_Delay(delay);
}

void Menu::SetActionButtonsXY(int x, int y){
  if (actions_buttons != NULL)
    actions_buttons->SetSizePosition( Rectanglei(x, y, actions_buttons->GetSizeX(), actions_buttons->GetSizeY()) );
}
