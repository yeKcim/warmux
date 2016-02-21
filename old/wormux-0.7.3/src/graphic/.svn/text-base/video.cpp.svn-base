/******************************************************************************
 *  Wormux, a free clone of the game Worms from Team17.
 *  Copyright (C) 2001-2004 Lawrence Azzoug.
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
#include <SDL_endian.h>
#include <SDL_image.h>
#include "../game/config.h"
#include "../tool/error.h"
#include "../tool/i18n.h"
#include "../include/app.h"
#include "../include/constant.h"
#include "../map/camera.h"

Video::Video(){
	SetMaxFps (50);
	fullscreen = false;
	SDLReady = false;
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

bool Video::IsFullScreen(void) const{
	return fullscreen;
}

bool Video::SetConfig(int width, int height, bool _fullscreen){
	// initialize the main window
	if( window.IsNull() || 
			(width != window.GetWidth() || 
			 height != window.GetHeight() ) ){

		window.SetSurface( SDL_SetVideoMode(width, height, 32, 
				SDL_HWSURFACE | SDL_HWACCEL | SDL_DOUBLEBUF ), false );

		if( window.IsNull() ) 
			window.SetSurface( SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE) );

		if( window.IsNull() )
			return false;
		fullscreen = false;
	}

	if(fullscreen != _fullscreen ){
		SDL_WM_ToggleFullScreen( window.GetSurface() );
		fullscreen = _fullscreen;
	}

	camera.SetSize(width, height);

	return true;
}

void Video::InitWindow(){
	InitSDL();

	window.SetSurface( NULL , false );
	window.SetAutoFree( false );

	Config * config = Config::GetInstance();
	SetConfig(config->tmp.video.width,
			config->tmp.video.height,
			config->tmp.video.fullscreen);

	if( window.IsNull() )
		Error( "Unable to initialize SDL window.");

	SetWindowCaption( std::string("Wormux ") + Constants::VERSION );
	SetWindowIcon( config->GetDataDir() + PATH_SEPARATOR + "wormux-32.xpm" );
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

	if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 )
		Error( Format( _("Unable to initialize SDL library: %s"), SDL_GetError() ) ); 

	SDLReady = true;
}

void Video::Flip(){
	window.Flip();
}

