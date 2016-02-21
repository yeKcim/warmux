#include <iostream>
#include <glib.h>
#include <glib-object.h>
#include <SDL.h>

SDL_TimerID timer_id = 0;

Uint32 send_null_event(Uint32 interval, void */*param*/) {
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

namespace Glib
{
  GMainContext* maincontext = NULL;
  GMainLoop* mainloop = NULL;
  bool is_init = false;

  void Init()
  {
    if (is_init) return;

    g_type_init();
    maincontext = g_main_context_default();
    mainloop = g_main_loop_new(maincontext, false);
    init_timer();

    is_init = true;
  }

  void DeInit()
  {
    g_main_loop_unref(mainloop);
    g_main_context_unref(maincontext);
    remove_timer();
  }

  void Process(bool blocking)
  {
    if (maincontext == NULL)
      return;

    while(g_main_context_pending(maincontext)) {
      g_main_context_iteration(maincontext, blocking);
    }
  }

  void EnterSleep()
  {
    remove_timer();
    SDL_QuitSubSystem(SDL_INIT_TIMER);
  }

  void LeaveSleep()
  {
    SDL_Init(SDL_INIT_TIMER);
    init_timer();
  }

  GMainContext* GetContext()
  {
    return maincontext;
  }
}

