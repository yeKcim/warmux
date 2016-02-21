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
 * libosso support
 *****************************************************************************/

#include <iostream>
#include <libosso.h>
#include <SDL/SDL.h>
#include "sound/jukebox.h"
#include "game/game_time.h"
#include "interface/mouse.h"
#include "maemo/glib.h"
#include "maemo/osso.h"

namespace {

  bool display_off = false;

  static gboolean display_off_loop(gpointer data)
  {
    Glib::EnterSleep();
    GameTime::GetInstance()->SetWaitingForUser(true);
    JukeBox::GetInstance()->CloseDevice();

    while(display_off) {
      MSG_DEBUG("display", "display_off loop");
      Glib::Process(true);
    }

    if (Mouse::GetInstance()->HasFocus()) {
      JukeBox::GetInstance()->OpenDevice();
      JukeBox::GetInstance()->NextMusic();
      GameTime::GetInstance()->SetWaitingForUser(false);
    }

    Glib::LeaveSleep();
    return false;
  }

  static void osso_display_event_callback(osso_display_state_t state, gpointer data)
  {
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

}

namespace Osso {

  osso_context_t* osso_context = NULL;

  int Init()
  {
    Glib::Init();
    osso_context = osso_initialize("org.warmux.game", "1.0", 0, Glib::GetContext());

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

    return 0;
  }

  void DeInit()
  {
    if (osso_context != NULL)
      osso_deinitialize(osso_context);
  }

}

