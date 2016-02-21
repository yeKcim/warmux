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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *****************************************************************************/

#include <algorithm>
#include <iostream>
#include <SDL_image.h>
#ifdef ANDROID
#  include <SDL_screenkeyboard.h>
#endif

#include <WARMUX_file_tools.h>

#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "map/camera.h"


Video::Video()
  :SDLReady(false)
  , fullscreen(false)
  , icon(NULL)
{
  SetMaxFps(50);

  Config * config = Config::GetInstance();
  SetMaxFps(config->GetMaxFps());

  InitSDL();

#ifdef WIN32
  // The SDL manual of SDL_WM_SetIcon states that "Win32 icons must be 32x32.":
  icon = IMG_Load((config->GetDataDir() + "icon\\warmux.xpm").c_str());
#elif !defined(ANDROID)
  // The icon must be larger then 32x32 pixels as some desktops display
  // larger icons. For example on a mac system, the icon is displayed
  // in a resolution of 64x64 pixels.
  // The even higher resolution allows the system to scale the icon down
  // to an anti-aliased version.
  icon = IMG_Load((config->GetDataDir() + "icon/warmux_128x128.xpm").c_str());
#endif
  // SDL_WM_SetIcon must be called before the first call to SDL_SetVideoMode
#ifndef MAEMO
  SDL_WM_SetIcon(icon, NULL);
#endif

  window.SetSurface(NULL, false);
  window.SetAutoFree(false);

  ComputeAvailableConfigs();

  // See if we can add the current config resolution
  int w = config->GetVideoWidth();
  int h = config->GetVideoHeight();
  if (!w || !h) {
    w = available_configs.begin()->GetX();
    h = available_configs.begin()->GetY();
  }

  SetConfig(w, h, config->IsVideoFullScreen());

  if (window.IsNull()) {
    Error(Format("Unable to initialize SDL window: %s", SDL_GetError()));
    exit (1);
  }
  AddUniqueConfigSorted(window.GetWidth(), window.GetHeight());

  SetWindowCaption(std::string("WarMUX ") + Constants::WARMUX_VERSION);
}

Video::~Video()
{
  if (icon) {
    SDL_FreeSurface(icon);
    icon = NULL;
  }
  if (SDLReady) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDLReady = false;
  }
}

void Video::SetMaxFps(uint max_fps)
{
  m_max_fps = max_fps;
  if (0 < m_max_fps)
    m_max_delay = 1000/m_max_fps;
  else
    m_max_delay = 0;
}

static bool CompareConfigs(const Point2i& a, const Point2i& b)
{
  return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
}

void Video::AddUniqueConfigSorted(int w, int h)
{
  Point2i p(w, h);

  std::list<Point2i>::iterator res  = available_configs.begin(),
                               end  = available_configs.end();

  for (; res != end; res++) {
    // Are they identical ?
    if (p == *res)
      return;

    // Is it bigger?
    if (!CompareConfigs(*res, p)) {
      available_configs.insert(res, p);
      return;
    }
  }
  available_configs.push_back(p);
}

void Video::ComputeAvailableConfigs()
{
  //Generate video mode list
  SDL_Rect **modes;

  // Get available fullscreen modes
  modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_SWSURFACE);

  // Check is there are any modes available
  if (modes != NULL){
    // We also had the current window resolution if it is not already in the list!
    for (int i=0;modes[i];++i) {
      if (modes[i]->w>=480 && modes[i]->h>=320) {
        AddUniqueConfigSorted(modes[i]->w, modes[i]->h);
      }
    }
  }

  // If biggest resolution is big enough, we propose standard resolutions
  // such as 1600x1200, 1280x1024, 1024x768, 800x600.
  for (std::list<Point2i>::iterator res = Config::GetInstance()->GetResolutionAvailable().begin();
       res != Config::GetInstance()->GetResolutionAvailable().end();
       res++) {
    AddUniqueConfigSorted((*res).GetX(), (*res).GetY());
  }
}

