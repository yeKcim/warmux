#include <stdlib.h>
#include <resource.h>
#include "maemo/glib.h"
#include "sound/jukebox.h"

static void grant_callback (resource_set_t *resource_set,
                            uint32_t        resources,
                            void           *userdata)
{
  (void)resource_set;
  (void)userdata;

  printf("*** %s(): granted resources %s\n", __FUNCTION__, "");

}

static void advice_callback (resource_set_t *resource_set,
                             uint32_t        resources,
                             void           *userdata)
{
  (void)resource_set;
  (void)userdata;

  printf("*** %s(): adviced resources %s\n", __FUNCTION__, "");
}

static void error_callback (resource_set_t *resource_set,
                            uint32_t        errorcode,
                            const char     *errmsg,
                            void           *userdata)
{
  JukeBox::GetInstance()->CloseDevice();

  printf("*** %s(): error resources: %s\n", __FUNCTION__, errmsg);
}

namespace Resource
{

  resource_set_t *resource_set;

  void Init()
  {
    resource_set = NULL;
    setenv("PULSE_PROP_media.role", "x-maemo", 1);
  }

  bool AcquireResources()
  {
    if (resource_set)
      return false;

    resource_set = resource_set_create ("game", RESOURCE_AUDIO_PLAYBACK,0, 0,
                                                       grant_callback, NULL);
    resource_set_configure_advice_callback (resource_set, advice_callback, NULL);
    resource_set_configure_error_callback (resource_set, error_callback, NULL);
    resource_set_configure_audio (resource_set, "game", getpid(), "sdl12_audio");
    resource_set_acquire (resource_set);

    Glib::Process();

    return true;
  }

  bool ReleaseResources()
  {
    if (resource_set == NULL)
      return false;

    resource_set_destroy (resource_set);
    resource_set = NULL;
    return true;
  }

}
