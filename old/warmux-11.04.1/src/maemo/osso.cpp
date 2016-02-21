/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2010-2011 Warmux Team.
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
 * Maemo 
 *****************************************************************************/

#include <iostream>
#include <glib.h>
#include <glib-object.h>
#include <libosso.h>
#include <SDL/SDL.h>
#include "sound/jukebox.h"
#include "game/game_time.h"
#include "interface/mouse.h"

namespace {

  static void init_timer(void);
  static void remove_timer(void);


  GMainContext* maincontext = NULL;
  GMainLoop* mainloop = NULL;
  osso_context_t* osso_context = NULL;
  bool display_off = false;
  SDL_TimerID timer_id = 0;

  static gboolean display_off_loop(gpointer data) {
    remove_timer();
    SDL_QuitSubSystem(SDL_INIT_TIMER);
    GameTime::GetInstance()->SetWaitingForUser(true);
    JukeBox::GetInstance()->CloseDevice();

    while(display_off) {
      MSG_DEBUG("display", "display_off loop");
      g_main_context_iteration(maincontext, true);
    }

    if (Mouse::GetInstance()->HasFocus()) {
      JukeBox::GetInstance()->OpenDevice();
      JukeBox::GetInstance()->NextMusic();
      GameTime::GetInstance()->SetWaitingForUser(false);
    }

    SDL_InitSubSystem(SDL_INIT_TIMER);
    init_timer();

    return false;
  }

  static void osso_display_event_callback(osso_display_state_t state, gpointer data) {
    switch(state) {
    case OSSO_DISPLAY_ON:
      MSG_DEBUG("display", "DISPLAY ON");
      display_off = false;
      break;
    case OSSO_DISPLAY_OFF:
      MSG_DEBUG("display", "DISPLAY OFF");
      display_off = true;
      g_idle_add(display_off_loop, NULL);
      break;
    case OSSO_DISPLAY_DIMMED:
      MSG_DEBUG("display", "DISPLAY DIMMED");
      break;
    }
  }

  Uint32 send_null_event(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
  }

  /* Send SDL events regularly to guarantee Glib event processing when stuck with SDL_WaitEvent */
  static void init_timer() {
    if (timer_id == 0)
      timer_id = SDL_AddTimer(2000, send_null_event, NULL);
  }

  static void remove_timer() {
    if (timer_id != 0) {
      SDL_RemoveTimer(timer_id);
      timer_id = 0;
    }
  }
}

namespace Osso {

  int Init() {
    g_type_init();
    maincontext = g_main_context_default();
    mainloop = g_main_loop_new(maincontext, false);
    osso_context = osso_initialize("org.warmux.game", "1.0", 0, maincontext);

    if(!osso_context) {
      std::cerr << "could not initialize libosso\n";
      return -1;
    }

    osso_return_t ret;
    ret = osso_hw_set_display_event_cb(osso_context, osso_display_event_callback, NULL);
    if(ret != OSSO_OK) {
      std::cerr << "could not register osso display event callback\n";
      return -1;
    }

    init_timer();

    return 0;
  }

  void Process() {
    if (maincontext == NULL || osso_context == NULL)
      return;
    
    while(g_main_context_pending(maincontext)) {
      g_main_context_iteration(maincontext, false);
    }
  }

}

