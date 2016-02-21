/******************************************************************************
 *  Warmux is a convivial mass murder game.
 *  Copyright (C) 2001-2011 Warmux Team.
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
 *  Very simple timed events generator
 *****************************************************************************/

#include <SDL.h>
#include "tool/eventtimer.h"


static Uint32 send_null_event(Uint32 interval, void */*param*/)
{
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

void EventTimer::Start(int interval)
{
  if (timer_id == 0)
    timer_id = SDL_AddTimer(interval, send_null_event, NULL);
}

void EventTimer::Stop()
{
  if (timer_id != 0) {
    SDL_RemoveTimer(timer_id);
    timer_id = 0;
  }
}

bool EventTimer::IsRunning() const
{
  if (timer_id != 0)
    return true;
  return false;
}