bool Video::__SetConfig(const int width, const int height, const bool _fullscreen)
{
  int flags = SDL_SWSURFACE;
#ifdef HAVE_HANDHELD
  int bpp   = 16;
#else
  int bpp   = Config::GetInstance()->GetQuality()==QUALITY_32BPP ? 32 : 16;
#endif

#ifdef HAVE_TOUCHSCREEN
  bool __fullscreen = true;
#else
  bool __fullscreen = _fullscreen;
#endif

  flags |= (__fullscreen) ? SDL_FULLSCREEN : 0;
#ifdef __SYMBIAN32__
  flags = 0;
#endif

  window.SetSurface(SDL_SetVideoMode(width, height, bpp, flags));
  if (window.IsNull())
    return false;

  fullscreen = __fullscreen;

  return true;
}

bool Video::SetConfig(const int width, const int height, const bool _fullscreen)
{
  bool r;
  bool window_was_null = window.IsNull();

  if (!window_was_null
      && width == window.GetWidth()
      && height == window.GetHeight()
      && fullscreen == _fullscreen)
    return true; // nothing to change :-)

  int old_width, old_height;
  bool old_fullscreen;
  if (window_was_null) {
    old_width = available_configs.begin()->GetX();
    old_height = available_configs.begin()->GetY();
  } else {
    old_width = window.GetWidth();
    old_height = window.GetHeight();
  }
  old_fullscreen = fullscreen;

  // Trying with hardware acceleration
  r = __SetConfig(width, height, _fullscreen);
  if (!r) {
    fprintf(stderr,
            "WARNING: Fail to initialize main window with the following configuration:\n"
            " %dx%d, fullscreen: %d\n", old_width, old_height, _fullscreen);

    // Trying previous configuration
    if (! __SetConfig(old_width, old_height, old_fullscreen)) {

      // previous configuration fails !?!

      // let's have another try without fullscreen
      if (! __SetConfig(old_width, old_height, false)) {
        Error(Format("ERROR: Fail to initialize main window with the following configuration:\n"
                     " %dx%d, no fullscreen,\n", old_width, old_height));
        exit(EXIT_FAILURE);
      }
    }
  }

  Camera::GetInstance()->SetSize(width, height);

  // refresh all the map when switching to higher resolution
  if (!window_was_null)
    AppWarmux::GetInstance()->RefreshDisplay();

  return r;
}

void Video::ToggleFullscreen()
{
#ifndef WIN32
  SDL_WM_ToggleFullScreen(window.GetSurface());
  fullscreen = !fullscreen;
#else
  SetConfig(window.GetWidth(), window.GetHeight(), !fullscreen);
  AppWarmux::GetInstance()->RefreshDisplay();
#endif
}

void Video::SetWindowCaption(const std::string& caption) const
{
  SDL_WM_SetCaption(caption.c_str(), NULL);
}

void Video::InitSDL()
{
  if (SDLReady)
    return;

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    Error(Format("Unable to initialize SDL library: %s", SDL_GetError()));
    exit(EXIT_FAILURE);
  }

#ifdef ANDROID
  SDL_Rect r = {0,0,0,0};
  SDL_ANDROID_SetScreenKeyboardButtonPos(SDL_ANDROID_SCREENKEYBOARD_BUTTON_TEXT, &r);
#endif

  SDL_EnableUNICODE(1);
  SDLReady = true;
}

void Video::Flip()
{
  window.Flip();
}

bool Video::SaveScreenshot()
{
  int u = 0;
  char name[19];
  std::string path = Config::GetConstInstance()->GetPersonalDataDir();
  std::string filename;

  // Generate and try names
  do {
    snprintf(name, sizeof(name), "screenshot%04u.bmp", u);
    filename = path + name;
    u++;
  } while (DoesFileExist(filename) && u<10000);

  if (u == 10000)
    return false;
  if (window.ImgSave(filename, true)) {
    printf("Saved screenshot %s\n", filename.c_str());
    return true;
  }
  printf("Failed saving screenshot %s: %s\n", filename.c_str(), SDL_GetError());
  return false;
}


Surface& GetMainWindow()
{
  return AppWarmux::GetInstance()->video->window;
}

void SwapWindowClip(Rectanglei& r)
{
  AppWarmux::GetInstance()->video->window.SwapClipRect(r);
}
