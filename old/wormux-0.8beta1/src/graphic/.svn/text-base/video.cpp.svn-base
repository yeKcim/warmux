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

#include "video.h"
#include <string>
#include <iostream>
#include <SDL_endian.h>
#include <SDL_image.h>
#include "game/config.h"
#include "tool/error.h"
#include "tool/i18n.h"
#include "include/app.h"
#include "include/constant.h"
#include "map/camera.h"


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
}

void Video::SetMaxFps(uint max_fps){
  m_max_fps = max_fps;
  if (0 < m_max_fps)
    m_sleep_max_fps = 1000/m_max_fps;
  else
    m_sleep_max_fps = 0;
}

uint Video::GetMaxFps(){
  return m_max_fps;
}

uint Video::GetSleepMaxFps(){
  return m_sleep_max_fps;
}

bool Video::IsFullScreen() const{
  return fullscreen;
}

bool CompareConfigs(const Point2i& a, const Point2i& b)
{
  return  (a.x >= b.x) && (a.y >= b.y);
}

void Video::ComputeAvailableConfigs()
{
  // Add the current resolution
  available_configs.push_back(Point2i(window.GetWidth(),
                                      window.GetHeight()));

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

  // Sort the list
  available_configs.sort(CompareConfigs);

  // If biggest resolution is big enough, we propose standard resolution such as
  // 800x600, 1024x768, 1280x1024, 1600x1200
  Point2i a(800, 600);
  if ( CompareConfigs((*available_configs.begin()), a))
    available_configs.push_back(a);
  Point2i b(1024, 768);
  if ( CompareConfigs((*available_configs.begin()), b))
    available_configs.push_back(b);
  Point2i c(1280, 1024);
  if ( CompareConfigs((*available_configs.begin()), c))
    available_configs.push_back(c);
  Point2i d(1600, 1200);
  if ( CompareConfigs((*available_configs.begin()), d))
    available_configs.push_back(d);
  Point2i e(1400, 1050);
  if ( CompareConfigs((*available_configs.begin()), e))
    available_configs.push_back(e);
  Point2i f(1280, 800);
  if ( CompareConfigs((*available_configs.begin()), f))
    available_configs.push_back(f);

  // Sort the list again...
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

std::list<Point2i>& Video::GetAvailableConfigs()
{
  return available_configs;
}

bool Video::SetConfig(const int width, const int height, const bool _fullscreen){
  // initialize the main window
  if( window.IsNull() ||
     (width != window.GetWidth() ||
      height != window.GetHeight() ) ){

    window.SetSurface( SDL_SetVideoMode(width, height, 32,
                       SDL_HWSURFACE | SDL_HWACCEL | SDL_DOUBLEBUF ), false );

    if( window.IsNull() ) {
      window.SetSurface( SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE) );
      std::cerr << "WARNING: Video not using hardware acceleration!" << std::endl;
    }

    if( window.IsNull() )
      return false;
    fullscreen = false;
  }

  if(fullscreen != _fullscreen) {
    SDL_WM_ToggleFullScreen( window.GetSurface() );
    fullscreen = !fullscreen;
  }

  camera.SetSize(width, height);

  return true;
}

void Video::ToggleFullscreen()
{
  SDL_WM_ToggleFullScreen( window.GetSurface() );
  fullscreen = !fullscreen;
}

void Video::SetWindowCaption(std::string caption){
  SDL_WM_SetCaption( caption.c_str(), NULL );
}

void Video::SetWindowIcon(std::string filename){
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

