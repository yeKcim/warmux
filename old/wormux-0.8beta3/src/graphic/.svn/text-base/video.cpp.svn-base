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
 *  MERCHANTABILITY or FITNESS FOR A ARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU GeneralPublic License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *****************************************************************************/

#ifdef _MSC_VER
#  include <algorithm>
#endif
#include <iostream>
#include <SDL_image.h>
#include "game/config.h"
#include "graphic/video.h"
#include "include/app.h"
#include "include/constant.h"
#include "map/camera.h"
#include "tool/i18n.h"


Video::Video(){
  SetMaxFps (50);
  fullscreen = false;
  SDLReady = false;

  InitSDL();

  window.SetSurface( NULL , false );
  window.SetAutoFree( false );

  Config * config = Config::GetInstance();
  SetConfig((int)(config->GetVideoWidth()),
            (int)(config->GetVideoHeight()),
            config->IsVideoFullScreen());

  SetMaxFps(config->GetMaxFps());

  if( window.IsNull() ) {
    Error(Format("Unable to initialize SDL window: %s", SDL_GetError()));
    exit (1);
  }

  SetWindowCaption( std::string("Wormux ") + Constants::VERSION );
  SetWindowIcon( config->GetDataDir() + PATH_SEPARATOR + "wormux_32x32.xpm" );

  ComputeAvailableConfigs();
}

Video::~Video(){
  if( SDLReady )
    SDL_Quit();
  SDLReady = false;
}

void Video::SetMaxFps(uint max_fps){
  m_max_fps = max_fps;
  if (0 < m_max_fps)
    m_sleep_max_fps = 1000/m_max_fps;
  else
    m_sleep_max_fps = 0;
}

uint Video::GetMaxFps() const {
  return m_max_fps;
}

uint Video::GetSleepMaxFps() const {
  return m_sleep_max_fps;
}

bool Video::IsFullScreen() const{
  return fullscreen;
}

static bool CompareConfigs(const Point2i& a, const Point2i& b)
{
  return  (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
}

void Video::AddConfigIfAbsent(int w, int h)
{
  Point2i p(w, h);

  if ( !CompareConfigs((*available_configs.begin()), p) &&
       find(available_configs.begin(), available_configs.end(), p) == available_configs.end() )
    available_configs.push_back(p);
}

void Video::ComputeAvailableConfigs()
{
  //Generate video mode list
  SDL_Rect **modes;

  // Get available fullscreen/hardware modes
  modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

  // Check is there are any modes available
  if(modes != NULL){
    // We also had the current window resolution if it is not already in the list!
    for(int i=0;modes[i];++i) {
      // We accept only modes that are bigger than 800x600
      if (modes[i]->w < 800 || modes[i]->h < 600) break;
      available_configs.push_back(Point2i(modes[i]->w, modes[i]->h));
    }
  }

  // Add the current resolution
  AddConfigIfAbsent(window.GetWidth(), window.GetHeight());

  // If biggest resolution is big enough, we propose standard resolutions
  // such as 1600x1200, 1280x1024, 1024x768, 800x600.
  for(std::list<Point2i>::iterator res = Config::GetInstance()->GetResolutionAvailable().begin();
      res != Config::GetInstance()->GetResolutionAvailable().end();
      res++) {
    AddConfigIfAbsent((*res).GetX(), (*res).GetY());
  }

  // Sort the list
  available_configs.sort(CompareConfigs);

  // Remove double items
  std::list<Point2i>::iterator prev = available_configs.begin(),
    it = available_configs.begin() ,
    end = available_configs.end();

  for (++it; it != end ; ++it) {
    if ((*prev)==(*it)) // the two items are equals
      prev = available_configs.erase(prev);
    else
      prev++;
  }
}

const std::list<Point2i>& Video::GetAvailableConfigs() const
{
  return available_configs;
}

bool Video::SetConfig(const int width, const int height, const bool _fullscreen){
  int flag = (_fullscreen) ? SDL_FULLSCREEN : 0;
  bool window_was_null = window.IsNull();


  // update the main window if needed
  if( window.IsNull() ||
     (width != window.GetWidth() ||
      height != window.GetHeight() ) ||
      fullscreen != _fullscreen){

    window.SetSurface( SDL_SetVideoMode(width, height, 32,
                       SDL_HWSURFACE | SDL_HWACCEL | SDL_DOUBLEBUF | flag), false );

    if( window.IsNull() ) {
      window.SetSurface( SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE | flag) );
      std::cerr << "WARNING: Video not using hardware acceleration!" << std::endl;
    }

    if( window.IsNull() )
      return false;

    fullscreen = _fullscreen;
    Camera::GetInstance()->SetSize(width, height);

    // refresh all the map when switching to higher resolution
    if (!window_was_null)
      AppWormux::GetInstance()->RefreshDisplay();
  }

  return true;
}

void Video::ToggleFullscreen()
{
#ifndef WIN32
  SDL_WM_ToggleFullScreen( window.GetSurface() );
  fullscreen = !fullscreen;
#else
  SetConfig(window.GetWidth(), window.GetHeight(), !fullscreen);
  AppWormux::GetInstance()->RefreshDisplay();
#endif
}

void Video::SetWindowCaption(const std::string& caption) const {
  SDL_WM_SetCaption( caption.c_str(), NULL );
}

void Video::SetWindowIcon(const std::string& filename) const {
  SDL_WM_SetIcon( IMG_Load(filename.c_str()), NULL );
}

void Video::InitSDL(){
  if( SDLReady )
    return;

  if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 ) {
    Error(Format("Unable to initialize SDL library: %s", SDL_GetError()));
    exit (1);
  }

  SDL_EnableUNICODE(1);
  SDLReady = true;
}

void Video::Flip(){
  window.Flip();
}

