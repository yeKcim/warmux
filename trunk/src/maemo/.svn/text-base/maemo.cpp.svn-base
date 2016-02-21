#include <stdlib.h>
#include <WARMUX_config.h>
#include "maemo/maemo.h"
#include "maemo/conic.h"
#include "maemo/glib.h"
#include "maemo/osso.h"
#include "maemo/resource.h"

namespace Maemo
{
  void Init()
  {
    Glib::Init();
#ifdef HAVE_LIBOSSO
    Osso::Init();
#endif
#ifdef HAVE_LIBRESOURCE
    Resource::Init();
#endif
#ifdef HAVE_LIBCONIC
    Conic::Init();
#endif
    atexit(DeInit);

  }

  void Process(bool block)
  {
    Glib::Process(block);
  }

  void DeInit()
  {
#ifdef HAVE_LIBOSSO
    Osso::DeInit();
#endif
#ifdef HAVE_LIBRESOURCE
    Resource::ReleaseResources();
#endif
    Glib::DeInit();
  }
}
